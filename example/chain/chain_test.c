#include <roki-gl/rkgl_glut.h>
#include <roki-gl/rkgl_camera.h>
#include <roki-gl/rkgl_chain.h>

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
      clone_id = rkglChainDrawSeethru( &gr, 0.5 );
      rkChainLinkOrgPos(&chain,0)->e[zY] -= 0.5;
      rkChainUpdateFK( &chain );
    }
    break;
  case '+':
    dis += zDeg2Rad(5.0);
    rkChainLinkSetJointDis( &chain, 3, &dis );
    rkChainUpdateFK( &chain );
    break;
  case '-':
    dis -= zDeg2Rad(5.0);
    rkChainLinkSetJointDis( &chain, 3, &dis );
    rkChainUpdateFK( &chain );
    break;
  case 'r':
    gr.attr.disptype = RKGL_FACE;
    rkglChainLinkAlt( &gr, 3, &oi_alt, &gr.attr );
    break;
  case 'R':
    rkglChainLinkReset( &gr, 3 );
    break;
  case 'b':
    gr.attr.disptype = RKGL_BB;
    rkglChainUnload( &gr );
    rkglChainLoad( &gr, &chain, &gr.attr );
    break;
  case 's':
    gr.attr.disptype = RKGL_STICK;
    rkglChainUnload( &gr );
    rkglChainLoad( &gr, &chain, &gr.attr );
    break;
  case 'f':
    gr.attr.disptype = RKGL_FACE;
    rkglChainUnload( &gr );
    rkglChainLoad( &gr, &chain, &gr.attr );
    break;
  case 'q': case 'Q': case '\033':
    rkglChainUnload( &gr );
    rkChainDestroy( &chain );
    exit( EXIT_SUCCESS );
  default:
    id = key - '0';
    if( id >=0 && id < rkChainNum(&chain) && gr.info[id].list >= 0 ){
      gr.info[id].visible = 1 - gr.info[id].visible;
      glutPostRedisplay();
    }
  }
}

void init(void)
{
  rkglChainAttr attr;

  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  rkglCASet( &cam, 1, 1, 1, 45, -30, 0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0, 0 );
  rkglLightSetPos( &light, 1, 3, 6 );

  rkglChainAttrInit( &attr );
  rkChainReadFile( &chain, "../model/puma.zkc" );
  rkglChainLoad( &gr, &chain, &attr );
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 320, 320, argv[0] );

  glutDisplayFunc( display );
  glutIdleFunc( idle );
  glutReshapeFunc( resize );
  glutKeyboardFunc( keyboard );
  init();
  glutMainLoop();
  return 0;
}
