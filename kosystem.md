[kaf1] Kosystem anteckningar file 1
===================================

[01] Lab1 H1

P0 l = P1 mu
P1 l = P2 mu
P2 l = P3 mu
...

Pk = ro^k(1-ro)
(3) = Int(k*ro^(k-1)) = ro^k => d(ro*k) = k*ro^(k-1)
(4) = Sum(ro^k) = 1/(1-ro)
(1) + (2) => E(k) = Sum(k*ro^k(1-ro)) = (3) = Sum(d(ro^k)*ro(1-ro)) = ro(1-ro)*d(Sum(ro^k)) = ro(1-ro)*d(1/(1-ro)) = ro/(1-ro)

[03]
Pk = ro^k * P0
Sum(p) = 1
E(X)=Sum(Pk*k)=Sum(ro^k(1-ro))
P0=(1/(Sum(Pk))) = 1 - ro => Pk = ro^k(1-ro)

\begin{math}
$$
P_0 \lambda = P_1 \mu \Rightarrow P_1 = \ro P_0
P_1 \lambda = P_2 \mu \Rightarrow P_2 = \ro^2 P_0
P_2 \lambda = P_3 \mu \Rightarrow P_3 = \ro^3 P_0
$$
\end{math}
