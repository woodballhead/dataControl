//动态库，用于PPG数据实时的分析
//edited by woodballhead@163.com  QQ:53271921



#ifndef __ECG_REAL_ANALYSIS_H__
#define __ECG_REAL_ANALYSIS_H__

 int ppg_RealAnalysisFile(int * originppg, int * noiselevel, int len);
 void BestMorphMatch(int *newBeat, int *matchType, double *matchIndex, double *mi2,
	 int *shiftAdj);
 /* Malloc space for 1-D array */
 int * MakeArray1D(int col);
 /* Free space for 1-D array */
 void FreeArray1D(int * in);
 void ResetMatch(void);
 //static int Max(int *data, unsigned short len);
 //static int Min(int *data, unsigned short len);
 static int mean(int *vin, int width, int step);


#endif


