#include <stdio.h>
#include "lablebpalgorithm.h"
#include <math.h>
#include "Def.h"
#include <ctype.h>
#include "heart_rate.h"
#include <string.h>
#include "common.h" 

//#define _CRT_SECURE_NO_WARNINGS

int countline(char *filename);
int countline4txt(char *filename);
int readTxtfile(char *filename, int * ppg, int len);
int readTxtfile2(char *filename, int * systolic, int * diastolic, int len);
int openDAT(char *filename,int *data, int len);
int openDAT32(char *filename, int *data, int len);

int main(int argc, char **argv)
{
	int numdata = 0, i, j, id ;
	int init ;
	int *originppg = NULL;
	originppg = MakeArray1D(unitlen);
	LABEL_BP_PULSE_t lable;               //    分别记录了收缩压、舒张压、脉率和噪音指数                       
	int numppg;
	char* inPath = "D:\\data\\ppg\\开源数据库\\University of Queensland Vital Signs Dataset\\txtfile\\bp\\%s_bpValue.txt";
	char* inPath4ppg = "D:\\data\\ppg\\开源数据库\\University of Queensland Vital Signs Dataset\\txtfile\\%s_%d_ppg.txt";

	char *sin4bp = (char *)(YxAppMemMalloc(sizeof(char) * 512));
	char *sin4data = (char *)(YxAppMemMalloc(sizeof(char) * 512));

	char *sout = (char *)(YxAppMemMalloc(sizeof(char) * 512));
	char filename[] = "D:\\data\\ppg\\开源数据库\\University of Queensland Vital Signs Dataset\\txtfile\\bp\\LIST.TXT";
	char StrLine[1024];

	FILE *fp, *fp1;
	int *ppg = NULL, *systolic = NULL, *diastolic = NULL;
	if ((fp = fopen(filename, "r")) == NULL)
	{
		lemprintf("打开数据文件失败！");
		return 0;
	}

	//read data

	id = 0;
//	while (!feof(fp)){
	for (id = 0; id < 32; id++){
/*
		if (id == 26) {
			continue;
		}
*/
		fgets(StrLine, 1024, fp);  //读取一行

		char *find = strchr(StrLine, '\n');
		if (find) *find = '\0';
		printf("%s\n", StrLine); //输出

		sprintf(sin4bp, inPath, StrLine);

		numdata = countline4txt(sin4bp);

		if (numdata == 0)
			continue;

		systolic = MakeArray1D(numdata);
		diastolic = MakeArray1D(numdata);

		j = readTxtfile2(sin4bp, systolic, diastolic, numdata);

		ppg = MakeArray1D(unitlen);

		char *outPath = "D:\\test\\%s_feature.txt";

		sprintf(sout, outPath, StrLine);

		fp1 = fopen(sout, "w");

		if (fp1 == NULL) {
			printf("Can't save file.");
		}	

		for (i = 0; i < numdata; i++) {
			sprintf(sin4data, inPath4ppg, StrLine,i + 1);
			j = readTxtfile(sin4data, ppg, unitlen);
			lable = featureExtraction(ppg);
			if (lable.disTime < 0) {
				i++;
				i--;	
			}
			
			fprintf(fp1, " %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n", i + 1,  systolic[i], diastolic[i], lable.noise, lable.sysTime, lable.disTime, lable.sysArea, lable.disArea, lable.sysHeigth, lable.disHeigth, lable.sysSlope, lable.disSlope, lable.sysAveHeigth, lable.disAveHeigth, lable.span );
		}

		fclose(fp1);
		FreeArray1D(ppg);
		FreeArray1D(systolic);
		FreeArray1D(diastolic);


		/*
		systolic_cal:初始化时输入的收缩压
		diastolic_cal:初始化时输入的舒张压
		prebp[0] :计算出的收缩压
		prebp[1] :计算出的舒张压

		prebp[2] :计算出的脉率
		prebp[3] :计算出的噪音指数
		*/

		
	}
	
	fclose(fp);

	lemprintf("请按回车\n");
	getchar();//按回车结束  sin4bp

	FreeArray1D(originppg);

	lemfree(sin4bp);
	lemfree(sin4data);
	lemfree(sout);


	return 1;

}

int countline(char *filename) {

	FILE *fp;
	int length = 0;
	fp = fopen(filename, "r");
	if (NULL == fp) {
		lemprintf("Can not open file input.txt!\n");
		return 1;
	}
	fseek(fp, 0L, SEEK_END);    //定位到文件尾部
	length = ftell(fp) / sizeof(short);              //得到文件大小
	fclose(fp);

	return length;
}
int countline4txt(char *filename) {

	FILE *fp;
	int c, num = 0;
	fp = fopen(filename, "r");
	if (NULL == fp) {
		lemprintf("Can not open file input.txt!\n");
		return 0;
	}
	while ((c = fgetc(fp)) != EOF) {
		if (c == '\n')
			num++;
	}
		
	fclose(fp);

	return num;
}

int openDAT(char *filename, int *data,int len) {
	int i, tmp = 0;
	FILE *pfile = NULL;
	pfile = fopen(filename, "rb");
	if (pfile == NULL)
	{
		printf("Can't open file: %s", filename);
		return 0;
	}
	i = 0;
	for (i = 0;i < len;i++){
		fread(&tmp, sizeof(short), 1, pfile);
		data[i] = tmp;
	}
	return 1;
}
int openDAT32(char *filename, int *data, int len) {
	int i, tmp = 0;
	FILE *pfile = NULL;
	pfile = fopen(filename, "rb");
	if (pfile == NULL)
	{
		printf("Can't open file: %s", filename);
		return 0;
	}
	i = 0;
	for (i = 0; i < len; i++) {
		fread(&tmp, sizeof(int), 1, pfile);
		data[i] = tmp;
	}
	return 1;
}
int readTxtfile(char *filename, int * data, int len)
{

	FILE *fp;
	int j;
	if ((fp = fopen(filename, "r")) == NULL)
	{
		lemprintf("打开数据文件失败！");
		return 0;
	}

	//read data
	for (j = 0; j < len; j++)
	{
		fscanf(fp, "%d", data + j);
	}

	fclose(fp);

	return 1;
}

int readTxtfile2(char *filename, int * systolic, int * diastolic, int len)
{

	FILE *fp;
    int j;
	if ((fp = fopen(filename, "r")) == NULL)
	{
		lemprintf("打开数据文件失败！");
		return 0;
	}

	//read data
	for (j = 0; j < len; j++){
		fscanf(fp, "%d,%d", systolic + j, diastolic + j);
	}

	fclose(fp);

	return 1;
}
