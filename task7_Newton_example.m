f = @(x) exp(sin(x).^3) + x.^6 - 2*x.^4 - x.^3 - 1;
y = @(x) exp(sin(x).^3) * 3 * sin(x)^2 * cos(x) + 6*x^5 - 8*x^3 - 3*x^2;

eval = 20; % 20, -20, 0.5
index = 1;

while abs(f(eval)) > 10^-6
    oldEval = eval;
    eval = eval - f(eval)/y(eval);
    oldValues(index) = eval;
    index = index + 1;
end

disp('Using Newtons method')
eval

errors = oldValues - eval;
errors = abs(errors);

for i = 1:(length(errors) - 1)
    res(i) = errors(i + 1)/(errors(i));
end

disp('... and fzero')
fzero(f,2)
% The reason for ans < 0 using the following commands is
% that fzero finds the closest solution on the X-axis
disp('There are more solutions...')
fzero(f,0.01)
fzero(f,-2)

