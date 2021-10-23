module Slide7 where 

{-- 
Define (with types) a function flip that takes a function and 
returns a function that is like our original function, only the first two arguments are flipped
--}
mflip :: (a -> b -> c) -> b -> a -> c
mflip f = \x y -> f y x


{-- 
   Redefine maximum, reverse, product, head, last 
   using only foldr, foldl, foldr1, foldl1, if...then..else,  * , [], and lambdas
--}

mmaximum :: Ord a => [a] -> a
mmaximum = foldr1 (\x y -> if x > y then x else y)

mreverse, mreverse2 :: [a] -> [a]
mreverse = foldl (\x y -> y : x) []

mreverse2 = foldl (flip (:)) []

mproduct :: Num a => [a] -> a
mproduct = foldr (\x y -> x * y) 1

mfilter :: Eq a => (a -> Bool) -> [a] -> [a]
mfilter p = reverse . foldl (\acc x -> (if p x then (x:) else id) acc) []

mhead :: [a] -> a
mhead = foldr1 (\x _ -> x)

mlast :: [a] -> a
mlast = foldr1 (\_ x -> x)

{-- 
 1) Express the comprehension  [f x | x <- xs, p x] using the
 functions map and filter.
--}
mapfilter       :: (a -> a) -> (a -> Bool) -> [a] -> [a]
mapfilter f p xs = map f (filter p xs)

{--  
2) Define the higher-­order functions all, any, takeWhile, and
 dropWhile.

 Redefine map f and filter p using foldr.
--}

all     :: (a -> Bool) -> [a] -> Bool
all p xs = and [p x | x <- xs]

any     :: (a -> Bool) -> [a] -> Bool
any p xs = or [p x | x <- xs]

mytakeWhile     :: (a -> Bool) -> [a] -> [a]
mytakeWhile p [] = []
mytakeWhile p (x:xs)
      | p x        = x : takeWhile p xs
      | otherwise  = []

mydropWhile     :: (a -> Bool) -> [a] -> [a]
mydropWhile p [] = []
mydropWhile p (x:xs)
      | p x        = dropWhile p xs
      | otherwise  = x:xs 

mymap     :: (a -> b) -> [a] -> [b]
mymap f xs = foldr (\y ys -> (f y): ys) [] xs -- f a : (f b : (f c : (f d : [])))

myfilter     ::  (a -> Bool) -> [a] -> [a]
myfilter p xs = foldr (\y ys -> (if (p y) then [y] else []) ++ ys) [] xs

{--
 3) Using foldl or foldl1, define a function dec2int :: [Int] -> Int that converts a  
 decimal number into an integer. For example:
 > dec2int [2,3,4,5]  
 2345
--}
dec2int   :: [Int] -> Int
dec2int xs = foldl (\x y -> 10 * x + y) 0 xs 

{-- 
 4) Explain  why  the  following  definition  is  invalid:
 sumsqreven =  compose  [sum,  map  (^2),  filter even]

 Sum doesn't return a list lke map and filter, Haskell
 returns a type error.
--}

{-- 
    5) Define the higher­order  library function curry that converts a function
    on pairs into a curried function, and, conversely, the function uncurry
    that converts a curried function with two arguments into a function
    on pairs
--}

mycurry :: ((a,b) -> c) -> a -> b -> c
mycurry f = \x -> \y -> f (x,y)

myuncurry :: (a -> b -> c) -> (a,b) -> c
myuncurry f = \(x,y) -> f x y

