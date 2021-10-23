module Lab2 where

{--
	
	Esercitazione 2
--}

{-- 
	Es. 1
	Scrivere una funzione "ord1" che restituisce True se la 
	lista a cui viene applicata e' ordinata

--}
ord1 :: Ord a => [a] -> Bool
ord1 [] = True
ord1 (x:[]) = True
ord1 (x:xs) = if x <= head xs then ord1 xs else False


{-- 
	Es. 2a
	Scrivere una funzione “sottolisteF” che realizza la seguente funzionalità
	- parametro: un lista l
	- risultato: la lista di tutte le sottoliste finali di l
--}
sottolisteF     :: [a] -> [[a]]
sottolisteF []      = [[]]
sottolisteF (x:[])  = [[]]
sottolisteF (x:xs)  = [xs] ++ sottolisteF xs

{-- 
	Es. 2b
	Scrivere una funzione “sottolisteI” che realizza la seguente funzionalità
	- parametro: un lista l
	- risultato: la lista di tutte le sottoliste iniziali di l
--}

sottolisteI :: [a] -> [[a]]
sottolisteI [] = []
sottolisteI xs = xs : sottolisteI (init xs)

{-- 
	Es. 3
	Scrivere una funzione “sottostringa” che realizza la seguente funzionalità:
	- parametri: due stringhe s e l
	- risultato: la posizione della prima occorrenza della sottostringa s in l
	(la prima posizione e' la posizione 0).
	Se s non compare in l la funzione restituisce -1.
	Testare la funzione sugli argomenti “bba” e “bbbbbbba”.
--}

sottostringa          :: (Num a, Eq b) => [b] -> [b] -> a
sottostringa (x:xs) (y:ys) = sottostringa (x:xs) (y:ys) 0
    where
       sottostringa (x:xs) (y:ys) n | length (x:xs)>length (y:ys)               = -1
                                    | and[x==y | (x,y) <- zip (x:xs) (y:ys)]    = n
                                    | otherwise                                 = sottostringa (x:xs) ys (n+1)

-- *Lab2> sottostringa "bba" "bbbbbbba"
-- 5

{-- 
	Es. 4
	Scrivere una funzione "parole" che realizza la seguente specifica:
	- parametro: una stringa s contenete caratteri e spazi
	- risultato: la lista di tutte le "parole" presenti in s, cioè le sequenze di caratteri contigui che occorrono in s senza spazi intermedi.
	esempio: parole "domani non si lavora" = ["domani","non","si","lavora"]

--}

parole        :: [Char] -> [[Char]]
parole (x:xs) = parolesup (x:xs) ' '
    where
        parolesup [] delim = [""]
        parolesup (x:xs) delim  | x == delim      = "" : rest
                                | otherwise       = (x : head rest) : tail rest
                                         where
                                             rest = parolesup xs delim

{--
	es. 5
	Scrivere una funzione "uniocc" che realizza la seguente specifica:
	- parametro: un lista l
	- risultato: la lista di tutti gli elementi di l che occorrono UNA VOLTA SOLA in l
--}

uniocc      :: Eq a => [a] -> [a]
uniocc []   = []
uniocc (x:xs)   | or [ x==y | y <- xs ]     = uniocc ( remove x xs )
                | otherwise                 = x : uniocc xs
            where
                remove a []     = []
                remove a (x:xs) | a==x      = remove a xs
                                | otherwise = x : remove a xs 

{-- Es. 6

	Scrivere una funzione infissa “prodM” che realizza il prodotto di due matrici di numeri rappresentate 
	come liste di liste. La funzione deve verificare che le due matrici (cioè le loro rappresentazioni)
	siano ben formate e le loro dimensioni compatibili.

	Una matrice è ben formata se la lista di liste che la rappresenta
	soddisfa alla seguente condizioni
	Per ogni l in ls (la lunghezza di l è c) per un c in N
--}

prodM     :: Num a => [[a]] -> [[a]] -> [[a]]
prodM x y | numcol x /= numrows y   = [[]]
          | otherwise           = [ [ sum (zipWith (*) xs ys) | ys <- rotate y ] | xs <- x ]
                        where
                            rotate []   = []
                            rotate ([]:_)   = []
                            rotate xs   = map head xs : rotate (map tail xs)
 

rotate :: [[a]] -> [[a]]
rotate []   = []
rotate ([]:_)   = []
rotate xs   = map head xs : rotate (map tail xs)

numrows  :: Num a => [[a]] -> Int
numrows x = length x
 
numcol    :: Num a => [[a]] -> Int
numcol x = length (head x)

{--
Es. 7
 Il determinante di una matrice quadrata si calcola come segue
 se la matrice è 2x2: sia [[a,b],[c,d]] la matrice A. Allora il determinante di A, det(A), è ad - bc
 altrimenti, se la matrice A è n x n, sia i una riga qualsiasi. Allora il determinante è dato da questa formula:
 sommatoria di j da 1 a n (C_i,j * a_i,j)
 Dove a_i,j è il determinante della matrice ottenuta da A eliminando la riga i-esima e la colonna j-esima, 
 e C_i,j è il complemento
 algebrico di i e j, ovvero (-1)^(i+j)

 --}

deter       :: Num a => [[a]] -> a
deter [[x]]     = x
deter x     | numrows x /= numcol x   = 0
            | otherwise       = sum [((-1)^j)*((head x) !! j)*(deter (modify x j)) | j <- [0..n-1] ]
              where
                n = length (head x)
 
modify    :: [[a]] -> Int -> [[a]]                         
modify x i = removecol (tail x) i
 
removecol     ::  [[a]] -> Int -> [[a]]
removecol [] _     = []
removecol (x:xs) i = [ x !! j | j <- [0..n-1], j /= 1 ] : removecol xs i
            where
                n = length x
-- Es. 8
-- a) Non può esistere una funzione che trasforma una lista in una tupla
-- perché non si possono appendere dati alle tuple
-- b) SI! Perché nella lista non dobbiamo sapere a priori quanti elementi ci saranno 
-- c) No, non esiste una singola funzione che possa determinare il tipo a priori. 

-- Es. 9
modifica :: [(String, Int)] -> Char -> [(String, Int)]
modifica xs z = [ (x,y+1) | (x,y) <- xs, any (z==) x ]

-- Es. 10
-- f = \xs -> zip (map (\(y,z) -> y) xs) (map (\(y,z) -> z) xs)
-- E' funzione identità di una lista di coppie
-- Ad esempio, f [(1,2),(3,4)], restituisce [(1,2),(3,4)] 

{-- Es. 11

	Scrivere una funzione “permCoppie” che applicata ad una lista di coppie con elementi 
	di tipo diverso restituisca la lista di tutte le possibili coppie che includono le coppie della lista iniziale.

--}

permCoppie :: [(a,b)] -> [(a,b)]
permCoppie cs = [(x,y) | x <- map fst cs, y <- map snd cs]

{-- 
	Es 12
	
12a. Scrivere una funzione “applyM” che applicata a 3 liste, 
di cui la prima contiene funzioni binarie, restituisca tutti i possibili risultati 
dell’applicazione delle funzioninella prima lista agli elementi contenuti nelle altre 2 liste, 
supposte di tipo diverso.

12b. Scrivere la funzione “applyMS” che differisce da quella dell’esercizio precedente perché 
si suppone che la seconda e la terza lista abbiamo lo stesso tipo.
--}

applyM         :: [a -> b -> c] -> [a] -> [b] -> [c]
applyM xs ys zs = [ f y z | f <- xs, y <- ys, z <- zs ]

applyMS         :: [a -> a -> c] -> [a] -> [a] -> [c]
applyMS xs ys zs = [ f y z | f <- xs, y <- ys, z <- zs ]

{-- 
	Es 13
	13. Scrivere una funzione “applyF” che applicata a 2 liste, 
	di cui la prima contiene funzioni unarie, restituisca tutti i possibili risultati ottenuti componendo 
	le funzioni della prima lista ed applicandole alla seconda lista.
--}

applyF      :: [a -> b] -> [a] -> [b]
applyF xs ys = [ f y| f <- xs, y <- ys ]

{-- 

14. 

Scrivere una funzione “composeM” che applicata a 3 liste, di cui la prima contiene funzioni binarie, 
e le altre funzioni unarie, restituisca la lista di funzioni binarie ottenuta componendo una funzione binaria
 con 2 funzioni unarie prese dalle 2 liste in tutti i modi possibili. Ad esempio se f appartiene alla prima lista
e g alla seconda lista ed h alla terza lista, la lista risultato dovrà contenere la funzione \x y -> f (gx) (h y).

--}
composeM         :: [a -> b -> c] -> [a1 -> a] -> [b1 -> b] -> [a1 -> b1 -> c]
composeM xs ys zs = [ \x y->f (g x) (h y) | f <- xs, g <- ys, h <- zs]

{-- 


15. Scrivere una funzione “composeMS” che applicata a 2 liste, di cui la prima contiene
 funzioni binarie, e la seconda funzioni unarie, restituisca la lista di funzioni unarie ottenute 
 componendo una funzione binaria con 2 funzioni unarie in tutti i modi possibili. 
 
 Ad esempio se f appartiene alla prima lista e g ed h alla seconda lista, 
 la lista risultato dovrà contenere la funzione \x -> f (gx) (h x).

--}

composeMS      :: [a -> a -> b] -> [a1 -> a] -> [a1 -> b]
composeMS xs ys = [ \x->f (g x) (h x) | f <- xs, g <- ys, h <- ys ]

{--
Es 16
Scrivere una funzione “parentesi” che applicata ad una stringa di parentesi controlli 
che sia ben formata restituendo True o False.
--}

parentesi :: [Char] -> Bool
parentesi xs = par xs 0 
        where
            par xs n | xs==[] && n==0                  = True
                     | xs/=[] && head xs==')' && n>0   = par (tail xs) (n-1)
                     | xs/=[] && head xs=='('          = par (tail xs) (n+1) 
                     | otherwise                       = False
