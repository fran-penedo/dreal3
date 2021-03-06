/*********************************************************************
Author: Soonho Kong <soonhok@cs.cmu.edu>

dReal -- Copyright (C) 2013 - 2015, Soonho Kong, Sicun Gao, and Edmund Clarke

dReal is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

dReal is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with dReal. If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/

#include <algorithm>
#include <chrono>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <map>
#include <memory>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "nlopt.hpp"
#include "contractor/contractor.h"
#include "ibex/ibex.h"
#include "icp/icp.h"
#include "opensmt/egraph/Enode.h"
#include "util/box.h"
#include "constraint/constraint.h"
#include "util/logging.h"
#include "util/string.h"
#include "util/proof.h"
#include "util/eval.h"

using std::back_inserter;
using std::boolalpha;
using std::cerr;
using std::endl;
using std::function;
using std::initializer_list;
using std::make_shared;
using std::ostream;
using std::ostringstream;
using std::pair;
using std::queue;
using std::set;
using std::tuple;
using std::unordered_set;
using std::vector;

namespace dreal {

static unordered_map<Enode*, ibex::Interval> make_subst_from_value(box const & b, unordered_set<Enode *> const & vars) {
    unordered_map<Enode*, ibex::Interval> subst;
    for (Enode * const var : vars) {
        auto value = b[var];
        subst.emplace(var, value);
    }
    return subst;
}

contractor_generic_forall::contractor_generic_forall(box const & , generic_forall_constraint const * const ctr)
    : contractor_cell(contractor_kind::FORALL), m_ctr(ctr) {
}

box contractor_generic_forall::handle(box b, Enode * body, bool const p,  SMTConfig & config) const {
    if (body->isOr()) {
        vector<Enode *> vec = elist_to_vector(body->getCdr());
        return handle_disjunction(b, vec, p, config);
    } else if (body->isAnd()) {
        vector<Enode *> vec = elist_to_vector(body->getCdr());
        return handle_conjunction(b, vec, p, config);
    } else if (body->isNot()) {
        return handle(b, body->get1st(), !p, config);
    } else {
        return handle_atomic(b, body, p, config);
    }
}

static bool term_cond(dreal::box const & old_box, dreal::box const & new_box) {
    double const threshold = 0.01;
    // If there is a dimension which is improved more than
    // threshold, we stop the current fixed-point computation.
    for (unsigned i = 0; i < old_box.size(); i++) {
        double const new_box_i = new_box[i].diam();
        double const old_box_i = old_box[i].diam();
        if (new_box_i == numeric_limits<double>::infinity()) {
            continue;
        }
        if (old_box_i == 0) {
            // The i-th dimension was already a point, nothing to improve.
            continue;
        }
        double const improvement = 1 - new_box_i / old_box_i;
        assert(!std::isnan(improvement));
        if (improvement >= threshold) {
            return false;
        }
    }
    return true;
}

vector<Enode *> contractor_generic_forall::elist_to_vector(Enode * e) const {
    vector<Enode *> vec;
    while (!e->isEnil()) {
        vec.push_back(e->getCar());
        e = e->getCdr();
    }
    return vec;
}

double nlopt_eval_enode(const double* x, void * extra) {
    auto extra_info = static_cast<tuple<Enode *, box const &, bool> *>(extra);
    Enode * e = get<0>(*extra_info);
    box const & b = get<1>(*extra_info);
    bool const polarity = get<2>(*extra_info);
    unordered_map<Enode *, double> var_map;
    unsigned i = 0;
    for (Enode * e : b.get_vars()) {
        if (e->isForallVar()) {
            var_map.emplace(e, x[i]);
            i++;
        } else {
            var_map.emplace(e, b[e].mid());
        }
    }
    try {
        double const ret1 = eval_enode(e->get1st(), var_map);
        double const ret2 = eval_enode(e->get2nd(), var_map);
        double ret = 0;
        if (e->isLt() || e->isLeq() || e->isEq()) {
            ret = ret1 - ret2;
        } else if (e->isGt() || e->isGeq()) {
            ret = ret2 - ret1;
        } else if (e->isEq()) {
            throw runtime_error("nlopt_obj: something is wrong.");
        }
        if (!polarity) {
            ret = - ret;
        }
        return ret;
    } catch (exception & e) {
        DREAL_LOG_FATAL << "Exception in nlopt_eval_enode: " << e.what() << endl;
        throw e;
    }
}

void nlopt_fill_gradient(const double * x, double * grad, void * extra) {
    auto extra_info = static_cast<tuple<Enode *, box const &, bool> *>(extra);
    Enode * e = get<0>(*extra_info);
    box const & b = get<1>(*extra_info);
    bool const polarity = get<2>(*extra_info);
    unordered_map<Enode *, double> var_map;
    unsigned i = 0;
    vector<Enode*> forall_var_vec;
    for (Enode * e : b.get_vars()) {
        if (e->isForallVar()) {
            var_map.emplace(e, x[i]);
            i++;
            forall_var_vec.push_back(e);
        } else {
            var_map.emplace(e, b[e].mid());
        }
    }
    i = 0;
    for (Enode * var : forall_var_vec) {
        double deriv_i = deriv_enode(e->get1st(), var, var_map) - deriv_enode(e->get2nd(), var, var_map);
        if (e->isGt() || e->isGeq()) {
            deriv_i = - deriv_i;
        }
        if (!polarity) {
            deriv_i = - deriv_i;
        }
        grad[i] = deriv_i;
        i++;
    }
}

double nlopt_obj(unsigned, const double * x, double * grad, void * extra) {
    double const ret = nlopt_eval_enode(x, extra);
    if (grad) {
        nlopt_fill_gradient(x, grad, extra);
    }
    return ret;
}

double nlopt_side_condition(unsigned, const double * x, double * grad, void * extra) {
    double const ret = nlopt_eval_enode(x, extra);
    if (grad) {
        nlopt_fill_gradient(x, grad, extra);
    }
    return ret;
}

box refine_CE_with_nlopt_core(box counterexample, vector<Enode*> const & opt_ctrs, vector<Enode*> const & side_ctrs) {
    // Plug-in `a` into the constraint and optimize `b` in the counterexample `M` by solving:
    //
    //    ∃ y_opt ∈ I_y. ∀ y ∈ I_y. f(a, y_opt) >= f(a, y) — (2)
    //
    // using local optimizer (i.e. nlopt).
    // Let `M’ = (a, b_opt)` be a model for (2).

    DREAL_LOG_DEBUG << "================================" << endl;
    DREAL_LOG_DEBUG << "  Before Refinement              " << endl;
    DREAL_LOG_DEBUG << "================================" << endl;
    DREAL_LOG_DEBUG << counterexample << endl;
    DREAL_LOG_DEBUG << "================================" << endl;
    static bool initialized = false;
    static vector<double> lb, ub, init;
    init.clear();
    for (Enode * e : counterexample.get_vars()) {
        if (e->isForallVar()) {
            if (!initialized) {
                lb.push_back(e->getDomainLowerBound());
                ub.push_back(e->getDomainUpperBound());
            }
            init.push_back(counterexample[e].mid());
            DREAL_LOG_DEBUG << lb.back() << " <= " << init.back() << " <= " << ub.back() << endl;
        }
    }
    auto const n = init.size();
    static nlopt::opt opt(nlopt::LD_SLSQP, n);
    if (!initialized) {
        opt.set_lower_bounds(lb);
        opt.set_upper_bounds(ub);
        // set tollerance
        // TODO(soonhok): set precision
        // opt.set_xtol_rel(0.0001);
        opt.set_xtol_abs(0.001);
        opt.set_maxtime(0.01);
        initialized = true;
    }

    opt.remove_equality_constraints();
    opt.remove_inequality_constraints();

    // set objective function
    vector<tuple<Enode *, box const &, bool> *> extra_vec;
    Enode * e = opt_ctrs[0];
    bool polarity = false;
    while (e->isNot()) {
        e = e->get1st();
        polarity = !polarity;
    }
    auto extra = new tuple<Enode *, box const &, bool>(e, counterexample, polarity);
    extra_vec.push_back(extra);
    opt.set_min_objective(nlopt_obj, extra);
    opt.add_inequality_constraint(nlopt_side_condition, extra);
    DREAL_LOG_DEBUG << "objective function is added: " << e << endl;

    // set side conditions
    for (Enode * e : side_ctrs) {
        bool polarity = false;
        while (e->isNot()) {
            e = e->get1st();
            polarity = !polarity;
        }
        auto extra = new tuple<Enode *, box const &, bool>(e, counterexample, polarity);
        extra_vec.push_back(extra);
        DREAL_LOG_DEBUG << "refine_counterexample_with_nlopt: Side condition is added: " << e << endl;
        if (e->isEq()) {
            opt.add_equality_constraint(nlopt_side_condition, extra);
        } else if (e->isLt() || e->isLeq() || e->isGt() || e->isGeq()) {
            opt.add_inequality_constraint(nlopt_side_condition, extra);
        }
    }
    try {
        vector<double> output = opt.optimize(init);
        unsigned i = 0;
        for (Enode * e : counterexample.get_vars()) {
            if (e->isForallVar()) {
                counterexample[e] = output[i];
                i++;
            }
        }
    } catch (nlopt::roundoff_limited & e) {
    } catch (std::runtime_error & e) {
        DREAL_LOG_DEBUG << e.what() << endl;
    }

    for (auto extra : extra_vec) {
        delete extra;
    }
    DREAL_LOG_DEBUG << "================================" << endl;
    DREAL_LOG_DEBUG << "  After Refinement              " << endl;
    DREAL_LOG_DEBUG << "================================" << endl;
    DREAL_LOG_DEBUG << counterexample << endl;
    DREAL_LOG_DEBUG << "================================" << endl;
    return counterexample;
}

box refine_CE_with_nlopt(box const & b, vector<Enode*> const & vec) {
    vector<Enode *> opt_ctrs;
    vector<Enode *> side_ctrs;
    for (Enode * e : vec) {
        if (!e->get_exist_vars().empty()) {
            opt_ctrs.push_back(e);
        } else if (!e->get_forall_vars().empty()) {
            side_ctrs.push_back(e);
        }
    }
    if (opt_ctrs.size() != 1) { return b; }
    box refined_counterexample = refine_CE_with_nlopt_core(b, opt_ctrs, side_ctrs);
    if (refined_counterexample.is_subset(b)) {
        return b;
    } else {
        return refined_counterexample;
    }
}

contractor make_contractor(Enode * e, lbool const polarity, box const & b, vector<nonlinear_constraint *> & ctrs) {
    if (e->isNot()) {
        return make_contractor(e->get1st(), !polarity, b, ctrs);
    }
    if (e->isOr()) {
        // TODO(soonhok): arbitrary number of args
        assert(e->getArity() == 2);
        contractor c1 = make_contractor(e->get1st(), polarity, b, ctrs);
        contractor c2 = make_contractor(e->get2nd(), polarity, b, ctrs);
        return mk_contractor_join(c1, c2);
    }
    if (e->isAnd()) {
        vector<contractor> ctcs;
        e = e->getCdr();
        while (!e->isEnil()) {
            ctcs.push_back(make_contractor(e->getCar(), polarity, b, ctrs));
            e = e->getCdr();
        }
        return mk_contractor_seq(ctcs);
    } else {
        nonlinear_constraint * ctr = new nonlinear_constraint(e, polarity);
        ctrs.push_back(ctr);
        return mk_contractor_ibex_fwdbwd(b, ctr);
    }
}

box shrink_for_dop(box b) {
    for (Enode * e : b.get_vars()) {
        string const name = e->getCar()->getName();
        if (starts_with(name, "forall_")) {
            string const exist_var_name = name.substr(7);
            auto exist_var_intv = b[exist_var_name];
            b[name] = exist_var_intv;
        }
    }
    return b;
}

box find_CE_via_underapprox(box const & b, unordered_set<Enode*> const & forall_vars, vector<Enode*> const & vec, bool const p, SMTConfig & config) {
    box counterexample(b, forall_vars);
    if (config.nra_shrink_for_dop) {
        counterexample = shrink_for_dop(counterexample);
    }
    for (Enode * e : vec) {
        lbool polarity = p ? l_False : l_True;
        if (e->isNot()) {
            e = e->get1st();
            polarity = !polarity;
        }
        nonlinear_constraint ctr(e, polarity);
        auto numctr = ctr.get_numctr();

        // Construct iv from box b
        auto & var_array = ctr.get_var_array();
        ibex::IntervalVector iv(var_array.size());
        for (int i = 0; i < var_array.size(); i++) {
            iv[i] = counterexample[var_array[i].name];
        }
        if (numctr->op == ibex::CmpOp::GT || numctr->op == ibex::CmpOp::GEQ) {
            numctr->f.ibwd(ibex::Interval(-config.nra_precision, POS_INFINITY), iv);
        } else if (numctr->op == ibex::CmpOp::LT || numctr->op == ibex::CmpOp::LEQ) {
            numctr->f.ibwd(ibex::Interval(NEG_INFINITY, config.nra_precision), iv);
        } else if (numctr->op == ibex::CmpOp::EQ) {
            numctr->f.ibwd(ibex::Interval(-config.nra_precision, config.nra_precision), iv);
        } else {
            throw runtime_error("??");
        }
        if (iv.is_empty()) {
            counterexample.set_empty();
            return counterexample;
        } else {
            // Reconstruct box b from pruned result iv.
            for (int i = 0; i < var_array.size(); i++) {
                counterexample[var_array[i].name] = iv[i];
            }
        }
    }
    return counterexample;
}

box find_CE_via_overapprox(box const & b, unordered_set<Enode*> const & forall_vars, vector<Enode*> const & vec, bool const p, SMTConfig & config) {
    vector<nonlinear_constraint *> ctrs;
    vector<contractor> ctcs;
    box counterexample(b, forall_vars);
    if (config.nra_shrink_for_dop) {
        counterexample = shrink_for_dop(counterexample);
    }
    for (Enode * e : vec) {
        lbool polarity = p ? l_False : l_True;
        if (e->isNot()) {
            polarity = !polarity;
            e = e->get1st();
        }
        contractor ctc = make_contractor(e, polarity, counterexample, ctrs);
        ctcs.push_back(ctc);
    }
    contractor fp = mk_contractor_fixpoint(term_cond, ctcs);
    // double const prec = std::max(b.max_diam() / 10.0, config.nra_precision);
    double const prec = config.nra_precision;
    // cerr << "find CE, prec = " << prec << endl;
    counterexample = random_icp::solve(counterexample, fp, config, prec);
    for (auto ctr : ctrs) {
        delete ctr;
    }
    return counterexample;
}

box contractor_generic_forall::find_CE(box const & b, unordered_set<Enode*> const & forall_vars, vector<Enode*> const & vec, bool const p, SMTConfig & config) const {
    // static unsigned under_approx = 0;
    // static unsigned over_approx = 0;
    box counterexample = find_CE_via_underapprox(b, forall_vars, vec, p, config);
    if (!counterexample.is_empty()) {
        // ++under_approx;
        // cerr << "WE USE UNDERAPPROX: " << under_approx << "/" << over_approx<< "/" << (under_approx + over_approx) << endl;
    } else {
        counterexample = find_CE_via_overapprox(b, forall_vars, vec, p, config);
        // ++over_approx;
        // cerr << "WE USE FULL       : " << under_approx << "/" << over_approx << "/" << (under_approx + over_approx)
        //      << " " << counterexample.is_empty()
        //      << endl;
    }
    if (!counterexample.is_empty() && config.nra_local_opt) {
        return refine_CE_with_nlopt(counterexample, vec);
    }
    return counterexample;
}

box contractor_generic_forall::handle_disjunction(box b, vector<Enode *> const &vec, bool const p, SMTConfig & config) const {
    DREAL_LOG_DEBUG << "contractor_generic_forall::handle_disjunction" << endl;
    unordered_set<Enode *> forall_vars;
    for (Enode * e : vec) {
        std::unordered_set<Enode *> const & vars = e->get_forall_vars();
        forall_vars.insert(vars.begin(), vars.end());
    }

    unordered_map<Enode*, ibex::Interval> subst;
    if (!forall_vars.empty()) {
        // Step 2. Find a counter-example
        //         Solve(¬ l_1 ∧ ¬ l_2 ∧ ... ∧ ¬ l_n)
        //
        //         Make each ¬ l_i as a contractor ctc_i
        //         Make a fixed_point contractor with ctc_is.
        //         Pass it to icp::solve

        box counterexample = find_CE(b, forall_vars, vec, p, config);
        if (counterexample.is_empty()) {
            // Step 2.1. (NO Counterexample)
            //           Return B.
            DREAL_LOG_DEBUG << "handle_disjunction: no counterexample found." << endl
                            << "current box = " << endl
                            << b << endl;
            return b;
        } else {
            // Step 2.2. (There IS a counterexample C)
            //
            //      Using C, prune B.
            //
            // We've found a counterexample (c1, c2) where ¬ f(c1, c2) holds
            // Prune X using a point 'y = c2'. (technically, a point in c2, which is an interval)
            subst = make_subst_from_value(counterexample, forall_vars);
        }
    }

    // Step 3. Compute B_i = prune(B, l_i)
    //         Update B with ∨ B_i
    //                       i
    vector<box> boxes;
    for (Enode * e : vec) {
        if (!e->get_exist_vars().empty()) {
            lbool polarity = p ? l_True : l_False;
            if (e->isNot()) {
                polarity = !polarity;
                e = e->get1st();
            }
            nonlinear_constraint ctr(e, polarity, subst);
            if (ctr.get_var_array().size() == 0) {
                auto result = ctr.eval(b);
                if (result.first != false) {
                    boxes.emplace_back(b);
                }
            } else {
                contractor ctc = mk_contractor_ibex_fwdbwd(b, &ctr);
                box const bt = ctc.prune(b, config);
                boxes.emplace_back(bt);
            }
        }
    }
    b = hull(boxes);
    return b;
}

box contractor_generic_forall::handle_conjunction(box b, vector<Enode *> const & vec, bool const p, SMTConfig & config) const {
    DREAL_LOG_DEBUG << "contractor_generic_forall::handle_conjunction" << endl;
    for (Enode * e : vec) {
        DREAL_LOG_DEBUG << "process conjunction element : " << e << endl;
        b = handle(b, e, p, config);
        if (b.is_empty()) {
            return b;
        }
    }
    return b;
}
box contractor_generic_forall::handle_atomic(box b, Enode * body, bool const p, SMTConfig & config) const {
    vector<Enode*> vec;
    vec.push_back(body);
    return handle_disjunction(b, vec, p, config);
}

box contractor_generic_forall::prune(box b, SMTConfig & config) const {
    DREAL_LOG_DEBUG << "contractor_generic_forall prune: " << *m_ctr << endl;
    Enode * body = m_ctr->get_body();
    DREAL_LOG_DEBUG << "body = " << body << endl;
    b = handle(b, body, true, config);
    return b;
}

ostream & contractor_generic_forall::display(ostream & out) const {
    out << "contractor_generic_forall(" << *m_ctr << ")";
    return out;
}

contractor mk_contractor_generic_forall(box const & b, generic_forall_constraint const * const ctr) {
    return contractor(make_shared<contractor_generic_forall>(b, ctr));
}

}  // namespace dreal
