#pragma once
#include "Def.h"
#include "ecgcodes.h"
#include <stdlib.h>	
#include <stdio.h>
#include "heart_rate.h"
#include "MATCH.H"
#include "common.h" 

#define MATCH_LIMIT 1.3					
#define MATCH_WITH_AMP_LIMIT	2.5	
#define PVC_MATCH_WITH_AMP_LIMIT 0.9 
#define BL_SHIFT_LIMIT	100			
#define NEW_TYPE_NOISE_THRESHOLD	18	
#define NEW_TYPE_HF_NOISE_LIMIT 75	

#define MATCH_NOISE_THRESHOLD	0.7	


#define R2_DI_THRESHOLD 1.0		
#define R3_WIDTH_THRESHOLD	BEAT_MS90		
#define R7_DI_THRESHOLD	1.2		
#define R8_DI_THRESHOLD 1.5		
#define R9_DI_THRESHOLD	2.0		
#define R10_BC_LIM	3				
#define R10_DI_THRESHOLD	2.5	
#define R11_MIN_WIDTH BEAT_MS110		
#define R11_WIDTH_BREAK	BEAT_MS140			
#define R11_WIDTH_DIFF1	BEAT_MS40		
#define R11_WIDTH_DIFF2	BEAT_MS60			
#define R11_HF_THRESHOLD	45		
#define R11_MA_THRESHOLD	14		
#define R11_BC_LIM	1				
#define R15_DI_THRESHOLD	3.5	
#define R15_WIDTH_THRESHOLD BEAT_MS100	
#define R16_WIDTH_THRESHOLD BEAT_MS100	
#define R17_WIDTH_DELTA	BEAT_MS20			
#define R18_DI_THRESHOLD	1.5	
#define R19_HF_THRESHOLD	75		


#define DM_BUFFER_LENGTH	180
#define IRREG_RR_LIMIT	60


// Global Variables
int RRCount , BeatQueCount;
int DomType;
int RecentRRs[8], RecentTypes[8];
extern int BeatTemplates[MAXTYPES][BEATLGTH];
extern int BeatCounts[MAXTYPES];

int Classify(int *newBeat, int isoLevel,int init);

int detectPVB(int *BeatECG,  int isoLevel) {
	int beatType;

	beatType = Classify(BeatECG,  isoLevel, 0);

	return beatType;
}

void ResetBDAC(void)
{
	int i, j;
	int BeatBuffer[BEATLGTH] = {0};
	
	for (i = 0; i < MAXTYPES; i++) {
		for (j = 0; j < BEATLGTH; j++)
			BeatTemplates[i][j] = 0;
	}
	for (i = 0; i < MAXTYPES; i++) {
		BeatCounts[i] = 0;
	}	

	RRCount = 0;

	Classify(BeatBuffer , 0, 1);              //³õÊ¼»¯

	BeatQueCount = 0;
}

int Classify(int *newBeat, int isoLevel, int init) {
	int rhythmClass, i, blShift = 0;
	static int morphType, runCount = 0;
	double matchIndex, mi2;
	int shiftAdj;
	int hfNoise;
	static int lastRhythmClass = UNKNOWN, lastBeatWasNew = 0;
	int noiseLevel = 0;

	if (init)
	{
		ResetMatch();

		morphType = runCount = 0;
		lastBeatWasNew = 0;
		lastRhythmClass = UNKNOWN;

		for (i = 0; i < 8; i++)
		{
			RecentRRs[i] = 0;
			RecentTypes[i] = 0;
		}
		DomType = 0;

		return(0);
	}
	
	hfNoise = 5;
	rhythmClass = NORMAL;



	if ((blShift > BL_SHIFT_LIMIT)
		&& (lastBeatWasNew == 1)
		&& (lastRhythmClass == NORMAL)
		&& (rhythmClass == NORMAL))
		ClearLastNewType();

	lastBeatWasNew = 0;

	BestMorphMatch(newBeat, &morphType, &matchIndex, &mi2, &shiftAdj);

	if (matchIndex < MATCH_NOISE_THRESHOLD)
		hfNoise = noiseLevel = blShift = 0;

	if ((matchIndex < MATCH_LIMIT) && (rhythmClass == PVC) &&
		MinimumBeatVariation(morphType) && (mi2 > PVC_MATCH_WITH_AMP_LIMIT))
	{
		morphType = NewBeatType(newBeat);
		lastBeatWasNew = 1;
	}
	else if ((matchIndex < MATCH_LIMIT) && (mi2 <= MATCH_WITH_AMP_LIMIT))
		UpdateBeatType(morphType, newBeat, mi2, shiftAdj);
	else if ((blShift < BL_SHIFT_LIMIT) && (noiseLevel < NEW_TYPE_NOISE_THRESHOLD)
		&& (hfNoise < NEW_TYPE_HF_NOISE_LIMIT))
	{
		morphType = NewBeatType(newBeat);
		lastBeatWasNew = 1;
	}
	else if ((lastRhythmClass != NORMAL) || (rhythmClass != NORMAL))
	{
		morphType = NewBeatType(newBeat);
		lastBeatWasNew = 1;
	}
	else morphType = MAXTYPES;

	for (i = 7; i > 0; --i)
	{
		RecentRRs[i] = RecentRRs[i - 1];
		RecentTypes[i] = RecentTypes[i - 1];
	}
	RecentRRs[0] = 100;

	RecentTypes[0] = morphType;

	lastRhythmClass = rhythmClass;

	return 1;

}


#define AVELENGTH	BEAT_MS50

#define DM_BUFFER_LENGTH	180

int NewDom, DomRhythm;
int DMBeatTypes[DM_BUFFER_LENGTH], DMBeatClasses[DM_BUFFER_LENGTH];
int DMBeatRhythms[DM_BUFFER_LENGTH];
int DMNormCounts[8], DMBeatCounts[8], DMIrregCount = 0;

int GetNewDominantType(void)
{
	return(NewDom);
}


void AdjustDomData(int oldType, int newType)
{
	int i;

	for (i = 0; i < DM_BUFFER_LENGTH; ++i)
	{
		if (DMBeatTypes[i] == oldType)
			DMBeatTypes[i] = newType;
	}

	if (newType != MAXTYPES)
	{
		DMNormCounts[newType] = DMNormCounts[oldType];
		DMBeatCounts[newType] = DMBeatCounts[oldType];
	}

	DMNormCounts[oldType] = DMBeatCounts[oldType] = 0;

}

void CombineDomData(int oldType, int newType)
{
	int i;

	for (i = 0; i < DM_BUFFER_LENGTH; ++i)
	{
		if (DMBeatTypes[i] == oldType)
			DMBeatTypes[i] = newType;
	}

	if (newType != MAXTYPES)
	{
		DMNormCounts[newType] += DMNormCounts[oldType];
		DMBeatCounts[newType] += DMBeatCounts[oldType];
	}

	DMNormCounts[oldType] = DMBeatCounts[oldType] = 0;
}



