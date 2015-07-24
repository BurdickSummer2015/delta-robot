#pragma once

#ifdef BOOST_UBLAS_TYPE_CHECK
#	undef BOOST_UBLAS_TYPE_CHECK
#endif
#define BOOST_UBLAS_TYPE_CHECK 0
#ifndef _USE_MATH_DEFINES
#	define _USE_MATH_DEFINES
#endif

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <vector>
#include <stdexcept>
#include <boost/numeric/ublas/io.hpp>

//using namespace boost::numeric::ublas;
//using namespace std;



//template<typename T>
//T simpsonApproximation(double(*)(double)f, T a, T b, unsigned int N){
//	if(N % 2){
//		throw std::invalid_argument("N must be even");
//	}
//	T h = (b - a) / n;
//	T s = f(a) + f(b);
//
//	for (int i = 1; i < N; i+=2){
//		s += 4 * f(a + i * h)
//	}
//	for (int i = 2; i < N-1; i += 2){
//		s += 2 * f(a + i * h)
//	}
//}


template<typename T>
bool InvertMatrix(const boost::numeric::ublas::matrix<T> &input, boost::numeric::ublas::matrix<T> &inverse){
	using namespace boost::numeric::ublas;
	using namespace std;
	typedef permutation_matrix<std::size_t> pmatrix;

	// create a working copy of the input
	matrix<T> A(input);

	// create a permutation matrix for the LU-factorization
	pmatrix pm(A.size1());

	// perform LU-factorization
	int res = lu_factorize(A, pm);
	if (res != 0)
		return false;

	// create identity matrix of "inverse"
	identity_matrix<T> I(A.size1());
	inverse.assign(I);

	// backsubstitute to get the inverse
	lu_substitute(A, pm, inverse);

	return true;
}

//Multiplies the coefficients of two polynomials together
template<typename T>
void multiplyPolynomials(T *coeffs1, unsigned int numCoeffs1, T* coeffs2, unsigned int numCoeffs2, T* product){
	using namespace boost::numeric::ublas;
	using namespace std;
	matrix<T> big(numCoeffs2, numCoeffs1 + numCoeffs2 -1);
	matrix<T> small(1, numCoeffs2);
	for (int i = 0; i < big.size1(); i++){
		for (int j = 0; j < big.size2(); j++){
			big(i, j) = 0;
		}
		small(0, i) = coeffs2[i];
		//std::cout << coeffs2[i] << std::endl;
	}
	for (int i = 0; i < numCoeffs1; i++){
		for (int j = 0; j < numCoeffs2; j++){
			big(j, i + j) = coeffs1[i];
			//std::cout << coeffs1[i] << std::endl;
		}
	}
	/*for (int i = 0; i < big.size1(); i++){
		for (int j = 0; j < big.size2(); j++){
			std::cout << big(i, j) << std::endl;
		}
	}*/
	matrix<T> p(prec_prod(small, big));
	for (int i = 0; i < numCoeffs1+numCoeffs2-1; i++){
		product[i] = p(0, i);
		//std::cout << product[i] << std::endl;
	}

}

//Adds the coefficients of two polynomials together
template<typename T>
void sumPolynomials(T *coeffs1, unsigned int numCoeffs1, T* coeffs2, unsigned int numCoeffs2, T* product){
	unsigned int min = numCoeffs1;
	unsigned int max = numCoeffs2;
	int i = 0;
	if (min > max){ max = numCoeffs1; min = numCoeffs2;}
	for(; i < min;i++){
		product[i] = coeffs1[i] + coeffs2[i];
	}
	if(numCoeffs1 > numCoeffs2){
		for (; i < numCoeffs1; i++){
			product[i] = coeffs1[i];
		}
	}else{
		for (; i < numCoeffs2; i++){
			product[i] = coeffs2[i];
		}
	}
}
template<typename T>
void derivativePolynomial(T *coeffs, unsigned int numCoeffs, T* product){
	/*std::cout << "DERIVATIVE" << std::endl;*/
	for(int i =1; i < numCoeffs;i++){
		product[i - 1] = i*coeffs[i];
		//std::cout << coeffs[i] << " "<< i << std::endl;
	}
}




/*
	Finds the coefficients of a polynomial p(x) of degree n that fits the data, 
	p(x(i)) to y(i), in a least squares sense. The result p is a row vector of 
	length n+1 containing the polynomial coefficients in incremental powers.

	param:
		oX				x axis values
		oY				y axis values
		nDegree			polynomial degree including the constant

	return:
		coefficients of a polynomial starting at the constant coefficient and
		ending with the coefficient of power to nDegree. C++0x-compatible 
		compilers make returning locally created vectors very efficient.

*/



template<typename T>
std::vector<T> polyfit( const std::vector<T>& oX, const std::vector<T>& oY, int nDegree )
{
	using namespace boost::numeric::ublas;

	if ( oX.size() != oY.size() )
		throw std::invalid_argument( "X and Y vector sizes do not match" );

	// more intuative this way
	nDegree++;
	
	size_t nCount =  oX.size();
	matrix<T> oXMatrix( nCount, nDegree );
	matrix<T> oYMatrix( nCount, 1 );
	
	// copy y matrix
	for ( size_t i = 0; i < nCount; i++ )
	{
		oYMatrix(i, 0) = oY[i];
	}

	// create the X matrix
	for ( size_t nRow = 0; nRow < nCount; nRow++ )
	{
		T nVal = 1.0f;
		for ( int nCol = 0; nCol < nDegree; nCol++ )
		{
			oXMatrix(nRow, nCol) = nVal;
			nVal *= oX[nRow];
		}
	}

	// transpose X matrix
	matrix<T> oXtMatrix( trans(oXMatrix) );
	// multiply transposed X matrix with X matrix
	matrix<T> oXtXMatrix( prec_prod(oXtMatrix, oXMatrix) );
	// multiply transposed X matrix with Y matrix
	matrix<T> oXtYMatrix( prec_prod(oXtMatrix, oYMatrix) );

	// lu decomposition
	permutation_matrix<int> pert(oXtXMatrix.size1());
	const std::size_t singular = lu_factorize(oXtXMatrix, pert);
	// must be singular
	BOOST_ASSERT( singular == 0 );

	// backsubstitution
	lu_substitute(oXtXMatrix, pert, oXtYMatrix);

	// copy the result to coeff
	return std::vector<T>( oXtYMatrix.data().begin(), oXtYMatrix.data().end() );
}

/*
	Calculates the value of a polynomial of degree n evaluated at x. The input 
	argument pCoeff is a vector of length n+1 whose elements are the coefficients 
	in incremental powers of the polynomial to be evaluated.

	param:
		oCoeff			polynomial coefficients generated by polyfit() function
		oX				x axis values

	return:
		Fitted Y values. C++0x-compatible compilers make returning locally 
		created vectors very efficient.
*/
template<typename T>
T polyval( const std::vector<T>& oCoeff, T oX )
{
	
	size_t nDegree = oCoeff.size();
	//std::vector<T>	oY( nCount );

	/*for ( size_t i = 0; i < nCount; i++ )
	{*/
		T nY = 0;
		T nXT = 1;
		T nX = oX;
		for ( size_t j = 0; j < nDegree; j++ )
		{
			// multiply current x by a coefficient
			nY += oCoeff[j] * nXT;
			// power up the X
			nXT *= nX;
		}
	//}

	return nY;
}
