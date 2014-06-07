#include "notePicker.h"
#include <stdio.h>
#include <sys/time.h>

#define ESEGUI(p,l) {gettimeofday(&start,NULL);ris=pickSingleNote(l,p);gettimeofday(&stop,NULL);t=(stop.tv_usec-start.tv_usec);tAve += t;}

struct timeval start,stop;
uint8_t ris;
uint8_t t;
float tAve;

int main(int argc, char* argv){
	uint8_t probs0[] = {20,0,0,0,10,30,0,30,0,0,10,0};
	uint8_t probs1[] = {0,0,0,50,0,10,0,10,0,20,10,0};
	uint8_t probs2[] = {10,10,30,3,0,7,0,20,5,0,5,10};
	gettimeofday(&start,NULL);
	uint8_t prob1 = 10;
	uint8_t prob2 = 20;
	uint8_t prob3 = 30;
	uint8_t prob4 = 40;
	uint8_t prob5 = 50;
	uint8_t prob6 = 60;
	uint8_t prob7 = 70;
	uint8_t prob8 = 80;
	uint8_t prob9 = 90;
	uint8_t prob10 = 99;
	gettimeofday(&stop,NULL);
	printf("probs0 : {20,0,0,0,10,30,0,30,0,0,10,0}\ngauss0 : {10-4,20-0,30-7,5-30,10-10}\n");
	ESEGUI(prob1,probs0);
	printf("pick 10 - 4 : %d, t: %ld\n",ris,t);
	ESEGUI(prob2,probs0);
	printf("pick 20 - 0 : %d, t: %ld\n",ris,t);
	ESEGUI(prob3,probs0);
	printf("pick 30 - 0 : %d, t: %ld\n",ris,t);
	ESEGUI(prob4,probs0);
	printf("pick 40 - 7 : %d, t: %ld\n",ris,t);
	ESEGUI(prob5,probs0);
	printf("pick 50 - 7 : %d, t: %ld\n",ris,t);
	ESEGUI(prob6,probs0);
	printf("pick 60 - 7 : %d, t: %ld\n",ris,t);
	ESEGUI(prob7,probs0);
	printf("pick 70 - 5 : %d, t: %ld\n",ris,t);
	ESEGUI(prob8,probs0);
	printf("pick 80 - 5 : %d, t: %ld\n",ris,t);
	ESEGUI(prob9,probs0);
	printf("pick 90 - 5 : %d, t: %ld\n",ris,t);
	ESEGUI(prob10,probs0);
	printf("pick 99 - 10 : %d, t: %ld\n",ris,t);
	
	printf("probs1 : {0,0,0,50,0,10,0,10,0,20,10,0}\ngauss1 : {10-5,10-10,50-3,20-9,10-7}\n");
	ESEGUI(prob1,probs1);
	printf("pick 10 - 5 : %d, t: %ld\n",ris,t);
	ESEGUI(prob2,probs1);
	printf("pick 20 - 10 : %d, t: %ld\n",ris,t);
	ESEGUI(prob3,probs1);
	printf("pick 30 - 3 : %d, t: %ld\n",ris,t);
	ESEGUI(prob4,probs1);
	printf("pick 40 - 3 : %d, t: %ld\n",ris,t);
	ESEGUI(prob5,probs1);
	printf("pick 50 - 3 : %d, t: %ld\n",ris,t);
	ESEGUI(prob6,probs1);
	printf("pick 60 - 3 : %d, t: %ld\n",ris,t);
	ESEGUI(prob7,probs1);
	printf("pick 70 - 3 : %d, t: %ld\n",ris,t);
	ESEGUI(prob8,probs1);
	printf("pick 80 - 9 : %d, t: %ld\n",ris,t);
	ESEGUI(prob9,probs1);
	printf("pick 90 - 9 : %d, t: %ld\n",ris,t);
	ESEGUI(prob10,probs1);
	printf("pick 99 - 7 : %d, t: %ld\n",ris,t);
	
	printf("probs2 : {10,10,30,3,0,7,0,20,5,0,5,10}\ngauss2 : {3-3,5-10,10-0,10-11,30-2,20-7,10-1,7-5,5-8}\n");
	ESEGUI(prob1,probs2);
	printf("pick 10 - 0 : %d, t: %ld\n",ris,t);
	ESEGUI(prob2,probs2);
	printf("pick 20 - 11 : %d, t: %ld\n",ris,t);
	ESEGUI(prob3,probs2);
	printf("pick 30 - 2 : %d, t: %ld\n",ris,t);
	ESEGUI(prob4,probs2);
	printf("pick 40 - 2 : %d, t: %ld\n",ris,t);
	ESEGUI(prob5,probs2);
	printf("pick 50 - 2 : %d, t: %ld\n",ris,t);
	ESEGUI(prob6,probs2);
	printf("pick 60 - 7 : %d, t: %ld\n",ris,t);
	ESEGUI(prob7,probs2);
	printf("pick 70 - 7 : %d, t: %ld\n",ris,t);
	ESEGUI(prob8,probs2);
	printf("pick 80 - 1 : %d, t: %ld\n",ris,t);
	ESEGUI(prob9,probs2);
	printf("pick 90 - 5 : %d, t: %ld\n",ris,t);
	ESEGUI(prob10,probs2);
	printf("pick 99 - 8 : %d, t: %ld\n",ris,t);
	
	tAve = tAve / 30;
	printf("average time: %lf\n",tAve);
}
