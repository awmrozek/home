from math import exp

def f(x):
	return x*x * exp (-0.1 * x)

x = [5, 10, 15, 20, 25, 30];
for t in x:
	print t, f(t)
