from math import exp
import matplotlib.pyplot as plt

x = xrange(0, 100)

def f(x):
	return 4.73 * x + 1.05 * x ** 2 * exp(-0.1*x)


y = []
for t in x:
	y = y + [f(t)]

markers_on = [5, 10, 15, 20, 25, 30]
plt.plot(x, y, '-gD', markevery=markers_on)
plt.plot([5, 10, 15, 20, 25, 30], [30, 83, 126, 157, 169, 190], 'ro')
plt.show()
