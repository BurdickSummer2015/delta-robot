// Tests.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include "functions.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>
#include <time.h>


void compareOldAndNew(double stepsize);
void printTableAccuracy(double stepsize);
void RunSpeedTest_TablevsKin(double stepsize);
void RunSpeedTest_Acos(int num);
double getAngle1New(struct point p);
double getAngle2New(struct point p);
double getAngle3New(struct point p);
void getAnglesNew(double* angles, struct point p);
double fastACos(double x);

int main(){
	compareOldAndNew(.1);
	//printTableAccuracy(.1);
	//RunSpeedTest_TablevsKin(.1);
	//RunSpeedTest_Acos(1000000);
	while(1){}
	return 0;
}
void compareOldAndNew(double stepsize){
	double avgDiff = 0;
	double diff = 0;
	double anglesOld[3];
	double anglesNew[3];
	int count = 0;
	struct point p = { 0, 0, 0 };
	for (double x = X_TABLE_MIN; x <= X_TABLE_MAX; x += stepsize){
		for (double y = Y_TABLE_MIN; y <= Y_TABLE_MAX; y += stepsize){
			for (double z = Z_TABLE_MIN; z <= Z_TABLE_MAX; z += stepsize){
				p.x = x;
				p.y = y;
				p.z = z;
				struct point p = { x, y, z };
				getAngles(anglesOld, p);
				getAnglesNew(anglesNew, p);
				if (!(isnan(anglesOld[0]) | isnan(anglesOld[1]) | isnan(anglesOld[2]) |
					isnan(anglesNew[0]) | isnan(anglesNew[1]) | isnan(anglesNew[2]))){
					//printf("%f %f %f \n",x, y, z);
					double diffX = anglesOld[0] - anglesNew[0];
					double diffY = anglesOld[1] - anglesNew[1];
					double diffZ = anglesOld[2] - anglesNew[2];
					diff += sqrt(diffX*diffX + diffY*diffY + diffZ*diffZ);
					count++;
				}
			}
		}
	}
	avgDiff /= count;

	printf("AverageDiff: %f", diff);
}

void printTableAccuracy(double stepsize){
	printf("Checking LookupAngles Accuracy... \n");
	double minDiff = 100000000;
	double maxDiff = -100000000;
	double avgDiff = 0;
	double diff = 0;
	double anglesTable[3];
	double anglesKin[3];
	int count = 0;
	struct point p = {0,0,0};
	for (double x = X_TABLE_MIN; x <= X_TABLE_MAX; x += stepsize){
		for (double y = Y_TABLE_MIN; y <= Y_TABLE_MAX; y += stepsize){
			for (double z = Z_TABLE_MIN; z <= Z_TABLE_MAX; z += stepsize){
				//printf("%f %f %f \n",x, y, z);
				p.x = x;
				p.y = y;
				p.z = z;
				struct point p = { x, y, z };
				getAngles(anglesKin, p);
				lookupAngles(INVERSE_TABLE, anglesTable, p);
				if (!(isnan(anglesKin[0]) | isnan(anglesKin[1]) | isnan(anglesKin[2]) |
					isnan(anglesTable[0]) | isnan(anglesTable[1]) | isnan(anglesTable[2]))){
					//printf("%f %f %f \n", anglesKin[0], anglesKin[1], anglesKin[2]);
					//printf("%f %f %f \n", anglesTable[0], anglesTable[1], anglesTable[2]);
					double diffX = anglesKin[0] - anglesTable[0];
					double diffY = anglesKin[1] - anglesTable[1];
					double diffZ = anglesKin[2] - anglesTable[2];
					diff = sqrt(diffX*diffX + diffY*diffY + diffZ*diffZ);
					//printf("%f \n", avgDiff);
					//printf("%f %i \n", avgDiff, count);
					avgDiff += diff;
					count++;
					maxDiff = diff > maxDiff ? diff : maxDiff;
					minDiff = diff < minDiff ? diff : minDiff;
				}
			}
		}
	}
	avgDiff /= count;

	printf("AverageDiff: %f MinDiff: %f MaxDiff: %f", avgDiff, minDiff, maxDiff);
}

void RunSpeedTest_Acos(long num){
	int i;
	double a;
	clock_t start = 0;
	clock_t duration;
	start = clock();
	for (i = 0; i < num; i++){
		a = acos((double)i / num);
		//printf("%i\n", i);
	}
	printf("\n");
	duration = ((clock_t)clock() - start); //(double)CLOCKS_PER_SEC;
	printf("Duration acos: %ld \n", duration);

	start = clock();
	for (i = 0; i < num; i++){
		a = fastACos((double)i / num);
	}
	printf("\n");
	duration = ((clock_t)clock() - start); //(double)CLOCKS_PER_SEC;
	printf("Duration acos FAST: %ld \n", duration);
}


void RunSpeedTest_TablevsKin(double stepsize){
	clock_t start = 0;
	clock_t duration;
	double x,y,z;
	double angles[3];
	struct point p = { 0, 0, 0 };

	int count = 0;
	start = clock();
	for (x = X_TABLE_MIN; x <= X_TABLE_MAX; x += stepsize){
		for (y = Y_TABLE_MIN; y <= Y_TABLE_MAX; y += stepsize){
			for (z = Z_TABLE_MIN; z <= Z_TABLE_MAX; z += stepsize){
				//printf("%f", x);
				p.x = x;
				p.y = y;
				p.z = z;
				getAnglesNew(angles, p);
				count++;
			}
		}
	}
	printf("\n");
	duration = ((clock_t)clock() - start); //(double)CLOCKS_PER_SEC;
	printf("Duration: %ld \n", duration);
	printf("Count: %i \n", count);

	start = clock();	
	for (x = X_TABLE_MIN; x <= X_TABLE_MAX; x += stepsize){
		for (y = Y_TABLE_MIN; y <= Y_TABLE_MAX; y += stepsize){
			for (z = Z_TABLE_MIN; z <= Z_TABLE_MAX; z += stepsize){
				//printf("%f", x);
				p.x = x;
				p.y = y;
				p.z = z;
				getAngles(angles, p);
			}
		}
	}
	printf("\n");
	duration = ((clock_t)clock() - start); //(double)CLOCKS_PER_SEC;
	printf("Duration: %ld \n", duration);

	start = clock();
	for (x = X_TABLE_MIN; x <= X_TABLE_MAX; x += stepsize){
		for (y = Y_TABLE_MIN; y <= Y_TABLE_MAX; y += stepsize){
			for (z = Z_TABLE_MIN; z <= Z_TABLE_MAX; z += stepsize){
				//printf("%f", x);
				p.x = x;
				p.y = y;
				p.z = z;
				lookupAngles(INVERSE_TABLE, angles, p);
			}
		}
	}
	printf("\n");
	duration = ((clock_t)clock() - start); //(double)CLOCKS_PER_SEC;
	printf("Duration: %ld \n", duration);
}

void getAnglesNew(double* angles, struct point p)
{
	angles[0] = getAngle1New(p);
	angles[1] = getAngle2New(p);
	angles[2] = getAngle3New(p);
	if (isnan(angles[0]) || isnan(angles[1]) || isnan(angles[2]))
	{
		angles[0] = NAN;
		angles[1] = NAN;
		angles[2] = NAN;
	}
}
double fastACos(double x){
	return sqrt(1 - x)*((M_PI / 2) + x*(-0.213300989 + x*(0.077980478 + x*-0.02164095)));
}

/**
* Returns the required angle of servo actuator 1 to get to a desired point 'p'
* in degrees and servo coordinates. If no solution exists, returns NAN.
*/
double processArgOptions(double* options, double arg1, double arg2){
	if (fabs(arg1) > 1.0)
	{
		if (fabs(arg1 - 1.0) < 0.0001) /* Slightly over 1 (rounding error) */
		{
			options[0] = 0;
			options[1] = 0;
		}
		else if (fabs(arg1 - (-1.0)) < 0.0001) /* Slightly < 1 (round error) */
		{
			options[0] = M_PI;
			options[1] = -M_PI;
		}
		else
		{
			options[0] = NAN;
			options[1] = NAN;
		}
	}
	else
	{
		options[0] = acos(arg1);
		options[1] = -acos(arg1);
	}

	if (fabs(arg2) > 1.0)
	{
		if (fabs(arg2 - 1.0) < 0.0001) /* Slightly over 1 (rounding error) */
		{
			options[2] = 0;
			options[3] = 0;
		}
		else if (fabs(arg2 - (-1.0)) < 0.0001) /* Slightly < 1 (round error) */
		{
			options[2] = M_PI;
			options[3] = -M_PI;
		}
		else
		{
			options[2] = NAN;
			options[3] = NAN;
		}
	}
	else
	{
		options[2] = acos(arg2);
		options[3] = -acos(arg2);
	}
}
double extractAngleSolution(double* values, double* options){
	/* Find desired indices of 'options'. */
	int ind1 = minInd1(values, sizeof(values) / sizeof(values[0]));
	int ind2 = minInd2(values, sizeof(values) / sizeof(values[0]));
	/* Pick the two closest options to the original equation. */
	double viableOptions[2];
	viableOptions[0] = (ind1 == -1) ? NAN : options[ind1];
	viableOptions[1] = (ind2 == -1) ? NAN : options[ind2];
	/* REMOVE EXTRANEOUS SOLUTION + CONVERT ANGLE*/
	return modAngle(removeExtraneous(viableOptions));
}
double sqr(double x){
	return x*x;
}

double getAngle1New(struct point p)
{
	double options[4]; /* Mathematica outputs four possible expressions for the
					   angle. Two are invalid, and one is extraneous. */
	/* GENERATE FOUR POSSIBLE OPTIONS. */
	double x2 = p.x*p.x;
	double x3 = x2*p.x;
	double x4 = x3*p.x;
	double y2 = p.y*p.y;
	double y4 = y2*y2;
	double z2 = p.z*p.z;
	double z3 = z2*p.z;
	double z4 = z3*p.z;
	double A = -214 * sqrt(3) + 6 * sqrt(3) * x2 -
		x3 + 2 * sqrt(3) * y2 + 2 * sqrt(3) *
		z2 - p.x * (-83 + y2 + z2);
	double B = sqrt(-z2 * (10249 - 8 * sqrt(3) * x3
		+ x4 + y4 - 314 * z2 +
		z4 + 2 * y2 * (-107 + z2) -
		8 * sqrt(3) * p.x * (-157 + y2 + z2)
		+ 2 * x2 * (-133 + y2 +
		z2)));
	double C =(10 * (12 - 4 * sqrt(3) * p.x +
		x2 + z2));
	double arg1 = -(A + B) / C;
	double arg2 = -(A - B) / C;

	processArgOptions(options, arg1, arg2);
	

	/* REMOVE INVALID SOLUTIONS */
	double values[4]; /* One val for each option; pick 2 smallest. */
	int i; /* Iterate over options. */
	for (i = 0; i < 4; i++)
	{
		if (isnan(options[i]))
			values[i] = DBL_MAX;
		else
			values[i] =
			fabs(sqr(p.x + (4 - 10) / sqrt(3) - 5 * cos(options[i]))
			+ y2
			+ sqr(p.z - 5 * sin(options[i]))
			- 144);
		/* Above expression comes from original kinematics equation. */
	}
	return extractAngleSolution(values, options);
}

/**
* Returns the required angle of servo actuator 2 to get to a desired point 'p'
* in degrees and servo coordinates. If no solution exists, returns NAN.
*/
double getAngle2New(struct point p)
{
	double options[4]; /* Mathematica outputs four possible expressions for the
					   angle. Two are invalid, and one is extraneous. */
	/* GENERATE FOUR POSSIBLE OPTIONS. */
	double x2 = p.x*p.x;
	double x3 = x2*p.x;
	double x4 = x3*p.x;
	double y2 = p.y*p.y;
	double y3 = p.y*p.y;
	double y4 = y2*y2;
	double z2 = p.z*p.z;
	double z3 = z2*p.z;
	double z4 = z3*p.z;
	double A = -428 * sqrt(3) + x3 - sqrt(3) * x2 *
		(-6 + p.y) + 83 * sqrt(3) *  p.y + 10 * sqrt(3) *
		y2 - sqrt(3) * y3 + 4 * sqrt(3) *
		z2 - sqrt(3) * p.y * z2 + p.x * (-83 -
		12 * p.y + y2 + z2);
	double B = 2 *
		sqrt(-z2 * (10249 + 4 * sqrt(3) * x3 +
		x4 - 12 * y3 + y4 - 314 *
		z2 + z4 - 12 * p.y * (-157 + z2)
		+ y2 * (-253 + 2 *
		z2) + x2 * (-227 - 12 * p.y + 2 *
		y2 + 2 * z2) + 2 * sqrt(3) * p.x * (-314 +
		13 * p.y + 2 * y2 + 2 * z2)));
	double C = (5 * (48 +
		x2 - 2 * sqrt(3) * p.x * (-4 + p.y) - 24 * p.y + 3
		* y2 + 4 * z2));
	double arg1 = -(A + B) / C;
	double arg2 = -(A -B) / C;

	processArgOptions(options, arg1, arg2);

	/* REMOVE INVALID SOLUTIONS */
	double values[4]; /* One val for each option; pick 2 smallest. */
	int i; /* Iterate over options. */
	for (i = 0; i < 4; i++)
	{
		if (isnan(options[i]))
			values[i] = DBL_MAX;
		else
			values[i] = fabs(sqr(p.x + (10 - 4) / (2 * sqrt(3)) + 5 *
			cos(options[i]) / 2.0) + sqr(p.y + (4 - 10 - 5 *
			sqrt(3) * cos(options[i])) / 2.0) + sqr(p.z - 5 *
			sin(options[i])) - 144);
		/* Above expression comes from original kinematics equation. */
	}
	return extractAngleSolution(values, options);
}

/**
* Returns the required angle of servo actuator 3 to get to a desired point 'p'
* in degrees and servo coordinates. If no solution exists, returns NAN.
*/
double getAngle3New(struct point p)
{
	double options[4]; /* Mathematica outputs four possible expressions for the
					   angle. Two are invalid, and one is extraneous. */
	/* GENERATE FOUR POSSIBLE OPTIONS. */
	double x2 = p.x*p.x;
	double x3 = x2*p.x;
	double x4 = x3*p.x;
	double y2 = p.y*p.y;
	double y3 = p.y*p.y;
	double y4 = y2*y2;
	double z2 = p.z*p.z;
	double z3 = z2*p.z;
	double z4 = z3*p.z;
	double A = -428 * sqrt(3) + x3 - 83 * sqrt(3) * p.y +
		10 * sqrt(3) * y2 + sqrt(3) * y3 +
		sqrt(3) * x2 * (6 + p.y) + 4 * sqrt(3) *
		z2 + sqrt(3) * p.y * z2 + p.x * (-83 +
		12 * p.y + y2 + z2);
	double B = 2 * sqrt(
		-z2 * (10249 + 4 * sqrt(3) * x3 +
		x4 + 12 * y3 + y4 -
		314 * z2 + z4 + 12 * p.y * (-157 +
		z2) + y2 * (-253 + 2 * z2) +
		2 * sqrt(3) * p.x * (-314 - 13 * p.y + 2 * y2 +
		2 * z2) + x2 * (-227 + 12 * p.y + 2 *
		y2 + 2 * z2)));
	double C = (5 * (48 + x2 +
		24 * p.y + 3 * y2 + 2 * sqrt(3) * p.x * (4 + p.y)
		+ 4 * z2));
	double arg1 = -(A + B) / C;
	double arg2 = -(A - B) / C;

	processArgOptions(options, arg1, arg2);

	/* REMOVE INVALID SOLUTIONS */
	double values[4]; /* One val for each option; pick 2 smallest. */
	int i; /* Iterate over options. */
	for (i = 0; i < 4; i++)
	{
		if (isnan(options[i]))
			values[i] = DBL_MAX;
		else
			values[i] = fabs(sqr(p.x + (10 - 4) / (2 * sqrt(3)) + 5 *
			cos(options[i]) / 2.0) + sqr(p.y + (10 - 4 + 5 *
			sqrt(3) * cos(options[i])) / 2.0) + sqr(p.z - 5 *
			sin(options[i])) - 144);
		/* Above expression comes from original kinematics equation. */
	}
	return extractAngleSolution(values, options);
}
