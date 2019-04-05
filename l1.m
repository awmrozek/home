str = '@(x)7*x-13';
f = str2func(str) 
f(1)

n = 10;
strF = '@(x)';

xvec = [-1:0.1:1]
yvec = f(xvec) for i = 0:n
	%strF = strF + num2str(yvec(i)h) + '*L(i)';
	strF = strcat(strF, 2)
end

strF
