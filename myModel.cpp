// my model.  
#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include "camera.h"

#include <FL/gl.h>

#include "modelerglobals.h"

#define  MAX_COUNT    60

//static GLfloat lightPosition0[] = { VAL(LIGHT_X), VAL(LIGHT_Y), VAL(LIGHT_Z),0 };// { 4, 2, -4, 0 };  
//static GLfloat lightDiffuse0[] = { 1, 1, 1, 1 };
static GLfloat lightPosition1[] = { -8, 0, -20, 0 };
static GLfloat lightDiffuse1[] = { 1, 1, 1, 1 };

// To make a MyModel, we inherit off of ModelerView
class MyModel : public ModelerView 
{
public:
	MyModel(int x, int y, int w, int h, char *label)
		: ModelerView(x, y, w, h, label), animateCounter(0) {
		int i = 0;
		for (int j = 0; j < MAX_COUNT / 4; i++, j++){
			leftLegX[i] = -45 * j / (MAX_COUNT / 4);
			body[i] = leftLegX[i] * 0.05;
			light[i] = 0.5 + 2*j / (MAX_COUNT / 4);
		}
		for (int j = 0; j < MAX_COUNT / 4; i++, j++){
			leftLegX[i] = -45 + 45 * j / (MAX_COUNT / 4);
			body[i] = leftLegX[i] * 0.05;
			light[i] = 2.5 - 2 * j / (MAX_COUNT / 4);
		}
		for (int j = 0; j < MAX_COUNT / 4; i++, j++){
			leftLegX[i] = 45 * j / (MAX_COUNT / 4);
			body[i] = -leftLegX[i] * 0.05;
			light[i] = 0.5 + 2 * j / (MAX_COUNT / 4);
		}
		for (int j = 0; j < MAX_COUNT / 4; i++, j++){
			leftLegX[i] = 45 - 45 * j / (MAX_COUNT / 4);
			body[i] = -leftLegX[i] * 0.05;
			light[i] = 2.5 - 2 * j / (MAX_COUNT / 4);
		}
		for (i = 0; i < MAX_COUNT; i++){
			rightLegX[i] = -leftLegX[i];
		}

	}

	
    virtual void draw();
private:
	int animateCounter;
	int leftLegX[60];
	int rightLegX[60];
	float body[60];
	float light[60];
};

// We need to make a creator function, mostly because of
// nasty API stuff that we'd rather stay away from.
ModelerView* createMyModel(int x, int y, int w, int h, char *label)
{ 
	return new MyModel(x, y, w, h, label);
}

// We are going to override (is that the right word?) the draw()
// method of ModelerView to draw out MyModel
void MyModel::draw()
{

	if (VAL(FRAME_ALL)){
		ModelerView::m_camera->setDolly(-50.0);
	}
    // This call takes care of a lot of the nasty projection 
    // matrix stuff.  Unless you want to fudge directly with the 
	// projection matrix, don't bother with this ...
    ModelerView::draw();
	
	GLfloat lightPosition0[] = { VAL(LIGHT_X), VAL(LIGHT_Y), VAL(LIGHT_Z), 0 };
	
	GLfloat lightDiffuse0[] = { VAL(LIGHT_INTENSITY), VAL(LIGHT_INTENSITY), VAL(LIGHT_INTENSITY), 1 };
	
	if (VAL(ANIMATE)){
		lightDiffuse0[0] = light[animateCounter];
		lightDiffuse0[1] = light[animateCounter];
		lightDiffuse0[2] = light[animateCounter];
		lightDiffuse0[3] = 1;
	}

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse0);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse1);


	// draw the sample model
	setAmbientColor(0.1, 0.1, 0.1);
	setDiffuseColor(COLOR_BLUE);
	glPushMatrix();
	glTranslated(VAL(XPOS), VAL(YPOS)+body[animateCounter], VAL(ZPOS));
		
		// center of the body
		glPushMatrix();
		glScaled(2, 1, 1);
		drawSphere(1);
		glScaled(0.5, 1, 1);

			// Upper Body		
			glPushMatrix();
			glRotated(VAL(UPPER_BODY_X), 1.0, 0.0, 0.0);
			glRotated(VAL(UPPER_BODY_Y), 0.0, 1.0, 0.0);
			glRotated(VAL(UPPER_BODY_Z), 0.0, 0.0, 1.0);
			glRotated(-90, 1.0, 0.0, 0.0);
			glScaled(2, 1.4, 1);
			drawCylinderWithTexture(4, 1, 1.2, "./images/shirt.bmp");
			glScaled(0.5, 1 / 1.4, 1);
				// Left upper arm pivot
				setDiffuseColor(COLOR_SKIN);
				glPushMatrix();
				glTranslated(2.2, 0, 3.2);
				drawSphere(0.8);
					// Left upper arm
					glRotated(90, 1.0, 0.0, 0.0);
					glRotated(VAL(LEFT_UPPER_ARM_X) - VAL(LEFT_LINK_MOTION) * 70 / 90 + rightLegX[animateCounter], 1.0, 0.0, 0.0);
					glRotated(VAL(LEFT_UPPER_ARM_Y) - VAL(LEFT_LINK_MOTION) * 35 / 90, 0.0, 1.0, 0.0);
					glRotated(VAL(LEFT_UPPER_ARM_Z) + VAL(LEFT_LINK_MOTION) * 25 / 90, 0.0, 0.0, 1.0);
					glRotated(90, 1.0, 0.0, 0.0);
					if (VAL(META_ARM)){
						glScaled(0.2, 0.2, 0.5);
						glRotated(-90, 0.0, 1.0, 0.0);
						drawMetaarm(VAL(META_ARM_SIZE));
						glRotated(90, 0.0, 1.0, 0.0);
						glScaled(5, 5, 2);
					}
					else{
						drawCylinder(4.5, 0.8, 0.6);
					}
						// Left lower arm pivot
						glTranslated(0, 0, 4.5);
						drawSphere(0.6);
							// Left lower arm
							glRotated(-90, 1.0, 0.0, 0.0);
							glRotated(VAL(LEFT_LOWER_ARM_X) - VAL(LEFT_LINK_MOTION)*130/90, 1.0, 0.0, 0.0);
							glRotated(90, 1.0, 0.0, 0.0);
							drawCylinder(3.5,0.6,0.4);
								// Left hand pivot
								glTranslated(0, 0, 3.5);
								drawSphere(0.4);
									// Left hand
									glRotated(VAL(LEFT_HAND_X), 1.0, 0.0, 0.0);
									glRotated(VAL(LEFT_HAND_Z), 0.0, 1.0, 0.0);
									glTranslated(-0.2, -0.5, 0);
									drawBox(0.4, 1, 1.2);
										// Gun	
										if (VAL(GUN)){
											setDiffuseColor(0.55, 0.52, 0.41);
											glPushMatrix();
											glTranslated(-0.4, 0, 0.2);
											drawGun();
											glPopMatrix();
											setDiffuseColor(COLOR_SKIN);
										}
										// Left thumb
										glPushMatrix();
										glTranslated(0, 1, 0);
										glTranslated(0.2, 0, 0);
										glRotated(VAL(LEFT_THUMB), 1.0, 0.0, 0.0);
										glTranslated(-0.2, 0, 0);
										drawBox(0.4,0.4,0.6);
										glPopMatrix();   
				glPopMatrix();

				// Right upper arm pivot
				glPushMatrix();
				glTranslated(-2.2, 0, 3.2);
				drawSphere(0.8);
					// Right upper arm
					glRotated(90, 1.0, 0.0, 0.0);
					glRotated(VAL(RIGHT_UPPER_ARM_X) - VAL(RIGHT_LINK_MOTION) * 70 / 90 + leftLegX[animateCounter], 1.0, 0.0, 0.0);
					glRotated(VAL(RIGHT_UPPER_ARM_Y) + VAL(RIGHT_LINK_MOTION) * 35 / 90, 0.0, 1.0, 0.0);
					glRotated(VAL(RIGHT_UPPER_ARM_Z) - VAL(RIGHT_LINK_MOTION) * 25 / 90, 0.0, 0.0, 1.0);
					glRotated(90, 1.0, 0.0, 0.0);
					if (VAL(META_ARM)){
						glScaled(0.2, 0.2, 0.5);
						glRotated(-90, 0.0, 1.0, 0.0);
						drawMetaarm(VAL(META_ARM_SIZE));
						glRotated(90, 0.0, 1.0, 0.0);
						glScaled(5, 5, 2);
					}
					else{
						drawCylinder(4.5, 0.8, 0.6);
					}						
						// Right lower arm pivot
						glTranslated(0, 0, 4.5);
						drawSphere(0.6);
							// Right lower arm
							glRotated(-90, 1.0, 0.0, 0.0);
							glRotated(VAL(RIGHT_LOWER_ARM_X) - VAL(RIGHT_LINK_MOTION) * 130 / 90, 1.0, 0.0, 0.0);
							glRotated(90, 1.0, 0.0, 0.0);
							drawCylinder(3.5, 0.6, 0.4);
								// Right hand pivot
								glTranslated(0, 0, 3.5);
								drawSphere(0.4);
									// Right hand
									glRotated(VAL(RIGHT_HAND_X), 1.0, 0.0, 0.0);
									glRotated(VAL(RIGHT_HAND_Z), 0.0, 1.0, 0.0);
									glTranslated(-0.2, -0.5, 0);
									drawBox(0.4, 1, 1.2);
										// Right thumb
										glTranslated(0, 1, 0);
										glTranslated(0.2, 0, 0);
										glRotated(VAL(RIGHT_THUMB), 1.0, 0.0, 0.0);
										glTranslated(-0.2, 0, 0);
										drawBox(0.4, 0.4, 0.6);
				glPopMatrix();

				// Neck pivot
				glPushMatrix();				
					glTranslated(0,0,4);
					drawSphere(0.7);
						// Neck
						glRotated(90, 1, 0, 0);
						glRotated(VAL(HEAD_X), 1, 0, 0);
						glRotated(VAL(HEAD_Y), 0, 1, 0);
						glRotated(VAL(HEAD_Z), 0, 0, 1);						
						glRotated(-90, 1, 0, 0);
						drawCylinder(1,0.7,0.7);
							// head
							glTranslated(0,0,1.5);
							glScaled(1,1,1.5);
							drawHead(VAL(NOSE_SIZE), VAL(EYE_COLOR), VAL(EYE_SIZE), VAL(EYE_BALL_SIZE), VAL(EYE_DIST));
							glScaled(1, 1, 1.0/1.5);
								// hat
								if (VAL(HAT)){
									setDiffuseColor(0.76, 0.65, 0.19);
									glPushMatrix();
									glTranslated(0, 0, 1.8);
									glRotated(90, 1.0, 0.0, 0.0);									
									drawHat(1, 2);
									glPopMatrix();
									setDiffuseColor(COLOR_SKIN);
								}
								// cigar
								if (VAL(CIGAR)){
									setDiffuseColor(0.90, 0.40, 0.02);
									glPushMatrix();
									glTranslated(0, 0, -0.2);
									glRotated(90, 1.0, 0.0, 0.0);
									drawCylinder(1.5, 0.1, 0.15);
									glPopMatrix();
									setDiffuseColor(COLOR_SKIN);
								}
							setDiffuseColor(COLOR_SKIN);
				glPopMatrix();
			glPopMatrix();

			// Lower Body
			setDiffuseColor(COLOR_BLUE);
			glPushMatrix();
			glRotated(VAL(LOWER_BODY_X), 1.0, 0.0, 0.0);
			glRotated(VAL(LOWER_BODY_Y), 0.0, 1.0, 0.0);
			glRotated(VAL(LOWER_BODY_Z), 0.0, 0.0, 1.0);
			glRotated(90, 1.0, 0.0, 0.0);
			glScaled(2, 1.4, 1);
			drawCylinderWithTexture(3, 1, 1.2, "./images/trouser.bmp");
			glScaled(0.5, 1/1.4, 1);
				// Left upper leg pivot
				setDiffuseColor(COLOR_SKIN);
				glPushMatrix(); 
				glTranslated(1, 0, 3);
				drawSphere(1.0);
					// Left upper leg
					glRotated(-90, 1.0, 0.0, 0.0);
					glRotated(VAL(LEFT_UPPER_LEG_X)+leftLegX[animateCounter], 1.0, 0.0, 0.0);
					glRotated(VAL(LEFT_UPPER_LEG_Y), 0.0, 1.0, 0.0);
					glRotated(VAL(LEFT_UPPER_LEG_Z), 0.0, 0.0, 1.0);
					glRotated(90, 1.0, 0.0, 0.0);
					drawCylinder(4,1,0.8);
						// Left lower leg pivot
						glTranslated(0, 0, 4);
						drawSphere(0.8);
							// Left lower leg						
							glRotated(VAL(LEFT_LOWER_LEG_X), 1.0, 0.0, 0.0);
							drawCylinder(4,0.8,0.6);
								// Left foot pivot
								glTranslated(0, 0, 4);
								drawSphere(0.6);
									// Left foot
									setDiffuseColor(COLOR_RED);
									glRotated(VAL(LEFT_FOOT_X), 1.0, 0.0, 0.0);
									glTranslated(-0.6,-0.7,0);
									drawBox(1.2,2.6,0.6);
									setDiffuseColor(COLOR_SKIN);
				glPopMatrix();

				// Right upper leg pivot
				glPushMatrix();
				glTranslated(-1, 0, 3);
				drawSphere(1.0);
					// Right upper leg
					glRotated(-90, 1.0, 0.0, 0.0);
					glRotated(VAL(RIGHT_UPPER_LEG_X) + rightLegX[animateCounter], 1.0, 0.0, 0.0);
					glRotated(VAL(RIGHT_UPPER_LEG_Y), 0.0, 1.0, 0.0);
					glRotated(VAL(RIGHT_UPPER_LEG_Z), 0.0, 0.0, 1.0);					
					glRotated(90, 1.0, 0.0, 0.0);
					drawCylinder(4, 1, 0.8);
						// Right lower leg pivot
						glTranslated(0, 0, 4);
						drawSphere(0.8);
							// Right lower leg						
							glRotated(VAL(RIGHT_LOWER_LEG_X), 1.0, 0.0, 0.0);
							drawCylinder(4, 0.8, 0.6);
								// Right foot pivot
								glTranslated(0,0,4);
								drawSphere(0.6);
									//Right foot
									setDiffuseColor(COLOR_RED);
									glRotated(VAL(RIGHT_FOOT_X), 1.0, 0.0, 0.0);
									glTranslated(-0.6, -0.7, 0);
									drawBox(1.2, 2.6, 0.6);
				glPopMatrix();

			glPopMatrix();
		glPopMatrix();
	glPopMatrix();

	if (VAL(ANIMATE))
		animateCounter++;
	if (animateCounter >= MAX_COUNT)
		animateCounter = 0;
	
}

int main()
{
	// Initialize the controls
	// Constructor is ModelerControl(name, minimumvalue, maximumvalue, 
	// stepsize, defaultvalue)
    ModelerControl controls[NUMCONTROLS];
	controls[TEST] = ModelerControl("Test", 0, 1, 0.1f, 1);

    controls[XPOS] = ModelerControl("X Position", -5, 5, 0.1f, 0);
    controls[YPOS] = ModelerControl("Y Position", 0, 5, 0.1f, 0);
    controls[ZPOS] = ModelerControl("Z Position", -5, 5, 0.1f, 0);
    controls[HEIGHT] = ModelerControl("Height", 1, 2.5, 0.1f, 1);
	controls[ROTATE] = ModelerControl("Rotate", -135, 135, 1, 0);
	controls[LIGHT_INTENSITY] = ModelerControl("Light Intensity", 0, 5, 0.1f, 1);
	controls[LIGHT_X] = ModelerControl("Light X", -30, 30, 1, 4);
	controls[LIGHT_Y] = ModelerControl("Light Y", -30, 30, 1, 2);
	controls[LIGHT_Z] = ModelerControl("Light Z", -30, 30, 1, 4);
	controls[UPPER_BODY_X] = ModelerControl("Upper Body X", -90, 90, 1, 0);
	controls[UPPER_BODY_Y] = ModelerControl("Upper Body Y", -90, 90, 1, 0);
	controls[UPPER_BODY_Z] = ModelerControl("Upper Body Z", -45, 45, 1, 0);
	controls[LOWER_BODY_X] = ModelerControl("Lower Body X", -90, 90, 1, 0);
	controls[LOWER_BODY_Y] = ModelerControl("Lower Body Y", -90, 90, 1, 0);
	controls[LOWER_BODY_Z] = ModelerControl("Lower Body Z", -45, 45, 1, 0);
	controls[LEFT_UPPER_ARM_X] = ModelerControl("Left Upper Arm X", -180, 45, 1, -15); // -15
	controls[LEFT_UPPER_ARM_Y] = ModelerControl("Left Upper Arm Y", -180, 90, 1, 0);
	controls[LEFT_UPPER_ARM_Z] = ModelerControl("Left Upper Arm Z", 0, 180, 1, 15);  //15
	controls[RIGHT_UPPER_ARM_X] = ModelerControl("Right Upper Arm X", -180, 45, 1, -15); //-15
	controls[RIGHT_UPPER_ARM_Y] = ModelerControl("Right Upper Arm Y", -90, 180, 1, 0);
	controls[RIGHT_UPPER_ARM_Z] = ModelerControl("Right Upper Arm Z", -180, 0, 1, -15);//15
	controls[LEFT_LOWER_ARM_X] = ModelerControl("Left Lower Arm X", -140, 0, 1, 0);
	controls[RIGHT_LOWER_ARM_X] = ModelerControl("Right Lower Arm X", -140, 0, 1, 0); 
	controls[LEFT_HAND_X] = ModelerControl("Left Hand X", -30, 30, 1, 0); 
	controls[LEFT_HAND_Z] = ModelerControl("Left Hand Z", -60, 60, 1, 0);
	controls[LEFT_LINK_MOTION] = ModelerControl("Left Link Motion", 0, 90, 1, 0);
	controls[RIGHT_HAND_X] = ModelerControl("Right Hand X", -30, 30, 1, 0);
	controls[RIGHT_HAND_Z] = ModelerControl("Right Hand Z", -60, 60, 1, 0);
	controls[RIGHT_LINK_MOTION] = ModelerControl("Right Link Motion", 0, 90, 1, 0);
	controls[LEFT_UPPER_LEG_X] = ModelerControl("Left Upper Leg X", -90, 45, 1, 0);
	controls[LEFT_UPPER_LEG_Y] = ModelerControl("Left Upper Leg Y", -45, 90, 1, 0);
	controls[LEFT_UPPER_LEG_Z] = ModelerControl("Left Upper Leg Z", 0, 45, 1, 0);
	controls[RIGHT_UPPER_LEG_X] = ModelerControl("Right Upper Leg X", -90, 45, 1, 0);
	controls[RIGHT_UPPER_LEG_Y] = ModelerControl("Right Upper Leg Y", -90, 45, 1, 0);
	controls[RIGHT_UPPER_LEG_Z] = ModelerControl("Right Upper Leg Z", -45, 0, 1, 0);
	controls[LEFT_LOWER_LEG_X] = ModelerControl("Left Lower Leg X", 0, 160, 1, 0);
	controls[RIGHT_LOWER_LEG_X] = ModelerControl("Right Lower Leg X", 0, 160, 1, 0);
	controls[HEAD_X] = ModelerControl("Head X", -60, 60, 1, 0);
	controls[HEAD_Y] = ModelerControl("Head Y", -90, 90, 1, 0);
	controls[HEAD_Z] = ModelerControl("Head Z", -45, 45, 1, 0);
	controls[LEFT_FOOT_X] = ModelerControl("Left Foot X", -40, 40, 1, 0);
	controls[RIGHT_FOOT_X] = ModelerControl("Right Foot X", -40, 40, 1, 0);
	controls[LEFT_HAND_X] = ModelerControl("Left Hand X", -35, 35, 1, 0);
	controls[LEFT_HAND_Z] = ModelerControl("Left Hand Z", -60, 60, 1, 0);
	controls[RIGHT_HAND_X] = ModelerControl("Right Hand X", -35, 35, 1, 0);
	controls[RIGHT_HAND_Z] = ModelerControl("Right Hand Z", -60, 60, 1, 0);
	controls[LEFT_THUMB] = ModelerControl("Left Thumb", -90, 0, 1, 0);
	controls[RIGHT_THUMB] = ModelerControl("Right Thumb", -90, 0, 1, 0);
	controls[GUN] = ModelerControl("Gun?", 0, 1, 1, 0);
	controls[HAT] = ModelerControl("Hat?", 0, 1, 1, 0);
	controls[CIGAR] = ModelerControl("Cigar", 0, 1, 1, 0);
	controls[NOSE_SIZE] = ModelerControl("Nose Size", 1, 1.5, 0.1, 1);
	controls[EYE_COLOR] = ModelerControl("Eye Color", 0, 2, 1, 0);
	controls[EYE_SIZE] = ModelerControl("Eye Size", 1, 2, 0.1f, 1);
	controls[EYE_BALL_SIZE] = ModelerControl("Eye Ball Size", 1, 1.5, 0.1f, 1);
	controls[EYE_DIST] = ModelerControl("Eye Distance", 0.2, 0.5, 0.05, 0.3f);
	controls[FRAME_ALL] = ModelerControl("Frame All?", 0, 1, 1, 0);
	controls[LEFT_HAND_CONTRAINT_X] = ModelerControl("Left Hand Constraint X", 0, 1, 1, 0);
	controls[LEFT_HAND_CONTRAINT_Y] = ModelerControl("Left Hand Constraint Y", 0, 1, 1, 0);
	controls[LEFT_HAND_CONTRAINT_Z] = ModelerControl("Left Hand Constraint Z", 0, 1, 1, 0);
	controls[META_ARM] = ModelerControl("Meta Arm?", 0, 1, 1, 0);
	controls[META_ARM_SIZE] = ModelerControl("Meta Arm Size", 6, 25, 0.1f, 8.5);

	controls[ANIMATE] = ModelerControl("Animate", 0, 1, 1, 0);

    ModelerApplication::Instance()->Init(&createMyModel, controls, NUMCONTROLS);
    return ModelerApplication::Instance()->Run();
}
