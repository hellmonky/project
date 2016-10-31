;; 1 首先定义一个递归函数：
(define (fact n)
  (if (= n 1)
      1
      (* n (fact (- n 1)))))
(fact 3)

;; 2 然后使用def来定义这个函数的名称
(define factAN
  (lambda (n)
    (if (= n 1)
      1
      (* n (factAN (- n 1))))))
(factAN 3)

;; 3 然后将自己这个定义提取出来：这个函数接受一个函数作为参数，然后返回一个函数，返回的这个函数接受一个数量参数
(define factAN3
  (lambda (f)
    (lambda (n)
      (if (= n 1)
          1
          (* n ((f f) (- n 1)))))))
;; 调用的时候就是自己调用自己，产生了一个接受数量参数的函数，然后用这个函数调用数量参数
((factAN3 factAN3) 3)

;; 4 我们将上述实际运算的函数调用中的名称用函数本身的定义改写：
(((lambda (f)
    (lambda (n)
      (if (= n 1)
          1
          (* n ((f f) (- n 1))))))
 (lambda (f)
    (lambda (n)
      (if (= n 1)
          1
          (* n ((f f) (- n 1)))))))
3)

;; 5 到目前为止，已经不再需要名称了，但是为了进一步的抽象，将这个过程中的数量参数提取出来作为函数：
(define F
    ((lambda (f)
       (lambda (n)
         (if (= n 1)
             1
             (* n ((f f) (- n 1))))))
     (lambda (f)
       (lambda (n)
         (if (= n 1)
             1
             (* n ((f f) (- n 1))))))))
(F 4)

;; 6 现在虽然将数量参数独立出来，但是这个结构和计算斐波那契的计算过程紧密绑定了，需要再次抽象提取：
;; （1）首先定义一个内部包含自己调用自己的函数，这个函数接受一个数量参数，然后返回一个数量
(lambda (argu)
  ((f f) argu))
;; （2）然后用这个函数将上述函数过程替换：
(define F
    ((lambda (f)
       (lambda (n)
         (if (= n 1)
             1
             (* n
                ((lambda (argu)
                   ((f f) argu))
                 (- n 1))))))
     (lambda (f)
       (lambda (n)
         (if (= n 1)
             1
             (* n
                ((lambda (argu)
                   ((f f) argu))
                 (- n 1))))))))
;; （3）然后将这个过程命名后提取为一个函数：接受一个函数作为参数，返回一个带有自由变量的函数
(define (procedure)
  (lambda (argu)
    ((procedure procedure) argu)))
;; （4）将这个函数作为一个整体带入，然后应用在函数上，这样就可以提取出这个独立变量
(define F
    ((lambda (procedure)
       ((lambda (f)
         (lambda (n)
           (if (= n 1)
               1
               (* n (f (- n 1))))))
        ;; 将这个函数作为f被调用
        (lambda (argu) ((procedure procedure) argu))))
     (lambda (procedure)
        ((lambda (f)
          (lambda (n)
            (if (= n 1)
                1
                (* n (f (- n 1))))))
         ;; 将这个函数作为f被调用
         (lambda (argu) ((procedure procedure) argu))))))
(F 4)
;; （5）继续观察上述函数，这个函数中的斐波那契计算过程已经被独立出来了，我们就可以进行抽象了
(define calculate_special
  (lambda (f)
         (lambda (n)
           (if (= n 1)
               1
               (* n (f (- n 1)))))))
;; （6）使用上述的calculate_special函数可以进一步简化上述的F函数：
(define F
  ((lambda (procedure)
     (calculate_special (lambda (argu) ((procedure procedure) argu))))
   (lambda (procedure)
     (calculate_special (lambda (argu) ((procedure procedure) argu))))))
(F 6)
;; （7）可以看到（6）中的定义已经将具体计算过程提取为一个函数了，那么就可以将这个具体的函数抽象，欢迎Y的到来：
(define Y-combinator
  (lambda (function_argu)
    (
     (lambda (procedure)
       (function_argu (lambda (argu) ((procedure procedure) argu))))
     (lambda (procedure)
       (function_argu (lambda (argu) ((procedure procedure) argu)))))
    )
  )
;; Y-combinator函数接受两个参数:一个函数function_argu和一个数量参数argu，其中函数数function_argu接受一个函数作为参数，然后返回一个带有自由变量的函数
;; 首先Y-combinator函数绑定function_argu作为参数，用function_argu调用了一个中间匿名过程来产生一个新的含有自由变量argu的函数；
;; 然后这个新生成的含有自由变量的函数绑定在数量参数argu上，最终返回结果。
;; 其中最关键的就是函数参数调用匿名函数的时候完成了递归调用结构的构造。
((Y-combinator calculate_special) 3)

;; 7 可以看出来Y组合子中的函数参数是一个匿名的或者已经定义过的可以接受自己的函数过程，其中设置两个绑定变量：一个是函数参数f，一个是数量变量n：
(define recursion_funciton
  (lambda (f)
    (lambda (n)
      ("do calculate with f as the recursion function itself"))))
;; 上述这个函数就是使用Y-combinator时候定义的递归函数了，主要用f来代理自己来完成递归调用。

;; 8 获得Y组合子之后就需要考虑其他问题了：













