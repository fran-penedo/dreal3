(set-logic QF_NRA)

(declare-fun skoY () Real)
(declare-fun pi () Real)
(declare-fun skoX () Real)
(assert (and (not (<= (* skoY (* skoY (+ (- 1.) (* skoY (* skoY (+ (/ 1. 3.) (* skoY (* skoY (+ (/ (- 2.) 45.) (* skoY (* skoY (+ (/ 1. 315.) (* skoY (* skoY (+ (/ (- 2.) 14175.) (* skoY (* skoY (+ (/ 31. 7257600.) (* skoY (* skoY (+ (/ (- 1.) 10886400.) (* skoY (* skoY (+ (/ 101. 73156608000.) (* skoY (* skoY (+ (/ (- 1.) 73156608000.) (* skoY (* skoY (/ 1. 13168189440000.)))))))))))))))))))))))))))))) 0.)) (and (not (<= (* skoY (* skoY (+ (/ 1. 2.) (* skoY (* skoY (+ (/ (- 1.) 24.) (* skoY (* skoY (+ (/ 1. 720.) (* skoY (* skoY (+ (/ (- 1.) 40320.) (* skoY (* skoY (/ 1. 3628800.))))))))))))))) 1.)) (and (not (<= pi (/ 15707963. 5000000.))) (and (not (<= (/ 31415927. 10000000.) pi)) (not (<= skoY skoX)))))))
(set-info :status sat)
(check-sat)