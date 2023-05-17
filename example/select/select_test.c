#include <roki_gl/rkgl_glut.h>
#include <roki_gl/rkgl_select.h>

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
  rkglCALoad( &cam );
  rkglLightPut( &light );
  rkglClear();
  draw_scene();
  glutSwapBuffers();
}

void manip(GLuint selbuf[], int hits)
{
  unsigned int *ptr, j, n;

  for( ptr=selbuf; hits>0; hits-- ){
    n = *ptr;
    ptr += 3;
    for( j=0; j<n; j++, ptr++ ){
      if( object[*ptr].updown == 0 ){
        object[*ptr].y = 1.5;
        object[*ptr].updown = 1;
      } else{
        object[*ptr].y = 0.0;
        object[*ptr].updown = 0;
      }
    }
  }
}

void mouse(int button, int state, int x, int y)
{
  GLuint selbuf[BUFSIZ];

  switch( button ){
  case GLUT_LEFT_BUTTON:
    if( state == GLUT_DOWN )
      manip( selbuf, rkglPick( &cam, draw_scene, selbuf, BUFSIZ, x, y, 1, 1 ) );
    break;
  case GLUT_MIDDLE_BUTTON:
    break;
  case GLUT_RIGHT_BUTTON:
    break;
  default: ;
  }
}

void resize(int w, int h)
{
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglPerspective( &cam, 30.0, (double)w / (double)h, 1.0, 100.0 );
}

void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 'q':
  case 'Q':
  case '\033':
    exit(0);  /* '\033' は ESC の ASCII コード */
  default:
    break;
  }
}

void init(void)
{
  GLfloat color[4];
  int i;

  rkglBGSet( &cam, 1.0, 1.0, 1.0 );
  rkglCALookAt( &cam, 5.0, 4.0, 5.0, 0.4, 0.0, 0.0, 0.0, 1.0, 0.0 );

  glEnable(GL_LIGHTING);
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

void idle(void){ glutPostRedisplay(); }

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 320, 320, argv[0] );

  glutDisplayFunc( display );
  glutMouseFunc( mouse );
  glutReshapeFunc( resize );
  glutKeyboardFunc( keyboard );
  glutIdleFunc( idle );
  init();
  glutMainLoop();
  return 0;
}
