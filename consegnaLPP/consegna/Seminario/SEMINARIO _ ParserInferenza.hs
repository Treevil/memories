import Data.Char

main = loop

loop :: IO()
loop  = do
          putStr "\nInserisci un'espressione: "
          s <- getLine
          if null s
          then return ()
          else 
               let tokens = lexer s
                   tree = parse tokens 
                   val = evaluate tree
               in do
                     putStrLn $ "\nString iniziale: " ++ show s ++
                                "\n\nToken: " ++ show tokens ++
                                "\n\nAlbero: " ++ show tree ++ 
                                "\n\nRisultato inscatolato: " ++ show val ++ "\n" 
                     putStr "Risultato estratto: "
                     case val of
                          Left boolvar    -> print $ boolvar
                          Right doublevar -> print $ doublevar
                     loop 

-- struttura che tiene solo i valori essenziali da parsificare--
data Token = Num Double
           | Boolean Bool
           | Opar
           | Cpar
           | Plus
           | Minus
           | Mult
           | Div
           | Eq
           | Gt
           | Ge
           | Lt
           | Le
           | Ne
           | If
           | Then
           | Else
           | And
           | Or
           | EOF
           deriving (Show, Eq)

-- struttura dati che controlla il tipo nel parser--
data Type = Bool
          | Nat
          deriving (Show, Eq)

--lexer: per ogni carattere riconosciuto della grammatica 
--       restituisce il token corrispondente altrimenti
--       segnala un errore. I numeri possono essere decimali 
--       con il punto
lexer           :: String -> [Token]
lexer []        = [EOF]
lexer ('(': xs)                   = Opar  : lexer xs
lexer (')': xs)                   = Cpar  : lexer xs
lexer ('+': xs)                   = Plus  : lexer xs
lexer ('-': xs)                   = Minus : lexer xs
lexer ('*': xs)                   = Mult  : lexer xs
lexer ('/': xs)                   = Div   : lexer xs
lexer ('=':'=': xs)               = Eq    : lexer xs
lexer ('>':'=': xs)               = Ge    : lexer xs
lexer ('>': xs)                   = Gt    : lexer xs
lexer ('<':'=': xs)               = Le    : lexer xs
lexer ('<':'>': xs)               = Ne    : lexer xs
lexer ('<': xs)                   = Lt    : lexer xs
lexer ('|':'|': xs)               = Or    : lexer xs
lexer ('&':'&': xs)               = And   : lexer xs
lexer (x:xs) | isAlpha x          = lexerWord (x:xs)
lexer (x:xs) | isSpace x          =         lexer xs
lexer (x:xs) | isDigit x          = lexerNum x xs 
lexer (x:xs)                      = error $ "carattere scorretto: " ++ show x  

lexerNum     :: Char -> String -> [Token]
lexerNum n ns = let (number, tokens) = span (\x->isDigit x || x=='.') ns
                in Num (read (n:number)) : lexer tokens

lexerWord (c:cs) = let (word, tokens) = span isAlphaNum (c:cs)
                   in case word of
                        "if"      -> If              : lexer tokens
                        "then"    -> Then            : lexer tokens
                        "else"    -> Else            : lexer tokens
                        "true"    -> (Boolean True)  : lexer tokens
                        "false"   -> (Boolean False) : lexer tokens
                        _         -> error $ "parola sconosciuta: " ++ show word


{-- grammatica--

Stmt        ::= if <OrExp> then <Stmt> else <Stmt> EOF
             |  <OrExp> EOF
OrExp       ::= <AndExp> || <OrExp>
             |  <AndExp>
AndExp      ::= <RelExp> && <AndExp>
             |  <RelExp>
RelExp      ::= <Expression> == <Expression>
             |  <Expression> <> <Expression>
             |  <Expression> <= <Expression>
             |  <Expression> >= <Expression>
             |  <Expression> <  <Expression>
             |  <Expression> >  <Expression>
             |  <Expression>
Expression  ::= <Term> + <Expression> 
             |  <Term> - <Expression>
             |  <Term>
Term        ::= <Factor> * <Term>
             |  <Factor> / <Term>
             |  <Factor>
Factor      ::= <Number>
             |  + <Factor> 
             |  - <Factor> 
             |  '(' <Expression> ')' 
             |  true
             |  false
--}

-- struttuta dati che permette di creare l'albero di parsificazione 
data Tree = IfNode Tree Tree Tree
          | OrNode Tree Tree
          | AndNode Tree Tree
          | RelNode Token Tree Tree 
          | SumNode Token Tree Tree
          | ProdNode Token Tree Tree
          | SignedNum Token Tree
          | NumLeaf Double
          | BoolLeaf Bool 
    deriving Show


-- controlla che il token passato sia uguale a quello in testa. 
match :: Token -> [Token] -> [Token]
match x (c:cs) = if x == c
                 then cs
                 else error $ "errore nel match di " ++ show c

-- scorre la lista (come tail)
move :: [Token] -> [Token]
move [] = error "Stringa finita!"
move (t:ts) = ts

-- funzioni che rappresentano le riscritture della grammatica.
-- ognuna permette di leggere una determinata espressione (vedi grammatica).
-- viene ritornato l'albero di parsificazione otteuto fino a quel momento, 
-- il tipo di tutto l'albero (necessario per controllare i tipi nelle espressioni) 
-- e i token ancora da analizzare. 
stmt :: [Token] -> (Tree, Type, [Token])
stmt token = 
     case head token of
         If    -> 
             let (orExpTree, orExpType, orExpToken) = orExp (move token)      
             in 
                if orExpType /= Bool
                then error "errore nel tipo della condizione if!"
                else let orExpToken' = match Then orExpToken
                         (stmtTree1, stmtType1, stmtToken1) = stmt orExpToken'
                     in
                        let stmtToken1' = match Else stmtToken1
                            (stmtTree2, stmtType2, stmtToken2) = stmt stmtToken1'
                        in 
                           if stmtType1 == stmtType2
                           then (IfNode orExpTree stmtTree1 stmtTree2, stmtType1, stmtToken2)
                           else error "tipi differenti nei rami!"
         _     -> 
             let (orExpTree, orExpType, orExpToken) = orExp token        
             in  (orExpTree, orExpType, orExpToken)

orExp :: [Token] -> (Tree, Type, [Token])
orExp token = 
   let (andExpTree, andExpType, andExpToken) = andExp token
   in
      case head andExpToken of
         Or  ->                                                                 
            let (orExpTree, orExpType, orExpToken) = orExp (move andExpToken) 
            in 
               if andExpType == Bool && orExpType == Bool
               then (OrNode andExpTree orExpTree, orExpType, orExpToken)
               else error "errore nei tipi OR!"
         _     -> (andExpTree, andExpType, andExpToken)  



andExp :: [Token] -> (Tree, Type, [Token])
andExp token = 
   let (relExpTree, relExpType, relExpToken) = relExp token
   in
      case head relExpToken of
         And  -> 
            let (andExpTree, andExpType, andExpToken) = andExp (move relExpToken) 
            in 
               if relExpType == Bool && andExpType == Bool
               then (AndNode relExpTree andExpTree, Bool, andExpToken)
               else error "errore nei tipi AND!"
         _     -> (relExpTree, relExpType, relExpToken)


relExp :: [Token] -> (Tree, Type, [Token])
relExp token =
   let (expTree, expType, expToken) = expression token
   in 
      case head expToken of
         Eq  ->
            let (expTree', expType', expToken') = expression (move expToken) 
            in
                if expType == expType'
                then (RelNode Eq expTree expTree', Bool, expToken')
                else error "errore nei tipi di ==!"
         Ge  ->
            let (expTree', expType', expToken') = expression (move expToken) 
            in
                if expType == Nat && expType' == Nat
                then (RelNode Ge expTree expTree', Bool, expToken')
                else error "errore nei tipi di =>!"
         Gt  ->
            let (expTree', expType', expToken') = expression (move expToken) 
            in
                if expType == Nat && expType' == Nat
                then (RelNode Gt expTree expTree', Bool, expToken')
                else error "errore nei tipi di >!"
         Le  ->
            let (expTree', expType', expToken') = expression (move expToken) 
            in
                if expType == Nat && expType' == Nat
                then (RelNode Le expTree expTree', Bool, expToken')
                else error "errore nei tipi di <=!"
         Ne  ->
            let (expTree', expType', expToken') = expression (move expToken) 
            in
                if expType == expType'
                then (RelNode Ne expTree expTree', Bool, expToken')
                else error "errore nei tipi di <>!"
         Lt  ->
            let (expTree', expType', expToken') = expression (move expToken) 
            in
                if expType == Nat && expType' == Nat
                then (RelNode Lt expTree expTree', Bool, expToken')
                else error "errore nei tipi di <!"
         _   -> (expTree, expType, expToken)
         

expression :: [Token] -> (Tree, Type, [Token])
expression token = 
   let (termTree, termType, termToken) = term token
   in
      case head termToken of
         Plus  -> 
            let (expTree, expType, expToken) = expression (move termToken) 
            in 
               if termType == Nat && expType == Nat
               then (SumNode Plus termTree expTree, Nat, expToken)
               else error "tipi scorretti nell'addizione!"
         Minus -> 
            let (expTree, expType, expToken) = expression (move termToken) 
            in
                if termType == Nat && expType == Nat
                then (SumNode Minus termTree expTree, Nat, expToken)
                else error "tipi scorretti nella sottrazione!"
         _     -> (termTree, termType, termToken)


term :: [Token] -> (Tree, Type, [Token])
term token = 
   let (factorTree, factType, factorToken) = factor token
   in
      case head factorToken of
         Mult ->
            let (termTree, termType, termToken) = term (move factorToken) 
            in if factType == Nat && termType == Nat 
               then (ProdNode Mult factorTree termTree, Nat, termToken)
               else error "tipi scorretti nella moltiplicazione!"
         Div  ->
            let (termTree, termType, termToken) = term (move factorToken) 
            in if factType == Nat && termType == Nat
               then (ProdNode Div factorTree termTree, termType, termToken)
               else error "tipi scorretti nella divisione!"
         _    -> (factorTree, factType, factorToken)

factor :: [Token] -> (Tree, Type, [Token])
factor token = 
   case head token of
      Plus             -> 
                 let (factorTree, factType, factorToken) = factor (move token) 
                 in if factType == Nat 
                    then (SignedNum Plus factorTree, factType, factorToken)
                    else error "tipo scorretto dopo il segno +!"     
      Minus            ->
                 let (factorTree, factType, factorToken) = factor (move token) 
                 in if factType == Nat
                    then (SignedNum Minus factorTree, factType, factorToken)
                    else error "tipo scorretto dopo il segno -!"      
      Opar             -> 
                 let (orExpTree, orExpType, orExpToken) = orExp (move token)
                 in
                     if head orExpToken /= Cpar 
                     then error "Parentesi mancante"
                     else (orExpTree, orExpType, move orExpToken)
      (Boolean x)      -> (BoolLeaf x, Bool, move token)
      (Num x)          -> (NumLeaf x, Nat, move token)
      _                -> error $ "Errore sul token " ++ show token


-- controlla che non ci siano più token da analizzare altrimenti errore
parse :: [Token] -> Tree
parse token = let (tree, stmtType , stmtToken) = stmt token
              in
                 if stmtToken == [EOF] 
                 then tree
                 else error $ "Errore! Token rimanenti: " ++ show stmtToken


-- evaluator scorre l'albero di parsificazione e comincia a valutare le espressioni 
-- più in profondita fino a risalire. Pattern Matching che valuta tutti i tipi di nodi

evaluate :: Tree -> Either Bool Double
evaluate (IfNode bool choose1 choose2) = 
        if evaluate bool == Left True
        then evaluate choose1
        else evaluate choose2

evaluate (OrNode left right) = 
    let Left leftTree = evaluate left
    in 
       case leftTree of 
          True   -> Left True
          False  -> 
               let rightTree = evaluate right
               in rightTree

evaluate (AndNode left right) = 
    let Left leftTree = evaluate left
    in 
       case leftTree of 
         False  -> Left False
         True   -> 
              let rightTree = evaluate right
              in rightTree

evaluate (RelNode operator left right) =
    let leftTree = evaluate left
        rightTree = evaluate right
    in
        case operator of
         Eq  -> Left (leftTree == rightTree)
         Ge  -> Left (leftTree >= rightTree)
         Gt  -> Left (leftTree >  rightTree)
         Le  -> Left (leftTree <= rightTree)
         Ne  -> Left (leftTree /= rightTree)
         Lt  -> Left (leftTree <  rightTree)
   
evaluate (SumNode operator left right) = 
    let Right leftTree = evaluate left
        Right rightTree = evaluate right
    in
        case operator of
          Plus  -> Right (leftTree + rightTree)
          Minus -> Right (leftTree - rightTree)

evaluate (ProdNode operator left right) = 
    let Right leftTree = evaluate left
        Right rightTree = evaluate right
    in
        case operator of
          Mult -> Right (leftTree * rightTree)
          Div  -> Right (leftTree / rightTree)
  
evaluate (SignedNum operator tree) =
    let Right x = evaluate tree 
    in case operator of
         Plus  -> Right x
         Minus -> Right (-x)

evaluate (NumLeaf x) = Right x

evaluate (BoolLeaf x) = Left x


-- inference controlla che l'albero di parsificazione, e quindi 
-- tutti i nodi, abbiano il giusto tipo nei vari operandi (sottonodi)  

typeOf  :: Tree -> Type
typeOf t = case t of 
             BoolLeaf _              -> Bool
             NumLeaf _               -> Nat
             SignedNum tok exp       -> 
                             let t1 = typeOf exp
                             in
                                if t1 == Nat
                                then Nat 
                                else error "1 tipo non corretto"
             ProdNode tok exp1 exp2  -> 
                             let t1 = typeOf exp1
                             in
                                let t2 = typeOf exp2
                                in 
                                   if t1 == Nat && t2 == Nat
                                   then Nat
                                   else error "2 tipo non corretto"
             SumNode tok exp1 exp2   -> 
                             let t1 = typeOf exp1
                             in
                                let t2 = typeOf exp2
                                in 
                                   if t1 == Nat && t2 == Nat
                                   then Nat 
                                   else error "3 tipo non corretto"
             RelNode tok exp1 exp2   -> 
                             let t1 = typeOf exp1
                             in
                                let t2 = typeOf exp2
                                in 
                                   case tok of
                                     Eq      -> 
                                            if t1 == t2
                                            then Bool 
                                            else error "4 tipo non corretto"
                                     Ne      -> 
                                            if t1 == t2
                                            then Bool 
                                            else error "5 tipo non corretto"
                                     _       -> 
                                            if t1 == Nat && t2 == Nat 
                                            then Bool
                                            else error "6 tipo non corretto"
             AndNode exp1 exp2       -> 
                             let t1 = typeOf exp1
                             in
                                let t2 = typeOf exp2
                                in 
                                    if t1 == Bool && t2 == Bool
                                    then Bool 
                                    else error "7 tipo non corretto"
             OrNode exp1 exp2        -> 
                             let t1 = typeOf exp1
                             in
                                let t2 = typeOf exp2
                                in 
                                    if t1 == Bool && t2 == Bool
                                    then Bool 
                                    else error "8 tipo non corretto"
             IfNode exp1 exp2 exp3   -> 
                             let t1 = typeOf exp1
                             in
                                let t2 = typeOf exp2
                                in 
                                   let t3 = typeOf exp3
                                   in 
                                       if t1 == Bool && t2 == t3
                                       then t2 
                                       else error "9 tipo non corretto"
