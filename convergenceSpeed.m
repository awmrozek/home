f = @(x) (2*x + 2)^(1/3) - x;
x0 = 1;
x1 = 2;
x2 = 0;
array = [x0 x1];
errors = [];
linerr = [];
quaderr = [];

while abs(f(x1)) > 10^-20
	x2 = x1 - f(x1)*(x1-x0)/(f(x1)-f(x0))
	array = [array, x2]
	result = x2;
	x0 = x1;
	x1 = x2;
end

errors = array - result;
errors = abs(errors);

for i = 1:length(array) - 1
	linerr = [linerr, errors(i+1)/errors(i)];
	quaderr = [quaderr, errors(i+1)/(errors(i)^2)];
end

% Q = convergence of errors, as in {linear|quad}convergenceQ
linearConvergenceQ = 
for i = 1:length(array) - 1

end

array
linerr
quaderr
