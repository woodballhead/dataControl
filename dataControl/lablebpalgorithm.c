#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Def.h"
#include "heart_rate.h"
#include "lablebpalgorithm.h"
#include "common.h" 

int mean(int *vin, int width, int step);
int kdrift(int *vout, int *vin, int len, int width, int step);

extern int BeatCounts[MAXTYPES];
extern int BeatTemplates[MAXTYPES][BEATLGTH];

LABEL_BP_PULSE_t featureExtraction(int *originppg){
	LABEL_BP_PULSE_t lable;
	int width = 101, step = 5;
	int numppg, noise = 0;
	int * noiselevel = &noise;
	int i, j, max, maxI;
	int valleyVal1, valleyVal2, waveStart, waveEnd;
//	int point;

	unsigned short wavelen;
	int selectPPG[BEATLGTH] = { 0 };
	int anlylen = unitlen;
	int *ppg = NULL;

	lable.sysTime = 0;
	lable.disTime = 0;
	lable.sysArea = 0;
	lable.disArea = 0;
	lable.sysHeigth = 0;
	lable.disHeigth = 0;
	lable.sysAveHeigth = 0;
	lable.disAveHeigth = 0;
	lable.sysSlope = 0;
	lable.disSlope = 0;
	lable.span = 0;
	lable.noise = 0;

	ppg = MakeArray1D(anlylen);

	kdrift(ppg, originppg, anlylen, width, step);

	j = lowpass5(ppg, originppg, anlylen);                                //  originppg是经过滤波后的数据
	//对抗漂移后的ppg做噪音指数分析
	numppg = ppg_RealAnalysisFile(originppg, noiselevel, anlylen);

	lable.noise = noise;

	if (noise > noiseThreshold1) {
		goto sign;
	}

	// 对最大的模板计算峰值和谷值以及提取ppg
	max = BeatCounts[0];
	maxI = 0;
	for (i = 1; i < MAXTYPES; i++) {
		if (BeatCounts[i] > max) {
			max = BeatCounts[i];
			maxI = i;
		}
	}
	for (j = 0; j < BEATLGTH; j++) {
		selectPPG[j] = BeatTemplates[maxI][j];
	}
	//找出符合条件的第i个脉搏波，从起点到终点，输出数组bpuluse
	waveStart = 0;
	waveEnd = BEATLGTH - 1;
	valleyVal1 = selectPPG[0];
	for (i = 1; i < centralPoint; i++) {
		if (selectPPG[i] < valleyVal1) {
			valleyVal1 = selectPPG[i];
			waveStart = i;
		}
	}
	valleyVal2 = selectPPG[BEATLGTH - 1];
	for (i = BEATLGTH - 2; i > centralPoint; i--) {
		if (selectPPG[i] < valleyVal2) {
			valleyVal2 = selectPPG[i];
			waveEnd = i;
		}
	}
	wavelen = waveEnd - waveStart + 1;

	if (waveEnd >= BEATLGTH - 3 && selectPPG[BEATLGTH - 1] > selectPPG[0]) {
		lable.noise = 1000;
		goto sign;
	}
	lable.sysTime = centralPoint - waveStart;
	for (i = waveStart, lable.sysArea = 0; i < centralPoint; i++) {
		lable.sysArea += (selectPPG[i] - selectPPG[waveStart]);
	}


	lable.sysHeigth = -selectPPG[waveStart];
	if (lable.sysTime) {
		lable.sysAveHeigth = lable.sysArea / lable.sysTime;
		lable.sysSlope = (100 * lable.sysHeigth) / lable.sysTime;
	}


	if (waveEnd >= BEATLGTH - 3 && waveStart > 3) {
		lable.disHeigth = -selectPPG[waveStart];
		
		for (i = 0; i < waveStart; i++) {
//			point = i;			
			if (selectPPG[i] <= selectPPG[BEATLGTH - 1]) {
				break;
			}
	
		}
		
		lable.disTime = waveStart - i + waveEnd - centralPoint;
		if (lable.disTime < 0) {
			i++;
			i--;
		}
		for (j = centralPoint, lable.disArea = 0; j < BEATLGTH; j++) {
			lable.disArea += (selectPPG[j] - selectPPG[waveStart]);    // 舒张期右边的面积
		}
		for (j = i; j < waveStart; j++) {
			lable.disArea += (selectPPG[j] - selectPPG[waveStart]);    // 舒张期左边的面积
		}		
	}
	else{
		lable.disHeigth = -selectPPG[waveEnd];
		lable.disTime = waveEnd - centralPoint;
		if (lable.disTime < 0) {
			i++;
			i--;          
		}
		for (i = centralPoint, lable.disArea = 0; i < waveEnd; i++) {
			lable.disArea += (selectPPG[i] - selectPPG[waveEnd]);
		}		
	}
	if (lable.disTime) {
		lable.disSlope = (100 * lable.disHeigth) / lable.disTime;
		lable.disAveHeigth = lable.disArea / lable.disTime;
	}
	lable.span = lable.sysTime + lable.disTime;

/*
	char *sout = (char *)(malloc(sizeof(char) * 512));
	FILE *fp;
	sout = "F://ppg_low.txt";
	fp = fopen(sout, "w");

	if (fp == NULL) {
		printf("Can't save file.");
	}


	for (int i = 0; i < BEATLGTH; i++) {
		fprintf(fp, " %d\n", selectPPG[i]);
	}
	fclose(fp);
*/

sign:	
	FreeArray1D(ppg);

	return lable;
}

int kdrift(int *vout, int *vin, int len, int width, int step) {
	int j;
	int *buf=NULL;

	buf = MakeArray1D(len + width - 1);

	for (j = 0; j < (len + width - 1); ++j) {
		if (j < (width - 1) / 2)
			buf[j] = vin[0];
		else if (j < len + (width - 1) / 2)
			buf[j] = vin[j - (width - 1) / 2];
		else
			buf[j] = vin[len - 1];
	}

	for (j = 0; j < len; ++j) {
		vout[j] = vin[j] - mean(&buf[j], width, step);
	}

	if (buf)
		FreeArray1D(buf);

	return 1;

}
int mean(int *vin, int width, int step)
{
	int sum;
	int i;

	for (i = 0, sum = 0; i < width; i += step)
		sum += vin[i];
	sum /= (width / step);      //  计算vin的均值

	return (sum);
}
/*
static int Max(int *data, unsigned short len)
{
	int max = data[0];
	int i;

	for (i = 1; i < len; i++)
	{
		if (max < data[i])
		{
			max = data[i];
		}
	}

	return max;
}

static int Min(int *data, unsigned short len)
{
	int min = data[0];
	int i;

	for (i = 1; i < len; i++)
	{
		if (min > data[i])
		{
			min = data[i];
		}
	}

	return min;
}
*/
