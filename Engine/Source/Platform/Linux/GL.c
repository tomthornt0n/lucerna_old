/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 22 Oct 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef void ( *PFNGLXDESTROYCONTEXTPROC) (Display *dpy, GLXContext ctx);
typedef const char *( *PFNGLXQUERYEXTENSIONSSTRINGPROC) (Display *dpy, int screen);
typedef void ( *PFNGLXSWAPBUFFERSPROC) (Display *dpy, GLXDrawable drawable);

struct
{
    PFNGLATTACHSHADERPROC            AttachShader;
    PFNGLBINDBUFFERPROC              BindBuffer;
    PFNGLBINDTEXTUREPROC          BindTexture;
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

static bool
lcGLIsExtensionSupported(Display *display,
                         int screen,
                         char *extension)
{
    const char *start;
    start = gl.XQueryExtensionsString(display, screen);

    LC_ASSERT(start, "Error getting gl extensions string");

    for (;;)
    {
        const char *at;
        const char *terminator;

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

void
lcGLLoad(void)
{
    gl.AttachShader            = (PFNGLATTACHSHADERPROC           )glXGetProcAddress((const GLubyte *)"glAttachShader");
    gl.BindBuffer              = (PFNGLBINDBUFFERPROC             )glXGetProcAddress((const GLubyte *)"glBindBuffer");
    gl.BindTexture             = (PFNGLBINDTEXTUREPROC            )glXGetProcAddress((const GLubyte *)"glBindTexture");
    gl.BindVertexArray         = (PFNGLBINDVERTEXARRAYPROC        )glXGetProcAddress((const GLubyte *)"glBindVertexArray");
    gl.BufferData              = (PFNGLBUFFERDATAPROC             )glXGetProcAddress((const GLubyte *)"glBufferData");
    gl.BufferSubData           = (PFNGLBUFFERSUBDATAPROC          )glXGetProcAddress((const GLubyte *)"glBufferSubData");
    gl.Clear                   = (PFNGLCLEARPROC                  )glXGetProcAddress((const GLubyte *)"glClear");
    gl.ClearColor              = (PFNGLCLEARCOLORPROC             )glXGetProcAddress((const GLubyte *)"glClearColor");
    gl.CompileShader           = (PFNGLCOMPILESHADERPROC          )glXGetProcAddress((const GLubyte *)"glCompileShader");
    gl.CreateProgram           = (PFNGLCREATEPROGRAMPROC          )glXGetProcAddress((const GLubyte *)"glCreateProgram");
    gl.CreateShader            = (PFNGLCREATESHADERPROC           )glXGetProcAddress((const GLubyte *)"glCreateShader");
    gl.DeleteBuffers           = (PFNGLDELETEBUFFERSPROC          )glXGetProcAddress((const GLubyte *)"glDeleteBuffers");
    gl.DeleteProgram           = (PFNGLDELETEPROGRAMPROC          )glXGetProcAddress((const GLubyte *)"glDeleteProgram");
    gl.DeleteShader            = (PFNGLDELETESHADERPROC           )glXGetProcAddress((const GLubyte *)"glDeleteShader");
    gl.DeleteVertexArrays      = (PFNGLDELETEVERTEXARRAYSPROC     )glXGetProcAddress((const GLubyte *)"glDeleteVertexArrays");
    gl.DetachShader            = (PFNGLDETACHSHADERPROC           )glXGetProcAddress((const GLubyte *)"glDetachShader");
    gl.DrawElements            = (PFNGLDRAWELEMENTSPROC           )glXGetProcAddress((const GLubyte *)"glDrawElements");
    gl.EnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glXGetProcAddress((const GLubyte *)"glEnableVertexAttribArray");
    gl.GenBuffers              = (PFNGLGENBUFFERSPROC             )glXGetProcAddress((const GLubyte *)"glGenBuffers");
    gl.GenTextures             = (PFNGLGENTEXTURESPROC            )glXGetProcAddress((const GLubyte *)"glGenTextures");
    gl.GenVertexArrays         = (PFNGLGENVERTEXARRAYSPROC        )glXGetProcAddress((const GLubyte *)"glGenVertexArrays");
    gl.GetError                = (PFNGLGETERRORPROC               )glXGetProcAddress((const GLubyte *)"glGetError");
    gl.GetProgramiv            = (PFNGLGETPROGRAMIVPROC           )glXGetProcAddress((const GLubyte *)"glGetProgramiv");
    gl.GetUniformLocation      = (PFNGLGETUNIFORMLOCATIONPROC     )glXGetProcAddress((const GLubyte *)"glGetUniformLocation");
    gl.GetShaderInfoLog        = (PFNGLGETSHADERINFOLOGPROC       )glXGetProcAddress((const GLubyte *)"glGetShaderInfoLog");
    gl.GetShaderiv             = (PFNGLGETSHADERIVPROC            )glXGetProcAddress((const GLubyte *)"glGetShaderiv");
    gl.LinkProgram             = (PFNGLLINKPROGRAMPROC            )glXGetProcAddress((const GLubyte *)"glLinkProgram");
    gl.ShaderSource            = (PFNGLSHADERSOURCEPROC           )glXGetProcAddress((const GLubyte *)"glShaderSource");
    gl.TexImage2D              = (PFNGLTEXIMAGE2DPROC             )glXGetProcAddress((const GLubyte *)"glTexImage2D");
    gl.TexParameteri           = (PFNGLTEXPARAMETERIPROC          )glXGetProcAddress((const GLubyte *)"glTexParameteri");
    gl.UniformMatrix4fv        = (PFNGLUNIFORMMATRIX4FVPROC       )glXGetProcAddress((const GLubyte *)"glUniformMatrix4fv");
    gl.UseProgram              = (PFNGLUSEPROGRAMPROC             )glXGetProcAddress((const GLubyte *)"glUseProgram");
    gl.VertexAttribPointer     = (PFNGLVERTEXATTRIBPOINTERPROC    )glXGetProcAddress((const GLubyte *)"glVertexAttribPointer");
    gl.Viewport                = (PFNGLVIEWPORTPROC               )glXGetProcAddress((const GLubyte *)"glViewport");

    gl.XChooseFBConfig         = (PFNGLXCHOOSEFBCONFIGPROC        )glXGetProcAddress((const GLubyte *)"glXChooseFBConfig");
    gl.XCreateNewContext       = (PFNGLXCREATENEWCONTEXTPROC      )glXGetProcAddress((const GLubyte *)"glXCreateNewContext");
    gl.XCreateWindow           = (PFNGLXCREATEWINDOWPROC          )glXGetProcAddress((const GLubyte *)"glXCreateWindow");
    gl.XDestroyContext         = (PFNGLXDESTROYCONTEXTPROC        )glXGetProcAddress((const GLubyte *)"glXDestroyContext");
    gl.XDestroyWindow          = (PFNGLXDESTROYWINDOWPROC         )glXGetProcAddress((const GLubyte *)"glXDestroyWindow");
    gl.XGetFBConfigAttrib      = (PFNGLXGETFBCONFIGATTRIBPROC     )glXGetProcAddress((const GLubyte *)"glXGetFBConfigAttrib");
    gl.XMakeContextCurrent     = (PFNGLXMAKECONTEXTCURRENTPROC    )glXGetProcAddress((const GLubyte *)"glXMakeContextCurrent");
    gl.XQueryExtensionsString  = (PFNGLXQUERYEXTENSIONSSTRINGPROC )glXGetProcAddress((const GLubyte *)"glXQueryExtensionsString");
    gl.XSwapBuffers            = (PFNGLXSWAPBUFFERSPROC           )glXGetProcAddress((const GLubyte *)"glXSwapBuffers");
    
    gl.XSwapIntervalEXT        = (PFNGLXSWAPINTERVALEXTPROC       )glXGetProcAddress((const GLubyte *)"glXSwapIntervalEXT");
    gl.XSwapIntervalMESA       = (PFNGLXSWAPINTERVALMESAPROC      )glXGetProcAddress((const GLubyte *)"glXSwapIntervalMESA");
    gl.XSwapIntervalSGI        = (PFNGLXSWAPINTERVALSGIPROC       )glXGetProcAddress((const GLubyte *)"glXSwapIntervalSGI");
    
    if (!gl.AttachShader            ||
        !gl.BindBuffer              ||
        !gl.BindBuffer              ||
        !gl.BindTexture             ||
        !gl.BindVertexArray         ||
        !gl.BufferData              ||
        !gl.BufferSubData           ||
        !gl.Clear                   ||
        !gl.ClearColor              ||
        !gl.CompileShader           ||
        !gl.CreateProgram           ||
        !gl.CreateShader            ||
        !gl.DeleteBuffers           ||
        !gl.DeleteProgram           ||
        !gl.DeleteShader            ||
        !gl.DeleteVertexArrays      ||
        !gl.DetachShader            ||
        !gl.DrawElements            ||
        !gl.EnableVertexAttribArray ||
        !gl.GenBuffers              ||
        !gl.GenTextures             ||
        !gl.GenVertexArrays         ||
        !gl.GetProgramiv            ||
        !gl.GetUniformLocation      ||
        !gl.GetShaderInfoLog        ||
        !gl.GetShaderiv             ||
        !gl.LinkProgram             ||
        !gl.ShaderSource            ||
        !gl.TexImage2D              ||
        !gl.TexParameteri           ||
        !gl.UniformMatrix4fv        ||
        !gl.UseProgram              ||
        !gl.VertexAttribPointer     ||
        !gl.Viewport                ||
        !gl.XChooseFBConfig         ||
        !gl.XCreateNewContext       ||
        !gl.XCreateWindow           ||
        !gl.XDestroyContext         ||
        !gl.XDestroyWindow          ||
        !gl.XGetFBConfigAttrib      ||
        !gl.XMakeContextCurrent     ||
        !gl.XQueryExtensionsString  ||
        !gl.XSwapBuffers            ||
        (!gl.XSwapIntervalEXT       &&
         !gl.XSwapIntervalMESA      &&
         !gl.XSwapIntervalSGI))
    {
        LC_CORE_LOG_WARN("Failed to load [some] OpenGL function[s].");
    }
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
