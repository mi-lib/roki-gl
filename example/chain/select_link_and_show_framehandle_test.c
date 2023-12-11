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
/* to avoid duplication with selected_link and selected_parts_id */
/* NOFFSET must be enough large than rkChainLinkNum */
#define NOFFSET 50

static const zAxis g_AXES[NOBJECTS] = {zX, zY, zZ, zX, zY, zZ};


rkglCamera cam;
rkglLight light;

rkChain g_chain;
rkglChain gr;

static const GLdouble g_znear = -100.0;
static const GLdouble g_zfar  = 100.0;
static double g_scale = 0.001;
static const double g_LENGTH = 0.2;
static const double g_MAGNITUDE = g_LENGTH * 0.5;

/* draw FrameHandle parts shape */
void draw_fhpts(int id, void (*fhpts_callback)(zFrame3D*, zAxis, double, double, bool))
{
  /* frame.ang (AA) -> angle & axis */
  zVec6D aa;
  zFrame3DToVec6DAA( &g_fh.frame, &aa );
  zVec3D axis;
  double angle = 0.0;
  if( !zVec3DIsTiny( zVec6DAng( &aa ) ) )
  {
    angle = zVec3DNormalize( zVec6DAng( &aa ), &axis ) * 180.0 / zPI;
  }

  /* start draw */
  glLoadName( id + NOFFSET );
  glPushMatrix();

  if( g_fh.partsInfo[id].updown == 0 )
    fhpts_callback( &g_fh.frame, g_AXES[id], g_LENGTH, g_MAGNITUDE, false );
  else
    fhpts_callback( &g_fh.frame, g_AXES[id], g_LENGTH, g_MAGNITUDE, true );

  glRotated( angle, axis.c.x, axis.c.y, axis.c.z );
  glTranslated( g_fh.frame.pos.c.x, g_fh.frame.pos.c.y, g_fh.frame.pos.c.z );

  glCallList( g_fh.partsInfo[id].list );
  glPopMatrix();
  /* end draw */
}

static int selected_link = -1;
static int selected_parts_id = -1;

void draw_scene(void)
{
  if( selected_link >= 0 ){
    /* pos */
    int i = 0;
    for( i = 0; i < NPOSSIZE; i++ ){
      draw_fhpts( i, rkglFrameHandleArrowParts );
    }
    /* rot */
    for( i = NPOSSIZE; i < NOBJECTS; i++ ){
      draw_fhpts( i, rkglFrameHandleTorusParts );
    }
  }

  rkglChainDraw( &gr );
}

void display(void)
{
  rkglCALoad( &cam );
  rkglLightPut( &light );
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

void reset_link(void)
{
  rkglChainLinkReset( &gr, selected_link );
}

void select_link(GLuint selbuf[], int hits)
{
  GLuint *ns;
  zOpticalInfo oi_alt;

  reset_link();
  if( !( ns = rkglFindNearside( selbuf, hits ) ) ){
    selected_link = -1;
    printf( "selected_link = %d, ", selected_link );
    return;
  }
  int nearest_link = ns[3];
  int pre_selected_link = selected_link;
  if( nearest_link < rkChainLinkNum(gr.chain) ){
    selected_link = nearest_link; /* simple reference to link name */
  }
  printf( "selected_link = %d, ", selected_link );

  /* re-drawing selected link */
  if( selected_link >= 0
      && selected_link < rkChainLinkNum(gr.chain) )
  {
    /* zOpticalInfoCreateSimple( &oi_alt, 1.0, 0.0, 0.0, NULL ); */
    zOpticalInfoCreate( &oi_alt, 0.8, 0.5, 0.5, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.2, NULL );
    gr.attr.disptype = RKGL_FACE;
    rkglChainLinkAlt( &gr, selected_link, &oi_alt, &gr.attr, &light );

    /* update frame handle location */
    if( selected_link != pre_selected_link ) {
      zFrame3DCopy( rkChainLinkWldFrame(gr.chain, selected_link), &g_fh.frame );
    }
  }
  /* end of if( selected_link >= 0 && selected_link < rkChainLinkNum(gr.chain) ) */
}


void reset_parts(void)
{
  /* if( selected_parts_id > rkChainLinkNum(gr.chain) ){ */
  if( selected_parts_id >= 0 ){
    glDeleteLists( g_fh.partsInfo[ selected_parts_id ].list, 1 );
    g_fh.partsInfo[ selected_parts_id ].list = -1;
    g_fh.partsInfo[ selected_parts_id ].updown = 0;

    selected_parts_id = -1;
  }
}

void select_parts(GLuint selbuf[], int x, int y, int hits)
{
  reset_parts();

  GLuint *ns;
  if( !( ns = rkglFindNearside( selbuf, hits ) ) ){
    selected_parts_id = -1;
    printf("  selected_parts_id = %d \n", selected_parts_id );
    return;
  }
  if( ns[3] < NOFFSET ){
    selected_parts_id = -1;
    printf("  selected_parts_id = %d \n", selected_parts_id );
    return;
  }
  selected_parts_id = ns[3] - NOFFSET; /* simple reference to link name */
  printf("  selected_parts_id = %d \n", selected_parts_id );

  /* moving mode */
  if( selected_parts_id >= 0 && selected_parts_id < NOBJECTS ){

    g_fh.partsInfo[ selected_parts_id ].updown = 1;

    zFrame3DCopy( &g_fh.frame, &g_frame3D_org );
    /* zmin(=ns[1]) is GLuint data. GLuint maximum value is 0xffffffff. */
    g_zmin = ns[1] / (GLdouble)(0xffffffff);

    zVec3D parts_axis_start_3D;
    rkglUnproject( &cam, x, y, g_zmin, &parts_axis_start_3D );
    /* project to view port */
    rkglUnproject( &cam, x, y, g_znear, &g_vp_mouse_start_3D );

    if( selected_parts_id >= 0 && selected_parts_id < NPOSSIZE ){
      /* translate mode */
      if( selected_parts_id == 0 ){
        /* x */
        zFrame3DCopy( &g_fh.frame.att.b.x, &g_parts_axis_unit_3D_vector );
      } else if( selected_parts_id == 1 ){
        /* y */
        zFrame3DCopy( &g_fh.frame.att.b.y, &g_parts_axis_unit_3D_vector );
      } else if( selected_parts_id == 2 ){
        /* z */
        zFrame3DCopy( &g_fh.frame.att.b.z, &g_parts_axis_unit_3D_vector );
      }
      zVec3D parts_axis_unit_end_3D;
      zVec3DAdd( &parts_axis_start_3D, &g_parts_axis_unit_3D_vector, &parts_axis_unit_end_3D );

      /* project to view port */
      int ax, ay;
      rkglProject( &cam, &parts_axis_unit_end_3D, &ax, &ay );
      zVec3D vp_parts_axis_unit_end_3D;
      rkglUnproject( &cam, ax, ay, g_znear, &vp_parts_axis_unit_end_3D );
      zVec3DSub( &vp_parts_axis_unit_end_3D, &g_vp_mouse_start_3D, &g_vp_parts_axis_3D_vector );

      /* end of translate mode */
    } else{
      /* rotate mode */
      if( selected_parts_id == 3 ){
        /* x */
        zFrame3DCopy( &g_fh.frame.att.b.x, &g_parts_axis_unit_3D_vector );
      } else if( selected_parts_id == 4 ){
        /* y */
        zFrame3DCopy( &g_fh.frame.att.b.y, &g_parts_axis_unit_3D_vector );
      } else if( selected_parts_id == 5 ){
        /* z */
        zFrame3DCopy( &g_fh.frame.att.b.z, &g_parts_axis_unit_3D_vector );
      } else{
        ZRUNERROR( "selected_parts_id is ERROR\n" );
      }
      /* project to view port */
      int cx, cy;
      rkglProject( &cam, &g_fh.frame.pos, &cx, &cy );
      rkglUnproject( &cam, cx, cy, g_znear, &g_vp_circle_center_3D );
      zVec3DSub( &g_vp_mouse_start_3D, &g_vp_circle_center_3D, &g_vp_radial_dir_center_to_start);
    }
    /* end of rotate mode */
  }
  /* end moving mode */
}

void update_alljoint_by_ik_with_frame(zFrame3D *ref_frame)
{
  zVec dis;
  dis = zVecAlloc( rkChainJointSize( &g_chain ) );
  rkChainGetJointDisAll(&g_chain, dis);

  rkIKAttr attr;
  attr.id = selected_link;

  rkIKCell *cell[2];
  cell[0] = rkChainRegIKCellWldAtt( &g_chain, &attr, RK_IK_ATTR_ID );
  cell[1] = rkChainRegIKCellWldPos( &g_chain, &attr, RK_IK_ATTR_ID );
  rkChainDeactivateIK( &g_chain );
  rkChainBindIK( &g_chain );

  zVec3D zyx;
  zMat3DToZYX( &(ref_frame->att), &zyx );
  rkIKCellSetRefVec( cell[0], &zyx );
  rkIKCellSetRefVec( cell[1], &(ref_frame->pos) );

  rkChainFK( &g_chain, dis ); /* copy state to mentatin result consistency */

  rkChainIK( &g_chain, dis, zTOL, 0 );
  rkChainUnregIKCell( &g_chain, cell[1] );
  rkChainUnregIKCell( &g_chain, cell[0] );

  rkChainSetJointDisAll( &g_chain, dis );
  rkChainUpdateFK( &g_chain );
}

void move_link(double angle)
{
  double dis;

  if( selected_link < 0 ) return;
  rkChainLinkJointGetDis( &g_chain, selected_link, &dis );
  dis += angle;
  rkChainLinkJointSetDis( &g_chain, selected_link, &dis );
  rkChainUpdateFK( &g_chain );
}

void mouse(int button, int state, int x, int y)
{
  GLuint selbuf[BUFSIZ];

  switch( button ){
  case GLUT_LEFT_BUTTON:
    if( state == GLUT_DOWN ){
      int hits = rkglPick( &cam, draw_scene, selbuf, BUFSIZ, x, y, 1, 1 );
      select_link( selbuf, hits );
      select_parts( selbuf, x, y, hits );
    }
    break;
  case GLUT_RIGHT_BUTTON:
    if( state == GLUT_DOWN )
      reset_link();
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
  } else{
    /* moving mode */
    zVec3D vp_mouse_goal_3D;
    rkglUnproject( &cam, x, y, g_znear, &vp_mouse_goal_3D );

    if( selected_parts_id >= 0 && selected_parts_id <= 2 ){
      /* translate mode */
      zVec3D vp_mouse_drag_3D_vector;
      zVec3DSub( &vp_mouse_goal_3D, &g_vp_mouse_start_3D, &vp_mouse_drag_3D_vector );

      double project_3D_scale = zVec3DInnerProd( &vp_mouse_drag_3D_vector, &g_vp_parts_axis_3D_vector ) / zVec3DSqrNorm( &g_vp_parts_axis_3D_vector );

      zVec3D dir_3D;
      zVec3DMul( &g_parts_axis_unit_3D_vector, project_3D_scale, &dir_3D );

      /* translate frame */
      zVec3DAdd( &g_frame3D_org.pos, &dir_3D, &g_fh.frame.pos );

      /* end of translate mode */
    } else{
      /* rotate mode */
      zVec3D vp_radial_dir_center_to_goal;
      zVec3DSub( &vp_mouse_goal_3D, &g_vp_circle_center_3D, &vp_radial_dir_center_to_goal);

      if ( !zVec3DIsTiny( &g_vp_radial_dir_center_to_start ) ){
        /* oval circle rotation on view port plane */
        double theta = zVec3DAngle( &g_vp_radial_dir_center_to_start, &vp_radial_dir_center_to_goal, &g_parts_axis_unit_3D_vector);
        zMat3D m;
        /* zVec3D axis; */
        if( selected_parts_id == 3 ){
          /* x */
          zMat3DFromZYX( &m, 0, 0, theta );
        } else if( selected_parts_id == 4 ){
          /* y */
          zMat3DFromZYX( &m, 0, theta, 0 );
        } else if( selected_parts_id == 5 ){
          /* z */
          zMat3DFromZYX( &m, theta, 0, 0 );
        } else{
          ZRUNERROR( "selected_parts_id is ERROR\n" );
        }
        zMulMat3DMat3D( &g_frame3D_org.att, &m, &g_fh.frame.att );
      }
      /* end of if g_vp_radial_dir_center_to_start is not Tiny */
    }
    /* end of rotate mode */

    update_alljoint_by_ik_with_frame( &g_fh.frame );
  }
  /* end moving mode */
}


void resize(int w, int h)
{
  rkglVPCreate( &cam, 0, 0, w, h );
  /* rkglFrustumScale( &cam, 1.0/1000, 0.5, 10 ); */
  rkglOrthoScale( &cam, g_scale, g_znear, g_zfar );
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
  case '8': g_scale += 0.001; rkglOrthoScale( &cam, g_scale, g_znear, g_zfar ); break;
  case '*': g_scale -= 0.001; rkglOrthoScale( &cam, g_scale, g_znear, g_zfar ); break;
  case 'g': move_link( zDeg2Rad(5) ); break;
  case 'h': move_link(-zDeg2Rad(5) ); break;
  case 'q': case 'Q': case '\033':
    rkglChainUnload( &gr );
    rkChainDestroy( &g_chain );
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  rkglSetCallbackParamGLUT( &cam, 0, 0, 0, 0, 0 );

  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  /* rkglCASet( &cam, 1, 1, 1, 45, -30, 0 ); */
  rkglCASet( &cam, 1, 1, 1, 45, -30, 0 );

  glEnable(GL_LIGHTING);
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 1, 3, 6 );

  rkglChainAttr attr;
  rkglChainAttrInit( &attr );
  rkChainReadZTK( &g_chain, "../model/puma.ztk" );
  rkglChainLoad( &gr, &g_chain, &attr, &light );
  rkChainCreateIK( &g_chain );
  rkChainRegIKJointAll( &g_chain, 0.001 );

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

  glutMotionFunc( motion );

  glutReshapeFunc( resize );
  glutKeyboardFunc( keyboard );
  glutIdleFunc( idle );
  init();
  glutMainLoop();
  return 0;
}
