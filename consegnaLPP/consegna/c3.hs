module Slide3 where

{-- 
1) What are the types of the (1) following values?
	['a','b','c']            :: [Char]
	('a','b','c')            :: (Char,Char,Char)
	[(False,'0'),(True,'0')] :: [(Bool,Char)]
	[(False,True),('0','1')] :: Errore
	[tail,init,reverse]      :: [[a] -> [a]]
--}

{-- 
	2) What are the types of the following functions?
--}
second   :: [a] -> a
second xs = head (tail xs)

swap      :: (a,b) -> (b,a)
swap (x,y) = (y,x)

pair    :: a -> b -> (a,b)
pair x y = (x,y)

double  :: Int -> Int
double x = x*2

palindrome   :: [Char] -> Bool
palindrome xs = reverse xs == xs

twice    :: (a -> a) -> a -> a
twice f x = f (f x)