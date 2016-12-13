import Data.Char

data Token = Num Double
           | Opar
           | Cpar
           | Plus
           | Minus
           | Mult
           | Div
           | EOF
           deriving (Show, Eq)

lexer           :: String -> [Token]
lexer []        = [EOF]
lexer (x : xs)  | x == '('          = Opar  : lexer xs
                | x == ')'          = Cpar  : lexer xs
                | x == '+'          = Plus  : lexer xs
                | x == '-'          = Minus : lexer xs
                | x == '*'          = Mult  : lexer xs
                | x == '/'          = Div   : lexer xs
                | isSpace x         =         lexer xs
                | isDigit x         = lexerNum x xs 
                | otherwise         = error $ "carattere inaspettato " ++ show x  

lexerNum     :: Char -> String -> [Token]
lexerNum n ns = let (number, tokens) = span (\x->isDigit x || x=='.') ns
                in Num (read (n:number)) : lexer tokens

-- parser--

data Tree = SumNode Token Tree Tree
          | ProdNode Token Tree Tree
          | UnaryNode Token Tree
          | NumNode Double
    deriving Show

match :: [Token] -> Token
match [] = EOF
match (c:cs) = c

move :: [Token] -> [Token]
move [] = error "Stringa finita"
move (t:ts) = ts

expression :: [Token] -> (Tree, [Token])
expression token = 
   let (termTree, termToken) = term token
   in
      case match termToken of
         Plus -> 
            let (expTree, expToken) = expression (move termToken) 
            in (SumNode Plus termTree expTree, expToken)
         Minus -> 
            let (expTree, expToken) = expression (move termToken) 
            in (SumNode Minus termTree expTree, expToken)
         _ -> (termTree, termToken)


term :: [Token] -> (Tree, [Token])
term token = 
   let (factorTree, factorToken) = factor token
   in
      case match factorToken of
         Mult ->
            let (termTree, termToken) = term (move factorToken) 
            in (ProdNode Mult factorTree termTree, termToken)
         Div ->
            let (termTree, termToken) = term (move factorToken) 
            in (ProdNode Div factorTree termTree, termToken)
         _ -> (factorTree, factorToken)

factor :: [Token] -> (Tree, [Token])
factor token = 
   case match token of
      (Num x)     -> (NumNode x, move token)
      (Plus)      -> 
            let (factorTree, factorToken) = factor (move token) 
            in (UnaryNode Plus factorTree, factorToken)
      (Minus)     -> 
            let (factorTree, factorToken) = factor (move token) 
            in (UnaryNode Minus factorTree, factorToken)
      (Opar)      -> 
         let (expTree, expToken) = expression (move token)
         in
            if match expToken /= Cpar 
            then error "Missing right parenthesis"
            else (expTree, move expToken)
      _           -> error $ "Parse error on token: " ++ show token
 
parse :: [Token] -> Tree
parse token = let (tree, expToken) = expression token
             in
               if expToken == [EOF] 
               then tree
               else error $ "Leftover tokens: " ++ show expToken


---- evaluator ----

evaluate :: Tree -> Double
evaluate (SumNode op left right) = 
    let lft = evaluate left
        rgt = evaluate right
    in
        case op of
          Plus  -> lft + rgt
          Minus -> lft - rgt

evaluate (ProdNode op left right) = 
    let lft = evaluate left
        rgt = evaluate right
    in
        case op of
          Mult -> lft * rgt
          Div   -> lft / rgt

evaluate (UnaryNode op tree) =
    let x = evaluate tree 
    in case op of
         Plus  -> x
         Minus -> -x

evaluate (NumNode x) = x


lppParser :: String -> Double
lppParser x = evaluate . parse . lexer $ x




