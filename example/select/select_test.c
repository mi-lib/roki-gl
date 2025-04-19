#include <roki_gl/roki_glut.h>

typedef struct{
  GLint list;
  int updown;
  double x, y, z;
} object_t;

#define NOBJECTS 5
object_t object[NOBJECTS];

rkglCamera cam;
rkglLight light;

void draw_scene(void)
{
  int i;

  for( i=0; i<NOBJECTS; i++ ){
    glLoadName( i );
    glPushMatrix();
    glTranslated( object[i].x, object[i].y, object[i].z );
    glCallList( object[i].list );
    glPopMatrix();
  }
}

void display(void)
{
  rkglCameraPut( &cam );
  rkglLightPut( &light );
  rkglClear();
  draw_scene();
  glutSwapBuffers();
}

void manip(rkglSelectionBuffer *sb)
{
  unsigned int i;

  rkglSelectionRewind( sb );
  for( i=0; i<rkglSelectionHits(sb); i++ ){
    if( object[rkglSelectionName(sb,0)].updown == 0 ){
      object[rkglSelectionName(sb,0)].y = 1.5;
      object[rkglSelectionName(sb,0)].updown = 1;
    } else{
      object[rkglSelectionName(sb,0)].y = 0.0;
      object[rkglSelectionName(sb,0)].updown = 0;
    }
    rkglSelectionNext( sb );
  }
}

void mouse(int button, int state, int x, int y)
{
  rkglSelectionBuffer sb;

  switch( button ){
  case GLUT_LEFT_BUTTON:
    if( state == GLUT_DOWN ){
      rkglSelect( &sb, &cam, draw_scene, x, y, 1, 1 );
      manip( &sb );
    }
    break;
  case GLUT_MIDDLE_BUTTON:
    break;
  case GLUT_RIGHT_BUTTON:
    break;
  default: ;
  }
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'q': case 'Q': case '\033':
    exit(0);
  default: ;
  }
}

void init(void)
{
  GLfloat color[4];
  int i;

  rkglCameraSetBackground( &cam, 1.0, 1.0, 1.0 );
  rkglCameraLookAt( &cam, 5.0, 4.0, 5.0, 0.4, 0.0, 0.0, 0.0, 1.0, 0.0 );
  rkglCameraFitPerspective( &cam, 30.0, 1, 100 );
  rkglSetDefaultCamera( &cam );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 4.0, 8.0, 6.0 );

  for( i=0; i<NOBJECTS; i++ ){
    object[i].updown = 0;
    object[i].x = 4.0*i/(NOBJECTS-1) - 2.0;
    object[i].y = 0;
    object[i].z = 0;
    color[0] = 1.0 * ( (i&0x1)>>0 );
    color[1] = 1.0 * ( (i&0x2)>>1 );
    color[2] = 1.0 * ( (i&0x4)>>2 );
    color[3] = 0.8;
    object[i].list = glGenLists( 1 );
    glNewList( object[i].list, GL_COMPILE );
    glMaterialfv( GL_FRONT, GL_DIFFUSE, color );
    glutSolidCube( 0.8 );
    glEndList();
  }
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 320, 320, argv[0] );

  glutDisplayFunc( display );
  glutMouseFunc( mouse );
  glutKeyboardFunc( keyboard );
  glutReshapeFunc( rkglReshapeFuncGLUT );
  glutIdleFunc( rkglIdleFuncGLUT );
  init();
  glutMainLoop();
  return 0;
}
