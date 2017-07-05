module Slide2 where

{-- 

	Author: Devalle Trevor
	Number: 2/ 17

--}

{-- 
	1) Fix the syntax errors in the program below.
	  
  	  N = a 'div' length xs
            where
               a = 10
 		      xs = [1,2,3,4,5] <-- Error

--}
n  :: Int
n   = a `div` length xs
    where 
       a  = 10
       xs = [1,2,3,4,5]


{-- 

	2) Show how the library function last that selects
	the last element of a list can be defined using
	the functions introduced in this lecture.

--}

mylast    :: [a] -> a
mylast xs  = head (reverse xs)

{-- 
	
	3) Can you think of another possible definition?

--}
mylast2   :: [a] -> a
mylast2 xs = xs !! (length xs - 1)

{-- 
	
	4) Similarly, show how the library function init
 	that removes the last element from a list can
 	be defined in two different ways.
 
 --}

init1   :: [a] -> [a]
init1 xs = reverse (tail (reverse xs))

init2   :: [a] -> [a]
init2 xs = take (length xs - 1) xs 

