#include <roki_gl/roki_glut.h>

/* suggestion to add rk_shape.c ************************************************************/

void rkglFrameAxisMaterial(zAxis a)
{
  zOpticalInfo oi;
  switch(a){
  case zX:
    /* Red */
    zOpticalInfoCreate( &oi, 0.5, 0.5, 0.5, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, NULL );
    break;
  case zY:
    /* Green */
    zOpticalInfoCreate( &oi, 0.5, 0.5, 0.5, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, NULL );
    break;
  case zZ:
    /* Blue */
    zOpticalInfoCreate( &oi, 0.5, 0.5, 0.5, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, NULL );
    break;
  default: return;
  }
  rkglMaterial( &oi );
}

void rkglFrameHandleArrowParts(zFrame3D *f, zAxis a, double l, double mag)
{
  zVec3D v, vb;

  zVec3DMul( &zFrame3DAtt(f)->v[a], 0.5*l, &v );
  zVec3DAdd( zFrame3DPos(f), &v, &vb );
  rkglArrow( &vb, &v, mag );
  zVec3DRevDRC( &v );
  zVec3DAdd( zFrame3DPos(f), &v, &vb );
  rkglArrow( &vb, &v, mag );
}

void rkglFrameHandleTorusParts(zFrame3D *f, zAxis a, double l, double mag)
{
  double r1, r2;

  r1 = l * 0.5 + RKGL_ARROW_BOTTOM_RAD * mag;
  r2 = l * 0.5 - RKGL_ARROW_BOTTOM_RAD * mag;

  rkglTorus( zFrame3DPos(f), &zFrame3DAtt(f)->v[a], r1, r2, RKGL_ARROW_DIV*4, RKGL_ARROW_DIV, RKGL_FACE );
}

/* End of suggestion to add rk_shape.c *****************************************************/


/* the number of FrameHandle parts */
#define NOBJECTS 6
/* the number of translation size (x,y,z : 3) */
#define NPOSSIZE 3

static const zAxis g_AXES[NOBJECTS] = {zX, zY, zZ, zX, zY, zZ};

typedef struct{
  GLint list;
  int updown;
} partsInfo_t;

typedef struct{
  partsInfo_t partsInfo[NOBJECTS];
  zFrame3D frame;
} frameHandle_t;

/* the main target of this sample code */
frameHandle_t g_fh;

/* viewing parameters */
rkglCamera g_cam;
rkglLight g_light;
static const GLdouble g_znear = -100.0;
static const GLdouble g_zfar  = 100.0;
static double g_scale = 0.01;

/* FrameHandle shape property */
static const double g_LENGTH = 2.0;
static const double g_MAGNITUDE = 1.0;

static int g_selected_parts_id = -1;

/* judge whether the selected shape ID is translation moving parts or not */
bool is_translation_mode(int fh_parts_id)
{
  /* define the range of translation ID */
  return (fh_parts_id >= 0 && fh_parts_id < NPOSSIZE);
}

/* judge whether the selected shape ID is rotation moving parts or not */
bool is_rotation_mode(int fh_parts_id)
{
  /* define the range of rotation ID */
  return (fh_parts_id >= NPOSSIZE && fh_parts_id < NOBJECTS);
}

/* draw FrameHandle parts shape */
void draw_fh_parts(void)
{
  int i;
  for( i=0; i < NOBJECTS; i++ ){
    glLoadName( i );
    if( g_fh.partsInfo[i].updown == 0 )
      rkglFrameAxisMaterial( g_AXES[i] );
    else
      rkglMaterial(NULL);
    glPushMatrix();
    rkglXform( &g_fh.frame );
    glCallList( g_fh.partsInfo[i].list );
    glPopMatrix();
  }
}

void draw_scene(void)
{
  draw_fh_parts();
}

void display(void)
{
  rkglCALoad( &g_cam );
  rkglLightPut( &g_light );
  rkglClear();
  draw_scene();
  glutSwapBuffers();
}

static double g_zmin;
static zFrame3D g_frame3D_org;
static zVec3D g_vp_mouse_start_3D;
static zVec3D g_vp_parts_axis_3D_vector;
static zVec3D g_parts_axis_unit_3D_vector;
static zVec3D g_vp_circle_center_3D;
static zVec3D g_vp_radial_dir_center_to_start;

void reset_parts(void)
{
  if( g_selected_parts_id >= 0 ){
    g_fh.partsInfo[ g_selected_parts_id ].updown = 0;
  }
}

void select_fh_parts(GLuint selbuf[], int x, int y, int hits)
{
  reset_parts();

  GLuint *ns;
  if( !( ns = rkglFindNearside( selbuf, hits ) ) ){
    g_selected_parts_id = -1;
    return;
  }
  g_selected_parts_id = ns[3]; /* simple reference to link name */

  /* moving mode */
  if( g_selected_parts_id >= 0 && g_selected_parts_id < NOBJECTS ){

    g_fh.partsInfo[ g_selected_parts_id ].updown = 1;

    zFrame3DCopy( &g_fh.frame, &g_frame3D_org );
    /* zmin(=ns[1]) is GLuint data. GLuint maximum value is 0xffffffff. */
    g_zmin = ns[1] / (GLdouble)(0xffffffff);

    zVec3D parts_axis_start_3D;
    rkglUnproject( &g_cam, x, y, g_zmin, &parts_axis_start_3D );
    /* project to view port */
    rkglUnproject( &g_cam, x, y, g_znear, &g_vp_mouse_start_3D );

    if( is_translation_mode( g_selected_parts_id ) ){
      /* translate mode */
      if( g_selected_parts_id == 0 ){
        /* x */
        zFrame3DCopy( &g_fh.frame.att.b.x, &g_parts_axis_unit_3D_vector );
      } else if( g_selected_parts_id == 1 ){
        /* y */
        zFrame3DCopy( &g_fh.frame.att.b.y, &g_parts_axis_unit_3D_vector );
      } else if( g_selected_parts_id == 2 ){
        /* z */
        zFrame3DCopy( &g_fh.frame.att.b.z, &g_parts_axis_unit_3D_vector );
      }
      zVec3D parts_axis_unit_end_3D;
      zVec3DAdd( &parts_axis_start_3D, &g_parts_axis_unit_3D_vector, &parts_axis_unit_end_3D );

      /* project to view port */
      int ax, ay;
      rkglProject( &g_cam, &parts_axis_unit_end_3D, &ax, &ay );
      zVec3D vp_parts_axis_unit_end_3D;
      rkglUnproject( &g_cam, ax, ay, g_znear, &vp_parts_axis_unit_end_3D );
      zVec3DSub( &vp_parts_axis_unit_end_3D, &g_vp_mouse_start_3D, &g_vp_parts_axis_3D_vector );

      /* end of translate mode */
    } else if( is_rotation_mode( g_selected_parts_id ) ){
      /* rotate mode */
      if( g_selected_parts_id == 3 ){
        /* x */
        zFrame3DCopy( &g_fh.frame.att.b.x, &g_parts_axis_unit_3D_vector );
      } else if( g_selected_parts_id == 4 ){
        /* y */
        zFrame3DCopy( &g_fh.frame.att.b.y, &g_parts_axis_unit_3D_vector );
      } else if( g_selected_parts_id == 5 ){
        /* z */
        zFrame3DCopy( &g_fh.frame.att.b.z, &g_parts_axis_unit_3D_vector );
      }
      /* project to view port */
      int cx, cy;
      rkglProject( &g_cam, &g_fh.frame.pos, &cx, &cy );
      rkglUnproject( &g_cam, cx, cy, g_znear, &g_vp_circle_center_3D );
      zVec3DSub( &g_vp_mouse_start_3D, &g_vp_circle_center_3D, &g_vp_radial_dir_center_to_start);
    } else{
      ZRUNERROR( "selected_parts_id is ERROR\n" );
    }
    /* end of rotate mode */
  }
  /* end moving mode */
}

void mouse(int button, int state, int x, int y)
{
  GLuint selbuf[BUFSIZ];

  switch( button ){
  case GLUT_LEFT_BUTTON:
    if( state == GLUT_DOWN )
      select_fh_parts( selbuf, x, y, rkglPick( &g_cam, draw_scene, selbuf, BUFSIZ, x, y, 1, 1 ) );
    break;
  case GLUT_MIDDLE_BUTTON:
    break;
  case GLUT_RIGHT_BUTTON:
    break;
  default: ;
  }

  if( g_selected_parts_id == -1 )
  {
    rkglMouseFuncGLUT(button, state, x, y);
  }
}

void motion(int x, int y)
{
  if( g_selected_parts_id == -1 ){
    rkglMouseDragFuncGLUT(x, y);
  } else{
    /* moving mode */
    zVec3D vp_mouse_goal_3D;
    rkglUnproject( &g_cam, x, y, g_znear, &vp_mouse_goal_3D );

    if( is_translation_mode( g_selected_parts_id ) ){
      /* translate mode */
      zVec3D vp_mouse_drag_3D_vector;
      zVec3DSub( &vp_mouse_goal_3D, &g_vp_mouse_start_3D, &vp_mouse_drag_3D_vector );

      double project_3D_scale = zVec3DInnerProd( &vp_mouse_drag_3D_vector, &g_vp_parts_axis_3D_vector ) / zVec3DSqrNorm( &g_vp_parts_axis_3D_vector );

      zVec3D dir_3D;
      zVec3DMul( &g_parts_axis_unit_3D_vector, project_3D_scale, &dir_3D );

      /* translate frame */
      zVec3DAdd( &g_frame3D_org.pos, &dir_3D, &g_fh.frame.pos );

      /* end of translate mode */
    } else if( is_rotation_mode( g_selected_parts_id ) ){
      /* rotate mode */
      zVec3D vp_radial_dir_center_to_goal;
      zVec3DSub( &vp_mouse_goal_3D, &g_vp_circle_center_3D, &vp_radial_dir_center_to_goal);

      if ( !zVec3DIsTiny( &g_vp_radial_dir_center_to_start ) ){
        /* oval circle rotation on view port plane */
        double theta = zVec3DAngle( &g_vp_radial_dir_center_to_start, &vp_radial_dir_center_to_goal, &g_parts_axis_unit_3D_vector);
        zMat3D m;
        /* zVec3D axis; */
        if( g_selected_parts_id == 3 ){
          /* x */
          zMat3DFromZYX( &m, 0, 0, theta );
        } else if( g_selected_parts_id == 4 ){
          /* y */
          zMat3DFromZYX( &m, 0, theta, 0 );
        } else if( g_selected_parts_id == 5 ){
          /* z */
          zMat3DFromZYX( &m, theta, 0, 0 );
        }
        zMulMat3DMat3D( &g_frame3D_org.att, &m, &g_fh.frame.att );
      } else{
        ZRUNERROR( "selected_parts_id is ERROR\n" );
      }
      /* end of if g_vp_radial_dir_center_to_start is not Tiny */
    }
    /* end of rotate mode */
  }
  /* end moving mode */
}

void resize(int w, int h)
{
  rkglVPCreate( &g_cam, 0, 0, w, h );
  rkglOrthoScale( &g_cam, g_scale, g_znear, g_zfar );
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'u': rkglCALockonPTR( &g_cam, 5, 0, 0 ); break;
  case 'U': rkglCALockonPTR( &g_cam,-5, 0, 0 ); break;
  case 'i': rkglCALockonPTR( &g_cam, 0, 5, 0 ); break;
  case 'I': rkglCALockonPTR( &g_cam, 0,-5, 0 ); break;
  case 'o': rkglCALockonPTR( &g_cam, 0, 0, 5 ); break;
  case 'O': rkglCALockonPTR( &g_cam, 0, 0,-5 ); break;
  case '8': g_scale += 0.001; rkglOrthoScale( &g_cam, g_scale, g_znear, g_zfar ); break;
  case '*': g_scale -= 0.001; rkglOrthoScale( &g_cam, g_scale, g_znear, g_zfar ); break;
  case '9': rkglCARelMove( &g_cam, 0, 0.05, 0 ); break;
  case '(': rkglCARelMove( &g_cam, 0,-0.05, 0 ); break;
  case '0': rkglCARelMove( &g_cam, 0, 0, 0.05 ); break;
  case ')': rkglCARelMove( &g_cam, 0, 0,-0.05 ); break;
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init_fh_parts(void)
{
  int i;
  zFrame3DIdent( &g_fh.frame );
  for( i=0; i<NOBJECTS; i++ ){
    g_fh.partsInfo[i].updown = 0;
    if( glIsList( g_fh.partsInfo[i].list ) )
      glDeleteLists( g_fh.partsInfo[i].list, 1 );
    glPushMatrix();
    rkglXform( &g_fh.frame );
    rkglFrameAxisMaterial( g_AXES[i] );
    /* start register */
    g_fh.partsInfo[i].list = glGenLists( 1 );
    glNewList( g_fh.partsInfo[i].list, GL_COMPILE );
    if( i < NPOSSIZE ){
      /* translation arrow shape */
      rkglFrameHandleArrowParts( &g_fh.frame, g_AXES[i], g_LENGTH, g_MAGNITUDE );
    } else {
      /* rotation torus shape */
      rkglFrameHandleTorusParts( &g_fh.frame, g_AXES[i], g_LENGTH, g_MAGNITUDE );
    }
    glEndList();
    /* end register */
    glPopMatrix();
  }
}

void init(void)
{
  rkglSetCallbackParamGLUT( &g_cam, 0, 0, 0, 0, 0 );
  rkglBGSet( &g_cam, 0.5, 0.5, 0.5 );
  rkglCASet( &g_cam, 5, 0, 2, 0, -20, 0 );
  glEnable( GL_LIGHTING );
  rkglLightCreate( &g_light, 0.4, 0.4, 0.4, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &g_light, 8, 0, 8 );
  /* frame handle */
  init_fh_parts();
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
