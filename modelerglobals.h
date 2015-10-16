#ifndef _MODELER_GLOBALS_H
#define _MODELER_GLOBALS_H

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502
#endif

#include "tex.h"

// This is a list of the controls for the myModel
// We'll use these constants to access the values 
// of the controls from the user interface.
enum myModelControls
{ 
	TEST,
	XPOS,
	YPOS,
	ZPOS,
	HEIGHT,
	ROTATE,
	LIGHT_X,
	LIGHT_Y,
	LIGHT_Z,
	LIGHT_INTENSITY,
	UPPER_BODY_X,
	UPPER_BODY_Y,
	UPPER_BODY_Z,
	LOWER_BODY_X,
	LOWER_BODY_Y,
	LOWER_BODY_Z,
	LEFT_UPPER_ARM_X,
	LEFT_UPPER_ARM_Y,
	LEFT_UPPER_ARM_Z,
	RIGHT_UPPER_ARM_X,
	RIGHT_UPPER_ARM_Y,
	RIGHT_UPPER_ARM_Z,
	LEFT_LOWER_ARM_X,
	RIGHT_LOWER_ARM_X,
	LEFT_HAND_X,
	LEFT_HAND_Z,
	LEFT_LINK_MOTION,
	LEFT_THUMB,
	RIGHT_HAND_X,
	RIGHT_HAND_Z,
	RIGHT_LINK_MOTION,
	RIGHT_THUMB,
	LEFT_UPPER_LEG_X,
	LEFT_UPPER_LEG_Y,
	LEFT_UPPER_LEG_Z,
	RIGHT_UPPER_LEG_X,
	RIGHT_UPPER_LEG_Y,
	RIGHT_UPPER_LEG_Z,
	LEFT_LOWER_LEG_X,
	RIGHT_LOWER_LEG_X,
	HEAD_X,
	HEAD_Y,
	HEAD_Z,
	LEFT_FOOT_X,
	RIGHT_FOOT_X,
	ANIMATE,
	GUN,
	HAT,
	CIGAR,
	NOSE_SIZE,
	EYE_COLOR,
	EYE_SIZE,
	EYE_BALL_SIZE,
	EYE_DIST,
	FRAME_ALL,
	LEFT_HAND_CONTRAINT_X,
	LEFT_HAND_CONTRAINT_Y,
	LEFT_HAND_CONTRAINT_Z,
	NUMCONTROLS
};

// Colors
#define COLOR_RED		1.0f, 0.0f, 0.0f
#define COLOR_GREEN		0.0f, 1.0f, 0.0f
#define COLOR_BLUE		0.0f, 0.0f, 1.0f
#define COLOR_SKIN		1.0f, 0.8f, 0.6f

// We'll be getting the instance of the application a lot; 
// might as well have it as a macro.
#define VAL(x) (ModelerApplication::Instance()->GetControlValue(x))

#endif