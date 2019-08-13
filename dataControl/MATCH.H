

int NewBeatType(int *beat) ;
void BestMorphMatch(int *newBeat,int *matchType,double *matchIndex, double *mi2, int *shiftAdj) ;
void UpdateBeatType(int matchType,int *newBeat, double mi2, int shiftAdj) ;
int GetTypesCount(void) ;

void SetBeatClass(int type, int beatClass) ;

void ResetMatch(void) ;
void ClearLastNewType(void) ;

int MinimumBeatVariation(int type) ;

int WideBeatVariation(int type) ;


