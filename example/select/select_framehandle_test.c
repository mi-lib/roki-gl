#include <roki_gl/roki_glut.h>

typedef struct{
  GLint list;
  int updown;
} partsInfo_t;

#define NOBJECTS 6
typedef struct{
  partsInfo_t partsInfo[NOBJECTS];
  zFrame3D frame;
} frameHandle_t;

/* the main target of this sample code */
frameHandle_t g_fh;

#define NPOSSIZE 3
static const zAxis g_AXES[NOBJECTS] = {zX, zY, zZ, zX, zY, zZ};

rkglCamera cam;
rkglLight light;

static const double g_LENGTH = 2.0;
static const double g_MAGNITUDE = 1.0;

/* draw FrameHandle parts shape */
void draw_fhpts(int id, void (*fhpts_callback)(zFrame3D*, zAxis, double, double, bool))
{
  /* frame.ang (AA) -> angle & axis */
  zVec6D aa;
  zFrame3DToVec6DAA( &g_fh.frame, &aa );
  zVec3D axis;
  double angle = zVec3DNormalize( zVec6DAng( &aa ), &axis );

  /* start draw */
  glLoadName( id );
  glPushMatrix();

  if( g_fh.partsInfo[id].updown == 0 )
    fhpts_callback( &g_fh.frame, g_AXES[id], g_LENGTH, g_MAGNITUDE, false );
  else
    fhpts_callback( &g_fh.frame, g_AXES[id], g_LENGTH, g_MAGNITUDE, true );

  glTranslated( g_fh.frame.pos.c.x, g_fh.frame.pos.c.y, g_fh.frame.pos.c.y );
  glRotated( angle, axis.c.x, axis.c.y, axis.c.z);

  glCallList( g_fh.partsInfo[id].list );
  glPopMatrix();
  /* end draw */
}

void draw_scene(void)
{
  /* pos */
  int i = 0;
  for( i = 0; i < NPOSSIZE; i++ )
  {
    draw_fhpts( i, rkglFrameHandleArrowParts );
  }
  /* rot */
  for( i = NPOSSIZE; i < NOBJECTS; i++ )
  {
    draw_fhpts( i, rkglFrameHandleTorusParts );
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

static int selected_parts_id = -1;

void reset_parts(void)
{
  if( selected_parts_id >= 0 ){
    glDeleteLists( g_fh.partsInfo[ selected_parts_id ].list, 1 );
    g_fh.partsInfo[ selected_parts_id ].list = -1;
    g_fh.partsInfo[ selected_parts_id ].updown = 0;

    selected_parts_id = -1;
  }
}

void select_parts(GLuint selbuf[], int hits)
{
  GLuint *ns;

  reset_parts();
  if( !( ns = rkglFindNearside( selbuf, hits ) ) ) return;
  selected_parts_id = ns[3]; /* simple reference to link name */

  if( selected_parts_id >= 0 && selected_parts_id < NOBJECTS )
    g_fh.partsInfo[ selected_parts_id ].updown = 1;
}

void mouse(int button, int state, int x, int y)
{
  GLuint selbuf[BUFSIZ];

  switch( button ){
  case GLUT_LEFT_BUTTON:
    if( state == GLUT_DOWN )
      select_parts( selbuf, rkglPick( &cam, draw_scene, selbuf, BUFSIZ, x, y, 1, 1 ) );
    break;
  case GLUT_MIDDLE_BUTTON:
    break;
  case GLUT_RIGHT_BUTTON:
    break;
  default: ;
  }

  if( selected_parts_id == -1 )
  {
    rkglMouseFuncGLUT(button, state, x, y);
  }
}

void motion(int x, int y)
{
  if( selected_parts_id == -1 ){
    rkglMouseDragFuncGLUT(x, y);
  }
}

void resize(int w, int h)
{
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglFrustumScale( &cam, 0.002, 1, 20 );
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'u': rkglCALockonPTR( &cam, 5, 0, 0 ); break;
  case 'U': rkglCALockonPTR( &cam,-5, 0, 0 ); break;
  case 'i': rkglCALockonPTR( &cam, 0, 5, 0 ); break;
  case 'I': rkglCALockonPTR( &cam, 0,-5, 0 ); break;
  case 'o': rkglCALockonPTR( &cam, 0, 0, 5 ); break;
  case 'O': rkglCALockonPTR( &cam, 0, 0,-5 ); break;
  case '8': rkglCARelMove( &cam, 0.05, 0, 0 ); break;
  case '*': rkglCARelMove( &cam,-0.05, 0, 0 ); break;
  case '9': rkglCARelMove( &cam, 0, 0.05, 0 ); break;
  case '(': rkglCARelMove( &cam, 0,-0.05, 0 ); break;
  case '0': rkglCARelMove( &cam, 0, 0, 0.05 ); break;
  case ')': rkglCARelMove( &cam, 0, 0,-0.05 ); break;
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  rkglSetCallbackParamGLUT( &cam, 0, 0, 0, 0, 0 );

  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  rkglCASet( &cam, 5, 0, 2, 0, -20, 0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.4, 0.4, 0.4, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 8, 0, 8 );

  int i;
  for( i=0; i<NOBJECTS; i++ ){
    g_fh.partsInfo[i].updown = 0;
    glDeleteLists( g_fh.partsInfo[i].list, 1 );
    g_fh.partsInfo[i].list = glGenLists( 1 );
    glNewList( g_fh.partsInfo[i].list, GL_COMPILE );
    glEndList();
  }
}

void idle(void){ glutPostRedisplay(); }

int main(int argc, char *argv[])
{
  /* initialize */
  zFrame3DFromAA( &g_fh.frame, 0.0, 0.0, 0.0,  0.0, 0.0, 1.0);

  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 320, 320, argv[0] );

  glutDisplayFunc( display );
  glutMouseFunc( mouse );

  /* Add */
  glutMotionFunc( motion );

  glutReshapeFunc( resize );
  glutKeyboardFunc( keyboard );
  glutIdleFunc( idle );
  init();
  glutMainLoop();
  return 0;
}
