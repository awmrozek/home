% Book on page 32
function xc = myfpi2(x0, k)
x(1) = x0;
for i = 1:k
	x(i+1)=f(x(i));
end
xc=x(k+1);
end