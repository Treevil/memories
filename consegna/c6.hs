module Slide6 where

{-- 
6/17

(0) Define (with types!) and using recursion the
functions that:
calculate the sum of a list of numbers
--}

mysum       :: [Int] -> Int
mysum []     = 0
mysum (x:xs) = x + mysum xs

{-- 
      take a given number of elements from the start of a list
--}

mytake         :: Int -> [a] -> [a]
mytake 0 _      = []
mytake _ []     = []
mytake n (x:xs) = x : mytake (n-1) xs

{-- 
      select the last element of a non-empty list
--}
mylast       :: [a] -> a
mylast [x]    = x
mylast (x:xs) = mylast xs

{-- 

select the maximum element of a non-empty list

--}
mymax    :: Ord a => [a] -> a
mymax [x]    = x 
mymax (x:xs) = if x > mymax xs 
               then x 
               else mymax xs

{-- 
      repeat builds an infinite list of a given element
--}

myrepeat  :: a -> [a]
myrepeat x = x : myrepeat x

{-- 
Without looking at the standard prelude, define
the following library functions using recursion.
--}
myand    :: [Bool] -> Bool
myand  []    = True 
myand (x:xs) = x && myand xs

myconcat       :: [[a]] -> [a]
myconcat []     = []
myconcat (x:xs) = x ++ myconcat xs

myreplicate   :: Int -> a -> [a]
myreplicate 0 _  = []
myreplicate n x  = x : myreplicate (n-1) x

myselect :: [a] -> Int -> a
myselect (x:xs) 0 = x
myselect (x:xs) n = myselect xs (n-1)

myelem         :: Eq a => a -> [a] -> Bool
myelem _ []     = False
myelem n (x:xs) = if n == x
                  then True
                  else myelem n xs

{-- 
 2) Define a recursive function
        merge :: [Int] -> [Int] -> [Int]
 that merges two sorted lists of integers to give
 a single sorted list. For example:
      > merge [2,5,6] [1,3,4]
      [1,2,3,4,5,6]
--}

merge :: [Int] -> [Int] -> [Int]
merge [] ys = ys
merge xs [] = xs
merge (x:xs) (y:ys) = if x <= y
                      then x : merge xs (y:ys)
                      else y : merge (x:xs) ys

{--
3)
 Define a recursive function
        msort :: [Int] -> [Int]
 that implements merge sort, which can be
 specified by the following two rules:
 1) Lists of length = 1 are already sorted;
 2) Other lists can be sorted by sorting the two
 halves and merging the resulting lists.
--}
msort    :: [Int] -> [Int]
msort []  = []
msort [x] = [x]
msort xs  = merge (msort (take (div (length xs) 2) xs)) (msort (drop (div (length xs) 2) xs))

