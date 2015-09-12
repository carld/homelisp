; Lisp functions
; Includes implementation of eval by McCarthy
; that came from http://ep.yimg.com/ty/cdn/paulgraham/jmc.lisp
; and an implementation of reverse that
; came from http://stackoverflow.com/questions/19529829/how-to-recursively-reverse-a-list-using-only-basic-operations

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
  (eq x '()))

(def not (x)
  (cond (x '())
        ('t 't)))

(def list (x y)
  (cons x (cons y '())))

(def and (x y)
    (cond (x (cond (y 't)
                   ('t '())))
          ('t '() )))

(def append (x y)
    (cond ((null? x) y)
          ('t (cons (car x) (append (cdr x) y)))))

(def pair (x y)
  (cond ((and (null? x) (null? y)) '())
        ((and (not (atom x)) (not (atom y)))
           (cons (list (car x) (car y))
                 (pair (cdr x) (cdr y))))))

(def assoc (x y)
  (cond ((eq (caar y) x) (cadar y))
        ('t (assoc x (cdr y)))))

(def eval (e a)
  (cond
    ((atom e) (assoc e a))
    ((atom (car e))
     (cond
       ((eq (car e) 'quote) (cadr e))
       ((eq (car e) 'atom)  (atom   (eval (cadr e) a)))
       ((eq (car e) 'eq)    (eq     (eval (cadr e) a)
                                    (eval (caddr e) a)))
       ((eq (car e) 'car)   (car    (eval (cadr e) a)))
       ((eq (car e) 'cdr)   (cdr    (eval (cadr e) a)))
       ((eq (car e) 'cons)  (cons   (eval (cadr e) a)
                                    (eval (caddr e) a)))
       ((eq (car e) 'cond)  (evcon (cdr e) a))
       ('t (eval (cons (assoc (car e) a) (cdr e)) a))))
    ((eq (caar e) 'label)
     (eval (cons (caddar e) (cdr e))
            (cons (list (cadar e) (car e)) a)))
    ((eq (caar e) 'lambda)
     (eval (caddar e)
           (append (pair (cadar e) (evlis (cdr e) a)) a)))))

(def evcon (c a)
  (cond ((eval (caar c) a) (eval (cadar c) a))
        ('t (evcon (cdr c) a))))

(def evlis (m a)
  (cond ((null? m) '())
        ('t (cons (eval (car m) a)
                    (evlis (cdr m) a)))))


(def factorial (x)
  (cond ((eq x 1) 1)
        ('t (* x (factorial (- x 1))))))

(def map (fn ls)
  (cond ((eq ls '())  '())
        ('t (cons (fn (car ls)) (map fn (cdr ls))))))

(def fibonacci (x)
  (cond ((eq x 0) 0)
        ((eq x 1) 1)
        ('t (+ (fibonacci (- x 1))
                 (fibonacci (- x 2))))))

(def reverse (ls)
  (cond
    ((null? ls) ls)
    ((null? (cdr ls)) ls)
    ('t
      (cons (car (reverse (cdr ls)))
            (reverse (cons (car ls)
                       (reverse (cdr (reverse (cdr ls))))))))))

(def string-join (ls de)
  (cond
    ((null? ls) "")
    (#t (string-append (car ls) 
                       (cond ((cdr ls) (string-append de (string-join (cdr ls) de)))
                             (#t ""))))))

