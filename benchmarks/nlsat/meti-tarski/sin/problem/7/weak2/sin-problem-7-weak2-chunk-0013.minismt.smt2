(set-logic QF_NRA)
(declare-fun skoA () Real)
(declare-fun pi () Real)
(declare-fun skoX () Real)
(assert (and (<= (* (- 1) skoA) 0) (and (not (= skoA 0)) (and (not (<= (* 5000000 pi) 15707963)) (and (not (<= (* (- 10000000) pi) (- 31415927))) (and (not (<= (+ (* (- 2) skoA) pi) 0)) (and (not (<= skoX 0)) (not (<= (+ skoA (* (- 1) skoX)) 0)))))))))
(set-info :status sat)
(check-sat)