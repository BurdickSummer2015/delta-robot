// Tests.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include "functions.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>
#include <time.h>
#include <io.h>


void compareOldAndNew(double stepsize);
void printTableAccuracy(double stepsize);
void RunSpeedTest_TablevsKin(double stepsize);
void RunSpeedTest_Acos(int num);
//double getAngle1New(struct point p);
//double getAngle2New(struct point p);
//double getAngle3New(struct point p);
//void getAnglesNew(double* angles, struct point p);
double fastACos(double x);

int main(){
	//compareOldAndNew(.1);
	//printTableAccuracy(.1);
	RunSpeedTest_TablevsKin(.1);
	//RunSpeedTest_Acos(1000000);
	//double list[3] = { 1.0, 2.0, 3.0 };
	//double list2[3] = { 0,0,0 };
	//FILE stream;
	//fwrite(list,sizeof(list[0]), sizeof(list[0]), &stream);
	//fread(list2, sizeof(list[0]), sizeof(list[0]), &stream);

	//printf("%f, %f, %f", list2[0], list2[1], list2[3]);
	while(1){}
	return 0;
}
/*
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
				//printf("%f\n", anglesOld[1]);
				getAnglesNew(anglesNew, p);
				//printf("%f\n", anglesNew[1]);
				if (!(isnan(anglesOld[0]) | isnan(anglesOld[1]) | isnan(anglesOld[2]) |
					isnan(anglesNew[0]) | isnan(anglesNew[1]) | isnan(anglesNew[2]))){
					//printf("%f %f %f \n",x, y, z);
					double diffX = anglesOld[0] - anglesNew[0];
					double diffY = anglesOld[1] - anglesNew[1];
					double diffZ = anglesOld[2] - anglesNew[2];
					//printf("%f %f %f \n",diffX, diffY, diffZ);
					diff += sqrt(diffX*diffX + diffY*diffY + diffZ*diffZ);
					count++;
				}
				//printf("\n");
			}
		}
	}
	avgDiff /= count;

	printf("AverageDiff: %f", diff);
}
*/
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
	/*
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
	*/
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

double fastACos(double x){
	return sqrt(1 - x)*((M_PI / 2) + x*(-0.213300989 + x*(0.077980478 + x*-0.02164095)));
}
