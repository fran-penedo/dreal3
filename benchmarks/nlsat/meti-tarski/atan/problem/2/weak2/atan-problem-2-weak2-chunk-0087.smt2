(set-logic QF_NRA)

(declare-fun skoT () Real)
(declare-fun skoB () Real)
(declare-fun skoA () Real)
(assert (and (not (<= 0. skoA)) (and (not (<= (* skoT (+ (* skoB (* skoB (- 1.))) (* skoT (* skoT (- 1.))))) 0.)) (and (not (<= (* skoT (+ (* skoB (* skoB (* skoA (/ (- 157.) 100.)))) (* skoT (+ (+ (* skoA (* skoA (- 1.))) (* skoB (* skoB (+ (- 1.) (* skoA (/ (- 3.) 10.)))))) (* skoT (+ (* skoA (/ (- 157.) 100.)) (* skoT (+ (- 1.) (* skoA (/ (- 3.) 10.)))))))))) (* skoB (* skoB (* skoB skoA))))) (and (not (<= skoB (* skoA (- 1.)))) (and (not (<= skoT 1.)) (not (<= skoB skoA))))))))
(set-info :status unsat)
(check-sat)