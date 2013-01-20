(set-logic QF_NRA)
(declare-fun buscore0dollarsk!0 () Real)
(declare-fun a () Real)
(declare-fun c () Real)
(declare-fun y0 () Real)
(declare-fun x0 () Real)
(declare-fun t1uscore0dollarsk!3 () Real)
(declare-fun t2uscore0dollarsk!2 () Real)
(declare-fun tuscore0dollarsk!1 () Real)
(assert (= (+ (* c c) (* (- 1) (* a a)) (* (- 1) (* buscore0dollarsk!0 buscore0dollarsk!0))) 0))
(assert (= (+ (* c c) (* (- 1) (* buscore0dollarsk!0 buscore0dollarsk!0)) (* (- 1) (* x0 x0)) (* 2 (* buscore0dollarsk!0 y0)) (* (- 1) (* y0 y0))) 0))
(assert (= (+ (* (- 1) a) (* (- 1) x0) (* y0 t1uscore0dollarsk!3)) 0))
(assert (= (+ (* (- 1) a) x0 (* y0 t2uscore0dollarsk!2)) 0))
(assert (= (+ (* (- 1) t1uscore0dollarsk!3) (* (- 1) t2uscore0dollarsk!2) tuscore0dollarsk!1 (* (- 1) (* t1uscore0dollarsk!3 t2uscore0dollarsk!2 tuscore0dollarsk!1))) 0))
(assert (not (= y0 0)))
(assert (not (= a 0)))
(assert (not (= (+ (* (- 1) a) (* buscore0dollarsk!0 tuscore0dollarsk!1)) 0)))
(check-sat)