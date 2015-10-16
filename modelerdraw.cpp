#include "modelerdraw.h"
//#include "modelerglobals.h"
#include <FL/gl.h>
#include <GL/glu.h>
#include <cstdio>
#include <math.h>
#include "cube.h"
#include "metaballs.h"


// ********************************************************
// Support functions from previous version of modeler
// ********************************************************
void _dump_current_modelview( void )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    if (mds->m_rayFile == NULL)
    {
        fprintf(stderr, "No .ray file opened for writing, bailing out.\n");
        exit(-1);
    }
    
    GLdouble mv[16];
    glGetDoublev( GL_MODELVIEW_MATRIX, mv );
    fprintf( mds->m_rayFile, 
        "transform(\n    (%f,%f,%f,%f),\n    (%f,%f,%f,%f),\n     (%f,%f,%f,%f),\n    (%f,%f,%f,%f),\n",
        mv[0], mv[4], mv[8], mv[12],
        mv[1], mv[5], mv[9], mv[13],
        mv[2], mv[6], mv[10], mv[14],
        mv[3], mv[7], mv[11], mv[15] );
}

void _dump_current_material( void )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    if (mds->m_rayFile == NULL)
    {
        fprintf(stderr, "No .ray file opened for writing, bailing out.\n");
        exit(-1);
    }
    
    fprintf( mds->m_rayFile, 
        "material={\n    diffuse=(%f,%f,%f);\n    ambient=(%f,%f,%f);\n}\n",
        mds->m_diffuseColor[0], mds->m_diffuseColor[1], mds->m_diffuseColor[2], 
        mds->m_diffuseColor[0], mds->m_diffuseColor[1], mds->m_diffuseColor[2]);
}

// ****************************************************************************

// Initially assign singleton instance to NULL
ModelerDrawState* ModelerDrawState::m_instance = NULL;

ModelerDrawState::ModelerDrawState() : m_drawMode(NORMAL), m_quality(MEDIUM)
{
    float grey[]  = {.5f, .5f, .5f, 1};
    float white[] = {1,1,1,1};
    float black[] = {0,0,0,1};
    
    memcpy(m_ambientColor, black, 4 * sizeof(float));
    memcpy(m_diffuseColor, grey, 4 * sizeof(float));
    memcpy(m_specularColor, white, 4 * sizeof(float));
    
    m_shininess = 0.5;
    
    m_rayFile = NULL;
}

// CLASS ModelerDrawState METHODS
ModelerDrawState* ModelerDrawState::Instance()
{
    // Return the singleton if it exists, otherwise, create it
    return (m_instance) ? (m_instance) : m_instance = new ModelerDrawState();
}

// ****************************************************************************
// Modeler functions for your use
// ****************************************************************************
// Set the current material properties

void setAmbientColor(float r, float g, float b)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_ambientColor[0] = (GLfloat)r;
    mds->m_ambientColor[1] = (GLfloat)g;
    mds->m_ambientColor[2] = (GLfloat)b;
    mds->m_ambientColor[3] = (GLfloat)1.0;  
    
    if (mds->m_drawMode == NORMAL)
        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mds->m_ambientColor);
}

void setDiffuseColor(float r, float g, float b)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_diffuseColor[0] = (GLfloat)r;
    mds->m_diffuseColor[1] = (GLfloat)g;
    mds->m_diffuseColor[2] = (GLfloat)b;
    mds->m_diffuseColor[3] = (GLfloat)1.0;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mds->m_diffuseColor);
    else
        glColor3f(r,g,b);
}

void setSpecularColor(float r, float g, float b)
{	
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_specularColor[0] = (GLfloat)r;
    mds->m_specularColor[1] = (GLfloat)g;
    mds->m_specularColor[2] = (GLfloat)b;
    mds->m_specularColor[3] = (GLfloat)1.0;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mds->m_specularColor);
}

void setShininess(float s)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_shininess = (GLfloat)s;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialf( GL_FRONT, GL_SHININESS, mds->m_shininess);
}

void setDrawMode(DrawModeSetting_t drawMode)
{
    ModelerDrawState::Instance()->m_drawMode = drawMode;
}

void setQuality(QualitySetting_t quality)
{
    ModelerDrawState::Instance()->m_quality = quality;
}

bool openRayFile(const char rayFileName[])
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	fprintf(stderr, "Ray file format output is buggy (ehsu)\n");
    
    if (!rayFileName)
        return false;
    
    if (mds->m_rayFile) 
        closeRayFile();
    
    mds->m_rayFile = fopen(rayFileName, "w");
    
    if (mds->m_rayFile != NULL) 
    {
        fprintf( mds->m_rayFile, "SBT-raytracer 1.0\n\n" );
        fprintf( mds->m_rayFile, "camera { fov=30; position=(0,0.8,5); direction=(0,-0.8,-5); }\n\n" );
        fprintf( mds->m_rayFile, 
            "directional_light { direction=(-1,-2,-1); color=(0.7,0.7,0.7); }\n\n" );
        return true;
    }
    else
        return false;
}

void _setupOpenGl()
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
	switch (mds->m_drawMode)
	{
	case NORMAL:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glShadeModel(GL_SMOOTH);
		break;
	case FLATSHADE:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glShadeModel(GL_FLAT);
		break;
	case WIREFRAME:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glShadeModel(GL_FLAT);
	default:
		break;
	}

}

void closeRayFile()
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    if (mds->m_rayFile) 
        fclose(mds->m_rayFile);
    
    mds->m_rayFile = NULL;
}

void drawSphere(double r)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	_setupOpenGl();
    
    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile, "scale(%f,%f,%f,sphere {\n", r, r, r );
        _dump_current_material();
        fprintf(mds->m_rayFile, "}))\n" );
    }
    else
    {
        int divisions; 
        GLUquadricObj* gluq;
        
        switch(mds->m_quality)
        {
        case HIGH: 
            divisions = 32; break;
        case MEDIUM: 
            divisions = 20; break;
        case LOW:
            divisions = 12; break;
        case POOR:
            divisions = 8; break;
        }
        
        gluq = gluNewQuadric();
        gluQuadricDrawStyle( gluq, GLU_FILL );
        gluQuadricTexture( gluq, GL_TRUE );
        gluSphere(gluq, r, divisions, divisions);
        gluDeleteQuadric( gluq );
    }
}


void drawBox( double x, double y, double z )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	_setupOpenGl();
    
    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile,  
            "scale(%f,%f,%f,translate(0.5,0.5,0.5,box {\n", x, y, z );
        _dump_current_material();
        fprintf(mds->m_rayFile,  "})))\n" );
    }
    else
    {
        /* remember which matrix mode OpenGL was in. */
        int savemode;
        glGetIntegerv( GL_MATRIX_MODE, &savemode );
        
        /* switch to the model matrix and scale by x,y,z. */
        glMatrixMode( GL_MODELVIEW );
        glPushMatrix();
        glScaled( x, y, z );
        
        glBegin( GL_QUADS );
        
        glNormal3d( 0.0, 0.0, -1.0 );
        glVertex3d( 0.0, 0.0, 0.0 ); glVertex3d( 0.0, 1.0, 0.0 );
        glVertex3d( 1.0, 1.0, 0.0 ); glVertex3d( 1.0, 0.0, 0.0 );
        
        glNormal3d( 0.0, -1.0, 0.0 );
        glVertex3d( 0.0, 0.0, 0.0 ); glVertex3d( 1.0, 0.0, 0.0 );
        glVertex3d( 1.0, 0.0, 1.0 ); glVertex3d( 0.0, 0.0, 1.0 );
        
        glNormal3d( -1.0, 0.0, 0.0 );
        glVertex3d( 0.0, 0.0, 0.0 ); glVertex3d( 0.0, 0.0, 1.0 );
        glVertex3d( 0.0, 1.0, 1.0 ); glVertex3d( 0.0, 1.0, 0.0 );
        
        glNormal3d( 0.0, 0.0, 1.0 );
        glVertex3d( 0.0, 0.0, 1.0 ); glVertex3d( 1.0, 0.0, 1.0 );
        glVertex3d( 1.0, 1.0, 1.0 ); glVertex3d( 0.0, 1.0, 1.0 );
        
        glNormal3d( 0.0, 1.0, 0.0 );
        glVertex3d( 0.0, 1.0, 0.0 ); glVertex3d( 0.0, 1.0, 1.0 );
        glVertex3d( 1.0, 1.0, 1.0 ); glVertex3d( 1.0, 1.0, 0.0 );
        
        glNormal3d( 1.0, 0.0, 0.0 );
        glVertex3d( 1.0, 0.0, 0.0 ); glVertex3d( 1.0, 1.0, 0.0 );
        glVertex3d( 1.0, 1.0, 1.0 ); glVertex3d( 1.0, 0.0, 1.0 );
        
        glEnd();
        
        /* restore the model matrix stack, and switch back to the matrix
        mode we were in. */
        glPopMatrix();
        glMatrixMode( savemode );
    }
}

void drawTextureBox( double x, double y, double z )
{
    // NOT IMPLEMENTED, SORRY (ehsu)
}

void drawCylinderWithTexture(double h, double r1, double r2, const char* dir)
{
	ModelerDrawState *mds = ModelerDrawState::Instance();
	int divisions;

	_setupOpenGl();

	switch (mds->m_quality)
	{
	case HIGH:
		divisions = 32; break;
	case MEDIUM:
		divisions = 20; break;
	case LOW:
		divisions = 12; break;
	case POOR:
		divisions = 8; break;
	}

	if (mds->m_rayFile)
	{
		_dump_current_modelview();
		fprintf(mds->m_rayFile,
			"cone { height=%f; bottom_radius=%f; top_radius=%f;\n", h, r1, r2);
		_dump_current_material();
		fprintf(mds->m_rayFile, "})\n");
	}
	else
	{
		GLUquadricObj* gluq;

		// GLU will again do the work.  draw the sides of the cylinder. 
		glEnable(GL_TEXTURE_2D);
		Texture tex;
		tex.loadBMP_custom(dir);
		gluq = gluNewQuadric();
		gluQuadricDrawStyle(gluq, GLU_FILL);
		gluQuadricTexture(gluq, GL_TRUE);
		gluCylinder(gluq, r1, r2, h, divisions, divisions);
		gluDeleteQuadric(gluq);
		glDisable(GL_TEXTURE_2D);

		if (r1 > 0.0)
		{
			// if the r1 end does not come to a point, draw a flat disk to
			//cover it up. 

			gluq = gluNewQuadric();
			gluQuadricDrawStyle(gluq, GLU_FILL);
			gluQuadricTexture(gluq, GL_TRUE);
			gluQuadricOrientation(gluq, GLU_INSIDE);
			gluDisk(gluq, 0.0, r1, divisions, divisions);
			gluDeleteQuadric(gluq);
		}

		if (r2 > 0.0)
		{
			// if the r2 end does not come to a point, draw a flat disk to
			//cover it up. 

			// save the current matrix mode. 
			int savemode;
			glGetIntegerv(GL_MATRIX_MODE, &savemode);

			// translate the origin to the other end of the cylinder. 
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glTranslated(0.0, 0.0, h);

			// draw a disk centered at the new origin. 
			gluq = gluNewQuadric();
			gluQuadricDrawStyle(gluq, GLU_FILL);
			gluQuadricTexture(gluq, GL_TRUE);
			gluQuadricOrientation(gluq, GLU_OUTSIDE);
			gluDisk(gluq, 0.0, r2, divisions, divisions);
			gluDeleteQuadric(gluq);

			// restore the matrix stack and mode. 
			glPopMatrix();
			glMatrixMode(savemode);
		}
	}

}


void drawCylinder( double h, double r1, double r2 )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    int divisions;

	_setupOpenGl();
    
    switch(mds->m_quality)
    {
    case HIGH: 
        divisions = 32; break;
    case MEDIUM: 
        divisions = 20; break;
    case LOW:
        divisions = 12; break;
    case POOR:
        divisions = 8; break;
    }
    
    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile, 
            "cone { height=%f; bottom_radius=%f; top_radius=%f;\n", h, r1, r2 );
        _dump_current_material();
        fprintf(mds->m_rayFile, "})\n" );
    }
    else
    {
        GLUquadricObj* gluq;
        
        /* GLU will again do the work.  draw the sides of the cylinder. */
        gluq = gluNewQuadric();
        gluQuadricDrawStyle( gluq, GLU_FILL );
        gluQuadricTexture( gluq, GL_TRUE );
        gluCylinder( gluq, r1, r2, h, divisions, divisions);
        gluDeleteQuadric( gluq );
        
        if ( r1 > 0.0 )
        {
        /* if the r1 end does not come to a point, draw a flat disk to
            cover it up. */
            
            gluq = gluNewQuadric();
            gluQuadricDrawStyle( gluq, GLU_FILL );
            gluQuadricTexture( gluq, GL_TRUE );
            gluQuadricOrientation( gluq, GLU_INSIDE );
            gluDisk( gluq, 0.0, r1, divisions, divisions);
            gluDeleteQuadric( gluq );
        }
        
        if ( r2 > 0.0 )
        {
        /* if the r2 end does not come to a point, draw a flat disk to
            cover it up. */
            
            /* save the current matrix mode. */	
            int savemode;
            glGetIntegerv( GL_MATRIX_MODE, &savemode );
            
            /* translate the origin to the other end of the cylinder. */
            glMatrixMode( GL_MODELVIEW );
            glPushMatrix();
            glTranslated( 0.0, 0.0, h );
            
            /* draw a disk centered at the new origin. */
            gluq = gluNewQuadric();
            gluQuadricDrawStyle( gluq, GLU_FILL );
            gluQuadricTexture( gluq, GL_TRUE );
            gluQuadricOrientation( gluq, GLU_OUTSIDE );
            gluDisk( gluq, 0.0, r2, divisions, divisions);
            gluDeleteQuadric( gluq );
            
            /* restore the matrix stack and mode. */
            glPopMatrix();
            glMatrixMode( savemode );
        }
    }
    
}
void drawTriangle( double x1, double y1, double z1,
                   double x2, double y2, double z2,
                   double x3, double y3, double z3 )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	_setupOpenGl();

    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile, 
            "polymesh { points=((%f,%f,%f),(%f,%f,%f),(%f,%f,%f)); faces=((0,1,2));\n", x1, y1, z1, x2, y2, z2, x3, y3, z3 );
        _dump_current_material();
        fprintf(mds->m_rayFile, "})\n" );
    }
    else
    {
        double a, b, c, d, e, f;
        
        /* the normal to the triangle is the cross product of two of its edges. */
        a = x2-x1;
        b = y2-y1;
        c = z2-z1;
        
        d = x3-x1;
        e = y3-y1;
        f = z3-z1;
        
        glBegin( GL_TRIANGLES );
        glNormal3d( b*f - c*e, c*d - a*f, a*e - b*d );
        glVertex3d( x1, y1, z1 );
        glVertex3d( x2, y2, z2 );
        glVertex3d( x3, y3, z3 );
        glEnd();
    }
}

void drawHead(float noseScale, int eyeColor, float eyeScale, float eyeBallScale, float eyeDist)
{
	glPushMatrix();

	setDiffuseColor(0.0f, 0.8f, 0.8f);
	//up front
	drawTriangle(0.8, -0.4, 0, -0.8, -0.4, 0, -0.6, -0.2, 1.6);
	drawTriangle(0.8, -0.4, 0, -0.6, -0.2, 1.6, 0.6, -0.2, 1.6);

	//down front
	drawTriangle(0.8, -0.4, 0, -0.8, -0.4, 0, 0.4, -0.2, -0.8);
	drawTriangle(-0.8, -0.4, 0, -0.4, -0.2, -0.8, 0.4, -0.2, -0.8);

	//top
	setDiffuseColor(0.98f, 0.96f, 0.2f);
	drawTriangle(0.6, -0.2, 1.6, -0.6, -0.2, 1.6, -0.6, 0.2, 1.6);
	drawTriangle(0.6, -0.2, 1.6, 0.6, 0.2, 1.6, -0.6, 0.2, 1.6);
	setDiffuseColor(0.0f, 0.8f, 0.8f);

	//up back
	setDiffuseColor(0.98f, 0.96f, 0.2f);
	drawTriangle(0.8, 0.4, 0, -0.8, 0.4, 0, -0.6, 0.2, 1.6);
	drawTriangle(0.8, 0.4, 0, -0.6, 0.2, 1.6, 0.6, 0.2, 1.6);
	setDiffuseColor(0.0f, 0.8f, 0.8f);

	//down back
	drawTriangle(0.8, 0.4, 0, -0.8, 0.4, 0, 0.4, 0.2, -0.8);
	drawTriangle(-0.8, 0.4, 0, -0.4, 0.2, -0.8, 0.4, 0.2, -0.8);

	//down
	drawTriangle(0.4, -0.2, -0.8, -0.4, -0.2, -0.8, -0.4, 0.2, -0.8);
	drawTriangle(0.4, -0.2, -0.8, 0.4, 0.2, -0.8, -0.4, 0.2, -0.8);

	//up left
	drawTriangle(-0.8, -0.4, 0, -0.8, 0.4, 0, -0.6, -0.2, 1.6);
	setDiffuseColor(0.98f, 0.96f, 0.2f);
	drawTriangle(-0.6, 0.2, 1.6, -0.8, 0.4, 0, -0.6, -0.2, 1.6);
	setDiffuseColor(0.0f, 0.8f, 0.8f);

	//up right
	drawTriangle(0.8, -0.4, 0, 0.8, 0.4, 0, 0.6, -0.2, 1.6);
	setDiffuseColor(0.98f, 0.86f, 0.2f);
	drawTriangle(0.6, 0.2, 1.6, 0.8, 0.4, 0, 0.6, -0.2, 1.6);
	setDiffuseColor(0.0f, 0.8f, 0.8f);

	//down left
	drawTriangle(-0.8, -0.4, 0, -0.8, 0.4, 0, -0.4, -0.2, -0.8);
	drawTriangle(-0.4, 0.2, -0.8, -0.8, 0.4, 0, -0.4, -0.2, -0.8);

	//down right
	drawTriangle(0.8, -0.4, 0, 0.8, 0.4, 0, 0.4, -0.2, -0.8);
	drawTriangle(0.4, 0.2, -0.8, 0.8, 0.4, 0, 0.4, -0.2, -0.8);

	//nose
	glPushMatrix();
	glScaled(noseScale, noseScale, noseScale);
	drawTriangle(0, -0.375, 0.6, -0.2, -0.4, 0, 0, -0.6, 0.2);
	drawTriangle(0, -0.375, 0.6, 0.2, -0.4, 0, 0, -0.6, 0.2);
	drawTriangle(-0.2, -0.4, 0, 0.2, 0.4, 0, 0, -0.6, 0.2);
	glPopMatrix();

	// eye1
	glPushMatrix();
	glTranslated(-eyeDist, -0.2, 0.8);
	//glPushMatrix();
	//glScaled(eyeBallScale, eyeBallScale, eyeBallScale);
	drawSphere(0.3);
	//glPopMatrix();
	switch (eyeColor){
	case 0:
		// black
		setDiffuseColor(0.0f, 0.0f, 0.0f);
		break;
	case 1:
		// blue
		setDiffuseColor(COLOR_BLUE);
		break;
	case 2:
		// red
		setDiffuseColor(COLOR_RED);
		break;
	}
	glTranslated(0.02, -0.28, 0);
	glPushMatrix();
		glScaled(eyeScale, eyeScale, eyeScale);
		drawSphere(0.05);
	glPopMatrix();
	setDiffuseColor(0.0f, 0.8f, 0.8f);
	glPopMatrix();


	// eye2
	glPushMatrix();
	glTranslated(eyeDist, -0.2, 0.8);   //0.3
	//glPushMatrix();
	//glScaled(eyeBallScale, eyeBallScale, eyeBallScale);
	drawSphere(0.3);
	//glPopMatrix();
	switch (eyeColor){
	case 0:
		// black
		setDiffuseColor(0.0f, 0.0f, 0.0f);
		break;
	case 1:
		// blue
		setDiffuseColor(COLOR_BLUE);
		break;
	case 2:
		// red
		setDiffuseColor(COLOR_RED);
		break;
	}
	glTranslated(-0.02, -0.28, 0);
	glPushMatrix();
		glScaled(eyeScale, eyeScale, eyeScale);
		drawSphere(0.05);
	glPopMatrix();
	setDiffuseColor(0.0f, 0.8f, 0.8f);
	glPopMatrix();

	glPopMatrix();
	//glEnd();
}

void drawGun(){
	// right and left
	drawTriangle(-0.4, 0, 0, -0.4, 0, 1, -0.4, 1.5, 0);
	drawTriangle(-0.4, 0, 1, -0.4, 1.5, 1, -0.4, 1.5, 0);
	drawTriangle(0.4, 0, 0, 0.4, 0, 1, 0.4, 1.5, 0);
	drawTriangle(0.4, 0, 1, 0.4, 1.5, 1, 0.4, 1.5, 0);

	// under
	drawTriangle(-0.4, 0, 0, -0.4, 0, 1, 0.4, 0, 0);
	drawTriangle(-0.4, 0, 1, 0.4, 0, 1, 0.4, 0, 0);

	// front and back
	drawTriangle(-0.4, 0, 1, 0.4, 0, 1, -0.4, 1.5, 1);
	drawTriangle(0.4, 0, 1, 0.4, 1.5, 1, -0.4, 1.5, 1);
	drawTriangle(-0.4, 0, 0, 0.4, 0, 0, -0.4, 3, 0);
	drawTriangle(0.4, 0, 0, 0.4, 3, 0, -0.4, 3, 0);

	// up right and up left
	drawTriangle(-0.4, 1.5, 0, -0.4, 1.5, 4, -0.4, 3, 0);
	drawTriangle(-0.4, 1.5, 4, -0.4, 3, 4, -0.4, 3, 0);
	drawTriangle(0.4, 1.5, 0, 0.4, 1.5, 4, 0.4, 3, 0);
	drawTriangle(0.4, 1.5, 4, 0.4, 3, 4, 0.4, 3, 0);

	// up under and up up 
	drawTriangle(-0.4, 1.5, 0, -0.4, 1.5, 4, 0.4, 1.5, 4);
	drawTriangle(0.4, 1.5, 4, 0.4, 1.5, 0, -0.4, 1.5, 0);
	drawTriangle(-0.4, 3, 0, -0.4, 3, 4, 0.4, 3, 4);
	drawTriangle(0.4, 3, 4, 0.4, 3, 0, -0.4, 3, 0);

	// front front pyramid
	drawTriangle(0, 2.25, 5, -0.4, 3, 4, -0.4, 1.5, 4);
	drawTriangle(0, 2.25, 5, -0.4, 1.5, 4, 0.4, 1.5, 4);
	drawTriangle(0, 2.25, 5, 0.4, 1.5, 4, 0.4, 3, 4);
	drawTriangle(0, 2.25, 5, 0.4, 3, 4, -0.4, 3, 4);
}

void drawSemiSphere(double r){
	double clipEq[4] = { 0, 1, 0, 0 };

	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, clipEq);
		
		drawSphere(r);
	
	
	glDisable(GL_CLIP_PLANE0);
}

void drawHat(double innerR, double outerR){
	glPushMatrix();
	drawSemiSphere(innerR);
	glRotated(-90, 1.0, 0.0, 0.0);
	drawCylinder(0.1, outerR, outerR);
	glPopMatrix();
}

void drawMetaarm(float size)
{
	CUBE_GRID cubeGrid;
	cubeGrid.CreateMemory();
	cubeGrid.Init(40);

	for (int i = 0; i<cubeGrid.numVertices; i++)
	{
		cubeGrid.vertices[i].value = 0.0f;
		cubeGrid.vertices[i].normal.LoadZero();
	}

	//evaluate the scalar field at each point

	const int numMetaballs = 3;
	METABALL metaballs[numMetaballs];

	metaballs[0].Init(VECTOR3D(2.0f, 0.0f, 0.0f), 6.0f);
	metaballs[1].Init(VECTOR3D(3.8f, 0.0f, 0.0f), size);//8.5f
	metaballs[2].Init(VECTOR3D(7.4f, 0.0f, 0.0f), 5.0f);
	VECTOR3D ballToPoint;
	float squaredRadius;
	VECTOR3D ballPosition;
	float normalScale;
	for (int i = 0; i<numMetaballs; i++)
	{
		squaredRadius = metaballs[i].squaredRadius;
		ballPosition = metaballs[i].position;

		//VC++6 standard does not inline functions
		//by inlining these maually, in this performance-critical area,
		//almost a 100% increase in speed is found
		for (int j = 0; j<cubeGrid.numVertices; j++)
		{
			//ballToPoint=cubeGrid.vertices[j].position-ballPosition;
			ballToPoint.x = cubeGrid.vertices[j].position.x - ballPosition.x;
			ballToPoint.y = cubeGrid.vertices[j].position.y - ballPosition.y;
			ballToPoint.z = cubeGrid.vertices[j].position.z - ballPosition.z;

			//get squared distance from ball to point
			//float squaredDistance=ballToPoint.GetSquaredLength();
			float squaredDistance = ballToPoint.x*ballToPoint.x +
				ballToPoint.y*ballToPoint.y +
				ballToPoint.z*ballToPoint.z;
			if (squaredDistance == 0.0f)
				squaredDistance = 0.0001f;

			//value = r^2/d^2
			cubeGrid.vertices[j].value += squaredRadius / squaredDistance;

			//normal = (r^2 * v)/d^4
			normalScale = squaredRadius / (squaredDistance*squaredDistance);
			//cubeGrid.vertices[j].normal+=ballToPoint*normalScale;
			cubeGrid.vertices[j].normal.x += ballToPoint.x*normalScale;
			cubeGrid.vertices[j].normal.y += ballToPoint.y*normalScale;
			cubeGrid.vertices[j].normal.z += ballToPoint.z*normalScale;
		}
	}


	cubeGrid.DrawSurface(1.0f);
}


/*
void drawSemiSphere(int scalex, int scaley, GLfloat r){
	
	int i, j;
	const int scale = scalex*scaley;
	GLfloat v[1][3];

	for (i = 0; i<scalex; ++i) {
		for (j = 0; j<scaley; ++j) {
			v[i*scaley + j][0] = r*cos(j * 2 * M_PI / scaley)*cos(i*M_PI / (2 * scalex));
			v[i*scaley + j][1] = r*sin(i*M_PI / (2 * scalex));
			v[i*scaley + j][2] = r*sin(j * 2 * M_PI / scaley)*cos(i*M_PI / (2 * scalex));
		}
	}

	glBegin(GL_QUADS);
	for (i = 0; i<scalex - 1; ++i) {
		for (j = 0; j<scaley; ++j) {
			glVertex3fv(v[i*scaley + j]);
			glVertex3fv(v[i*scaley + (j + 1) % scaley]);
			glVertex3fv(v[(i + 1)*scaley + (j + 1) % scaley]);
			glVertex3fv(v[(i + 1)*scaley + j]);
		}
	}
	glEnd();
	
}

*/







