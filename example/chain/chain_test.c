#include <roki_gl/roki_glut.h>

rkChain chain;
rkglChain gc;

rkglCamera cam;
rkglLight light;

int clone_id = -1;

void display(void)
{
  rkglCameraPut( &cam );
  rkglLightPut( &light );

  glPushMatrix();
  rkglClear();
  rkglChainDraw( &gc );
  if( clone_id >= 0 ) glCallList( clone_id );
  glPopMatrix();
  glutSwapBuffers();
}

#define toggle_disptype( gc, type ) do{\
  if( (gc)->attr.disptype & (type) )\
    (gc)->attr.disptype &= ~(type);\
  else\
    (gc)->attr.disptype |= (type);\
} while(0)

void keyboard(unsigned char key, int x, int y)
{
  int id;
  zOpticalInfo oi_alt;
  static double dis = 0;

  zOpticalInfoCreateSimple( &oi_alt, 1.0, 0.0, 0.0, NULL );
  switch( key ){
  case 'c':
    if( clone_id >= 0 ){
      glDeleteLists( clone_id, 1 );
      clone_id = -1;
    } else{
      eprintf( ">>> clone!\n" );
      rkChainLinkOrgPos(&chain,0)->e[zY] += 0.5;
      rkChainUpdateFK( &chain );
      clone_id = rkglBeginList();
      rkglChainPhantomize( &gc, 0.5, &light );
      glEndList();
      rkChainLinkOrgPos(&chain,0)->e[zY] -= 0.5;
      rkChainUpdateFK( &chain );
    }
    break;
  case '+':
    dis += zDeg2Rad(5.0);
    rkChainLinkJointSetDis( &chain, 3, &dis );
    rkChainUpdateFK( &chain );
    break;
  case '-':
    dis -= zDeg2Rad(5.0);
    rkChainLinkJointSetDis( &chain, 3, &dis );
    rkChainUpdateFK( &chain );
    break;
  case 'r':
    toggle_disptype( &gc, RKGL_FACE );
    rkglChainAlternateLinkOptic( &gc, 3, &oi_alt, &light );
    break;
  case 'R':
    rkglChainResetLinkOptic( &gc, 3 );
    break;
  case 'b':
    toggle_disptype( &gc, RKGL_BB );
    rkglChainUnload( &gc );
    rkglChainLoad( &gc, &chain, &gc.attr, &light );
    break;
  case 's':
    toggle_disptype( &gc, RKGL_STICK );
    rkglChainUnload( &gc );
    rkglChainLoad( &gc, &chain, &gc.attr, &light );
    break;
  case 'w':
    toggle_disptype( &gc, RKGL_WIREFRAME );
    rkglChainUnload( &gc );
    rkglChainLoad( &gc, &chain, &gc.attr, &light );
    break;
  case 'f':
    toggle_disptype( &gc, RKGL_FACE );
    rkglChainUnload( &gc );
    rkglChainLoad( &gc, &chain, &gc.attr, &light );
    break;
  case 'm':
    toggle_disptype( &gc, RKGL_FRAME );
    rkglChainUnload( &gc );
    rkglChainLoad( &gc, &chain, &gc.attr, &light );
    break;
  case 'q': case 'Q': case '\033':
    rkglChainUnload( &gc );
    rkChainDestroy( &chain );
    exit( EXIT_SUCCESS );
  default:
    id = key - '0';
    if( id >=0 && id < rkChainLinkNum(&chain) && gc.linkinfo[id].list >= 0 ){
      gc.linkinfo[id].visible = 1 - gc.linkinfo[id].visible;
      glutPostRedisplay();
    }
  }
}

void init(void)
{
  rkglChainAttr attr;

  rkglCameraInit( &cam );
  rkglCameraSetBackground( &cam, 0.5, 0.5, 0.5 );
  rkglCameraSetViewframe( &cam, 1.0, 1.0, 1.0, 45, -30, 0 );
  rkglCameraFitPerspective( &cam, 30.0, 1.0, 20.0 );
  rkglSetDefaultCamera( &cam );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 1, 3, 6 );

  rkglChainAttrInit( &attr );
  rkChainReadZTK( &chain, "../model/puma" );
  rkglChainLoad( &gc, &chain, &attr, &light );
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 480, 480, argv[0] );

  glutDisplayFunc( display );
  glutKeyboardFunc( keyboard );
  glutSpecialFunc( rkglSpecialFuncGLUT );
  glutReshapeFunc( rkglReshapeFuncGLUT );
  glutIdleFunc( rkglIdleFuncGLUT );
  init();
  glutMainLoop();
  return 0;
}
