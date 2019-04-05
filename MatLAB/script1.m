% program 1.2 fixed point iteration
%g=@(x) cos(x)

function xc=fpi(g, x0, k);
x(1) = x0;

for i=1:k
	x(i+1)=g(x(i));
end

xc=x(k+1);
%xc=fpi(g,0,10);
