/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 19 August 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef void         (*glAttachShaderProc) (GLuint, GLuint);
typedef void         (*glBindBufferProc) (GLenum, GLuint);
typedef void         (*glBindVertexArrayProc) (GLuint);
typedef void         (*glBufferDataProc) (GLenum, GLsizeiptr, const void *, GLenum);
typedef void         (*glBufferSubDataProc) (GLenum, GLintptr, GLsizeiptr, const void *);
typedef void         (*glClearProc) (GLbitfield);
typedef void         (*glClearColorProc) (GLfloat, GLfloat, GLfloat, GLfloat);
typedef void         (*glCompileShaderProc) (GLuint);
typedef GLuint       (*glCreateProgramProc) (void);
typedef GLuint       (*glCreateShaderProc) (GLenum);
typedef void         (*glDeleteBuffersProc) (GLsizei,  const GLuint *);
typedef void         (*glDeleteProgramProc) (GLuint);
typedef void         (*glDeleteShaderProc) (GLuint);
typedef void         (*glDeleteVertexArraysProc) (GLsizei, const GLuint *);
typedef void         (*glDetachShaderProc) (GLuint, GLuint);
typedef void         (*glDrawElementsProc) (GLenum, GLsizei, GLenum, const void *);
typedef void         (*glEnableVertexAttribArrayProc) (GLuint);
typedef void         (*glGenBuffersProc) (GLsizei, GLuint *);
typedef void         (*glGenVertexArraysProc) (GLsizei, GLuint *);
typedef void         (*glGetProgramivProc) (GLuint, GLenum, GLint *);
typedef GLint        (*glGetUniformLocationProc) (GLuint, const GLchar *);
typedef void         (*glGetShaderInfologProc) (GLuint, GLsizei, GLsizei *, GLchar *);
typedef void         (*glGetShaderivProc) (GLuint, GLenum, GLint *);
typedef void         (*glLinkProgramProc) (GLuint);
typedef void         (*glShaderSourceProc) (GLuint, GLsizei, const GLchar * const *, const GLint *);
typedef void         (*glUniformMatrix4fvProc) (GLint, GLsizei, GLboolean, const GLfloat *);
typedef void         (*glUseProgramProc) (GLuint);
typedef void         (*glVertexAttribPointerProc) (GLuint, GLint, GLenum, GLboolean, GLsizei, const void *);
typedef void         (*glViewportProc) (GLint, GLint, GLsizei, GLsizei);

typedef GLXFBConfig *(*glXChooseFBConfigProc) (Display *, int, const int *, int *);
typedef GLXContext   (*glXCreateNewContextProc) (Display *,GLXFBConfig, int, GLXContext, bool);
typedef GLXWindow    (*glXCreateWindowProc) (Display *, GLXFBConfig, Window, const int *);
typedef void         (*glXDestroyContextProc) (Display *, GLXContext);
typedef void         (*glXDestroyWindowProc) (Display *, GLXWindow);
typedef int          (*glXGetFBConfigAttribProc) (Display *, GLXFBConfig, int, int *);
typedef bool         (*glXMakeContextCurrentProc) (Display *, GLXDrawable, GLXDrawable, GLXContext);
typedef const char  *(*glXQueryExtensionsStringProc) (Display *, int);
typedef void         (*glXSwapBuffersProc) (Display *, GLXDrawable);
typedef void         (*glXSwapIntervalEXTProc) (Display *, GLXDrawable, int);
typedef void         (*glXSwapIntervalMESAProc) (int);
typedef void         (*glXSwapIntervalSGIProc) (int);

struct
{
    glAttachShaderProc            AttachShader;
    glBindBufferProc              BindBuffer;
    glBindVertexArrayProc         BindVertexArray;
    glBufferDataProc              BufferData;
    glBufferSubDataProc           BufferSubData;
    glClearProc                   Clear;
    glClearColorProc              ClearColor;
    glCompileShaderProc           CompileShader;
    glCreateProgramProc           CreateProgram;
    glCreateShaderProc            CreateShader;
    glDeleteBuffersProc           DeleteBuffers;
    glDeleteProgramProc           DeleteProgram;
    glDeleteShaderProc            DeleteShader;
    glDeleteVertexArraysProc      DeleteVertexArrays;
    glDetachShaderProc            DetachShader;
    glDrawElementsProc            DrawElements;
    glEnableVertexAttribArrayProc EnableVertexAttribArray;
    glGenBuffersProc              GenBuffers;
    glGenVertexArraysProc         GenVertexArrays;
    glGetProgramivProc            GetProgramiv;
    glGetUniformLocationProc      GetUniformLocation;
    glGetShaderInfologProc        GetShaderInfoLog;
    glGetShaderivProc             GetShaderiv;
    glLinkProgramProc             LinkProgram;
    glShaderSourceProc            ShaderSource;
    glUniformMatrix4fvProc        UniformMatrix4fv;
    glUseProgramProc              UseProgram;
    glVertexAttribPointerProc     VertexAttribPointer;
    glViewportProc                Viewport;

    glXChooseFBConfigProc         XChooseFBConfig;
    glXCreateNewContextProc       XCreateNewContext;
    glXCreateWindowProc           XCreateWindow;
    glXDestroyContextProc         XDestroyContext;
    glXDestroyWindowProc          XDestroyWindow;
    glXGetFBConfigAttribProc      XGetFBConfigAttrib;
    glXMakeContextCurrentProc     XMakeContextCurrent;
    glXQueryExtensionsStringProc  XQueryExtensionsString;
    glXSwapBuffersProc            XSwapBuffers;
    glXSwapIntervalEXTProc        XSwapIntervalEXT;
    glXSwapIntervalMESAProc       XSwapIntervalMESA;
    glXSwapIntervalSGIProc        XSwapIntervalSGI;

} gl;

static bool
lcGLIsExtensionSupported(Display *display,
                         int screen,
                         char *extension)
{
    const char *start = gl.XQueryExtensionsString(display, screen);

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
    gl.AttachShader            = (glAttachShaderProc           )glXGetProcAddress("glAttachShader");
    gl.BindBuffer              = (glBindBufferProc             )glXGetProcAddress("glBindBuffer");
    gl.BindVertexArray         = (glBindVertexArrayProc        )glXGetProcAddress("glBindVertexArray");
    gl.BufferData              = (glBufferDataProc             )glXGetProcAddress("glBufferData");
    gl.BufferSubData           = (glBufferSubDataProc          )glXGetProcAddress("glBufferSubData");
    gl.Clear                   = (glClearProc                  )glXGetProcAddress("glClear");
    gl.ClearColor              = (glClearColorProc             )glXGetProcAddress("glClearColor");
    gl.CompileShader           = (glCompileShaderProc          )glXGetProcAddress("glCompileShader");
    gl.CreateProgram           = (glCreateProgramProc          )glXGetProcAddress("glCreateProgram");
    gl.CreateShader            = (glCreateShaderProc           )glXGetProcAddress("glCreateShader");
    gl.DeleteBuffers           = (glDeleteBuffersProc          )glXGetProcAddress("glDeleteBuffers");
    gl.DeleteProgram           = (glDeleteProgramProc          )glXGetProcAddress("glDeleteProgram");
    gl.DeleteShader            = (glDeleteShaderProc           )glXGetProcAddress("glDeleteShader");
    gl.DeleteVertexArrays      = (glDeleteVertexArraysProc     )glXGetProcAddress("glDeleteVertexArrays");
    gl.DetachShader            = (glDetachShaderProc           )glXGetProcAddress("glDetachShader");
    gl.DrawElements            = (glDrawElementsProc           )glXGetProcAddress("glDrawElements");
    gl.EnableVertexAttribArray = (glEnableVertexAttribArrayProc)glXGetProcAddress("glEnableVertexAttribArray");
    gl.GenBuffers              = (glGenBuffersProc             )glXGetProcAddress("glGenBuffers");
    gl.GenVertexArrays         = (glGenVertexArraysProc        )glXGetProcAddress("glGenVertexArrays");
    gl.GetProgramiv            = (glGetProgramivProc           )glXGetProcAddress("glGetProgramiv");
    gl.GetUniformLocation      = (glGetUniformLocationProc     )glXGetProcAddress("glGetUniformLocation");
    gl.GetShaderInfoLog        = (glGetShaderInfologProc       )glXGetProcAddress("glGetShaderInfoLog");
    gl.GetShaderiv             = (glGetShaderivProc            )glXGetProcAddress("glGetShaderiv");
    gl.LinkProgram             = (glLinkProgramProc            )glXGetProcAddress("glLinkProgram");
    gl.ShaderSource            = (glShaderSourceProc           )glXGetProcAddress("glShaderSource");
    gl.UniformMatrix4fv        = (glUniformMatrix4fvProc       )glXGetProcAddress("glUniformMatrix4fv");
    gl.UseProgram              = (glUseProgramProc             )glXGetProcAddress("glUseProgram");
    gl.VertexAttribPointer     = (glVertexAttribPointerProc    )glXGetProcAddress("glVertexAttribPointer");
    gl.Viewport                = (glViewportProc               )glXGetProcAddress("glViewport");
    gl.XChooseFBConfig         = (glXChooseFBConfigProc        )glXGetProcAddress("glXChooseFBConfig");
    gl.XCreateNewContext       = (glXCreateNewContextProc      )glXGetProcAddress("glXCreateNewContext");
    gl.XCreateWindow           = (glXCreateWindowProc          )glXGetProcAddress("glXCreateWindow");
    gl.XDestroyContext         = (glXDestroyContextProc        )glXGetProcAddress("glXDestroyContext");
    gl.XDestroyWindow          = (glXDestroyWindowProc         )glXGetProcAddress("glXDestroyWindow");
    gl.XGetFBConfigAttrib      = (glXGetFBConfigAttribProc     )glXGetProcAddress("glXGetFBConfigAttrib");
    gl.XMakeContextCurrent     = (glXMakeContextCurrentProc    )glXGetProcAddress("glXMakeContextCurrent");
    gl.XQueryExtensionsString  = (glXQueryExtensionsStringProc )glXGetProcAddress("glXQueryExtensionsString");
    gl.XSwapBuffers            = (glXSwapBuffersProc           )glXGetProcAddress("glXSwapBuffers");
    
    gl.XSwapIntervalEXT        = (glXSwapIntervalEXTProc       )glXGetProcAddress("glXSwapIntervalEXT");
    gl.XSwapIntervalMESA       = (glXSwapIntervalMESAProc      )glXGetProcAddress("glXSwapIntervalMESA");
    gl.XSwapIntervalSGI        = (glXSwapIntervalSGIProc       )glXGetProcAddress("glXSwapIntervalSGI");
    
    if (!gl.AttachShader            ||
        !gl.BindBuffer              ||
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
        !gl.GenVertexArrays         ||
        !gl.GetProgramiv            ||
        !gl.GetUniformLocation      ||
        !gl.GetShaderInfoLog        ||
        !gl.GetShaderiv             ||
        !gl.LinkProgram             ||
        !gl.ShaderSource            ||
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
