A=[1 2 3;4 5 6; 1 2 3]
s = A(n,n); [Q,R] = qr(A - s*I);A = R*Q + s*I
