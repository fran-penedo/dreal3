(set-logic QF_NRA)
(declare-fun skoX () Real)
(declare-fun skoS () Real)
(declare-fun skoC () Real)
(assert (and (not (<= (+ (* (- 248062500000000000000) skoS) (* 225000000000000000000 skoC) (* 6945750000000000000 (* skoX skoS)) (* (- 6300000000000000000) (* skoX skoC)) (* (- 97240500000000000) (* skoX skoX skoS)) (* 88200000000000000 (* skoX skoX skoC)) (* 907578000000000 (* skoX skoX skoX skoS)) (* (- 823200000000000) (* skoX skoX skoX skoC)) (* (- 5558915250000) (* skoX skoX skoX skoX skoS)) (* 5042100000000 (* skoX skoX skoX skoX skoC)) (* 22235661000 (* skoX skoX skoX skoX skoX skoS)) (* (- 20168400000) (* skoX skoX skoX skoX skoX skoC)) (* (- 51883209) (* skoX skoX skoX skoX skoX skoX skoS)) (* 47059600 (* skoX skoX skoX skoX skoX skoX skoC))) 0)) (and (not (<= skoX 0)) (and (= (+ (* skoS skoS) (* skoC skoC)) 1) (and (<= skoX 75) (<= (* (- 1) skoX) 0))))))
(set-info :status sat)
(check-sat)