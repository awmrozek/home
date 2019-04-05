%x = -3:0.1:4;
%y = x.^4-x.^3-10;
%plot(x, y);

function p = myfpi(maxint)
p=100;
for i = 0:maxint
   p=f(p);
end
end