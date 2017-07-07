module Slide9 where



{--  
Slide 27
Define  an  action readLine  ::  IO String  
 that behaves in the same way as getLine, except that it also permits  
 the delete key to be used to remove characters.  
 Hint: the delete character is ’\DEL’, and the control string for  
 moving the cursor back one character is "\ESC[1D".  
--}

readLine :: IO String
readLine = do x <- getChar
              case x of
                '\n'   ->  return []
                '\DEL' ->  do putStr "\ESC[1D"
                              xs <- readLine
                              return xs
                _      ->  do xs <- readLine
                              return (x:xs)

{-- 
Slide 24
 Implement  a  for  loop:  i is  the  initial  value  of  the  counter,  p  
 is  the  predicate  to  stop  the  loop,  f  is  the  function  to  
 increment  the  counter  and  job  is  the  job  to  be  repeated
 after  applying  it  to  the  current  value  of  the  counter.
--}
for :: Int -> (Int -> Bool) -> (Int -> Int) -> (Int -> IO ()) -> IO ()
for i p f job = if (p i)
                then do job i
                        for (f i) p f job
                else return ()
{--
Slide 25 
 Definire la funzione mapIO che esegue il risultato  
 dell'applicazione di una funzione che ha come  
 codominio un'azione
 Ad  esempio  
 >  mapIO  print  [1,2]
        1
        2
 >  
 definire mapIO usando seqn
--}
seqn       :: [IO a] -> IO ()
seqn []     = return ()
seqn (a:as) = do a
                 seqn as

mapIO     :: (a -> IO ()) -> [a] -> IO ()
mapIO f xs = seqn (map f xs) 