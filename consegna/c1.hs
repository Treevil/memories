module Slide1 where

{-- 
1) Show that sum [x]=x for any number x.
--}

mysum       :: [Int] -> Int
mysum []     = 0
mysum (x:xs) = x + mysum xs

{-- 
	2) Define a function product that produces the product of a list of numbers, 
	and show using your definition that product [ 2, 3, 4 ] = 24. 
--}

myproduct       :: [Int] -> Int
myproduct []     = 1
myproduct (x:xs) = x * myproduct xs

-- ghci 
-- *Slide1> myproduct [2, 3, 4]
-- 24

{-- 
	3) How should the definition of the function qsort be modified so that it 
 	produces a reverse sorted version of a list?
--}
 
qsort1         :: Ord a => [a] -> [a]
qsort1 []       = []
qsort1 (x : xs) = qsort1 smaller ++ [x] ++ qsort1 larger
               where
                   smaller = [a | a <- xs, a > x ]
                   larger  = [b | b <- xs, b <= x ]

{-- 
	4) What would be the effect of replacing <= by < in the definition of qsort?
	Answer: An ordered list without duplicates.
--}
