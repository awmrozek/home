% Fixed point iteration

% -----------------------------------------------------
% A)
x = 1000;
while abs((2*x + 2)^(1/3) - x) > 10^-8
   x = (2*x + 2)^(1/3);
end

x

% ------------------------------------------------------
% B)
x = 3;
while abs(log(7 - x) - x)> 10^-8
   x = log(7 - x);
end

x

% ------------------------------------------------------
% C)

x = 3;
while abs(log(4 - sin(x)) - x)> 10^-8
   x = log(4 - sin(x));
end
x

