module Lab3 where 
	
import Data.Char
import Data.Ord
import System.Environment

{-- 
	Es. 1
	Scrivere una funzione printSC che prende in input una stringa e
	un carattere e stampa (con print!) i caratteri della stringa eliminando
	il carattere dato come parametro.
--}

printSC :: String -> Char -> IO ()
printSC s c = print [ch | ch <- s, (ch /= c)]

{-- 
	Es. 2
	Scrivere una funzione rF che prende in input il nome di un file, 
	legge il contenuto e lo mostra sul video.
--}
rF :: FilePath -> IO ()
rF file = do
		   s <- readFile file
		   print s

{-- 
	Es. 3
	Scrivere una funzione ioFile che prende in input i nomi di 2 files, 
	legge una stringa sul primo file e la riscrive con modificando i caratteri
	da minuscolo a maiuscolo sul secondo file. Suggerimento: usare toUpper.
--}

ioFile :: FilePath -> FilePath -> IO ()
ioFile f1 f2 = do
			    s <- readFile f1
			    writeFile f2 (upperS s)			    

upperS :: String -> String
upperS s = [toUpper c | c <- s]

{-- 
	Es. 4
	Scrivere una funzione rKwF che legge una stringa da tastiera
	e la memorizza in un file il cui nome è dato in input alla funzione.
--}
rKwF :: FilePath -> IO ()
rKwF path = do
			 print "Inserire Stringa: \n"
			 s <- getLine
			 writeFile path s			 

{-- 
	Es. 5
	Scrivere una funzione rFl che restituisce il primo elemento di una lista di
	interi memorizzata come testo in un file.
--}
rFl :: FilePath -> IO Int
rFl path = do
		s <- readFile path
		let l = read s :: [Int]
		return (head l)

{-- 
	Es. 6
	Scrivere una funzione factIO che, dopo aver scritto un messaggio di benvenuto,
	legge un numero, ne stampa il relativo fattoriale e resta in attesa di un altro 
	numero, fino a che non viene immesso un carattere particolare che termina il dialogo. 
	Il carattere di fine dialogo viene dato in input ad ogni chiamata della funzione. 
--}

factIO  :: Char -> IO()
factIO c = do
            putStrLn "Benvenuto, inserisci un numero: "
            n <- getLine
            if head n ==  c
               then return ()
            else if isNumber (head n)
               then do 
                    print (fact (read n :: Int))
                    factIO c
               else do
                    print "Non hai inserito un numero. Riprova"
                    factIO c
        where 
            fact 0 = 1
            fact m = m * fact (m-1)

{-- 
	Es. 7
	Scrivere una funzione anagrammi che legge una stringa da tastiera e
	forma i possibili anagrammi utilizzando un insieme di parole memorizzate 
	in un file.
--}
anagrammi :: IO ()
anagrammi = do putStrLn ("Insersci una parola")
               xs <- getLine
               yss <- readFile "lista.txt"
               putStrLn ("L'anagramma di " ++ xs ++ " è " ++ (check xs (read (yss))))

check :: [Char] -> [[Char]] -> [Char]
check xs xss = confrontaAnagrammi (permutations xs) xss

confrontaAnagrammi :: [[Char]] -> [[Char]] -> [Char]
confrontaAnagrammi [] _ = []
confrontaAnagrammi xss yss = if elem (head xss) yss then (head xss) else confrontaAnagrammi (tail xss) yss


permutations :: Eq a => [a] -> [[a]]
permutations [x] = [[x]]
permutations l = [x : m | x <- l, m <- permutations (remove x l)]

remove :: Eq a => a -> [a] -> [a]
remove x [] = []
remove x (y:ys) | x == y = remove x ys
			    | otherwise = y : remove x ys

