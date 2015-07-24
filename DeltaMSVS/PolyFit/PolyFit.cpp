// PolyFit.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include "polyfit.hpp"
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <float.h>
//#include <boost/numeric/bindings/lapack/getsvd.hpp>

//using std::vector;
using namespace boost::numeric::ublas;


//Get the coefficients of a polynomial through a set of points of the form {(x0,y0)...(xi,yi)...(xN-1,yN-1)}
double* getCoeffs( const double* aX,const double* aY, const unsigned int numPoints,double* output){
	
	//Define Matrix A of size NxN; Matrix A^-1 of size NxN; and Matrix B of size 1xN
	matrix<double> A(numPoints, numPoints);
	matrix<double> Ainv(numPoints , numPoints);
	matrix<double> B(1, numPoints);
	//Fill Matrix A with {1...(xi)^j...(xN)^N)} and Matrix B with {y0...yi...yN}
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
	
	//Multiply matrix B by matrix A^-1
	matrix<double> C(prec_prod(B, Ainv));

	for (int i = 0; i < C.size2(); i++){
		output[i] = C(0, i);
	}

	return output;
}

//A 4 dimensional point of the form (x,y,z,t)
struct point{
	double t;
	double x;
	double y;
	double z;
};
//double(*f)(double)

double simpsonApproximation(std::function<double(double)> f, double a, double b, unsigned int N){
	if (N % 2){
		throw std::invalid_argument("N must be even");
	}
	double h = (b - a) / N;
	double s = f(a) + f(b);

	for (int i = 1; i < N; i += 2){
		s += 4 * f(a + i * h);
	}
	for (int i = 2; i < N - 1; i += 2){
		s += 2 * f(a + i * h);
	}
	return s * h / 3;
}
double valueAt(double * coeffs, unsigned int num, double oX){
	double nY = 0;
	double x = 1;
	for (int j = 0; j < num; j++){
		nY += coeffs[j] * x;
		x *= oX;
	}
	return nY;
}

#define SIMP_N 20

void getCurveParameterization(const point *points, const unsigned int numPoints, double *coeffsX, double *coeffsY, double *coeffsZ, double *coeffsSqrtDS){
	//Initialize containers
	//The x, y, and z coordinates of points[i]
	double *xs = new double[numPoints];
	double *ys = new double[numPoints];
	double *zs = new double[numPoints];
	double *ts = new double[numPoints];
	//The number of coefficients in a polynomial that is the square of the derivative of a polynomial of degree numPoints-1
	unsigned int numDervSqr = (numPoints - 1) * 2 - 1;
	double *dxs = new double[numPoints * 5];
	double *dys = new double[numPoints * 5];
	double *dzs = new double[numPoints * 5];
	double *dss;

	/*double tMax = DBL_MIN;
	double tMin = DBL_MAX;*/
	double prevt = DBL_MIN;
	//Take the input points and put them in a list
	for (int i = 0; i < numPoints; i++){
		point p = points[i];
		xs[i] = p.x;
		ys[i] = p.y;
		zs[i] = p.z;
		ts[i] = p.t;
		/*if (p.t > tMax)tMax = p.t;
		if (p.t < tMin)tMin = p.t;*/
		if(p.t < prevt){
			throw bad_argument("Points must be in order of acending t's");
		}
		prevt = p.t;
	}
	//Get the coefficients for polynomials x(t), y(t), and z(t)
	getCoeffs(ts, xs, numPoints, coeffsX);
	getCoeffs(ts, ys, numPoints, coeffsY);
	getCoeffs(ts, zs, numPoints, coeffsZ);

	//Get the coefficients for polynomials x'(t), y'(t), and z'(t)
	derivativePolynomial(coeffsX, numPoints, dxs);
	derivativePolynomial(coeffsY, numPoints, dys);
	derivativePolynomial(coeffsZ, numPoints, dzs);
	
	//Find the coefficients for polynomials (x'(t))^2, (y'(t))^2, and (z'(t))^2
	dss = dxs;
	multiplyPolynomials(dxs, numPoints - 1, dxs, numPoints - 1, dxs);
	multiplyPolynomials(dys, numPoints - 1, dys, numPoints - 1, dys);
	multiplyPolynomials(dzs, numPoints - 1, dzs, numPoints - 1, dzs);
	
	//Find (x'(t))^2 + (y'(t))^2 + (z'(t))^2
	sumPolynomials(dxs, numDervSqr, dys, numDervSqr, dss);
	sumPolynomials(dss, numDervSqr, dzs, numDervSqr, dss);

	auto f = [&](double x) { return sqrt(valueAt(dss, numDervSqr, x)); }; 
	double length = simpsonApproximation(f, ts[0], ts[numPoints - 1], SIMP_N);
	double elapseT = ts[numPoints - 1] - ts[0];

	double *newxs = new double[numPoints * 2];
	double *newys = new double[numPoints * 2];
	double *newzs = new double[numPoints * 2];
	double *newts = new double[numPoints * 2];

	//newts[0] = ts[0];
	for (int i = 1; i < numPoints-1; i++){
		ts[i] = ts[0] + elapseT*(simpsonApproximation(f, ts[0], ts[i], SIMP_N) / length);
		std::cout << "T:" << ts[i] << std::endl;
	}
	for (int i = 0; i < numPoints-1; i++){
		newts[i * 2] = ts[i];
		newts[i * 2 + 1] = ts[0] + elapseT*(simpsonApproximation(f, ts[0], (ts[i+1] + ts[i]) / 2, SIMP_N) / length);
		newxs[i * 2] = xs[i];
		newys[i * 2] = ys[i];
		newzs[i * 2] = zs[i];
		newxs[i * 2 + 1] = valueAt(coeffsX, numPoints, newts[i * 2 + 1]);
		newys[i * 2 + 1] = valueAt(coeffsY, numPoints, newts[i * 2 + 1]);
		newzs[i * 2 + 1] = valueAt(coeffsZ, numPoints, newts[i * 2 + 1]);
	}
	newxs[(numPoints - 1) * 2] = xs[numPoints - 1];
	newys[(numPoints - 1) * 2] = ys[numPoints - 1];
	newzs[(numPoints - 1) * 2] = zs[numPoints - 1];
	newts[(numPoints - 1) * 2] = ts[numPoints - 1];
	

	std::cout << "LENGTH1:" << length << std::endl;
	std::cout << "P1:" << simpsonApproximation(f, ts[0], ts[1], SIMP_N) << std::endl;
	/*newts[numPoints] = ts[numPoints - 1];
	xs[numPoints] = xs[numPoints - 1];
	ys[numPoints] = ys[numPoints - 1];
	zs[numPoints] = zs[numPoints - 1];
	newts[numPoints - 1] = ts[0] + elapseT*(simpsonApproximation(f, ts[0], (ts[numPoints - 2] + ts[numPoints - 1]) / 2, SIMP_N) / length);
	xs[numPoints - 1] = valueAt(coeffsX, numPoints, newts[numPoints - 1]);
	ys[numPoints - 1] = valueAt(coeffsY, numPoints, newts[numPoints - 1]);
	zs[numPoints - 1] = valueAt(coeffsZ, numPoints, newts[numPoints - 1]);*/
	for (int i = 0; i< (numPoints*2)-1; i++){
		std::cout << newxs[i] << ", " << newys[i] << ", " << newzs[i] << ", " << newts[i] << std::endl;
	}
	std::cout << "END" << std::endl;
	
	
	//Get the coefficients for polynomials x(t), y(t), and z(t)
	getCoeffs(newts, newxs, (numPoints * 2) - 1, coeffsX);
	getCoeffs(newts, newys, (numPoints * 2) - 1, coeffsY);
	getCoeffs(newts, newzs, (numPoints * 2) - 1, coeffsZ);

	std::cout << "COEFF" << std::endl;
	for (int i = 0; i< (numPoints * 2) - 1; i++){
		std::cout << coeffsX[i] << ", " << coeffsY[i] << ", " << coeffsZ[i] << std::endl;
	}
	

	//Get the coefficients for polynomials x'(t), y'(t), and z'(t)
	derivativePolynomial(coeffsX, (numPoints * 2) - 1, dxs);
	derivativePolynomial(coeffsY, (numPoints * 2) - 1, dys);
	derivativePolynomial(coeffsZ, (numPoints * 2) - 1, dzs);

	std::cout << "DERIVATIVE" << std::endl;
	for (int i = 0; i< (numPoints * 2) - 2; i++){
		std::cout << dxs[i] << ", " << dys[i] << ", " << dzs[i] << std::endl;
	}

	//Find the coefficients for polynomials (x'(t))^2, (y'(t))^2, and (z'(t))^2
	dss = dxs;
	multiplyPolynomials(dxs, (numPoints * 2) - 2, dxs, (numPoints * 2) - 2, dxs);
	multiplyPolynomials(dys, (numPoints * 2) - 2, dys, (numPoints * 2) - 2, dys);
	multiplyPolynomials(dzs, (numPoints * 2) - 2, dzs, (numPoints * 2) - 2, dzs);


	numDervSqr = ((numPoints * 2) - 2) * 2 - 1;

	/*std::cout << "MULTIPLY" << std::endl;
	for (int i = 0; i< numDervSqr; i++){
		std::cout << dxs[i] << ", " << dys[i] << ", " << dzs[i] << std::endl;
	}*/

	
	//Find (x'(t))^2 + (y'(t))^2 + (z'(t))^2
	sumPolynomials(dxs, numDervSqr, dys, numDervSqr, dss);
	sumPolynomials(dss, numDervSqr, dzs, numDervSqr, dss);

	//auto f = [&](double x) { return sqrt(valueAt(dss, numDervSqr, x)); };
	auto f2 = [&](double x) { return sqrt(valueAt(dss, numDervSqr, x)); };

	length = simpsonApproximation(f2, newts[0], newts[numPoints], SIMP_N);
	
	std::cout << "LENGTH2:" << length << std::endl;
	std::cout << "P1:" << simpsonApproximation(f2, newts[0], newts[1], SIMP_N) << std::endl;

	double prevVal = 0;
	for(int i =1; i< 100; i++){
		double dt = (i / 100.0)*elapseT;
		double val = simpsonApproximation(f2, ts[0], ts[0] + dt, SIMP_N);
		std::cout << val - prevVal << std::endl;
		prevVal = val;
	}

	//Delete allocated data
	delete[] xs;
	delete[] ys;
	delete[] zs;
	delete[] ts;
	delete[] newts;
	delete[] dxs;
	delete[] dys;
	delete[] dzs;

	return;
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

	num = 15;
	point points[] = { { 1.0, 2.0, 4.0, 5.0 }, { 2.0, 8.0, 9.0, 10.0 }, { 3.0, 1.0, 4.0, 6.0 }, { 4.0, 6.0, 6.0, 8.0 } };
	double *coeffsX = new double[num]();
	double *coeffsY = new double[num]();
	double *coeffsZ = new double[num]();
	double *coeffsS = new double[num]();
	getCurveParameterization(points, 4, coeffsX, coeffsY, coeffsZ, coeffsS);
	
	while(1){
	}
	return 0;
}
