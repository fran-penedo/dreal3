(set-logic QF_NRA)
(declare-fun skoY () Real)
(declare-fun skoX () Real)
(declare-fun skoS1 () Real)
(declare-fun skoS2 () Real)
(assert (and (not (= (+ (* 2 skoY) (* (- 1) (* skoX skoX)) (* (- 1) (* skoY skoY)) (* skoS1 skoS1)) 1)) (not (<= skoS2 2))))
(set-info :status sat)
(check-sat)