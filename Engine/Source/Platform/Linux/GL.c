/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 27 Oct 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef void ( *PFNGLXDESTROYCONTEXTPROC) (Display *dpy, GLXContext ctx);
typedef const char *( *PFNGLXQUERYEXTENSIONSSTRINGPROC) (Display *dpy, int screen);
typedef void ( *PFNGLXSWAPBUFFERSPROC) (Display *dpy, GLXDrawable drawable);

internal struct
{
    PFNGLATTACHSHADERPROC            AttachShader;
    PFNGLBINDBUFFERPROC              BindBuffer;
    PFNGLBINDTEXTUREPROC             BindTexture;
    PFNGLBINDVERTEXARRAYPROC         BindVertexArray;
    PFNGLBUFFERDATAPROC              BufferData;
    PFNGLBUFFERSUBDATAPROC           BufferSubData;
    PFNGLCLEARPROC                   Clear;
    PFNGLCLEARCOLORPROC              ClearColor;
    PFNGLCOMPILESHADERPROC           CompileShader;
    PFNGLCREATEPROGRAMPROC           CreateProgram;
    PFNGLCREATESHADERPROC            CreateShader;
    PFNGLDELETEBUFFERSPROC           DeleteBuffers;
    PFNGLDELETEPROGRAMPROC           DeleteProgram;
    PFNGLDELETESHADERPROC            DeleteShader;
    PFNGLDELETEVERTEXARRAYSPROC      DeleteVertexArrays;
    PFNGLDETACHSHADERPROC            DetachShader;
    PFNGLDRAWELEMENTSPROC            DrawElements;
    PFNGLENABLEVERTEXATTRIBARRAYPROC EnableVertexAttribArray;
    PFNGLGENBUFFERSPROC              GenBuffers;
    PFNGLGENTEXTURESPROC             GenTextures;
    PFNGLGENVERTEXARRAYSPROC         GenVertexArrays;
    PFNGLGETERRORPROC                GetError;
    PFNGLGETPROGRAMIVPROC            GetProgramiv;
    PFNGLGETUNIFORMLOCATIONPROC      GetUniformLocation;
    PFNGLGETSHADERINFOLOGPROC        GetShaderInfoLog;
    PFNGLGETSHADERIVPROC             GetShaderiv;
    PFNGLLINKPROGRAMPROC             LinkProgram;
    PFNGLSHADERSOURCEPROC            ShaderSource;
    PFNGLTEXIMAGE2DPROC              TexImage2D;
    PFNGLTEXPARAMETERIPROC           TexParameteri;
    PFNGLUNIFORMMATRIX4FVPROC        UniformMatrix4fv;
    PFNGLUSEPROGRAMPROC              UseProgram;
    PFNGLVERTEXATTRIBPOINTERPROC     VertexAttribPointer;
    PFNGLVIEWPORTPROC                Viewport;

    PFNGLXCHOOSEFBCONFIGPROC         XChooseFBConfig;
    PFNGLXCREATENEWCONTEXTPROC       XCreateNewContext;
    PFNGLXCREATEWINDOWPROC           XCreateWindow;
    PFNGLXDESTROYCONTEXTPROC         XDestroyContext;
    PFNGLXDESTROYWINDOWPROC          XDestroyWindow;
    PFNGLXGETFBCONFIGATTRIBPROC      XGetFBConfigAttrib;
    PFNGLXMAKECONTEXTCURRENTPROC     XMakeContextCurrent;
    PFNGLXQUERYEXTENSIONSSTRINGPROC  XQueryExtensionsString;
    PFNGLXSWAPBUFFERSPROC            XSwapBuffers;
    PFNGLXSWAPINTERVALEXTPROC        XSwapIntervalEXT;
    PFNGLXSWAPINTERVALMESAPROC       XSwapIntervalMESA;
    PFNGLXSWAPINTERVALSGIPROC        XSwapIntervalSGI;

} gl;

internal b8
_lcGLIsExtensionSupported(Display *display,
                          i32 screen,
                          i8 *extension)
{
    const i8 *start;
    start = gl.XQueryExtensionsString(display, screen);

    LC_ASSERT(start, "Error getting gl extensions string");

    for (;;)
    {
        i8 *at;
        i8 *terminator;

        at = strstr(start, extension);
        if (!at)
            return false;

        terminator = at + strlen(extension);
        if (at == start || *(at - 1) == ' ')
        {
            if (*terminator == ' ' || *terminator == '\0')
                break;
        }

        start = terminator;
    }

    return true;
}

internal void ( *_lcGLLoadFunction(i8 *func))(void)
{
    void (*p)(void);

    p = glXGetProcAddressARB((const GLubyte *)func);

    /* NOTE(tbt): Probably not very useful - glXGetProcAddress never returns a
                  NULL pointer as it may be called without an active gl context
                  , so has no way of knowing which functions are available
    */

    if (!p) LC_CORE_LOG_ERROR("Could not load OpenGL function '%s'.", func);

    return p;
}

internal void
_lcGLLoad(void)
{
    gl.AttachShader            = (PFNGLATTACHSHADERPROC           )_lcGLLoadFunction("glAttachShader");
    gl.BindBuffer              = (PFNGLBINDBUFFERPROC             )_lcGLLoadFunction("glBindBuffer");
    gl.BindTexture             = (PFNGLBINDTEXTUREPROC            )_lcGLLoadFunction("glBindTexture");
    gl.BindVertexArray         = (PFNGLBINDVERTEXARRAYPROC        )_lcGLLoadFunction("glBindVertexArray");
    gl.BufferData              = (PFNGLBUFFERDATAPROC             )_lcGLLoadFunction("glBufferData");
    gl.BufferSubData           = (PFNGLBUFFERSUBDATAPROC          )_lcGLLoadFunction("glBufferSubData");
    gl.Clear                   = (PFNGLCLEARPROC                  )_lcGLLoadFunction("glClear");
    gl.ClearColor              = (PFNGLCLEARCOLORPROC             )_lcGLLoadFunction("glClearColor");
    gl.CompileShader           = (PFNGLCOMPILESHADERPROC          )_lcGLLoadFunction("glCompileShader");
    gl.CreateProgram           = (PFNGLCREATEPROGRAMPROC          )_lcGLLoadFunction("glCreateProgram");
    gl.CreateShader            = (PFNGLCREATESHADERPROC           )_lcGLLoadFunction("glCreateShader");
    gl.DeleteBuffers           = (PFNGLDELETEBUFFERSPROC          )_lcGLLoadFunction("glDeleteBuffers");
    gl.DeleteProgram           = (PFNGLDELETEPROGRAMPROC          )_lcGLLoadFunction("glDeleteProgram");
    gl.DeleteShader            = (PFNGLDELETESHADERPROC           )_lcGLLoadFunction("glDeleteShader");
    gl.DeleteVertexArrays      = (PFNGLDELETEVERTEXARRAYSPROC     )_lcGLLoadFunction("glDeleteVertexArrays");
    gl.DetachShader            = (PFNGLDETACHSHADERPROC           )_lcGLLoadFunction("glDetachShader");
    gl.DrawElements            = (PFNGLDRAWELEMENTSPROC           )_lcGLLoadFunction("glDrawElements");
    gl.EnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)_lcGLLoadFunction("glEnableVertexAttribArray");
    gl.GenBuffers              = (PFNGLGENBUFFERSPROC             )_lcGLLoadFunction("glGenBuffers");
    gl.GenTextures             = (PFNGLGENTEXTURESPROC            )_lcGLLoadFunction("glGenTextures");
    gl.GenVertexArrays         = (PFNGLGENVERTEXARRAYSPROC        )_lcGLLoadFunction("glGenVertexArrays");
    gl.GetError                = (PFNGLGETERRORPROC               )_lcGLLoadFunction("glGetError");
    gl.GetProgramiv            = (PFNGLGETPROGRAMIVPROC           )_lcGLLoadFunction("glGetProgramiv");
    gl.GetUniformLocation      = (PFNGLGETUNIFORMLOCATIONPROC     )_lcGLLoadFunction("glGetUniformLocation");
    gl.GetShaderInfoLog        = (PFNGLGETSHADERINFOLOGPROC       )_lcGLLoadFunction("glGetShaderInfoLog");
    gl.GetShaderiv             = (PFNGLGETSHADERIVPROC            )_lcGLLoadFunction("glGetShaderiv");
    gl.LinkProgram             = (PFNGLLINKPROGRAMPROC            )_lcGLLoadFunction("glLinkProgram");
    gl.ShaderSource            = (PFNGLSHADERSOURCEPROC           )_lcGLLoadFunction("glShaderSource");
    gl.TexImage2D              = (PFNGLTEXIMAGE2DPROC             )_lcGLLoadFunction("glTexImage2D");
    gl.TexParameteri           = (PFNGLTEXPARAMETERIPROC          )_lcGLLoadFunction("glTexParameteri");
    gl.UniformMatrix4fv        = (PFNGLUNIFORMMATRIX4FVPROC       )_lcGLLoadFunction("glUniformMatrix4fv");
    gl.UseProgram              = (PFNGLUSEPROGRAMPROC             )_lcGLLoadFunction("glUseProgram");
    gl.VertexAttribPointer     = (PFNGLVERTEXATTRIBPOINTERPROC    )_lcGLLoadFunction("glVertexAttribPointer");
    gl.Viewport                = (PFNGLVIEWPORTPROC               )_lcGLLoadFunction("glViewport");

    gl.XChooseFBConfig         = (PFNGLXCHOOSEFBCONFIGPROC        )_lcGLLoadFunction("glXChooseFBConfig");
    gl.XCreateNewContext       = (PFNGLXCREATENEWCONTEXTPROC      )_lcGLLoadFunction("glXCreateNewContext");
    gl.XCreateWindow           = (PFNGLXCREATEWINDOWPROC          )_lcGLLoadFunction("glXCreateWindow");
    gl.XDestroyContext         = (PFNGLXDESTROYCONTEXTPROC        )_lcGLLoadFunction("glXDestroyContext");
    gl.XDestroyWindow          = (PFNGLXDESTROYWINDOWPROC         )_lcGLLoadFunction("glXDestroyWindow");
    gl.XGetFBConfigAttrib      = (PFNGLXGETFBCONFIGATTRIBPROC     )_lcGLLoadFunction("glXGetFBConfigAttrib");
    gl.XMakeContextCurrent     = (PFNGLXMAKECONTEXTCURRENTPROC    )_lcGLLoadFunction("glXMakeContextCurrent");
    gl.XQueryExtensionsString  = (PFNGLXQUERYEXTENSIONSSTRINGPROC )_lcGLLoadFunction("glXQueryExtensionsString");
    gl.XSwapBuffers            = (PFNGLXSWAPBUFFERSPROC           )_lcGLLoadFunction("glXSwapBuffers");
    
    gl.XSwapIntervalEXT        = (PFNGLXSWAPINTERVALEXTPROC       )_lcGLLoadFunction("glXSwapIntervalEXT");
    gl.XSwapIntervalMESA       = (PFNGLXSWAPINTERVALMESAPROC      )_lcGLLoadFunction("glXSwapIntervalMESA");
    gl.XSwapIntervalSGI        = (PFNGLXSWAPINTERVALSGIPROC       )_lcGLLoadFunction("glXSwapIntervalSGI");
}

/*
MIT License

Copyright (c) 2020 Tom Thornton

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
