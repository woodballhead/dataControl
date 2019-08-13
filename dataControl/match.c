#include <stdlib.h>
#include <stdio.h>
#include "ecgcodes.h"
#include "heart_rate.h"
#include "Def.h"
#include "common.h" 

#define MATCH_LENGTH	BEAT_MS300	
#define MATCH_LIMIT	1.2			
#define COMBINE_LIMIT	0.8		

#define MATCH_START	(FIDMARK-(MATCH_LENGTH/2))	
#define MATCH_END	(FIDMARK+(MATCH_LENGTH/2))		
#define MAXPREV	8	
#define MAX_SHIFT	BEAT_MS40

// Local prototypes.

double CompareBeats(int *beat1, int *beat2, int *shiftAdj) ;
double CompareBeats2(int *beat1, int *beat2, int *shiftAdj) ;
void UpdateBeat(int *aveBeat, int *newBeat, int shift) ;
void BeatCopy(int srcBeat, int destBeat) ;
int MinimumBeatVariation(int type) ;

// External prototypes.

void AdjustDomData(int oldType, int newType) ;
void CombineDomData(int oldType, int newType) ;

// Global variables.
int BeatTemplates[MAXTYPES][BEATLGTH] ;
int BeatCounts[MAXTYPES] ;
static int BeatWidths[MAXTYPES] ;
static int BeatClassifications[MAXTYPES] ;
static int BeatBegins[MAXTYPES] ;
static int BeatEnds[MAXTYPES] ;
static int BeatsSinceLastMatch[MAXTYPES] ;
static int BeatAmps[MAXTYPES] ;
static int BeatCenters[MAXTYPES] ;
static double MIs[MAXTYPES][8] ;

extern int PostClass[MAXTYPES][8] ;
extern int PCRhythm[MAXTYPES][8] ;

int TypeCount = 0 ;

void ResetMatch(void)
{
	int i, j ;
	TypeCount = 0 ;   
	for(i = 0; i < MAXTYPES; ++i)
	{
		BeatCounts[i] = 0 ;
		BeatClassifications[i] = UNKNOWN ;
		for(j = 0; j < 8; ++j)
		{
			MIs[i][j] = 0 ;
		}
	}
}

#define MATCH_START	(FIDMARK-(MATCH_LENGTH/2))
#define MATCH_END	(FIDMARK+(MATCH_LENGTH/2))

double CompareBeats(int *beat1, int *beat2, int *shiftAdj)
{
	int i, max, min, magSum, shift ;
	long beatDiff, meanDiff, minDiff, minShift ;
	double metric, scaleFactor, tempD ;

	max = min = beat1[MATCH_START] ;
	for(i = MATCH_START+1; i < MATCH_END; ++i)
		if(beat1[i] > max)
			max = beat1[i] ;
		else if(beat1[i] < min)
			min = beat1[i] ;

	magSum = max - min ;

	i = MATCH_START ;
	max = min = beat2[i] ;
	for(i = MATCH_START+1; i < MATCH_END; ++i)
		if(beat2[i] > max)
			max = beat2[i] ;
		else if(beat2[i] < min)
			min = beat2[i] ;

	scaleFactor = magSum ;
	scaleFactor /= max-min ;
	magSum *= 2 ;

	for(shift = -MAX_SHIFT; shift <= MAX_SHIFT; ++shift)
	{
		for(i = FIDMARK-(MATCH_LENGTH>>1), meanDiff = 0;
			i < FIDMARK + (MATCH_LENGTH>>1); ++i)
		{
			tempD = beat2[i+shift] ;
			tempD *= scaleFactor ;
			meanDiff += beat1[i]- tempD ; // beat2[i+shift] ;
		}
		meanDiff /= MATCH_LENGTH ;

		for(i = FIDMARK-(MATCH_LENGTH>>1), beatDiff = 0;
			i < FIDMARK + (MATCH_LENGTH>>1); ++i)
		{
			tempD = beat2[i+shift] ;
			tempD *= scaleFactor ;
			beatDiff += labs(beat1[i] - meanDiff- tempD) ; // beat2[i+shift]  ) ;
		}

		if(shift == -MAX_SHIFT)
		{
			minDiff = beatDiff ;
			minShift = -MAX_SHIFT ;
		}
		else if(beatDiff < minDiff)
		{
			minDiff = beatDiff ;
			minShift = shift ;
		}
	}

	metric = minDiff ;
	*shiftAdj = minShift ;
	metric /= magSum ;


	metric *= 30 ;
	metric /= MATCH_LENGTH ;
	return(metric) ;
}

double CompareBeats2(int *beat1, int *beat2, int *shiftAdj)
{
	int i, max, min, shift ;
	int mag1, mag2 ;
	long beatDiff, meanDiff, minDiff, minShift ;
	double metric ;
	
	max = min = beat1[MATCH_START] ;
	for(i = MATCH_START+1; i < MATCH_END; ++i)
		if(beat1[i] > max)
			max = beat1[i] ;
		else if(beat1[i] < min)
			min = beat1[i] ;

	mag1 = max - min ;

	i = MATCH_START ;
	max = min = beat2[i] ;
	for(i = MATCH_START+1; i < MATCH_END; ++i)
		if(beat2[i] > max)
			max = beat2[i] ;
		else if(beat2[i] < min)
			min = beat2[i] ;

	mag2 = max-min ;


	for(shift = -MAX_SHIFT; shift <= MAX_SHIFT; ++shift)
	{
		for(i = FIDMARK-(MATCH_LENGTH>>1), meanDiff = 0;
			i < FIDMARK + (MATCH_LENGTH>>1); ++i)
			meanDiff += beat1[i]- beat2[i+shift] ;
		meanDiff /= MATCH_LENGTH ;

		for(i = FIDMARK-(MATCH_LENGTH>>1), beatDiff = 0;
			i < FIDMARK + (MATCH_LENGTH>>1); ++i)
			beatDiff += abs(beat1[i] - meanDiff- beat2[i+shift]) ; ;

		if(shift == -MAX_SHIFT)
		{
			minDiff = beatDiff ;
			minShift = -MAX_SHIFT ;
		}
		else if(beatDiff < minDiff)
		{
			minDiff = beatDiff ;
			minShift = shift ;
		}
	}

	metric = minDiff ;
	*shiftAdj = minShift ;
	metric /= (mag1+mag2) ;

	metric *= 30 ;
	metric /= MATCH_LENGTH ;

	return(metric) ;
}

void UpdateBeat(int *aveBeat, int *newBeat, int shift)
{
	int i ;
	long tempLong ;

	for(i = 0; i < BEATLGTH; ++i)
	{
		if((i+shift >= 0) && (i+shift < BEATLGTH))
		{
			tempLong = aveBeat[i] ;
			tempLong *= 7 ;
			tempLong += newBeat[i+shift] ;
			tempLong >>= 3 ;
			aveBeat[i] = tempLong ;
		}
	}
}

int GetTypesCount(void)
{
	return(TypeCount) ;
}

void SetBeatClass(int type, int beatClass)
{
	BeatClassifications[type] = beatClass ;
}

int NewBeatType(int *newBeat )
{
	int i;
	int mcType ;

	for(i = 0; i < TypeCount; ++i)
		++BeatsSinceLastMatch[i] ;

	if(TypeCount < MAXTYPES)
	{
		for(i = 0; i < BEATLGTH; ++i)
			BeatTemplates[TypeCount][i] = newBeat[i] ;

		BeatCounts[TypeCount] = 1 ;
		BeatClassifications[TypeCount] = UNKNOWN ;

		BeatsSinceLastMatch[TypeCount] = 0 ;

		++TypeCount ;
		return(TypeCount-1) ;
	}
	else
	{
		mcType = -1 ;

		if(mcType == -1)
		{
			mcType = 0 ;
			for(i = 1; i < MAXTYPES; ++i)
				if(BeatCounts[i] < BeatCounts[mcType])
					mcType = i ;
				else if(BeatCounts[i] == BeatCounts[mcType])
				{
					if(BeatsSinceLastMatch[i] > BeatsSinceLastMatch[mcType])
						mcType = i ;
				}
		}	

		AdjustDomData(mcType,MAXTYPES) ;

		for(i = 0; i < BEATLGTH; ++i)
			BeatTemplates[mcType][i] = newBeat[i] ;

		BeatCounts[mcType] = 1 ;
		BeatClassifications[mcType] = UNKNOWN ;

		return(mcType) ;
	}
}

void BestMorphMatch(int *newBeat,int *matchType,double *matchIndex, double *mi2,
	int *shiftAdj)
{
	int type, i, bestMatch, nextBest, minShift, shift, temp ;
	int bestShift2, nextShift2 ;
	double bestDiff2, nextDiff2;
	double beatDiff, minDiff, nextDiff=10000 ;

	if(TypeCount == 0)
	{
		*matchType = 0 ;
		*matchIndex = 1000 ;		
		*shiftAdj = 0 ;			
		return ;
	}

	for(type = 0; type < TypeCount; ++type)
	{
		beatDiff = CompareBeats2(&BeatTemplates[type][0],newBeat,&shift) ;
		//   ¼õÉÙÁË²î±ð
//		beatDiff = (beatDiff / 8);                        
		if(type == 0)
		{
			bestMatch = 0 ;
			minDiff = beatDiff ;
			minShift = shift ;
		}
		else if(beatDiff < minDiff)
		{
			nextBest = bestMatch ;
			nextDiff = minDiff ;
			bestMatch = type ;
			minDiff = beatDiff ;
			minShift = shift ;
		}
		else if((TypeCount > 1) && (type == 1))
		{
			nextBest = type ;
			nextDiff = beatDiff ;
		}
		else if(beatDiff < nextDiff)
		{
			nextBest = type ;
			nextDiff = beatDiff ;
		}
	}

	if((minDiff < MATCH_LIMIT) && (nextDiff < MATCH_LIMIT) && (TypeCount > 1))
	{
		bestDiff2 = CompareBeats2(&BeatTemplates[bestMatch][0],newBeat,&bestShift2) ;
		nextDiff2 = CompareBeats2(&BeatTemplates[nextBest][0],newBeat,&nextShift2) ;
		if(nextDiff2 < bestDiff2)
		{
			temp = bestMatch ;
			bestMatch = nextBest ;
			nextBest = temp ;
			temp = minDiff ;
			minDiff = nextDiff ;
			nextDiff = temp ;
			minShift = nextShift2 ;
			*mi2 = bestDiff2 ;
		}
		else *mi2 = nextDiff2 ;

		beatDiff = CompareBeats2(&BeatTemplates[bestMatch][0],&BeatTemplates[nextBest][0],&shift) ;

		if((beatDiff < COMBINE_LIMIT) &&
			((*mi2 < 1.0) || (!MinimumBeatVariation(nextBest))))
		{

			if(bestMatch < nextBest)
			{
				for(i = 0; i < BEATLGTH; ++i)
				{
					if((i+shift > 0) && (i + shift < BEATLGTH))
					{
						BeatTemplates[bestMatch][i] += BeatTemplates[nextBest][i+shift] ;
						BeatTemplates[bestMatch][i] >>= 1 ;
					}
				}

				BeatCounts[bestMatch] += BeatCounts[nextBest] ;

				CombineDomData(nextBest,bestMatch) ;
			
				for(type = nextBest; type < TypeCount-1; ++type)
					BeatCopy(type+1,type) ;
			}		

			else
			{
				for(i = 0; i < BEATLGTH; ++i)
				{
					BeatTemplates[nextBest][i] += BeatTemplates[bestMatch][i] ;
					BeatTemplates[nextBest][i] >>= 1 ;
				}

				BeatCounts[nextBest] += BeatCounts[bestMatch] ;

				CombineDomData(bestMatch,nextBest) ;			

				for(type = bestMatch; type < TypeCount-1; ++type)
					BeatCopy(type+1,type) ;

				bestMatch = nextBest ;
			}
			--TypeCount ;
		}
	}
	*mi2 = CompareBeats2(&BeatTemplates[bestMatch][0],newBeat,&bestShift2) ;
	*matchType = bestMatch ;
	*matchIndex = minDiff ;
	*shiftAdj = minShift ;
}

void UpdateBeatType(int matchType,int *newBeat, double mi2,
	 int shiftAdj)
{
	int i ;

	for(i = 0; i < TypeCount; ++i)
	{
		if(i != matchType)
			++BeatsSinceLastMatch[i] ;
		else BeatsSinceLastMatch[i] = 0 ;
	}

	if(BeatCounts[matchType] == 1)
		for(i = 0; i < BEATLGTH; ++i)
		{
			if((i+shiftAdj >= 0) && (i+shiftAdj < BEATLGTH))
				BeatTemplates[matchType][i] = (BeatTemplates[matchType][i] + newBeat[i+shiftAdj])>>1 ;
		}

	else
		UpdateBeat(&BeatTemplates[matchType][0], newBeat, shiftAdj) ;


	++BeatCounts[matchType] ;

	for(i = MAXPREV-1; i > 0; --i)
		MIs[matchType][i] = MIs[matchType][i-1] ;
	MIs[matchType][0] = mi2 ;

}

void ClearLastNewType(void)
{
	if(TypeCount != 0)
		--TypeCount ;
}

void BeatCopy(int srcBeat, int destBeat)
{
	int i ;

	// Copy template.

	for(i = 0; i < BEATLGTH; ++i)
		BeatTemplates[destBeat][i] = BeatTemplates[srcBeat][i] ;

	// Move feature information.

	BeatCounts[destBeat] = BeatCounts[srcBeat] ;
	BeatWidths[destBeat] = BeatWidths[srcBeat] ;
	BeatCenters[destBeat] = BeatCenters[srcBeat] ;
	for(i = 0; i < MAXPREV; ++i)
	{
		PostClass[destBeat][i] = PostClass[srcBeat][i] ;
		PCRhythm[destBeat][i] = PCRhythm[srcBeat][i] ;
	}


	BeatsSinceLastMatch[destBeat] = BeatsSinceLastMatch[srcBeat];

	// Adjust data in dominant beat monitor.

	AdjustDomData(srcBeat,destBeat) ;
}


int MinimumBeatVariation(int type)
{
	int i ;
	for(i = 0; i < MAXTYPES; ++i)
		if(MIs[type][i] > 0.5)
			i = MAXTYPES+2 ;
	if(i == MAXTYPES)
		return(1) ;
	else return(0) ;
}

#define WIDE_VAR_LIMIT	0.50

int WideBeatVariation(int type)
{
	int i, n ;
	double aveMI ;

	n = BeatCounts[type] ;
	if(n > 8)
		n = 8 ;

	for(i = 0, aveMI = 0; i <n; ++i)
		aveMI += MIs[type][i] ;

	aveMI /= n ;
	if(aveMI > WIDE_VAR_LIMIT)
		return(1) ;
	else return(0) ;
}




