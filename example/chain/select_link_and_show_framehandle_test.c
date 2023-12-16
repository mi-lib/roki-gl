#include <roki_gl/roki_glut.h>

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

/* the main targets of this sample code */
frameHandle_t g_fh;
rkChain g_chain;
rkglChain gr;

/* viewing parameters */
rkglCamera cam;
rkglLight light;
static const GLdouble g_znear = -1000.0;
static const GLdouble g_zfar  = 100.0;
static double g_scale = 0.001;

/* FrameHandle viewing parameters */
static const double g_LENGTH = 0.2;
static const double g_MAGNITUDE = g_LENGTH * 0.5;

/* mouse selected information */
typedef enum{
  NONE=1,
  LINKFRAME,
  FRAMEHANDLE,
} selectedObj;

typedef struct{
  selectedObj obj;
  int link_id;
  int fh_parts_id; /* FrameHandle parts id */
} selectInfo;

selectInfo g_selected;

/* To avoid duplication between selected_link and selected_parts_id */
/* NOFFSET must be enough large than rkChainLinkNum(gr.chain)  */
#define NOFFSET 50

/* draw FrameHandle parts shape */
void draw_fh_parts(void)
{
  int i;
  for( i=0; i < NOBJECTS; i++ ){
    glLoadName( i + NOFFSET );
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
  rkglChainDraw( &gr );
  if( g_selected.obj != NONE ){
    if( gr.info[g_selected.link_id].list_alt == -1 ){
      /* re-drawing selected link once when mouse left clicked */
      zOpticalInfo oi_alt;
      zOpticalInfoCreate( &oi_alt, 0.8, 0.5, 0.5, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.2, NULL );
      gr.attr.disptype = RKGL_FACE;
      /* TODO : reuse selected link list value */
      /* (the current implementation generates new list value by glNewList() ) */
      rkglChainLinkAlt( &gr, g_selected.link_id, &oi_alt, &gr.attr, &light );
    }
    /* drawing FrameHandle */
    draw_fh_parts();
  }
  /* end of if( g_selected.obj != NONE ) */
}

void display(void)
{
  rkglCALoad( &cam );
  rkglLightPut( &light );
  rkglClear();
  draw_scene();
  glutSwapBuffers();
}

static double g_zmin; /* selected depth of the nearest object */
static zFrame3D g_frame3D_org;
static zVec3D g_vp_mouse_start_3D;
static zVec3D g_vp_parts_axis_3D_vector;
static zVec3D g_parts_axis_unit_3D_vector;
static zVec3D g_vp_circle_center_3D;
static zVec3D g_vp_radial_dir_center_to_start;

void reset_link(int new_link_id)
{
  if( new_link_id != g_selected.link_id ){
    if( g_selected.link_id >= 0 ){
      printf( "reset_link : pre gr.info[%d].list = %d, ", g_selected.link_id, gr.info[g_selected.link_id].list );
      printf( "list_alt = %d, ---> ", gr.info[g_selected.link_id].list_alt );

      rkglChainLinkReset( &gr, g_selected.link_id );

      printf( "list = %d, ", gr.info[g_selected.link_id].list );
      printf( "list_alt = %d\n  ", gr.info[g_selected.link_id].list_alt );
    }
    g_selected.link_id = new_link_id;
  }
}

void reset_fh_parts(void)
{
  if( g_selected.fh_parts_id >= 0 ){
    g_fh.partsInfo[ g_selected.fh_parts_id ].updown = 0;
  }
}

void select_object(GLuint selbuf[], int hits)
{
  reset_fh_parts();
  GLuint *ns;
  if( !( ns = rkglFindNearside( selbuf, hits ) ) ){
    g_selected.obj = NONE;
    reset_link( -1 );
    g_selected.fh_parts_id = -1;
    printf( "selected : link_id = %d, ", g_selected.link_id );
    printf( "fh_parts_id = %d \n\n", g_selected.fh_parts_id );
    return;
  }
  int nearest_shape_id = ns[3];
  if( nearest_shape_id >= 0
      && nearest_shape_id < rkChainLinkNum(gr.chain) ){
    g_selected.obj = LINKFRAME;
    reset_link( nearest_shape_id );
    g_selected.fh_parts_id = -1;
  } else if( nearest_shape_id >= NOFFSET
             && nearest_shape_id < NOFFSET + NOBJECTS ){
    g_selected.obj = FRAMEHANDLE;
    /* g_selected.link_id is not changed */
    g_selected.fh_parts_id = nearest_shape_id - NOFFSET;
    /* zmin(=ns[1]) is GLuint data. GLuint maximum value is 0xffffffff. */
    g_zmin = ns[1] / (GLdouble)(0xffffffff);
  } else{
    g_selected.obj = NONE;
    reset_link( -1 );
    g_selected.fh_parts_id = -1;
  }
  printf( "selected : link_id = %d, ", g_selected.link_id );
  printf( "fh_parts_id = %d \n\n", g_selected.fh_parts_id );
}

void select_fh_parts(int x, int y)
{
  g_fh.partsInfo[ g_selected.fh_parts_id ].updown = 1;
  zFrame3DCopy( &g_fh.frame, &g_frame3D_org );

  zVec3D parts_axis_start_3D;
  rkglUnproject( &cam, x, y, g_zmin, &parts_axis_start_3D );
  /* project to view port */
  rkglUnproject( &cam, x, y, g_znear, &g_vp_mouse_start_3D );

  if( g_selected.fh_parts_id >= 0 && g_selected.fh_parts_id < NPOSSIZE ){
    /* translate mode */
    if( g_selected.fh_parts_id == 0 ){
      /* x */
      zFrame3DCopy( &g_fh.frame.att.b.x, &g_parts_axis_unit_3D_vector );
    } else if( g_selected.fh_parts_id == 1 ){
      /* y */
      zFrame3DCopy( &g_fh.frame.att.b.y, &g_parts_axis_unit_3D_vector );
    } else if( g_selected.fh_parts_id == 2 ){
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
    if( g_selected.fh_parts_id == 3 ){
      /* x */
      zFrame3DCopy( &g_fh.frame.att.b.x, &g_parts_axis_unit_3D_vector );
    } else if( g_selected.fh_parts_id == 4 ){
      /* y */
      zFrame3DCopy( &g_fh.frame.att.b.y, &g_parts_axis_unit_3D_vector );
    } else if( g_selected.fh_parts_id == 5 ){
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

/* inverse kinematics */
void update_alljoint_by_ik_with_frame(zFrame3D *ref_frame)
{
  zVec dis;
  dis = zVecAlloc( rkChainJointSize( &g_chain ) );
  rkChainGetJointDisAll(&g_chain, dis);

  rkIKAttr attr;
  attr.id = g_selected.link_id;

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

  if( g_selected.link_id < 0 ) return;
  rkChainLinkJointGetDis( &g_chain, g_selected.link_id, &dis );
  dis += angle;
  rkChainLinkJointSetDis( &g_chain, g_selected.link_id, &dis );
  rkChainUpdateFK( &g_chain );
}

void mouse(int button, int state, int x, int y)
{
  GLuint selbuf[BUFSIZ];

  switch( button ){
  case GLUT_LEFT_BUTTON:
    if( state == GLUT_DOWN ){
      int pre_selected_link_id = g_selected.link_id;
      int hits = rkglPick( &cam, draw_scene, selbuf, BUFSIZ, x, y, 1, 1 );
      /* update g_selected status */
      select_object( selbuf, hits );
      switch( g_selected.obj ){
      case LINKFRAME:
        /* update frame handle location */
        if( g_selected.link_id != pre_selected_link_id ) {
          zFrame3DCopy( rkChainLinkWldFrame(gr.chain, g_selected.link_id), &g_fh.frame );
        }
        break;
      case FRAMEHANDLE:
        select_fh_parts( x, y );
        break;
      default: ;
      }
      /* end of switch( g_selected.obj ) */
    }
    break;
  case GLUT_RIGHT_BUTTON:
    if( state == GLUT_DOWN ){
    }
    break;
  default: ;
  }
  /* end of switch( button ) */

  if( g_selected.obj != FRAMEHANDLE )
  {
    rkglMouseFuncGLUT(button, state, x, y);
  }
}

void motion(int x, int y)
{
  if( g_selected.obj != FRAMEHANDLE ){
    rkglMouseDragFuncGLUT(x, y);
  } else{
    /* moving mode */
    zVec3D vp_mouse_goal_3D;
    rkglUnproject( &cam, x, y, g_znear, &vp_mouse_goal_3D );

    if( g_selected.fh_parts_id >= 0 && g_selected.fh_parts_id <= 2 ){
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
        if( g_selected.fh_parts_id == 3 ){
          /* x */
          zMat3DFromZYX( &m, 0, 0, theta );
        } else if( g_selected.fh_parts_id == 4 ){
          /* y */
          zMat3DFromZYX( &m, 0, theta, 0 );
        } else if( g_selected.fh_parts_id == 5 ){
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
  case '8': g_scale += 0.0001; rkglOrthoScale( &cam, g_scale, g_znear, g_zfar ); break;
  case '*': g_scale -= 0.0001; rkglOrthoScale( &cam, g_scale, g_znear, g_zfar ); break;
  case 'g': move_link( zDeg2Rad(5) ); break;
  case 'h': move_link(-zDeg2Rad(5) ); break;
  case 'q': case 'Q': case '\033':
    rkglChainUnload( &gr );
    rkChainDestroy( &g_chain );
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
    glPushMatrix();
    rkglXform( &g_fh.frame );
    rkglFrameAxisMaterial( g_AXES[i] );
    /* start register */
    g_fh.partsInfo[i].list = glGenLists( 1 );
    if( glIsList( g_fh.partsInfo[i].list ) )
      glDeleteLists( g_fh.partsInfo[i].list, 1 );
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
  rkglSetCallbackParamGLUT( &cam, 0, 0, 0, 0, 0 );

  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
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

  g_selected.obj = NONE;
  g_selected.link_id = -1;
  g_selected.fh_parts_id = -1;

  init_fh_parts();
}

void idle(void){ glutPostRedisplay(); }

int main(int argc, char *argv[])
{
  /* initialize the location of frame handle object */
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
