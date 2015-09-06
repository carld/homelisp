
(def caar (x)
  (car (car x)))

(def cadar (x)
  (car (cdr (car x))))

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

(def factorial (x)
  (cond ((eq x 1) 1)
        (true (* x (factorial (- x 1))))))

