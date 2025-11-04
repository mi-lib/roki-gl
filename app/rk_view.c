/* rk_view - 3D geometry viewer */

#if defined(__ROKI_GL_USE_GLX)
#include <unistd.h>
#include <roki_gl/roki_glx.h>
#include <zx11/zximage.h>

#define RK_VIEW_TITLE "RK-VIEW"

enum{
  OPT_PAN, OPT_TILT, OPT_ROLL, OPT_OX, OPT_OY, OPT_OZ, OPT_AUTO,
  OPT_WIDTH, OPT_HEIGHT,
  OPT_SCALE,
  OPT_NONFACE,
  OPT_WIREFRAME,
  OPT_BG,
  OPT_LX, OPT_LY, OPT_LZ,
  OPT_SMOOTH, OPT_SHADOW,
  OPT_PCD_NORMAL,
  OPT_CAPTURE,
  OPT_HELP,
  OPT_INVALID
};
zOption opt[] = {
  { "pan", NULL, "<pan value>", "set camera pan angle", (char *)"0", false },
  { "tilt", NULL, "<tilt value>", "set camera tilt angle", (char *)"0", false },
  { "roll", NULL, "<roll value>", "set camera roll angle", (char *)"0", false },
  { "x", NULL, "<value>", "camera position in x axis", (char *)"2", false },
  { "y", NULL, "<value>", "camera position in y axis", (char *)"0", false },
  { "z", NULL, "<value>", "camera position in z axis", (char *)"0", false },
  { "auto", NULL, NULL, "automatic allocation of camera", NULL, false },
  { "width", NULL, "<width>", "set window width", (char *)"500", false },
  { "height", NULL, "<height>", "set window height", (char *)"500", false },
  { "scale", NULL, "<scale>", "set scale factor", (char *)"1.0", false },
  { "nonface", NULL, NULL, "undraw solid model of the kinematic chain", NULL, false },
  { "wireframe", NULL, "<color name>", "draw wireframes of objects", (char *)"white", false },
  { "bg", NULL, "<RGB#hex>", "set background color", (char *)"#010101", false },
  { "lx", NULL, "<value>", "light position in x axis", (char *)"3", false },
  { "ly", NULL, "<value>", "light position in y axis", (char *)"0", false },
  { "lz", NULL, "<value>", "light position in z axis", (char *)"3", false },
  { "smooth", NULL, NULL, "enable antialias", NULL, false },
  { "shadow", NULL, NULL, "enable shadow", NULL, false },
  { "normal", NULL, "<radius of vicinity>", "enable normal vector estimation (only available for pointcloud)", (char *)"0.003", false },
  { "xwd", NULL, "<suf>", "output image format suffix", (char *)"png", false },
  { "help", NULL, NULL, "show this message", NULL, false },
  { NULL, NULL, NULL, NULL, NULL, false },
};

#define RK_VIEW_BUFSIZ 512

Window win;

rkglCamera cam;
rkglLight light;
rkglShadow shadow;
int model = -1;

/* view volume */
zSphere3D boundingsphere;

void rk_viewUsage(void)
{
  eprintf( "Usage: rk_view <options> [model file1] [model file2] ...\n" );
  eprintf( "<options>\n" );
  zOptionHelp( opt );
  exit( EXIT_SUCCESS );
}

int rk_viewChangeDir(char *pathname, char *dirname, char *filename, char *cwd, size_t size)
{
  if( !getcwd( cwd, size ) ){
    ZRUNERROR( "astray from directory" );
    return -1;
  }
  zGetDirFilename( pathname, dirname, filename, size );
  if( *dirname ){
    if( chdir( dirname ) < 0 ){
      ZRUNERROR( "cannot change directory to %s", dirname );
      return -1;
    }
  }
  return 0;
}

int rk_viewReturnDir(char *cwd)
{
  if( chdir( cwd ) < 0 ){
    ZRUNERROR( "cannot change directory to %s", cwd );
    return -1;
  }
  return 0;
}

void rk_viewReadPH(zMShape3D *ms, const char *filename, const char *sfx)
{
  zMShape3DInit( ms );
  zMShape3DAllocOpticArray( ms, 1 );
  zMShape3DAllocShapeArray( ms, 1 );
  if( zMShape3DOpticNum(ms) != 1 || zMShape3DShapeNum(ms) != 1 ){
    ZALLOCERROR();
    exit( EXIT_FAILURE );
  }
  zOpticalInfoInit( zMShape3DOptic(ms,0) );
  if( strcmp( sfx, "dae" ) == 0 || strcmp( sfx, "DAE" ) == 0 ){
    if( !zShape3DReadFileDAE( zMShape3DShape(ms,0), filename ) ) exit( EXIT_FAILURE );
    return;
  }
  if( strcmp( sfx, "stl" ) == 0 || strcmp( sfx, "STL" ) == 0 ){
    if( !zShape3DReadFileSTL( zMShape3DShape(ms,0), filename ) ) exit( EXIT_FAILURE );
    return;
  }
  if( strcmp( sfx, "obj" ) == 0 || strcmp( sfx, "OBJ" ) == 0 ){
    if( !zShape3DReadFileOBJ( zMShape3DShape(ms,0), filename ) ) exit( EXIT_FAILURE );
    return;
  }
  if( strcmp( sfx, "ply" ) == 0 || strcmp( sfx, "PLY" ) == 0 ){
    if( !zShape3DReadFilePLY( zMShape3DShape(ms,0), filename ) ) exit( EXIT_FAILURE );
    return;
  }
  ZRUNERROR( "unknown format %s", sfx );
  rk_viewUsage();
}

void rk_viewReadModel(zStrAddrList *modellist)
{
  zStrListCell *cp;
  zVec3DList pointlist_all;
  zVec3DData pointdata, normaldata, pointdata_all;
  const char *sfx;
  double scale;
  zMShape3D ms;
  ubyte disptype;
  char dirname[BUFSIZ], filename[BUFSIZ], cwd[BUFSIZ];
  int i;

  zListInit( &pointlist_all );
  model = rkglBeginList();
  zListForEach( modellist, cp ){
    if( !( sfx = zGetSuffix( cp->data ) ) ) sfx = "ztk";
    rk_viewChangeDir( cp->data, dirname, filename, cwd, BUFSIZ );
    if( strcmp( sfx, "pcd" ) == 0 ){
      if( !zVec3DDataReadPCDFile( &pointdata, filename ) ){
        ZOPENERROR( cp->data );
        rk_viewUsage();
      }
      rkglRGBByStr( "white" ); /* for point cloud */
      glPointSize( 1.0 );
      rkglPointCloud( &pointdata );
      if( opt[OPT_PCD_NORMAL].flag ){
        zVec3DDataNormalVec_Octree( &pointdata, atof( opt[OPT_PCD_NORMAL].arg ), &normaldata );
        rkglRGBByStr( "green" ); /* for normal vectors */
        glLineWidth( 0.5 );
        rkglPointCloudNormal( &pointdata, &normaldata, 0.01 );
        zVec3DDataDestroy( &normaldata );
      }
      if( opt[OPT_AUTO].flag )
        zVec3DListAppendArray( &pointlist_all, pointdata.data.array );
      else
        zVec3DDataDestroy( &pointdata );
      continue;
    }
    if( strcmp( sfx, "ztk" ) == 0 ){
      if( !zMShape3DReadZTK( &ms, filename ) ){
        ZOPENERROR( cp->data );
        rk_viewUsage();
      }
    } else
      rk_viewReadPH( &ms, filename, sfx );
    rk_viewReturnDir( cwd );

    if( opt[OPT_SCALE].flag ){
      scale = atof( opt[OPT_SCALE].arg );
      for( i=0; i<zMShape3DShapeNum(&ms); i++ )
        zPH3DScaleDRC( zShape3DPH(zMShape3DShape(&ms,i)), scale );
    }
    disptype = opt[OPT_NONFACE].flag ? 0 : RKGL_FACE;
    if( opt[OPT_WIREFRAME].flag ){
      disptype |= RKGL_WIREFRAME;
      rkglRGBByStr( opt[OPT_WIREFRAME].arg );
    }
    rkglMShape( &ms, disptype, &light );
    if( opt[OPT_AUTO].flag ){
      zMShape3DVertData( &ms, &pointdata );
      zListAppend( &pointlist_all, pointdata.data.list );
    }
    zMShape3DDestroy( &ms );
  }
  glEndList();
  if( opt[OPT_AUTO].flag ){
    zVec3DDataAssignList( &pointdata_all, &pointlist_all );
    if( !zVec3DDataBoundingBall( &pointdata_all, &boundingsphere, NULL ) ) exit( EXIT_FAILURE );
    zVec3DDataDestroy( &pointdata_all );
    zVec3DListDestroy( &pointlist_all );
  }
  if( model < 0 ) exit( EXIT_FAILURE );
}

void rk_viewResetCamera(void)
{
  double vv_fovy, vv_near, vv_far;

  if( opt[OPT_AUTO].flag ){
    rkglCameraLookAt( &cam,
      zSphere3DCenter(&boundingsphere)->c.x+zSphere3DRadius(&boundingsphere)*18, zSphere3DCenter(&boundingsphere)->c.y, zSphere3DCenter(&boundingsphere)->c.z,
      zSphere3DCenter(&boundingsphere)->c.x, zSphere3DCenter(&boundingsphere)->c.y, zSphere3DCenter(&boundingsphere)->c.z,
      0, 0, 1 );
    vv_fovy = 2 * zRad2Deg( asin( 1.0/18 ) );
    vv_near = zSphere3DRadius(&boundingsphere);
    vv_far = 1000*zSphere3DRadius(&boundingsphere);
  } else{
    rkglCameraSetViewframe( &cam,
      atof(opt[OPT_OX].arg), atof(opt[OPT_OY].arg), atof(opt[OPT_OZ].arg),
      atof(opt[OPT_PAN].arg), atof(opt[OPT_TILT].arg), atof(opt[OPT_ROLL].arg) );
    vv_fovy = 30.0;
    vv_near = 1;
    vv_far = 200;
  }
  rkglCameraFitPerspective( &cam, vv_fovy, vv_near, vv_far );
  rkglSetDefaultCamera( &cam );
}

void rk_viewResetLight(void)
{
  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.5, 0.5, 0.5, 0.8, 0.8, 0.8, 0, 0, 0 );
  rkglShadowInit( &shadow, 512, 512, 1.5, 0.2, 0.1 );
}

void rk_viewPutLight(void)
{
  double x, y, z, ratio;

  x = atof(opt[OPT_LX].arg);
  y = atof(opt[OPT_LY].arg);
  z = atof(opt[OPT_LZ].arg);
  if( opt[OPT_AUTO].flag ){
    ratio = zSphere3DRadius(&boundingsphere) * 10 / sqrt( x*x + y*y + z*z );
    x *= ratio;
    y *= ratio;
    z *= ratio;
  }
  rkglLightMove( &light, x, y, z );
}

void rk_viewInit(void)
{
  zRGB rgb;

  win = rkglWindowCreateGLX( NULL, 0, 0, atoi(opt[OPT_WIDTH].arg), atoi(opt[OPT_HEIGHT].arg), RK_VIEW_TITLE );
  rkglWindowKeyEnableGLX( win );
  rkglWindowMouseEnableGLX( win );
  rkglWindowOpenGLX( win );

  zRGBDecodeStr( &rgb, opt[OPT_BG].arg );
  rkglCameraInit( &cam );
  rkglCameraSetBackground( &cam, rgb.r, rgb.g, rgb.b );
  rkglCameraSetViewport( &cam, 0, 0, atoi(opt[OPT_WIDTH].arg), atoi(opt[OPT_HEIGHT].arg) );

  rkglTextureEnable();
  if( opt[OPT_SMOOTH].flag ) glEnable( GL_LINE_SMOOTH );
}

bool rk_viewCommandArgs(int argc, char *argv[])
{
  zStrAddrList modellist;

  if( argc <= 1 ) rk_viewUsage();
  zOptionRead( opt, argv, &modellist );
  if( opt[OPT_HELP].flag ) rk_viewUsage();

  if( zListIsEmpty( &modellist ) ){
    ZRUNERROR( "model not specified" );
    return false;
  }
  zPH3DEchoOn();
  rk_viewInit();
  rk_viewResetLight();
  rk_viewReadModel( &modellist );
  rk_viewPutLight();
  rk_viewResetCamera();
  zStrAddrListDestroy( &modellist );
  return true;
}

void rk_viewExit(void)
{
  rkglWindowCloseGLX( win );
  rkglExitGLX();
}

/**********************************************************/

void rk_viewDraw(void)
{
  glCallList( model );
}

void rk_viewDisplay(void)
{
  rkglWindowActivateGLX( win );
  if( opt[OPT_SHADOW].flag ){
    /* shadow-map rendering */
    rkglShadowDraw( &shadow, &cam, &light, rk_viewDraw );
  } else{
    /* non-shadowed rendering */
    rkglClear();
    rkglCameraPut( &cam );
    rkglLightPut( &light );
    rk_viewDraw();
  }
  rkglWindowSwapBuffersGLX( win );
  rkglFlushGLX();
}

void rk_viewReshape(void)
{
  zxRegion reg;

  zxGetGeometry( win, &reg );
  rkglCameraSetViewport( &cam, 0, 0, reg.width, reg.height );
  rkglCameraPerspective( &cam );
}

void rk_viewCapture(void)
{
  zxRegion reg;
  zxImage img;
  static char imgfile[RK_VIEW_BUFSIZ];
  static int count = 0;

  sprintf( imgfile, "%04d.%s", count++, opt[OPT_CAPTURE].arg );
  zxGetGeometry( win, &reg );
  zxImageAllocDefault( &img, reg.width, reg.height );
  zxImageFromPixmap( &img, win, img.width, img.height );
  zxImageWritePNGFile( &img, imgfile );
  zxImageDestroy( &img );
}

int rk_viewKeyPress(void)
{
  static bool from_light = false;

  zxModkeyOn( zxKeySymbol() );
  switch( zxKeySymbol() ){
  case XK_l: /* toggle viewpoint to light/camera */
    if( ( from_light = 1 - from_light ) ){
      rkglCameraLookAt( &cam,
        atof(opt[OPT_LX].arg), atof(opt[OPT_LY].arg), atof(opt[OPT_LZ].arg), 0, 0, 0, -1, 0, 1 );
    } else
      rk_viewResetCamera();
    break;
  case XK_c:
    rk_viewCapture();
    break;
  case XK_r:
    rk_viewResetCamera();
    break;
  case XK_q:
    return -1;
  }
  return 0;
}

int rk_viewEvent(void)
{
  int event;

  switch( ( event = zxDequeueEvent() ) ){
  case Expose:
  case ConfigureNotify: rk_viewReshape();             break;
  case ButtonPress:
  case ButtonRelease:   rkglMouseFuncGLX( &cam, event ); break;
  case MotionNotify:    rkglMouseDragFuncGLX( &cam ); break;
  case KeyPress:        if( rk_viewKeyPress() >= 0 )  break; return -1;
  case KeyRelease:      zxModkeyOff( zxKeySymbol() ); break;
  default: ;
  }
  return 0;
}

void rk_viewLoop(void)
{
#define RK_VIEW_LOOP_SKIP 5
  int count = 0;

  /* initial draw */
  rk_viewReshape();
  rk_viewDisplay();
  rkglInitGLEW();
  while( rk_viewEvent() == 0 ){
    if( count++ > RK_VIEW_LOOP_SKIP ){
      rk_viewDisplay();
      count = 0;
    }
  }
}

int main(int argc, char *argv[])
{
  rkglInitGLX();
  if( !rk_viewCommandArgs( argc, argv+1 ) ) return EXIT_FAILURE;
  rk_viewLoop();
  rk_viewExit();
  return EXIT_SUCCESS;
}
#else
#include <roki_gl/roki_gl.h>

int main(int argc, char *argv[])
{
  ROKI_GL_LIB_MANDATORY( "GLX", argv[0] );
  return EXIT_SUCCESS;
}
#endif /* __ROKI_GL_USE_GLX */
