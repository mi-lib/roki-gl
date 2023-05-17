#include <roki_gl/rkgl_glut.h>
#include <roki_gl/rkgl_camera.h>
#include <roki_gl/rkgl_chain.h>

rkChain chain;
rkglChain gr;

rkglCamera cam;
rkglLight light;

int clone_id = -1;

void display(void)
{
  rkglCALoad( &cam );
  rkglLightPut( &light );

  glPushMatrix();
  rkglClear();
  rkglChainDraw( &gr );
  if( clone_id >= 0 ) glCallList( clone_id );
  glPopMatrix();
  glutSwapBuffers();
}

void idle(void){ glutPostRedisplay(); }

void resize(int w, int h)
{
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglFrustumScale( &cam, 1.0/1000, 0.5, 10 );
}

#define toggle_disptype( gr, type ) do{\
  if( (gr)->attr.disptype & (type) )\
    (gr)->attr.disptype &= ~(type);\
  else\
    (gr)->attr.disptype |= (type);\
} while(0)

void keyboard(unsigned char key, int x, int y)
{
  int id;
  zOpticalInfo oi_alt;
  static double dis = 0;

  zOpticalInfoCreateSimple( &oi_alt, 1.0, 0.0, 0.0, NULL );
  switch( key ){
  case 'u': rkglCALockonPTR( &cam, 5, 0, 0 ); break;
  case 'U': rkglCALockonPTR( &cam,-5, 0, 0 ); break;
  case 'i': rkglCALockonPTR( &cam, 0, 5, 0 ); break;
  case 'I': rkglCALockonPTR( &cam, 0,-5, 0 ); break;
  case 'o': rkglCALockonPTR( &cam, 0, 0, 5 ); break;
  case 'O': rkglCALockonPTR( &cam, 0, 0,-5 ); break;
  case 'c':
    if( clone_id >= 0 ){
      glDeleteLists( clone_id, 1 );
      clone_id = -1;
    } else{
      eprintf( ">>> clone!\n" );
      rkChainLinkOrgPos(&chain,0)->e[zY] += 0.5;
      rkChainUpdateFK( &chain );
      clone_id = rkglChainDrawSeethru( &gr, 0.5, &light );
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
    toggle_disptype( &gr, RKGL_FACE );
    rkglChainLinkAlt( &gr, 3, &oi_alt, &gr.attr, &light );
    break;
  case 'R':
    rkglChainLinkReset( &gr, 3 );
    break;
  case 'b':
    toggle_disptype( &gr, RKGL_BB );
    rkglChainUnload( &gr );
    rkglChainLoad( &gr, &chain, &gr.attr, &light );
    break;
  case 's':
    toggle_disptype( &gr, RKGL_STICK );
    rkglChainUnload( &gr );
    rkglChainLoad( &gr, &chain, &gr.attr, &light );
    break;
  case 'w':
    toggle_disptype( &gr, RKGL_WIREFRAME );
    rkglChainUnload( &gr );
    rkglChainLoad( &gr, &chain, &gr.attr, &light );
    break;
  case 'f':
    toggle_disptype( &gr, RKGL_FACE );
    rkglChainUnload( &gr );
    rkglChainLoad( &gr, &chain, &gr.attr, &light );
    break;
  case 'm':
    toggle_disptype( &gr, RKGL_FRAME );
    rkglChainUnload( &gr );
    rkglChainLoad( &gr, &chain, &gr.attr, &light );
    break;
  case 'q': case 'Q': case '\033':
    rkglChainUnload( &gr );
    rkChainDestroy( &chain );
    exit( EXIT_SUCCESS );
  default:
    id = key - '0';
    if( id >=0 && id < rkChainLinkNum(&chain) && gr.info[id].list >= 0 ){
      gr.info[id].visible = 1 - gr.info[id].visible;
      glutPostRedisplay();
    }
  }
}

void init(void)
{
  rkglChainAttr attr;

  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  rkglCASet( &cam, 0.5, 0.5, 0.5, 45, -30, 0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 1, 3, 6 );

  rkglChainAttrInit( &attr );
  rkChainReadZTK( &chain, "../model/puma" );
  rkglChainLoad( &gr, &chain, &attr, &light );
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 480, 480, argv[0] );

  glutDisplayFunc( display );
  glutIdleFunc( idle );
  glutReshapeFunc( resize );
  glutKeyboardFunc( keyboard );
  init();
  glutMainLoop();
  return 0;
}
