#ifndef LABLEBPALGORITHM_H

#define LABLEBPALGORITHM_H

/* Include Files */
#include <stddef.h>
#include <stdlib.h>
typedef struct
{
	int noise;
	int sysTime;
	int disTime;
	int sysArea;
	int disArea;

	int sysHeigth;
	int disHeigth;
	int sysSlope;
	int disSlope;   
	int sysAveHeigth;
	int disAveHeigth;

	int span;
}LABEL_BP_PULSE_t;

/* Function Declarations */

LABEL_BP_PULSE_t featureExtraction(int *originppg);

void BloodPressure(double m2, double v0, double v_inf, int *singlePause, int singlePause_len, int *prebp);
#endif
