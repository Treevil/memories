module MyMonads where 

import Control.Monad
import Data.Monoid
import Control.Applicative


rank :: Char -> Maybe Int 
rank x = lookup x (zip ['2', '3', '4', '5', '6', '7', '8', '9', '0', 'J', 'Q', 'K', 'A'] [1..]) 

--diamonds (♦), clubs (♣), hearts (♥) and spades (♠)z
suit :: Char -> Maybe Int 
suit x = lookup x (zip ['D', 'C', 'H', 'S'] [1..]) 

evalpreflot :: String -> Maybe Int 
evalpreflot [] = Nothing
evalpreflot (x:[]) = rank x
evalpreflot (x:xs) = evalpreflot xs >>= (\y -> (rank x) >>= (\z -> return (y + z)  ))

-- --flush :: String -> IO()
flush :: String -> Maybe Int 
flush [] = Nothing
flush (x:[]) = suit x
flush (x:xs) = flush xs >>= (\y -> (suit x) >>= (\z -> if (y == z) then return y else return z))

-- Just 0 >>= (\ x -> if (x == 0) then fail "zero" else Just (x + 1) )

-- test :: String -> Maybe Int
-- test [] = Nothing
-- test [x] = value x
-- test (x:xs) = test xs >>= (\y -> (value x) >>= (\z -> return (y + z)))
 



-- test' :: String -> Maybe Int
-- test' xs = sequence (xs >>= (\x -> [value x])) >>= (\y -> return (sum y))