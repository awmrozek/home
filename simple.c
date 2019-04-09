#include <stdbool.h>
//#include "frac.c"



//TODO make sure that denominator is alsways positive!

typedef struct frac {
	int numerator;
	int denominator;
} frac;


void printq(frac q) {
	printf("%d/%d\n",q.numerator,q.denominator);
}

int properMod(int a, int n) {
//	printf ("a = %d n = %d\n", a, n);
	if(a >= 0) return a % abs(n);
	return properMod(a+((abs(a/n))+1)*abs(n),abs(n));
}

int abs(int a) {
	if(a < 0) return -a;
	return a;
}

frac fabs(frac a) {
	frac p = a;
	p.numerator = abs(p.numerator);
	p.denominator = abs(p.denominator);

	return p;
}

int gcd(int a, int b) {
	if(abs(a) > abs(b)) return gcd(b,a);
	if(a == 0 && b == 0) return 1;
	if(a == 0) return b;
	return gcd(properMod(b,a),a);
}

frac reduce(frac a) {
	int f = gcd(a.numerator,a.denominator);
	frac res;
	res.numerator = a.numerator/f;
	res.denominator = a.denominator/f;
	return res;
}

frac addq(frac a, frac b) {
	frac res;
	res.numerator = a.numerator*b.denominator + b.numerator*a.denominator;
	res.denominator = a.denominator*b.denominator;
	return reduce(res);
}

frac mulq(frac a, frac b) {
	frac res;
	res.numerator = a.numerator*b.numerator;
	res.denominator = a.denominator*b.denominator;
	return reduce(res);
}

frac minusq(frac a) {
	frac res;
	res.numerator = -a.numerator;
	res.denominator = a.denominator;
	return res;
}

frac invq(frac a) {
	frac res;
	res.numerator = a.denominator;
	res.denominator = a.numerator;
	return res;
}

frac subq(frac a, frac b) {
	return addq(a,minusq(b));
}

// Hack
frac fixsgn(frac a) {
	if (a.denominator < 0) {
		a.numerator *= (-1);
		a.denominator *= (-1);
	}
	return a;
}

frac divq(frac a, frac b) {
	return fixsgn(mulq(a,invq(b)));
}

bool fm_chd(size_t rows, size_t cols, frac a[rows][cols], frac c[rows]);

bool fm(size_t rows, size_t cols, signed char a[rows][cols], signed char c[rows])
{
	frac aNew[rows][cols];
	frac cNew[rows];

	int i;
	int j;
	for(i = 0; i < rows; ++i) {
		for(j = 0; j < cols; ++j) {
			frac temp = {a[i][j],1};
			aNew[i][j] = temp;
		}
		frac temp = {c[i],1};
		cNew[i] = temp;
	}
	return fm_chd(rows,cols,aNew,cNew);
}

bool fm_chd(size_t rows, size_t cols, frac a[rows][cols], frac c[rows])
{
	printf("%d x %d matrix\n", rows, cols);
	int i;
	int j;

	for (i=0; i < rows; ++i) {
		for (j=0; j<cols; j++) {
			printf(" ");
			printq(a[i][j]);
		}
		printf("\n");
	}
	printf("\n");

	printf("c = \n");
	for (i=0; i < rows; ++i) {
		printq(c[i]);
		//printf("\n", c[i]); //Question: When does c[i] disappear?
		printf("\n");
	}

	printf("\n");

	if(rows == 0) {
		printf("Solution exists, ran out of rows!\n");
		return 1;
	} else if (cols == 0) {
		int res = 1;
		printf("Got 0 x n matrix.\n");
		for(i = 0; i < rows; i++) {
			printq(c[i]);
			if(c[i].numerator < 0) res = 0;
		}
		return res;
	} else {
		int sel = 0;	//IF THIS IS CHANGED. CODE BELOW DOESNT WORK. SEE BELOW
		int nPos = 0;
		int nNeg = 0;
		int nZer = 0;
		for(i = 0; i < rows; i++) {
			frac aa = a[i][sel];
			if (aa.numerator > 0) {++nPos;} else if (aa.numerator < 0) {++nNeg;} else {++nZer;}
		}

		frac aPos[nPos][cols];
		frac aNeg[nNeg][cols];
		frac aZer[nZer][cols];

		frac cPos[nPos];
		frac cNeg[nPos];
		frac cZer[nZer];

		int iPos = 0;
		int iNeg = 0;
		int iZer = 0;
		for(i = 0; i < rows; i++) {
			frac aa = a[i][sel];
			if(aa.numerator > 0) {
				for(j = 0; j < cols; j++) {
					aPos[iPos][j] = a[i][j];
				}
				cPos[iPos] = c[i];
				++iPos;
			} else if(aa.numerator < 0) {
				for(j = 0; j < cols; j++) {
					aNeg[iNeg][j] = a[i][j];
				}
				cNeg[iNeg] = c[i];
				++iNeg;
			} else {
				for(j = 0; j < cols; j++) {
					aZer[iZer][j] = a[i][j];
				}
				cZer[iZer] = c[i];
				++iZer;
			}
		}

		for (iPos = 0; iPos < nPos; ++iPos) {
			frac ddiv = fabs(aPos[iPos][sel]);
			for (j = 0; j < cols; ++j) {
				aPos[iPos][j] = divq(aPos[iPos][j], ddiv);
			}
			cPos[iPos] = divq(cPos[iPos],ddiv);
		}

		for (iNeg = 0; iNeg < nNeg; ++iNeg) {
			frac ddiv = fabs(aNeg[iNeg][sel]);
			for (j = 0; j < cols; ++j) {
				aNeg[iNeg][j] = divq(aNeg[iNeg][j], ddiv);
			}
			cNeg[iNeg] = divq(cNeg[iNeg],ddiv);
		}

		size_t newRows = nPos*nNeg + nZer;
		size_t newCols = cols-1;

		frac newA[newRows][newCols];
		frac newC[newRows];

		i = 0;

		for(iPos = 0; iPos < nPos; ++iPos) {
			for(iNeg = 0; iNeg < nNeg; ++iNeg) {
				for(j = 0; j < newCols; j++) {
					newA[i][j] = addq(aPos[iPos][j+1], aNeg[iNeg][j+1]); //THIS HAS TO BE CHANGED IF sel IS CHANGED
				}
				++i;
				newC[i] = addq(cPos[iPos], cNeg[iNeg]);
			}
		}

		for(iZer = 0; iZer < nZer; ++iZer) {
			for(j = 0; j < newCols; j++) {
				newA[i][j] = aZer[iZer][j+1];
			}
			newC[i] = cZer[iZer];
			++i;
		}

		return fm_chd(newRows,newCols,newA,newC);

	}
	return 0;
}
