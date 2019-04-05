% Newton's method
f = @(x) x/(exp(x));
y = @(x) -(x-1)*exp(-x);
eval = 2;
for i = 1:100000
	eval = eval - f(eval)/y(eval);
end

eval

