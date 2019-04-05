%Program1 Bisection method
%Input: function handle f;
%Output: Approximate solution xc
function xc = bisection1(a,b,tol)
if (~(sign(sign(f(a))*sign(f(b))))) < 0
	error('f(a)f(b) < 0 not satisfied!') %cease execution
end
fa=f(a);
fb=f(b);
while (b-a)/2>tol
	c=(a+b)/2;
	fc=f(c);
	if fc == 0 % c is solution, done
	break
end
if (sign(fc)*sign(fa) < 0)
	b=c; fb=fc;
else
	a=c;fa=fc;
end
xc = (a+b)/2;
end