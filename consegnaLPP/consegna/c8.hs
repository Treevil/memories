module Slide8 where

----------------
-- 8/17

-- Define a function to give the perimeter of a shape.
-- Add triangles to the type and the function definition. 

data Shape = Circle Float | Rect Float Float | Triangle Float

perimeter           :: Shape -> Float
perimeter (Circle r) = 2*pi*r
perimeter (Rect x y) = x*2 + y*2
perimeter (Triangle x) = x*3

area           :: Shape -> Float
area (Circle r) = pi*r*r
area (Rect x y) = x*y
area (Triangle x) = (x*h)/2 
    where h = sqrt (x^2 - (x/2)^2)

{--

data Tree a   = Leaf a | Node (Tree a) (Tree a)
data Tree a   = Leaf | Node (Tree a) a (Tree a)  
data Tree a b = Leaf a | Node (Tree a b) b (Tree a b)  
data Tree a   = Node a [Tree a]


 Write examples showing different trees of the above types and for
 each type define a function occurs and a function flatten

--}
data Tree1 = Leaf1 Int | Node1 Tree1 Int Tree1
            deriving (Show)
data Tree2 a = Leaf2 a | Node2 (Tree2 a) (Tree2 a)
data Tree3 a = Leaf3 | Node3 (Tree3 a) a (Tree3 a)  
data Tree4 a b = Leaf4 a | Node4 (Tree4 a b) b (Tree4 a b)  
data Tree5 a = Node5 a [Tree5 a]

occurs2              :: Eq a => a -> Tree2 a -> Bool
occurs2 m (Leaf2 n)   = m == n
occurs2 m (Node2 l r) = occurs2 m l || occurs2 m r

flatten2            :: Tree2 a -> [a]
flatten2 (Leaf2 a)   = [a]
flatten2 (Node2 l r) = flatten2 l ++ flatten2 r

occurs3                :: Eq a => a -> Tree3 a -> Bool
occurs3 m (Leaf3)       = False
occurs3 m (Node3 l n r) = m == n || occurs3 m l || occurs3 m r

flatten3              :: Tree3 a -> [a]
flatten3 (Leaf3)       = []
flatten3 (Node3 l n r) = flatten3 l ++ [n] ++ flatten3 r    

occurs4leaf                :: Eq a => a -> Tree4 a b -> Bool
occurs4leaf m (Leaf4 n)     = m == n
occurs4leaf m (Node4 l n r) = occurs4leaf m l || occurs4leaf m r

occurs4node                :: Eq b => b -> Tree4 a b -> Bool
occurs4node m (Leaf4 n)     = False
occurs4node m (Node4 l n r) = m == n || occurs4node m l || occurs4node m r

flatten4leaf              :: Tree4 a b -> [a]
flatten4leaf (Leaf4 n)       = [n]
flatten4leaf (Node4 l n r) = flatten4leaf l ++ flatten4leaf r

flatten4node              :: Tree4 a b -> [b]
flatten4node (Leaf4 n)       = []
flatten4node (Node4 l n r) = flatten4node l ++ [n] ++ flatten4node r

occurs5 :: Eq a => a -> Tree5 a -> Bool
occurs5 m (Node5 n xs) = m == n || (or [occurs5 m x | x <- xs])

flatten5 :: Tree5 a -> [a]
flatten5 (Node5 n []) = [n]
flatten5 (Node5 n xs) = n : concat [flatten5 x | x <- xs]

{--

 1) Using recursion and the function add, define a  
 function that multiplies two natural numbers.

--}

recurMult :: Int -> Int -> Int
recurMult 0 _ = 0    
recurMult _ 0 = 0
recurMult m n = m + recurMult m (n-1)


{-- 
 Define a suitable function fold for expressions,  
 and give a few examples of its use.
--}

data Expr = Val Int | Add Expr Expr | Mul Expr Expr
            deriving (Show)

eval :: Expr -> Int
eval (Val n)   = n
eval (Add x y) = eval x + eval y
eval (Mul x y) = eval x * eval y


{--
Example Usage:
*Main> eval(Add (Val 5) (Val 3))
8
*Main> eval(Mul (Val 5) (Val 3))
15
*Main> eval(Mul (Val 5) (Val 3999999))
19999995
--}

{-- 
 A binary tree is complete if the two sub-­trees of  
 every node are of equal size.
 Define a function that decides if a binary tree is complete.
    Ex: > isComplete (Node (Node (Node (Leaf 3) 1 (Leaf 2)) 3 (Leaf 4)) 2 (Leaf 1))
        > False
        > isComplete (Node (Node (Leaf 1) 3 (Leaf 2)) 2 (Node (Leaf 1) 3 (Leaf 2)))
        > True

--}

isComplete :: Tree1 -> Bool
isComplete (Leaf1 n)     = True
isComplete (Node1 l n r) = size l == size r && isComplete l && isComplete r

size :: Tree1 -> Int
size (Leaf1 n)     = 1
size (Node1 l n r) = 1 + size l + size r 

{--
Test: True
--}
ex1 :: Tree1
ex1 = Node1 (Node1 (Leaf1 1) 3 (Leaf1 2)) 5 (Node1 (Leaf1 3) 7 (Leaf1 4))

{--
Test: False
--}
ex12 :: Tree1
ex12 = Node1 (Node1 (Leaf1 1) 3 (Leaf1 2)) 5 (Node1 (Node1 (Leaf1 10) 12 (Leaf1 23)) 7 (Leaf1 4))



-- The standard library defines 
--               data Ordering  =  LT | EQ | GT  
-- together with a function
--               compare  ::  Ord  a  => a -> a -> Ordering  
-- that decides if one value in an ordered type is less than (LT),  
-- equal to (EQ), or greater than (GT) another such value.
-- Using this function, redefine the function
--               occurs  ::  Int  -> Tree -> Bool  
-- for search trees.
-- Why is this new definition more efficient than the original version?  
  
--    Because with search tree we don't have to search the whole tree!

occurs :: Int -> Tree1 -> Bool
occurs m (Leaf1 n)     = compare m n == EQ
occurs m (Node1 l n r)  | compare m n == EQ = True
                        | compare m n == LT = occurs m l
                        | compare m n == GT = occurs m r



-- Consider the following type of binary trees:  
--         data Tree = Leaf Int | Node Tree Tree  
--  Let us say that such a tree is balanced if the number of leaves  
--  in the left and right subtree of every node differs by at most one,  
--  with leaves themselves being trivially balanced. Define a function  
--         balanced :: Tree -> Bool 
--  that decides if a tree is balanced or not.  
--  Hint: first define a function that returns the number of leaves in  
--  a tree.
 
data Tree6 = Leaf6 Int | Node6 Tree6 Tree6

balanced :: Tree6 -> Bool
balanced (Leaf6 n)   = True
balanced (Node6 l r) = abs (numLeaves l - numLeaves r) <= 1 && balanced l && balanced r


numLeaves :: Tree6 -> Int
numLeaves (Leaf6 n)   = 1
numLeaves (Node6 l r) = numLeaves l + numLeaves r

--Tree for tests 
-- Case for True
tree61 :: Tree6 
tree61 = Node6 (Node6 (Leaf6 1) (Leaf6 2))  (Node6 (Node6 (Leaf6 10)  (Leaf6 23)) (Leaf6 4))
-- Case for False
tree62 :: Tree6 
tree62 = Node6 (Node6 (Leaf6 1) (Leaf6 2))  (Node6 (Node6 (Leaf6 10)  (Leaf6 23)) (Node6 (Leaf6 2) (Leaf6 300)))


-- Define a function  
--        balance :: [Int] -> Tree  
-- that converts a non-­empty list of integers into a balanced tree.  
-- Hint: first define a function that splits a list into two halves  
-- whose length differs by at most one.  

balance :: [Int] -> Tree1
balance [x] = Leaf1 x
balance xs  = (Node1 (balance (fst (split (tail xs)))) (head xs) (balance (snd (split (tail xs)))))

split   :: [Int] -> ([Int],[Int])
split xs = (take n xs, drop n xs)
     where 
        n = div (length xs) 2
