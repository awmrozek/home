f = @(x) x.^4 - x.^3 - 10;
% Bisection method to find zeroes!
a = 2;
b = 3;
m = 0;
counter = 0;
tol = 10^(-10);
while abs(a-b) >= tol;
	counter = counter + 1;
	m = (a+b)/2;
	if feval(f, a) * feval(f, m) > 0;
	a = m;
	else
	b = m;
	end
end
counter
fval = a
