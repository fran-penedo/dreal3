(set-logic QF_NRA)
(declare-fun skoY () Real)
(declare-fun skoX () Real)
(declare-fun skoZ () Real)
(assert (and (<= (+ (* 89019 skoY) (* 89019 skoX) (* 89019 skoZ) (* (- 56700) (* skoY skoX)) (* (- 75600) (* skoX skoX)) (* (- 119700) (* skoY skoY)) (* (- 56700) (* skoY skoZ)) (* (- 56700) (* skoX skoZ)) (* 29673 (* skoX skoX skoX)) (* 98910 (* skoY skoY skoX)) (* 29673 (* skoY skoX skoX)) (* 98910 (* skoY skoY skoY)) (* (- 89019) (* skoY skoX skoZ)) (* 29673 (* skoX skoX skoZ)) (* 98910 (* skoY skoY skoZ)) (* (- 44100) (* skoY skoY skoY skoX)) (* (- 102900) (* skoY skoY skoX skoX)) (* (- 57600) (* skoY skoY skoY skoY)) (* (- 6300) (* skoY skoY skoX skoZ)) (* 37800 (* skoY skoX skoX skoZ)) (* (- 44100) (* skoY skoY skoY skoZ)) (* 32970 (* skoY skoY skoX skoX skoX)) (* 21195 (* skoY skoY skoY skoY skoX)) (* 32970 (* skoY skoY skoY skoX skoX)) (* 21195 (* skoY skoY skoY skoY skoY)) (* (- 29673) (* skoY skoX skoX skoX skoZ)) (* (- 98910) (* skoY skoY skoY skoX skoZ)) (* 32970 (* skoY skoY skoX skoX skoZ)) (* 21195 (* skoY skoY skoY skoY skoZ)) (* 6300 (* skoY skoY skoY skoX skoX skoX)) (* (- 3840) (* skoY skoY skoY skoY skoY skoX)) (* (- 32700) (* skoY skoY skoY skoY skoX skoX)) (* 18900 (* skoY skoY skoX skoX skoX skoZ)) (* 30600 (* skoY skoY skoY skoY skoX skoZ)) (* 48300 (* skoY skoY skoY skoX skoX skoZ)) (* (- 3840) (* skoY skoY skoY skoY skoY skoZ)) (* (- 3840) (* skoY skoY skoY skoY skoY skoY)) (* 7065 (* skoY skoY skoY skoY skoX skoX skoX)) (* 7065 (* skoY skoY skoY skoY skoY skoX skoX)) (* (- 32970) (* skoY skoY skoY skoX skoX skoX skoZ)) (* (- 21195) (* skoY skoY skoY skoY skoY skoX skoZ)) (* 7065 (* skoY skoY skoY skoY skoX skoX skoZ)) (* 3220 (* skoY skoY skoY skoY skoY skoX skoX skoX)) (* 14700 (* skoY skoY skoY skoY skoX skoX skoX skoZ)) (* 3840 (* skoY skoY skoY skoY skoY skoY skoX skoZ)) (* 12220 (* skoY skoY skoY skoY skoY skoX skoX skoZ)) (* (- 1280) (* skoY skoY skoY skoY skoY skoY skoX skoX)) (* (- 7065) (* skoY skoY skoY skoY skoY skoX skoX skoX skoZ)) (* 1280 (* skoY skoY skoY skoY skoY skoY skoX skoX skoX skoZ))) 56700) (and (not (<= (* (- 1) skoX) 0)) (and (not (<= (* (- 1) (* skoY skoX)) (- 1))) (and (not (<= (+ skoY skoX skoZ (* skoY skoX) (* (- 1) (* skoY skoX skoZ))) 1)) (and (or (not (<= (+ (* (- 1) skoY) (* (- 1) skoX) (* (- 1) skoZ) (* skoY skoX skoZ)) 0)) (<= (* (- 1) skoY) 0)) (and (or (<= (* (- 1) skoY) 0) (<= (+ skoY skoX skoZ (* skoY skoX) (* (- 1) (* skoY skoX skoZ))) 1)) (and (or (not (<= (* (- 1) skoY) 0)) (or (<= (+ (* (- 1) skoY) (* (- 1) skoX) (* (- 1) skoZ) (* skoY skoX skoZ)) 0) (<= (+ (* 3 skoZ) (* skoX skoX skoX) (* 3 (* skoY skoY skoX)) (* 3 (* skoY skoX skoX)) (* (- 3) (* skoY skoX skoZ)) (* skoX skoX skoZ) (* skoY skoY skoX skoX skoX) (* (- 1) (* skoY skoX skoX skoX skoZ))) 0))) (and (not (<= skoZ 0)) (and (not (<= skoX (- 1))) (and (not (<= (* (- 1) skoY) (- 1))) (not (<= (+ skoY (* (- 1) skoX)) 0)))))))))))))
(set-info :status sat)
(check-sat)