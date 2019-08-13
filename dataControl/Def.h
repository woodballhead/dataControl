// Time interval constants.
#define unitlen	1500	  //输入单元数组长度
//#define anlylen	unitlen	  //分析单元数组长度

#define noiseThreshold1	700	  //参数获取阶段噪音阈值
#define noiseThreshold2	800	  //血压计算阶段噪音阈值

#define SAMPLE_RATE	100	/* Sample rate in Hz. */
#define MS_PER_SAMPLE	( (double) 1000/ (double) SAMPLE_RATE)  // 5
#define MS10	((int) (10/ MS_PER_SAMPLE + 0.5))               // 2.5
#define MS25	((int) (25/MS_PER_SAMPLE + 0.5))                // 5.5
#define MS30	((int) (30/MS_PER_SAMPLE + 0.5))                // 6.5
#define MS80	((int) (80/MS_PER_SAMPLE + 0.5))                // 16.5
#define MS95	((int) (95/MS_PER_SAMPLE + 0.5))                // 19.5
#define MS100	((int) (100/MS_PER_SAMPLE + 0.5))               // 20.5
#define MS125	((int) (125/MS_PER_SAMPLE + 0.5))               // 25.5
#define MS150	((int) (150/MS_PER_SAMPLE + 0.5))               // 30.5
#define MS160	((int) (160/MS_PER_SAMPLE + 0.5))               // 32.5
#define MS175	((int) (175/MS_PER_SAMPLE + 0.5))               // 35.5
#define MS195	((int) (195/MS_PER_SAMPLE + 0.5))               // 39.5
#define MS200	((int) (200/MS_PER_SAMPLE + 0.5))               // 40.5
#define MS220	((int) (220/MS_PER_SAMPLE + 0.5))               // 44.5
#define MS230	((int) (230/MS_PER_SAMPLE + 0.5))               // 44.5
#define MS250	((int) (250/MS_PER_SAMPLE + 0.5))               // 50.5
#define MS275	((int) (275/MS_PER_SAMPLE + 0.5))               // 50.5
#define MS300	((int) (300/MS_PER_SAMPLE + 0.5))               // 60.5
#define MS360	((int) (360/MS_PER_SAMPLE + 0.5))               // 72.5
#define MS450	((int) (450/MS_PER_SAMPLE + 0.5))               // 90.5
#define MS1000	SAMPLE_RATE
#define MS1500	((int) (1500/MS_PER_SAMPLE))                    // 300

#define BEAT_SAMPLE_RATE	100
#define BEAT_MS_PER_SAMPLE	( (double) 1000/ (double) BEAT_SAMPLE_RATE)


#define BEAT_MS10		((int) (10/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS20		((int) (20/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS40		((int) (40/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS50		((int) (50/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS60		((int) (60/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS70		((int) (70/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS80		((int) (80/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS90		((int) (90/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS100	((int) (100/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS110	((int) (110/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS130	((int) (130/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS140	((int) (140/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS150	((int) (150/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS250	((int) (250/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS280	((int) (280/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS300	((int) (300/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS350	((int) (350/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS400	((int) (400/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS500	((int) (500/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS1000	BEAT_SAMPLE_RATE
#define ECG_BUFFER_LENGTH	SAMPLE_RATE
#define BEATLGTH	BEAT_MS1000
#define MAXTYPES 8
#define FIDMARK BEAT_MS400
#define DERIV_LENGTH	MS10
#define LPBUFFER_LGTH ((int) (2*MS25))                          // 10
#define HPBUFFER_LGTH MS125

#define ISO_LENGTH1  BEAT_MS50
#define ISO_LENGTH2	BEAT_MS80
#define ISO_LIMIT	20
#define INF_CHK_N	BEAT_MS40

#define centralPoint  2 * BEATLGTH / 5
/*
#define WINDOW_WIDTH	MS80			// Moving window integration width.
#define	FILTER_DELAY (int) (((double) DERIV_LENGTH/2) + ((double) LPBUFFER_LGTH/2 - 1) + (((double) HPBUFFER_LGTH-1)/2) + MS275)  
#define DER_DELAY	WINDOW_WIDTH + FILTER_DELAY + MS100
*/




