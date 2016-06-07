#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"

#ifdef PC32

#include <GL/gl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
SDL_Surface *screen = NULL;
#endif


#ifdef GP2XWIZ
#include "Panel/fake_os.h"
#endif

#if defined(GP2X) || defined(PANDORA) || defined(GCW)
#if defined(PANDORA)
#include <X11/Xlib.h>
#include "GLES/gl.h"
#include "GLES/egl.h"
#include "GLES/glext.h"
#include <SDL/SDL_syswm.h>

Display *g_x11Display = NULL;
#elif defined(GP2XCAANOO) || defined(GCW)
#include "GLES/gl.h"
#include "GLES/egl.h"
#include "GLES/glext.h"
#endif
#ifdef GP2XWIZ
#include "OpenGLES/gl.h"
#include "OpenGLES/egl.h"
#include "OpenGLES/glext.h"
#endif

EGLDisplay glDisplay;
EGLConfig glConfig;
EGLContext glContext;
EGLSurface glSurface;
NativeWindowType hNativeWnd=0;
const char *gl_vendor,*gl_renderer,*gl_version,*gl_extensions;
#if defined(PANDORA)

EGLint attrib_list_fsaa[]= {
	EGL_RED_SIZE,		5,
	EGL_GREEN_SIZE,		6,
	EGL_BLUE_SIZE,		5,
	EGL_DEPTH_SIZE,		16,
	EGL_SURFACE_TYPE,	EGL_WINDOW_BIT,
	EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES_BIT,
	EGL_SAMPLE_BUFFERS,	1,
	EGL_SAMPLES,		1,
	EGL_NONE
};

EGLint attrib_list[]= {
	EGL_RED_SIZE,		5,
	EGL_GREEN_SIZE,		6,
	EGL_BLUE_SIZE,		5,
	EGL_DEPTH_SIZE,		16,
	EGL_SURFACE_TYPE,	EGL_WINDOW_BIT,
	EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES_BIT,
	EGL_SAMPLE_BUFFERS,	1,
	EGL_SAMPLES,		4,
	EGL_NONE
};
#else
EGLint attrib_list_fsaa[]= {EGL_SURFACE_TYPE,EGL_WINDOW_BIT,EGL_BUFFER_SIZE,16,EGL_DEPTH_SIZE,16,EGL_SAMPLE_BUFFERS,1,EGL_SAMPLES,4,EGL_NONE};
EGLint attrib_list[]= {EGL_SURFACE_TYPE,EGL_WINDOW_BIT,EGL_BUFFER_SIZE,16,EGL_DEPTH_SIZE,16,EGL_NONE};
#endif

SDL_Surface *screen = NULL;
#endif

SDL_Joystick *gamepad=NULL, *pissdoranub2=NULL;;

int audio_channels=2,audio_rate=22050,audio_buffers=1024;
Uint16 audio_format= AUDIO_S16;

#include "vars.h"
#include "zresm.h"
#include "render.h"
#include "zcsound.h"

char spath[256];

// Video SubSystem Begin

GLushort texturedata_0[65536];

u8 filtertag[256]=
{
    0,0,0,0,1,0,0,1, 0,0,1,0,0,0,0,0,
    1,1,1,1,1,1,1,1, 0,0,0,1,1,1,1,1,
    1,0,0,0,0,0,0,0, 0,0,0,1,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,1,1,0,0,

    0,0,0,0,0,0,0,0, 0,0,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
    1,1,1,1,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,//last one is noicemap

    0,0,1,1,1,1,1,1, 1,1,1,1,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,

    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0
};

void corereloadtexture(unsigned char index)
{
    u32 ii,i;
    u16 x,y;
    i=0;
    ii=texturepointer[index];
    for (y=0; y<textureheader[index][2]; y++) {
        for (x=0; x<textureheader[index][1]; x++)texturedata_0[i++]=texturedata[ii++];
    }

    glBindTexture(GL_TEXTURE_2D,zc_texture[index]);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

//if (index<24 | index>31)
    {
//if ((textureheader[index][2]==16)|(index==8)|(index>=100))
        if (filtertag[index]==0)
        {
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        }
    }
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,textureheader[index][1],textureheader[index][2],0,GL_RGBA,GL_UNSIGNED_SHORT_4_4_4_4,texturedata_0);
}




int8_t CheckGLESErrors( const char* file, uint16_t line )
{
   EGLenum error;
   const char* errortext;
   const char* description;
   error = eglGetError();
   if (error != EGL_SUCCESS && error != 0)
   {
      switch (error)
      {
         case EGL_NOT_INITIALIZED:
            errortext = "EGL_NOT_INITIALIZED.";
            description = "EGL is not or could not be initialized, for the specified display.";
            break;
         case EGL_BAD_ACCESS:
            errortext = "EGL_BAD_ACCESS EGL";
            description = "Cannot access a requested resource (for example, a context is bound in another thread).";
            break;
         case EGL_BAD_ALLOC:
            errortext = "EGL_BAD_ALLOC EGL";
            description = "Failed to allocate resources for the requested operation.";
            break;
         case EGL_BAD_ATTRIBUTE:
            errortext = "EGL_BAD_ATTRIBUTE";
            description = "An unrecognized attribute or attribute value was passed in anattribute list.";
            break;
         case EGL_BAD_CONFIG:
            errortext = "EGL_BAD_CONFIG";
            description = "An EGLConfig argument does not name a valid EGLConfig.";
            break;
         case EGL_BAD_CONTEXT:
            errortext = "EGL_BAD_CONTEXT";
            description = "An EGLContext argument does not name a valid EGLContext.";
            break;
         case EGL_BAD_CURRENT_SURFACE:
            errortext = "EGL_BAD_CURRENT_SURFACE";
            description = "The current surface of the calling thread is a window, pbuffer,or pixmap that is no longer valid.";
            break;
         case EGL_BAD_DISPLAY:
            errortext = "EGL_BAD_DISPLAY";
            description = "An EGLDisplay argument does not name a valid EGLDisplay.";
            break;
         case EGL_BAD_MATCH:
            errortext = "EGL_BAD_MATCH";
            description = "Arguments are inconsistent; for example, an otherwise valid context requires buffers (e.g. depth or stencil) not allocated by an otherwise valid surface.";
            break;
         case EGL_BAD_NATIVE_PIXMAP:
            errortext = "EGL_BAD_NATIVE_PIXMAP";
            description = "An EGLNativePixmapType argument does not refer to a validnative pixmap.";
            break;
         case EGL_BAD_NATIVE_WINDOW:
            errortext = "EGL_BAD_NATIVE_WINDOW";
            description = "An EGLNativeWindowType argument does not refer to a validnative window.";
            break;
         case EGL_BAD_PARAMETER:
            errortext = "EGL_BAD_PARAMETER";
            description = "One or more argument values are invalid.";
            break;
         case EGL_BAD_SURFACE:
            errortext = "EGL_BAD_SURFACE";
            description = "An EGLSurface argument does not name a valid surface (window,pbuffer, or pixmap) configured for rendering";
            break;
         case EGL_CONTEXT_LOST:
            errortext = "EGL_CONTEXT_LOST";
            description = "A power management event has occurred. The application must destroy all contexts and reinitialise client API state and objects to continue rendering.";
            break;
         default:
            errortext = "Unknown EGL Error";
            description = "";
            break;
      }
      printf( "OpenGLES ERROR: EGL Error detected in file %s at line %d: %s (0x%X)\n Description: %s\n", file, line, errortext, error, description );
      return 1;
   }
   return 0;
}





void coreupdatetextures(void)
{
    u16 i;
    for (i=0; i<256; i++)if (texturereload[i]) {
            texturereload[i]=0;
            if (textureheader[i][0]) corereloadtexture(i);
        }
}

void zcore_video_init(void)
{
    screenwidth=640;
    screenheight=480;
    SDL_InitSubSystem(SDL_INIT_VIDEO);
    SDL_ShowCursor(0);
    SDL_ShowCursor(0);
#ifdef PC32
//screenwidth=800;
//screenheight=600;
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
    screen=SDL_SetVideoMode(screenwidth,screenheight,32, SDL_OPENGL); //| SDL_FULLSCREEN);
//screen=SDL_SetVideoMode(screenwidth,screenheight,32, SDL_OPENGL);

    glVertexPointer(3,GL_FLOAT,0,mesh);
    glTexCoordPointer(2,GL_FLOAT,0,mesht);
    glFogi(GL_FOG_MODE,GL_LINEAR);

    glFogf(GL_FOG_START,5.0f);
    glFogf(GL_FOG_END,7.0f);


    glAlphaFunc(GL_GREATER,0.5);
    glClear(GL_ACCUM_BUFFER_BIT);
//glHint (GL_FOG_HINT, GL_NICEST);
#endif





#if defined(GCW)

////////////////////////////////////////////////////////////////////////////////////////
   screen=SDL_SetVideoMode(320,240,16, SDL_OPENGL);

   const char* output;

   EGLint egl_config_attr[] = {
      EGL_BUFFER_SIZE,   16,
      EGL_DEPTH_SIZE,    16,     
      EGL_STENCIL_SIZE,  0,
      EGL_SURFACE_TYPE,     EGL_WINDOW_BIT,
      EGL_NONE           // This is important as it tells EGL when to stop looking in the array
   };

	screenwidth=320;
	screenheight=240;
	int screenbpp=16;
	EGLint numConfigs,majorVersion,minorVersion;
	EGLBoolean result;
	glDisplay=eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (glDisplay == EGL_NO_DISPLAY)
	{
      CheckGLESErrors( __FILE__, __LINE__ );
      printf( "OpenGLES ERROR: Unable to create EGL display.\n" );
	}
	printf( "OpenGLES: Initializing\n" );
	result = eglInitialize(glDisplay, &majorVersion, &minorVersion );
	if (result != EGL_TRUE )
	{
	  CheckGLESErrors( __FILE__, __LINE__ );
	  printf( "OpenGLES ERROR: Unable to initialize EGL display.\n" );
	}
   /* Get EGL Library Information */
   printf( "EGL Implementation Version: Major %d Minor %d\n", majorVersion, minorVersion );
   output = eglQueryString( glDisplay, EGL_VENDOR );
   printf( "EGL_VENDOR: %s\n", output );
   output = eglQueryString( glDisplay, EGL_VERSION );
   printf( "EGL_VERSION: %s\n", output );
   output = eglQueryString( glDisplay, EGL_EXTENSIONS );
   printf( "EGL_EXTENSIONS: %s\n", output );
   printf("OpenGLES: Requesting %dx%d %dbpp configuration\n", screenwidth, screenheight, screenbpp);
   result = eglChooseConfig(glDisplay, egl_config_attr, &glConfig, 1, &numConfigs);

   if (result != EGL_TRUE || numConfigs == 0)
   {
      CheckGLESErrors( __FILE__, __LINE__ );
      printf( "EGLport ERROR: Unable to query for available configs, found %d.\n", numConfigs );
   }

   printf( "OpenGLES: Creating context\n" );
   glContext = eglCreateContext(glDisplay, glConfig, EGL_NO_CONTEXT, NULL);
   if (glContext== EGL_NO_CONTEXT)
   {
      CheckGLESErrors( __FILE__, __LINE__ );
      printf( "OpenGLES ERROR: Unable to create GLES context!\n");
   }

   // For raw FBDEV on GCW0:
   hNativeWnd=0;

   printf( "OpenGLES: Creating window surface\n" );
   glSurface = eglCreateWindowSurface(glDisplay, glConfig, hNativeWnd, NULL);
   if (glSurface == EGL_NO_SURFACE)
   {
      CheckGLESErrors( __FILE__, __LINE__ );
      printf( "OpenGLES ERROR: Unable to create EGL surface!\n" );
   }

   printf( "OpenGLES: Making current the new context\n" );
   result = eglMakeCurrent(glDisplay, glSurface, glSurface, glContext);
   if (result != EGL_TRUE)
   {
      CheckGLESErrors( __FILE__, __LINE__ );
      printf( "OpenGLES ERROR: Unable to make GLES context current\n" );
   }

   printf( "OpenGLES: Setting swap interval\n" );
   eglSwapInterval(glDisplay, 1);      // We want VSYNC

   printf( "OpenGLES: Initialization complete\n" );
   CheckGLESErrors( __FILE__, __LINE__ );

	SDL_ShowCursor(SDL_DISABLE);
    glVertexPointer(3,GL_FIXED,0,mesh);
    glTexCoordPointer(2,GL_FIXED,0,mesht);
    glFogf(GL_FOG_MODE,GL_LINEAR);
    glAlphaFuncx(GL_GREATER,65536/2);

#endif

///////////////////////////////////////////////////////////////////////////////////////////////



#if defined(GP2X) || defined(PANDORA)
    EGLint numConfigs,majorVersion,minorVersion;

  #ifdef PANDORA
    //screenwidth=400;
    //screenheight=240;
    screenwidth=800;
    screenheight=480;
    screen=SDL_SetVideoMode( screenwidth, screenheight, 0, SDL_SWSURFACE | SDL_FULLSCREEN );
    g_x11Display = XOpenDisplay(NULL);
    #define _EGL_DSP (EGLNativeDisplayType)g_x11Display


  #else
    screen=SDL_SetVideoMode(320,240,16, SDL_OPENGL);
    #define _EGL_DSP (NativeDisplayType)0

    hNativeWnd=(EGLNativeWindowType)NULL;

  #endif

    glDisplay=eglGetDisplay(_EGL_DSP);
    eglInitialize(glDisplay,&majorVersion,&minorVersion);
    eglChooseConfig(glDisplay,attrib_list,&glConfig,1,&numConfigs);
  #ifdef PANDORA
    SDL_SysWMinfo sysInfo;
    SDL_VERSION(&sysInfo.version); //Set SDL version
    SDL_GetWMInfo(&sysInfo);
    glSurface=eglCreateWindowSurface(glDisplay,glConfig,(EGLNativeWindowType)sysInfo.info.x11.window,0);
    glContext=eglCreateContext(glDisplay,glConfig,NULL, NULL);
  #else
    glSurface=eglCreateWindowSurface(glDisplay,glConfig,hNativeWnd,attrib_list);
    glContext=eglCreateContext(glDisplay,glConfig,EGL_NO_CONTEXT,attrib_list);
  #endif
    eglMakeCurrent(glDisplay,glSurface,glSurface,glContext);
if (eglGetError()!=EGL_SUCCESS) printf("eglMakeCurrent FAILD\n");

    SDL_ShowCursor(SDL_DISABLE);
    glVertexPointer(3,GL_FIXED,0,mesh);
    glTexCoordPointer(2,GL_FIXED,0,mesht);
    glFogf(GL_FOG_MODE,GL_LINEAR);
    glAlphaFuncx(GL_GREATER,65536/2);
#endif

    glGenTextures(256,zc_texture);
    glColorPointer(4,GL_UNSIGNED_BYTE,0,meshc);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

//glLoadIdentity();
    coreupdatetextures();
}

void zcore_video_frame(void)
{
    if (thisframenice)
    {
        coreupdatetextures();
        corerenderrender();

#ifdef PC32
        SDL_GL_SwapBuffers();
#endif

#if defined(GP2X) || defined(PANDORA) || defined(GCW)
        eglSwapBuffers(glDisplay, glSurface);
#endif
    }
}

void zcore_video_down(void)
{
    glDeleteTextures(256,zc_texture);
#if defined(GP2X) || defined(PANDORA) || defined(GCW) 
    eglDestroySurface(glDisplay,glSurface);
    eglDestroyContext(glDisplay,glContext);
    eglTerminate(glDisplay);
	#ifndef GCW
    free(hNativeWnd);
	#endif
#endif
}

// Video SubSystem End
// Sound SubSystem Begin

void zcore_sound_init(void)
{
    SDL_InitSubSystem(SDL_INIT_AUDIO);

    Mix_OpenAudio(audio_rate,audio_format,audio_channels,audio_buffers);
    Mix_QuerySpec(&audio_rate,&audio_format,&audio_channels);

    zcinitsound();
}

void zcore_sound_frame(void)
{
    zcsoundstep();
}

void zcore_sound_down(void)
{

    Mix_CloseAudio();
}

// Sound SubSystem End
// Input SubSystem Begin

#ifdef PANDORA
int i_keyb[20];
static const SDLKey code_keyb[20]=
{
    SDLK_HOME, SDLK_PAGEDOWN, SDLK_END, SDLK_PAGEUP, SDLK_RSHIFT, SDLK_RCTRL ,SDLK_7,SDLK_8,
    SDLK_LALT,SDLK_c,SDLK_q,SDLK_w,SDLK_e,SDLK_r,SDLK_t,SDLK_F1,
    SDLK_UP,SDLK_RIGHT,SDLK_DOWN,SDLK_LEFT
};
s8 jkey_map[16]= {0,1,2,3,4,5,6,7,8,9,-1,-1,-1,-1,-1,-1};
#endif

#ifdef GCW
int i_keyb[20];
static const SDLKey code_keyb[20]=
{
    SDLK_LCTRL, SDLK_LALT, SDLK_RETURN, SDLK_SPACE, SDLK_TAB, SDLK_BACKSPACE ,SDLK_7,SDLK_8,
    SDLK_ESCAPE,SDLK_LSHIFT,SDLK_q,SDLK_w,SDLK_e,SDLK_r,SDLK_t,SDLK_HOME,
    SDLK_UP,SDLK_RIGHT,SDLK_DOWN,SDLK_LEFT
};
s8 jkey_map[16]= {0,1,2,3,4,5,6,7,8,9,-1,-1,-1,-1,-1,-1};
#endif

#ifdef PC32
int i_keyb[20];
static const SDLKey code_keyb[20]=
{
    SDLK_LCTRL,SDLK_SPACE,SDLK_LALT,SDLK_z,SDLK_LSHIFT,SDLK_x,SDLK_7,SDLK_8,
    SDLK_ESCAPE,SDLK_c,SDLK_q,SDLK_w,SDLK_e,SDLK_r,SDLK_t,SDLK_BACKSPACE,
    SDLK_UP,SDLK_RIGHT,SDLK_DOWN,SDLK_LEFT
};
s8 jkey_map[16]= {0,1,2,3,4,5,6,7,8,9,-1,-1,-1,-1,-1,-1};
#endif

#ifdef GP2XWIZ
s8 jkey_map[16]= {12,14,13,15,10,11,17,16,8,9,-1,-1,-1,-1,-1,-1};
#endif

#ifdef GP2XCAANOO
s8 jkey_map[16]= { 0, 1, 2, 3, 4, 5, 0, 0,6,8,-1,-1,-1,-1,-1,-1};
#endif

u16 s_mbutton;

s32 mousetapfade;

void zcore_input_init(void)
{
    u8 i;

    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    for (i=0; i<16; i++) button[i]=0;
    if (SDL_NumJoysticks()>0) {
#ifdef PANDORA
        gamepad=SDL_JoystickOpen(1);
        pissdoranub2=SDL_JoystickOpen(2);
#else
        gamepad=SDL_JoystickOpen(0);
#endif
    }
}

u16 s_button[16];

void zcore_input_frame(void)
{
    u8 i,k;
    for (i=0; i<2; i++) axis[i]=0;

    if (SDL_NumJoysticks()>0)
    {   SDL_JoystickUpdate();
        for (k=0; k<16; k++) if (jkey_map[k]>=0) {
                if (SDL_JoystickGetButton(gamepad,jkey_map[k])>0) s_button[k]++;
                else s_button[k]=0;
            }
        axis[0]=SDL_JoystickGetAxis(gamepad,0);
        axis[1]=SDL_JoystickGetAxis(gamepad,1);

        axis[0]=axis[0]/256;
        axis[1]=axis[1]/256;
#ifdef PANDORA
        axis[2]=SDL_JoystickGetAxis(pissdoranub2,0);
        axis[3]=SDL_JoystickGetAxis(pissdoranub2,1);
        axis[2]=axis[2]/256;
        axis[3]=axis[3]/256;
#endif
#ifdef GP2XWIZ
        if (SDL_JoystickGetButton(gamepad,0)>0) axis[1]=-128;
        else if (SDL_JoystickGetButton(gamepad,1)>0) {
            axis[1]=-128;
            axis[0]=-128;
        }
        else if (SDL_JoystickGetButton(gamepad,2)>0) {
            axis[1]=0;
            axis[0]=-128;
        }
        else if (SDL_JoystickGetButton(gamepad,3)>0) {
            axis[1]=128;
            axis[0]=-128;
        }
        else if (SDL_JoystickGetButton(gamepad,4)>0) {
            axis[1]=128;
            axis[0]=0;
        }
        else if (SDL_JoystickGetButton(gamepad,5)>0) {
            axis[1]=128;
            axis[0]=128;
        }
        else if (SDL_JoystickGetButton(gamepad,6)>0) {
            axis[1]=0;
            axis[0]=128;
        }
        else if (SDL_JoystickGetButton(gamepad,7)>0) {
            axis[1]=-128;
            axis[0]=128;
        }
#endif
    }
    else for (i=0; i<16; i++) s_button[i]=0;

    SDL_Event event;
    while (SDL_PollEvent (&event))
        switch (event.type)
        {
#if defined(PC32) || defined(PANDORA) || defined(GCW)
        case SDL_KEYDOWN:
            for (i=0; i<20; i++) if (event.key.keysym.sym==code_keyb[i]) i_keyb[i]=1;
            break;
        case SDL_KEYUP:
            for (i=0; i<20; i++) if (event.key.keysym.sym==code_keyb[i]) i_keyb[i]=0;
            break;
#endif
        case SDL_QUIT:
            zcoreenabled=0;
            break;
        }

#if defined(PC32) || defined(PANDORA) || defined(GCW)
    for (k=0; k<16; k++) {
        if (i_keyb[k]>0) s_button[k]++;
    }
    if (i_keyb[16]>0) axis[1]=-128;
    if (i_keyb[17]>0) axis[0]=128;
    if (i_keyb[18]>0) axis[1]=128;
    if (i_keyb[19]>0) axis[0]=-128;
#endif

    for (i=0; i<16; i++)
        if (s_button[i]) button[i]++;
        else button[i]=0;

//TouchMouse
    if (SDL_GetMouseState(&mouseaxis[0],&mouseaxis[1])) s_mbutton++;
    else s_mbutton=0;

//TODO scale correctly the mouseaxis values
mouseaxis[0]=(mouseaxis[0]*320)/screenwidth;
mouseaxis[1]=(mouseaxis[1]*240)/screenheight;

    if (s_mbutton==1) {
        holdmouseaxis[0]=mouseaxis[0];
        holdmouseaxis[1]=mouseaxis[1];
    }

    mousetap=0;
    if (s_mbutton>0 & mousebutton[0]==0 & mousetapfade>0) mousetap=1;

    mousebutton[0]=s_mbutton;

    if (mousebutton[0]==1) mousetapfade=32;
    else if (mousetapfade>0) mousetapfade--;

#ifdef GP2XWIZ
    if (button[6]) {
        if (configdata[8]>0) configdata[8]--;
        if (configdata[9]>0) configdata[9]--;
    }
    if (button[7]) {
        if (configdata[8]<128) configdata[8]++;
        if (configdata[9]<128) configdata[9]++;
    }

#endif

}

void zcore_input_down(void)
{
}

// Input SubSystem End
s32 fpsdone,tickcount2=0,tickcount3=0,fstick1,fstick0;
void calcfps(void)
{
    tickcount3=tickcount;
    tickcount=SDL_GetTicks();
    frametime=tickcount-tickcount3;
    if (tickcount-tickcount2>=1000)
    {
        fps=fpsdone;
        fpsdone++;
        tickcount2=tickcount;
        fpsdone=0;
    }
    fpsdone++;
}

void zcoreinit(void)
{
    zcore_sound_init();
    zcore_video_init();
    zcore_input_init();
    zlextinit();
}

void zcorestep(void)
{
    frameskip=configdata[12];
//if (frameskip) thisframenice=((count+1) & 1); else thisframenice=1;
    if (frameskip) thisframenice=((count) % (frameskip+1))==0;
    else thisframenice=1;
    if (gamemode==ZGM_MENU | gamemode==ZGM_CONFIG | gamemode==ZGM_SELECTOR) thisframenice=1;

    zcore_input_frame();
    fstick0=SDL_GetTicks();
    corerenderreset();
    zresmstep();
    zcore_video_frame();
    zcore_sound_frame();
    zlextframe();
    count++;
    fstick1=SDL_GetTicks();
    currenttick=fstick1;
    if (thisframenice) calcfps();
#ifdef PC32
    SDL_Delay(5);
#endif
#ifdef GP2X
    if (gamemode!=ZGM_MENU & gamemode!=ZGM_CONFIG & gamemode!=ZGM_SELECTOR)
        if (frameskip)
            if (thisframenice)
                if (fstick1-fstick0<40) SDL_Delay(1);
#endif


}

void zcoreloop(void)
{
    count=0;
    zcoreenabled=1;
    while (zcoreenabled!=0) {
        zcorestep();   // Embrace thy hopeless destiny loop
    }
}

void zcoredown(void)
{
    zcore_input_down();
    zcore_video_down();
    zcore_sound_down();
    zlextshutdown();
    SDL_Quit();

#ifdef GP2X
    chdir("/usr/gp2x");
    execl("/usr/gp2x/gp2xmenu","/usr/gp2x/gp2xmenu",NULL);
#endif
}

