
-- Dimostrare che gli alberi definiti nel lucido 13 sono dei funtori.

data Tree1  a   = Leaf1 a | Node1 (Tree1 a)     (Tree1 a) 
data Tree2  a   = Leaf2   | Node2 (Tree2 a)   a (Tree2 a) 
data Tree3  a b = Leaf3 a | Node3 (Tree3 a b) b (Tree3 a b)
data Tree4  a   = Node4 a [Tree4 a]

instance Functor Tree1 where
   fmap f (Leaf1 x)           =  Leaf1(f x)
   fmap f (Node1 left right)  =  Node1 (fmap f left)  (fmap f right)

instance Functor Tree2 where
   fmap f (Leaf2)               =  Leaf2 
   fmap f (Node2 left x right)  =  Node2 (fmap f left) (f x) (fmap f right)

instance Functor (Tree3 a) where
   fmap f (Leaf3 x)             =  Leaf(f x)
   fmap f (Node3 left y right)  =  Node3 (fmap f left) (f y) (fmap f right)   

instance Functor Tree4 where
    fmap f (Node4 x xs) = Node4 (f x) [ fmap f y| y <- xs ]



-- Verifichiamole per le liste, Maybe, 
-- gli alberi, IO, Either, le funzioni.


-- Leggi

1) fmap id = id
2) fmap (f . g) = fmap f . fmap g


----------------Liste---------------

instance Functor [] where
    fmap = map

-Regola 1 

1)
fmap id [] == map id []
           == []
           == id []

2)
fmap id xs == map id xs
           == [id x | x <- xs] 
           == [x | x <- xs] 
           == xs
           == id xs

-Regola 2

1)
fmap (f.g) [] == map (f.g) []
              == []

(fmap f . fmap g) [] == (map f . map g) []
                     == map f(map g [])
                     == map f []
                     == []

2)
fmap (f.g) xs == map (f.g) xs
              == [(f.g) x | x <- xs] 
              == [f (g x) | x <- xs] 
              == [f x | x <- fmap g xs] 
              == fmap f (fmap g xs) 
              == (fmap f.fmap g) xs


----------------Maybe---------------

instance Functor Maybe where
    fmap f Nothing = Nothing
    fmap f (Just x) = Just (f x)

-Regola 1

1)
fmap id Nothing == Nothing      
                == id Nothing           

2)
fmap id (Just a) == Just (id a)      
                 == Just a                       
                 == id (Just a)             

-Regola 2

1)
fmap (f . g) Nothing == Nothing

(fmap f . fmap g) Nothing  == fmap f (fmap g Nothing)
                           == fmap f Nothing
                           == Nothing

2)

fmap (f . g) Just a == Just ((f . g) a)

(fmap f . fmap g) Just a == fmap f (fmap g (Just a))
                         == fmap f (Just (g a))
                         == Just (f (g a))
                         == Just ((f . g) a)



----------------Tree---------------

instance Functor Tree where
    fmap f EmptyTree = EmptyTree
    fmap f (Node x leftsub rightsub) = Node	 (f x) (fmap f leftsub)	(fmap f rightsub)

-Regola 1

1)
fmap id EmptyTree == EmptyTree
                  == id EmptyTree

2)
fmap id (Node x sx dx) == Node (id x) (fmap id sx) (fmap id dx) 
					   == Node x sx dx
					   == id (Node x sx dx)

-Regola 2

1)
fmap (f . g) EmptyTree == EmptyTree

(fmap f . fmap g) EmptyTree == fmap f (fmap g EmptyTree)
							== fmap f EmptyTree
							== EmptyTree

2)
fmap (f.g) (Node x sx dx) == Node ((f.g) x) (fmap (f.g) sx) (fmap (f.g)dx) 
                          == Node (f (g x)) (fmap f (fmap g sx)) (fmap f (fmap g dx)) 
                          == fmap f (Node (g x) (fmap g sx) (fmap g dx)) 
                          == (fmap f (fmap g (Node x sx dx)) 
                          == (fmap f. fmap g) (Node x sx dx)



----------------IO---------------

instance Functor IO where
    fmap f action = do result <- action
                       return (f result)

-Regola 1

fmap id action == do result <- action
                     return (id result)
               == do result <- action
                     return result
               == do return action
               == action
               == id action

-Regola 2

fmap (f . g) action == do result <- action
                          return ((f . g) result)
                    == do return ((f . g) action)

(fmap f . fmap g) action == fmap f (fmap g action)
                         == fmap f (do result <- action
                                       return (g result))
                         == do result1 <- do {result <- action
                                              return (g result)}
                               return (f result1)
                         == do result1 <- do return (g action)
                               return (f result1)
                         == do result1 <- g action
                               return (f result1)
                         == do return (f (g action))
                         == do return ((f . g) action)


----------------Either---------------

instance Functor (Either a) where
    fmap f (Right x) = Right (f x)
    fmap f (Left x) = Left x

-Regola 1

1)
fmap id (Left x) == Left (id x)
				 == Left x
                 == id (Left x)

2)
fmap id (Right x) == Right (id x)
                  == Rigth x
                  == id (Right x)

-Regola 2

1)
fmap (f . g) (Left x) == Left ((f . g) x)
                      == Left (f (g x))

(fmap f . fmap g) (Left x) == fmap f (fmap g (Left x))
                           == fmap f (Left (g x))
                           == Left (f (g x))

2)
fmap (f . g) (Right x) == Right ((f . g) x)
                       == Right (f (g x))

(fmap f . fmap g) (Right x) == fmap f (fmap g (Right x))
                            == fmap f (Right (g x))
                            == Right (f (g x))


----------------Funzioni---------------

instance Functor ((->) r) where
    fmap f h = (\x -> f(h x))

-Regola 1

fmap id h == h
          == id h

-Regola 2

fmap (f . g) h == (f . g) h
               == f . g . h

(fmap f . fmap g) h == fmap f (fmap g h)
                    == fmap f (g . h)
                    == f (g . h)
                    == f . g . h



-- Verifichiamo le leggi dei funtori applicativi per le liste, Maybe, IO e funzioni.


-- Leggi

1) pure id <*> v = v                                -- identity
2) pure (.) <*> u <*> v <*> w = u <*> (v <*> w)     -- composition
3) pure f <*> pure x = pure (f x)                   -- homomorphism
4) u <*> pure y = pure ($y) <*> u                   -- interchange
5) fmap f x = pure f <*> x                          -- fmap


----------------Liste---------------

instance Applicative [] where
    pure x = [x]
    fs <*> xs = [f x | f <- fs, x <- xs]

-Regola 1

pure id <*> xs == [(id)] <*> xs
               == [f x |f <-[(id)], x <- xs]
               == [id x | x <- xs]
               == xs

-Regola 2

pure (.) <*> us <*> vs <*> ws  == [(.)] <*> us <*> vs <*> ws
                               == [(u.)| u <- us] <*> vs <*> ws 
                               == [(u.v)| u <- us, v <- vs] <*> ws 
                               == [(u.v) w| u <- us, v <- vs, w <- ws] 
                               == [u (v w)| u <- us, v <- vs, w <- ws] 
                               == us <*> [v w | v <- vs, w <- ws] 
                               == us <*> (vs <*> ws)

-Regola 3

pure f <*> pure x == [f x | f <- [f], x <- [x]] 
                  == [f x] 
                  == pure (f x)

-Regola 4

us <*> pure y = us <*> [y]
              = [u y | u <- us, y <- [y]]
              = [u y | u <- us]

pure ($ y) <*> us == [($ y)] <*> us
                  == [($ y) u | ($ y) <- [($ y)], u <- us]
                  == [($ y) u | u <- us]
                  == [(\f -> f y) u | u <- us]
                  == [u y | u <- us]]

-Regola 5

fmap f xs == map f xs 
          == [f x| x <- xs]
          == [f x| f <- [f], x <- xs] 
          == pure f <*> xs


----------------Maybe---------------

instance Applicative Maybe where
    pure = Just
    Nothing <*> _ = Nothing
    (Just f) <*> something = fmap f something

-Regola 1 

pure id <*> Just x == fmap id (Just x) 
                   == Just (id x) 
                   == Just x

pure id <*> Nothing == fmap id Nothing 
                    == Nothing

-Regola 2

pure (.) <*> Just u <*> Just v <*> Just w == Just (u.) <*> Just v <*> Just w 
                                          == Just (u.v) <*> Just w 
                                          == Just ((u.v) w) 
                                          == Just (u (v w)) 
                                          == Just u <*> Just (v w) 
                                          == Just u <*> (Just v <*> Just w)

-Regola 3

pure f <*> pure x == Just f <*> Just x 
                  == Just (f x) 
                  == pure (f x)

-Regola 4 

Just u <*> pure y == Just u <*> Just y 
				  == fmap u (Just y)
				  == Just (u y) 
                  == Just (($y) u) 
                  == Just ($y) <*> Just u

-Regola 5

fmap f (Just x) == Just (f x) 
                == Just f <*> Just x 
                == pure f <*> Just x


----------------IO---------------

instance Applicative IO where
    pure = return
    a <*> b = do f <- a
                 x <- b
                 return (f x)

-Regola 1

pure id <*> v == return id <*> v
              == do f <- return id
                   x <- v
                   return (f x)
              == do x <- v
                   return (id x)
              == do x <- v
                   return (x)
              == do v
              == v

-Regola 2

pure (.) <*> Just u <*> Just v <*> Just w == return (.) <*> Just u <*> Just v <*> Just w
										  == Just (u.) <*> Just v <*> Just w 
                                          == Just (u.v) <*> Just w 
                                          == Just ((u.v) w) 
                                          == Just (u (v w)) 
                                          == Just u <*> Just (v w) 
                                          == Just u <*> (Just v <*> Just w)

-Regola 3

pure f <*> pure x == return f <*> return x
                  == do g <- return f
                        y <- return x
                        return (g y)
                  == do return (f x)
                  == return (f x)
                  == pure (f x)

-Regola 4

u <*> pure y == u <*> return y
             == do f <- u
                   x <- return y
                   return (f x)
             == do f <- u
                   return (f y)

pure ($ y) <*> u == return ($ y) <*> u
                 == do g <- return ($ y)
                       f <- u
                       return (g f)
                 == do f <- u
                       return (($ y) f)
                 == do f <- u
                       return (f y)

-Regola 5

fmap f x == do y <- x
              return(f y)

pure f <*> x == return f <*> x
             == do g <- return f
                   y <- x
                   return (g x)
             == do y <- x
                   return (f x)


----------------Funzioni---------------

instance Applicative ((->) r) where
    pure x = (\_ -> x)
    f <*> g = (\x -> f x (g x))

-Regola 1

pure id <*> f == (\_ -> id) <*> f 
              == \x -> (\_ -> id) x (f x) 
              == \x -> id (f x) 
              == \x -> f x 
              == f

-Regola 2

pure (.) <*> u <*> v <*> w == (\_ -> (.)) <*> u <*> v <*> w
                           == (\x -> (\_ -> (.)) x (u x)) <*> v <*> w
                           == (\x -> (.)(u x)) <*> v <*> w
                           == (\x -> (u x).) <*> v <*> w
                           == (\y -> (\x -> (u x).) y (v y)) <*> w
                           == (\y -> (u y) . (v y)) <*> w
                           == (\z -> (\y -> (u y) . (v y)) z (w z))
                           == (\z -> ((u z) . (v z)) (w z))

u <*> (v <*> w) == u <*> (\x -> v x (w x) ) 
                == (\z -> u z ((\x -> v x (w x)) z))
                == (\z -> u z (v z (w z)))
                == (\z -> ((u z) . (v z)) w z )

-Regola 3

pure f <*> pure x == (\_ -> f) <*> (\_-> x)
				  == \y -> (\_ -> f) y ((\_ -> x) y) 
				  == \y -> (\_ -> f) y x) 
				  == \y -> f x
				  == \_ -> f x
                  == pure (f x)

-Regola 4

u <*> pure y == u <*> (\_ -> y)
			 == \x -> u x ((\_ -> y) x )
			 == \x -> u x y

pure ($ y) <*> u == (\_ -> $ y) <*> u
                 == (\x -> (\_ -> $ y) x (u x))
                 == (\x -> ($ y) (u x))
                 == (\x -> (\z -> z y) (u x))
                 == (\x -> u x y)

-Regola 5

fmap f x = (\y -> f (x y))

pure f <*> x = (\_ -> f) <*> x
             = (\y -> (\_ -> f) y (x y))
             = (\y -> f (x y))



-- Verifichiamo le leggi dei monoidi per le liste, Maybe, Ordering.


-- Leggi

1) mempty `mappend` x = x
2) x `mappend` mempty = x
3) (x `mappend` y) `mappend` z = x `mappend` (y `mappend` z)


----------------Liste---------------

instance Monoid [a] where
    mempty = []
    mappend = (++)

-Regola 1

mempty `mappend` x == [] ++ x
                   == x

-Regola 2

x `mappend` mempty == x ++ []
                   == x

-Regola 3

(x `mappend` y) `mappend` z == (x ++ y) ++ z
                            == x ++ y ++ z
                            == x ++ (y ++ z)
                            == x `mappend` (y `mappend` z)


---------------Maybe---------------

instance Monoid a => Monoid (Maybe a) where
    mempty = Nothing
    Nothing `mappend` m = m
    m `mappend` Nothing = m
    (Just m1) `mappend` (Just m2) =  Just (m1 `mappend` m2)

-Regola 1

mempty `mappend` x == Nothing `mappend` x 
				   == Nothing

-Regola 2

x `mappend` mempty == x `mappend` Nothing
				   == Nothing

-Regola 3

(Just x `mappend` Just y) `mappend` Just z == Just (x `mappend` y) `mappend` Just z 
                                           == Just ((x `mappend` y) `mappend` z) 
                                           == Just (x `mappend` (y `mappend` z))

Just x `mappend` (Just y `mappend` Just z) == Just (x `mappend` (Just y `mappend` Just z)) 
										   == Just (x `mappend` (y `mappend` z))                                        



---------------Ordering---------------

instance Monoid Ordering where
    mempty = EQ
    LT `mappend` _ = LT
    EQ `mappend` y = y
    GT `mappend` _ = GT

-Regola 1

mempty `mappend` x == EQ `mappend` x 
				   == x

-Regola 2

LT `mappend` mempty == LT `mappend` EQ = LT

EQ `mappend` mempty == EQ `mappend` EQ = EQ

GT `mappend` mempty == GT `mappend` EQ = GT


-Regola 3

1) x /= EQ

(x `mappend` y) `mappend` z = x `mappend` z = x
x `mappend` (y `mappend` z) = x

2) x == EQ, y == LT

EQ `mappend` (LT `mappend` z) = EQ `mappend` LT = LT
(EQ `mappend` LT) `mappend` z = LT `mappend` z = LT

3) x == EQ, y == GT

EQ `mappend` (GT `mappend` z) = EQ `mappend` GT = GT
(EQ `mappend` GT) `mappend` z = GT `mappend` z = GT

4) x == EQ, y == EQ

(x `mappend` y) `mappend` z = y `mappend` z = z
x `mappend` (y `mappend` z) = x `mappend` z = z




-- Verificare le leggi delle monadi per le liste e Maybe


-- Leggi

1) return x >>= f     = f x                         -- left identity
2) m >>= return       = m   						-- right identity
3) (m >>= f) >>= g    = m >>= (\x -> f x >>= g) 	-- associativity



---------------Liste---------------

instance Monad [] where
    return x = [x]
    xs >>= f = concat(map f xs)
    fail _ = []

-Regola 1

return x >>= f == [x] >>= f
			   == concat (map f [x])
			   == concat [f x] 
			   == f x

-Regola 2

(x:xs) >>= return == concat (map return (x:xs))
                  == concat ((return x) : (map return xs))
                  == [x] ++ xs
                  == (x:xs)

-Regola 3

(xs >>= f) >>= g == ?

xs >>= (\x -> f x >>= g) == ?


---------------Maybe---------------

instance Monad Maybe where
    return x = Just x
    Nothing >>= f = Nothing
    Just x >>= f = f x
    fail _ = Nothing

-Regola 1

return x >>= f == Just x >>= f 
			   == f x

-Regola 2

Just x >>= return == return x
                  == Just x

-Regola 3

(Just x >>= f) >>= g == f x >>= g

Just x >>= (\x -> f x >>= g) == (\y -> f y >>= g) x
                             == f x >>= g






