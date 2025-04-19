#include <roki_gl/roki_glut.h>

unsigned color[] = {
  0xff0000,
  0x00ff00,
  0x0000ff,
  0xffff00,
  0xff00ff,
  0x00ffff,
};

rkglCamera cam;

void display(void)
{
  int i;

  rkglCameraPut( &cam );
  glPushMatrix();
  glTranslated( 0, -3, 0 );
  rkglClear();
  for( i=0; i<6; i++ ){
    glBegin( GL_TRIANGLE_FAN );
    rkglColor24( color[i] );
    glVertex3d( 0,-2.5+i*2, 2 );
    glVertex3d( 0,-1.5+i*2, 2 );
    glVertex3d( 0,-1.5+i*2,-2 );
    glVertex3d( 0,-2.5+i*2,-2 );
    glEnd();
  }
  glPopMatrix();
  glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 't': color[0] -=0x010000; break;
  case 'y': color[1] -=0x000100; break;
  case 'u': color[2] -=0x000001; break;
  case 'i': color[3] -=0x010100; break;
  case 'o': color[4] -=0x010001; break;
  case 'p': color[5] -=0x000101; break;
  case 'T': color[0] +=0x010000; break;
  case 'Y': color[1] +=0x000100; break;
  case 'U': color[2] +=0x000001; break;
  case 'I': color[3] +=0x010100; break;
  case 'O': color[4] +=0x010001; break;
  case 'P': color[5] +=0x000101; break;
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
  glutPostRedisplay();
}

void init(void)
{
  glCullFace( GL_FRONT );
  rkglCameraSetBackground( &cam, 0.5, 0.5, 0.5 );
  rkglCameraSetViewframe( &cam, 6, 0, 0, 0, 0, 0 );
  rkglCameraFitPerspective( &cam, 90.0, 1, 100 );
  rkglSetDefaultCamera( &cam );
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 480, argv[0] );

  glutDisplayFunc( display );
  glutReshapeFunc( rkglReshapeFuncGLUT );
  glutKeyboardFunc( keyboard );
  glutIdleFunc( rkglIdleFuncGLUT );
  init();
  glutMainLoop();
  return 0;
}
