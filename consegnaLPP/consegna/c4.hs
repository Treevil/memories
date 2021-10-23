module Slide4 where

{-- 
 1) Consider a function safetail that behaves in the
 same way as tail, except that safetail maps the
 empty list to the empty list, whereas tail gives
 an error in this case. Define safetail using:
   (a) a conditional expression;
   (b) guarded equations;
   (c) pattern matching.
 Hint: the library function null :: [a] -> Bool can
 be used to test if a list is empty.

--}

safetail1   :: [a] -> [a]
safetail1 xs = if null xs
               then []
               else tail xs

safetail2   :: [a] -> [a]
safetail2 xs | null xs   = []
             | otherwise = tail xs

safetail3    :: [a] -> [a]
safetail3 []  = []
safetail3 xs  = tail xs

{-- 

 2) Give three possible definitions
 for the logical or operator (||) 
 using pattern matching.

--}

myor1            :: Bool -> Bool -> Bool
myor1 False False = False
myor1 _ _         = True

myor2        :: Bool -> Bool -> Bool
myor2 False b = b
myor2 True _  = True 

myor3        :: Bool -> Bool -> Bool
myor3 True _  = True
myor3 _ True  = True
myor3 _ _     = False

{-- 
 3) Redefine the following version of 
 (&&) using conditionals rather 
 than patterns:

    True && True = True
    _    && _    = False

--}

myand     :: Bool -> Bool -> Bool
myand a b  = if a==True && b==True
             then True
             else False 
{-- 
4) Do the same for the following version:

   True  && b = b
   False && _ = False

--}
myand2     :: Bool -> Bool -> Bool
myand2 a b  = if a
              then b
              else False 

{-- 

 5) Using library functions, define a function
 halve :: [ a ] -> ([ a ], [ a ])
 that splits an even-lengthed list into two halves. For
 example:
   > halve [1,2,3,4,5,6] ([1, 2, 3], [4, 5, 6])

--}

halve   :: [a] -> ([a],[a])
halve xs = (take n xs, drop n xs)
       where
          n = (length xs) `div` 2

{-- 

	6) Show how the curried function definition
        	mult x y z = x *y *z
		can be understood in terms of lambda expressions.

--}

mult :: Num a => a -> a -> a -> a
mult  = \x -> \y -> \z -> x * y * z


{-- 
 7) Valutare: 
 Prelude > ( \ f x y −> f x y ) ( + ) 3 4
 Answer: 7

 Prelude > map ( \ x y −> ( x ∗ x ) + ( y ∗ y ) ) [ 2 , 3 , 4 ]
 Prelude > [\y -> 4 + (y * y), \y -> 9 + (y * y), \y -> 16 + (y * y)]

 Prelude > ( \ f g x y −> ( x ‘ f ‘ x ) ‘ g ‘ ( y ‘ f ‘ y ) ) ( + ) ( ∗ ) 2 5
 Answer: 40

Test
*Slide4> let opt = ( \ f x y -> f x y )
*Slide4> opt (+) 3 4
7


*Slide4> let opt = \f g x y -> (x `f` x) `g` (y `f` y)
*Slide4> opt (+) (*) 2 5
40

  Cosa calcola la funzione j se
 		g = \x −> x ∗ x         g(x) = x * x
       h = \y −> g ( g y )     h(x) = g(g(x)) = x ^ 8
       j = h.h                 j(x) = h * h =  x^16 

  --}