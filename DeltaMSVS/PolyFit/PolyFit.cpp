// PolyFit.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include "polyfit.hpp"
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
//#include <boost/numeric/bindings/lapack/getsvd.hpp>

//using std::vector;
using namespace boost::numeric::ublas;



double* getCoeffs( const double* aX,const double* aY, const unsigned int numPoints,double* output){
	std::cout << numPoints<<std::endl<<std::endl;
	matrix<double> A(numPoints, numPoints);
	matrix<double> Ainv(numPoints , numPoints);
	matrix<double> B(1, numPoints);
	for (int i = 0; i < numPoints; i++){
		double x0 = aX[i];
		double x = 1;
		for (int j = 0; j < numPoints; j++){
			A(j, i) = x;
			x *= x0;
		}
		B(0, i) = aY[i];
	}
	InvertMatrix(A, Ainv);
	
	matrix<double> C(prec_prod(B, Ainv));

	for(int i =0; i < C.size2(); i++){
		output[i] = C(0, i);
		std::cout << C(0, i) << std::endl;
	}

	return output;
}

struct point{
	double x;
	double y;
	double z;
	double t;
}

/*
double* getCoeffs(double aX[], double aY[], unsigned int size, double* output){
	vector<double> oX;
	oX.insert(oX.end(), &aX[0], &aX[size]);

	vector<double> oY;
	oY.insert(oY.end(), &aY[0], &aY[size]);

	vector<double> ret = polyfit(oX, oY, oX.size() + 1);
	for (int i = 0; i < ret.size(); i++){
		output[i] = ret[i];
	}
	return output;
}
double valueAt(double * coeffs, unsigned int size, double oX){
	vector<double> oCoeffs;
	oCoeffs.insert(oCoeffs.end(), &coeffs[0], &coeffs[size]);
	return polyval(oCoeffs, oX);
}

*/
double valueAt(double * coeffs,unsigned int num, double oX){
	double nY = 0;
	double x = 1;
	for (int j = 0; j < num; j++){
		nY += coeffs[j] * x;
		x *= oX;
	}
	return nY;
}



int main()
{
	double xs[] = { 1.0, 2.0, 3.0, 4.0};
	double ys[] = { 10.0, 3.0, 4.0, 4.0 };
	unsigned int num = 4;
	double coeffs[4];
	getCoeffs(xs, ys, num, coeffs);
	for(int i= 0; i < num; i++){
		printf("%f\n, ", coeffs[i]);
	}
	printf("\n");
	std::cout << valueAt(coeffs, num,1.0)<<std::endl;
	std::cout << valueAt(coeffs, num,2.0)<<std::endl;
	std::cout << valueAt(coeffs, num,3.0)<<std::endl;
	std::cout << valueAt(coeffs, num,4.0)<<std::endl;

	
	double p1[] = { 1, 2, 3, 6, 7, 8};
	double p2[] = { 1, 2, 2, 5, 6 };
	//double p1[] = {1,2,3}
	double product[6];
	//multiplyPolynomials(p1, 4, p2, 5, product);
	//sumPolynomials(p1, 6, p2, 5, product);
	derivativePolynomial(p1, 6, product);
	for (int i = 0; i < 6; i++){
		printf("%f\n", product[i]);
	}

	while(1){
	}
	return 0;
}
