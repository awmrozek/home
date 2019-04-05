function y = average(x)
if ~isvector(x)
	error('not a vector')
end

y = sum(x)/length(x);
end