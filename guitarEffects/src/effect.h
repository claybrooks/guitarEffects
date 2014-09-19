#include "wah.h"

#define PI 3.1415926
#define DISTORTION	0
#define CRUNCH 		1
#define TREMOLO		2
#define WAH			3
#define	PHASER		4
#define FLANGE		5
#define REVERB		6
#define CHORUS		7
#define DELAY		8
#define PITCH_SHIFT	9

void queueEffect(int effect);
void clearPipeline();
int processEffect(int val);
void handleInput(int val);
