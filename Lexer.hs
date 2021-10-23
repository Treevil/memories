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
          | SignedNum Token Tree
          | NumLeaf Double
    deriving Show

match :: [Token] -> Token
match [] = EOF
match (c:cs) = c

move :: [Token] -> [Token]
move [] = error "Stringa finita"
move (t:ts) = ts      x

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
      (Num x)     -> (NumLeaf x, move token)
      (Plus)      -> 
            let (factorTree, factorToken) = factor (move token) 
            in (SignedNum Plus factorTree, factorToken)
      (Minus)     -> 
            let (factorTree, factorToken) = factor (move token) 
            in (SignedNum Minus factorTree, factorToken)
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
evaluate (SumNode operator left right) = 
    let leftTree = evaluate left
        rightTree = evaluate right
    in
        case operator of
          Plus  -> leftTree + rightTree
          Minus -> leftTree - rightTree

evaluate (ProdNode operator left right) = 
    let leftTree = evaluate left
        rightTree = evaluate right
    in
        case operator of
          Mult -> leftTree * rightTree
          Div  -> leftTree / rightTree

evaluate (SignedNum operator tree) =
    let x = evaluate tree 
    in case operator of
         Plus  -> x
         Minus -> -x

evaluate (NumLeaf x) = x


lppParser :: String -> Double
lppParser x = evaluate . parse . lexer $ x




