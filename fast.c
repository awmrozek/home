typedef signed long long int num;

#define bool int

typedef struct frac {
	num numerator;
	num denominator;
} frac;

num absolute(num a) {
	if(a < 0) return -a;
	return a;
}

/*num gcd(num a, num b) {
  if(abs(a) > abs(b)) {
  num c = b;
  b=a;
  a=c;
  }
  if(a == 0 && b == 0) return 1;
  if(a == 0) return b;
  return gcd(properMod(b,a),a);
  }*/


frac divq(frac A, frac B) {
	frac res;

	num a = A.numerator*B.denominator;
	num b = A.denominator*B.numerator;

	res.numerator = a;
	res.denominator = b;

	if (a == 0 || b == 0) {
		a = 1;
	} else {

		if (a < 0)
			a = -a;

		if (b < 0)
			b = -b;

		while (a != b) {
			if (a < b) {
				b -= a;
			} else {
				a -= b;
			}
		}
	}

	res.numerator = res.numerator/a;
	res.denominator = res.denominator/a;

	if (res.denominator < 0) {
		res.numerator = -res.numerator;
		res.denominator = -res.denominator;
	}

	return res;
}

bool fm_chd(size_t rows, size_t cols, frac a[rows][cols], frac c[rows]);

bool fm(size_t rows, size_t cols, signed char a[rows][cols], signed char c[rows])
{

	frac aNew[rows][cols];
	frac cNew[rows];

	size_t i;
	size_t j;
	for(i = 0; i < rows; ++i) {
		for(j = 0; j < cols; ++j) {
			frac temp = {a[i][j], 1};
			aNew[i][j] = temp;
		}
		frac temp = {c[i],1};
		cNew[i] = temp;
	}
	return fm_chd(rows,cols,aNew,cNew);
}


bool fm_chd(size_t rows, size_t cols, frac A[rows][cols], frac C[rows])
{

	size_t i;
	size_t j;
	if (rows == 0) {
		return 1;
	} else if (cols == 0) {
		int res = 1;
		for (i = 0; i < rows; i++) {
			if (C[i].numerator < 0) res = 0;
		}
		return res;
	} else {
		size_t nPos = 0;
		size_t nNeg = 0;
		size_t nZer = 0;
		for (i = 0; i < rows; i++) {
			frac aa = A[i][0];
			if (aa.numerator > 0) {++nPos;} else if (aa.numerator < 0) {++nNeg;} else {++nZer;}
		}

		size_t pos[nPos];
		size_t neg[nNeg];
		size_t zer[nZer];

		size_t iPos = 0;
		size_t iNeg = 0;
		size_t iZer = 0;
		for (i = 0; i < rows; i++) {
			frac aa = A[i][0];
			if(aa.numerator == 0) {
				zer[iZer] = i;
				++iZer;
			} else {
				frac ddiv = A[i][0];
				ddiv.numerator = absolute(ddiv.numerator);
				for (j = 0; j < cols; ++j) {
					A[i][j] = divq(A[i][j], ddiv);
				}
				C[i] = divq(C[i],ddiv);
				if (aa.numerator > 0) {
					pos[iPos] = i;
					++iPos;
				} else {
					neg[iNeg] = i;
					++iNeg;
				}
			}
		}

		size_t newRows = nPos*nNeg + nZer;
		size_t newCols = cols-1;

		frac newA[newRows][newCols];
		frac newC[newRows];

		i = 0;

		for (iPos = 0; iPos < nPos; ++iPos) {
			for (iNeg = 0; iNeg < nNeg; ++iNeg) {
				for (j = 0; j < newCols; j++) {
					frac res;
					frac a = A[pos[iPos]][j+1];
					frac b = A[neg[iNeg]][j+1];

					res.numerator = a.numerator*b.denominator + b.numerator*a.denominator;
					res.denominator = a.denominator*b.denominator;

					newA[i][j] = res; //THIS HAS TO BE CHANGED IF sel IS CHANGED
				}
				frac res;
				frac a = C[pos[iPos]];
				frac b = C[neg[iNeg]];

				res.numerator = a.numerator*b.denominator + b.numerator*a.denominator;
				res.denominator = a.denominator*b.denominator;

				newC[i] = res;
				++i;
			}
		}
		for (iZer = 0; iZer < nZer; ++iZer) {
			for (j = 0; j < newCols; j++) {
				newA[i][j] = A[zer[iZer]][j+1];
			}
			newC[i] = C[zer[iZer]];
			++i;
		}

		return fm_chd(newRows,newCols,newA,newC);

	}


}
