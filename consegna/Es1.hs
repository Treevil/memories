module Lab1 where

{-- 
	Es. 1: Algoritmo di Euclide
--}

euclide :: Int -> Int -> Int
euclide m n | m == 0 = n
		    | m < n = euclide n m
		    | otherwise = euclide (m - n) n

{-- 
	Es. 2 Elevamento a potenza
		   
		  | 1,                     se n = 0           
	a^n = | a^(n/2) * a^(n/2)      se n>0 e n è pari
		  | a * a^(n/2) * a^(n/2)  altrimenti
--}

pow :: (Num a, Integral b) => a -> b -> a
pow a n | n == 0 = 1
	    | n > 0 && even n =  p * p
	    | otherwise = a * p * p
	    where 
	    	p = (pow a (div n 2))

{-- 
	Es.3 Calcolare il K-esimo numero di Fibonacci
--}

fibonacci' :: Int -> Int -> Int -> Int
fibonacci' 0 m n = m
fibonacci' 1 m n = n
fibonacci' k m n = fibonacci' (k-1) n (m+n)

fibonacci :: Int -> Int
fibonacci k = fibonacci' k 0 1

{-- 
	Es. 4 Scrivere una funziona che calcola tutte le permutazioni degli 
		  elementi di una lista
--}

permutations :: Eq a => [a] -> [[a]]
permutations [x] = [[x]]
permutations l = [x : m | x <- l, m <- permutations (remove x l)]

remove :: Eq a => a -> [a] -> [a]
remove x [] = []
remove x (y:ys) | x == y = remove x ys
			    | otherwise = y : remove x ys