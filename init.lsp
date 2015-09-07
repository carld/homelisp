
(def caar (x)
  (car (car x)))

(def cadar (x)
  (car (cdr (car x))))

(def cadr (x)
  (car (cdr x)))

(def caddr (x)
  (car (cdr (cdr x))))

(def caddar (x)
  (car (cdr (cdr (car x)))))

(def null? (x)
  (eq x (quote ()) ))

(def not (x)
  (cond (x (quote ()) )
        (true true)) )

(def list (x y)
  (cons x (cons y (quote ()) )))

(def and (x y)
    (cond (x
              (cond (y true)
                    (true (quote ()) ))  )
          (true (quote ()) )))

(def append (x y)
    (cond ((null? x) y)
          (true (cons (car x) (append (cdr x) y)))))

(def pair (x y)
  (cond ((and (null? x) (null? y)) (quote ()))
        ((and (not (atom x)) (not (atom y)))
        (cons (list (car x) (car y))
        (pair (cdr x) (cdr y))))))

(def assoc (x y)
  (cond ((eq (caar y) x) (cadar y))
        (true (assoc x (cdr y)))))

(def eval (e a)
  (cond
    ((atom e) (assoc e a))
    ((atom (car e))
     (cond
       ((eq (car e) (quote quote)) (cadr e))
       ((eq (car e) (quote atom))  (atom   (eval (cadr e) a)))
       ((eq (car e) (quote eq))    (eq     (eval (cadr e) a)
                                    (eval (caddr e) a)))
       ((eq (car e) (quote car))   (car    (eval (cadr e) a)))
       ((eq (car e) (quote cdr))   (cdr    (eval (cadr e) a)))
       ((eq (car e) (quote cons))  (cons   (eval (cadr e) a)
                                    (eval (caddr e) a)))
       ((eq (car e) (quote cond))  (evcon (cdr e) a))
       (true (eval (cons (assoc (car e) a)
                        (cdr e))
                  a))))
    ((eq (caar e) (quote label))
     (eval (cons (caddar e) (cdr e))
            (cons (list (cadar e) (car e)) a)))
    ((eq (caar e) (quote lambda))
     (eval (caddar e)
            (append (pair (cadar e) (evlis (cdr e) a))
                     a)))))

(def evcon (c a)
  (cond ((eval (caar c) a)
                (eval (cadar c) a))
        (true (evcon (cdr c) a))))

(def evlis (m a)
  (cond ((null? m) (quote ()))
        (true (cons (eval  (car m) a)
              (evlis (cdr m) a)))))

(def factorial (x)
  (cond ((eq x 1) 1)
        (true (* x (factorial (- x 1))))))
