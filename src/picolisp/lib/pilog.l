# 09jul12abu
# (c) Software Lab. Alexander Burger

# *Rule

(de be CL
   (clause CL) )

(de clause (CL)
   (with (car CL)
      (if (== *Rule This)
         (queue (:: T) (cdr CL))
         (=: T (cons (cdr CL)))
         (setq *Rule This) )
      This ) )

(de repeat ()
   (conc (get *Rule T) (get *Rule T)) )

(de asserta (CL)
   (push (prop CL 1 T) (cdr CL)) )

(de assertz (CL)
   (queue (prop CL 1 T) (cdr CL)) )

(de retract (X)
   (if (sym? X)
      (put X T)
      (put (car X) T
         (delete (cdr X) (get (car X) T)) ) ) )

(de rules @
   (while (args)
      (let S (next)
         (for ((N . L) (get S T) L)
            (prin N " (be ")
            (print S)
            (for X (pop 'L)
               (space)
               (print X) )
            (prinl ")")
            (T (== L (get S T))
               (println '(repeat)) ) )
         S ) ) )

### Pilog Interpreter ###
(de goal ("CL" . @)
   (let "Env" '(T)
      (while (args)
         (push '"Env"
            (cons (cons 0 (next)) 1 (next)) ) )
      (while (and "CL" (pat? (car "CL")))
         (push '"Env"
            (cons
               (cons 0 (pop '"CL"))
               (cons 1 (eval (pop '"CL"))) ) ) )
      (cons
         (cons
            (conc (list 1 (0) NIL "CL" NIL) "Env") ) ) ) )

(de fail ()
   (goal '((NIL))) )

(de pilog ("CL" . "Prg")
   (for ("Q" (goal "CL") (prove "Q"))
      (bind @ (run "Prg")) ) )

(de solve ("CL" . "Prg")
   (make
      (if "Prg"
         (for ("Q" (goal "CL") (prove "Q"))
            (link (bind @ (run "Prg"))) )
         (for ("Q" (goal "CL") (prove "Q"))
            (link @) ) ) ) )

(de query ("Q" "Dbg")
   (use "R"
      (loop
         (NIL (prove "Q" "Dbg"))
         (T (=T (setq "R" @)) T)
         (for X "R"
            (space)
            (print (car X))
            (print '=)
            (print (cdr X))
            (flush) )
         (T (line)) ) ) )

(de ? "CL"
   (let "L"
      (make
         (while (nor (pat? (car "CL")) (lst? (car "CL")))
            (link (pop '"CL")) ) )
      (query (goal "CL") "L") ) )

### Basic Rules ###
(be repeat)
(repeat)

(be true)

(be not @P (1 -> @P) T (fail))
(be not @P)

(be call @P
   (2 cons (-> @P)) )

(be or @L (@C box (-> @L)) (_or @C))

(be _or (@C) (3 pop (-> @C)))
(be _or (@C) (@ not (val (-> @C))) T (fail))
(repeat)

(be nil (@X) (@ not (-> @X)))

(be equal (@X @X))

(be different (@X @X) T (fail))
(be different (@ @))

(be append (NIL @X @X))
(be append ((@A . @X) @Y (@A . @Z)) (append @X @Y @Z))

(be member (@X (@X . @)))
(be member (@X (@ . @Y)) (member @X @Y))

(be delete (@A (@A . @Z) @Z))
(be delete (@A (@X . @Y) (@X . @Z))
   (delete @A @Y @Z) )

(be permute ((@X) (@X)))
(be permute (@L (@X . @Y))
   (delete @X @L @D)
   (permute @D @Y) )

(be uniq (@B @X)
   (@ not (idx (-> @B) (-> @X) T)) )

(be asserta (@C) (@ asserta (-> @C)))

(be assertz (@C) (@ assertz (-> @C)))

(be retract (@C)
   (2 cons (-> @C))
   (@ retract (list (car (-> @C)) (cdr (-> @C)))) )

(be clause ("@H" "@B")
   ("@A" get (-> "@H") T)
   (member "@B" "@A") )

(be show (@X) (@ show (-> @X)))

(be val (@V . @L)
   (@V apply get (-> @L))
   T )

(be lst (@V . @L)
   (@Lst box (apply get (-> @L)))
   (_lst @V @Lst) )

(be _lst (@Val @Lst) (@ not (val (-> @Lst))) T (fail))
(be _lst (@Val @Lst) (@Val pop (-> @Lst)))
(repeat)

(be map (@V . @L)
   (@Lst box (apply get (-> @L)))
   (_map @V @Lst) )

(be _map (@Val @Lst) (@ not (val (-> @Lst))) T (fail))
(be _map (@Val @Lst) (@Val prog1 (val (-> @Lst)) (pop (-> @Lst))))
(repeat)

# vi:et:ts=3:sw=3
