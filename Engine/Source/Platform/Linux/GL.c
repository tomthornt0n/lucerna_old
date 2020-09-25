/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 25 Sep 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef void         (*glAttachShaderProc) (GLuint, GLuint);
typedef void         (*glBindBufferProc) (GLenum, GLuint);
typedef void         (*glBindTextureProc) (GLenum, GLuint);
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
typedef void         (*glGenTexturesProc) (GLsizei, GLuint *);
typedef void         (*glGenVertexArraysProc) (GLsizei, GLuint *);
typedef void         (*glGetProgramivProc) (GLuint, GLenum, GLint *);
typedef GLint        (*glGetUniformLocationProc) (GLuint, const GLchar *);
typedef void         (*glGetShaderInfologProc) (GLuint, GLsizei, GLsizei *, GLchar *);
typedef void         (*glGetShaderivProc) (GLuint, GLenum, GLint *);
typedef void         (*glLinkProgramProc) (GLuint);
typedef void         (*glShaderSourceProc) (GLuint, GLsizei, const GLchar * const *, const GLint *);
typedef void         (*glTexImage2DProc) (GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *);
typedef void         (*glTexParameteriProc) (GLenum, GLenum, GLint);
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
    glBindTextureProc             BindTexture;
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
    glGenTexturesProc             GenTextures;
    glGenVertexArraysProc         GenVertexArrays;
    glGetProgramivProc            GetProgramiv;
    glGetUniformLocationProc      GetUniformLocation;
    glGetShaderInfologProc        GetShaderInfoLog;
    glGetShaderivProc             GetShaderiv;
    glLinkProgramProc             LinkProgram;
    glShaderSourceProc            ShaderSource;
    glTexImage2DProc              TexImage2D;
    glTexParameteriProc           TexParameteri;
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
    gl.AttachShader            = (glAttachShaderProc           )glXGetProcAddress((const GLubyte *)"glAttachShader");
    gl.BindBuffer              = (glBindBufferProc             )glXGetProcAddress((const GLubyte *)"glBindBuffer");
    gl.BindTexture             = (glBindTextureProc            )glXGetProcAddress((const GLubyte *)"glBindTexture");
    gl.BindVertexArray         = (glBindVertexArrayProc        )glXGetProcAddress((const GLubyte *)"glBindVertexArray");
    gl.BufferData              = (glBufferDataProc             )glXGetProcAddress((const GLubyte *)"glBufferData");
    gl.BufferSubData           = (glBufferSubDataProc          )glXGetProcAddress((const GLubyte *)"glBufferSubData");
    gl.Clear                   = (glClearProc                  )glXGetProcAddress((const GLubyte *)"glClear");
    gl.ClearColor              = (glClearColorProc             )glXGetProcAddress((const GLubyte *)"glClearColor");
    gl.CompileShader           = (glCompileShaderProc          )glXGetProcAddress((const GLubyte *)"glCompileShader");
    gl.CreateProgram           = (glCreateProgramProc          )glXGetProcAddress((const GLubyte *)"glCreateProgram");
    gl.CreateShader            = (glCreateShaderProc           )glXGetProcAddress((const GLubyte *)"glCreateShader");
    gl.DeleteBuffers           = (glDeleteBuffersProc          )glXGetProcAddress((const GLubyte *)"glDeleteBuffers");
    gl.DeleteProgram           = (glDeleteProgramProc          )glXGetProcAddress((const GLubyte *)"glDeleteProgram");
    gl.DeleteShader            = (glDeleteShaderProc           )glXGetProcAddress((const GLubyte *)"glDeleteShader");
    gl.DeleteVertexArrays      = (glDeleteVertexArraysProc     )glXGetProcAddress((const GLubyte *)"glDeleteVertexArrays");
    gl.DetachShader            = (glDetachShaderProc           )glXGetProcAddress((const GLubyte *)"glDetachShader");
    gl.DrawElements            = (glDrawElementsProc           )glXGetProcAddress((const GLubyte *)"glDrawElements");
    gl.EnableVertexAttribArray = (glEnableVertexAttribArrayProc)glXGetProcAddress((const GLubyte *)"glEnableVertexAttribArray");
    gl.GenBuffers              = (glGenBuffersProc             )glXGetProcAddress((const GLubyte *)"glGenBuffers");
    gl.GenTextures             = (glGenTexturesProc            )glXGetProcAddress((const GLubyte *)"glGenTextures");
    gl.GenVertexArrays         = (glGenVertexArraysProc        )glXGetProcAddress((const GLubyte *)"glGenVertexArrays");
    gl.GetProgramiv            = (glGetProgramivProc           )glXGetProcAddress((const GLubyte *)"glGetProgramiv");
    gl.GetUniformLocation      = (glGetUniformLocationProc     )glXGetProcAddress((const GLubyte *)"glGetUniformLocation");
    gl.GetShaderInfoLog        = (glGetShaderInfologProc       )glXGetProcAddress((const GLubyte *)"glGetShaderInfoLog");
    gl.GetShaderiv             = (glGetShaderivProc            )glXGetProcAddress((const GLubyte *)"glGetShaderiv");
    gl.LinkProgram             = (glLinkProgramProc            )glXGetProcAddress((const GLubyte *)"glLinkProgram");
    gl.ShaderSource            = (glShaderSourceProc           )glXGetProcAddress((const GLubyte *)"glShaderSource");
    gl.TexImage2D              = (glTexImage2DProc             )glXGetProcAddress((const GLubyte *)"glTexImage2D");
    gl.TexParameteri           = (glTexParameteriProc          )glXGetProcAddress((const GLubyte *)"glTexParameteri");
    gl.UniformMatrix4fv        = (glUniformMatrix4fvProc       )glXGetProcAddress((const GLubyte *)"glUniformMatrix4fv");
    gl.UseProgram              = (glUseProgramProc             )glXGetProcAddress((const GLubyte *)"glUseProgram");
    gl.VertexAttribPointer     = (glVertexAttribPointerProc    )glXGetProcAddress((const GLubyte *)"glVertexAttribPointer");
    gl.Viewport                = (glViewportProc               )glXGetProcAddress((const GLubyte *)"glViewport");
    gl.XChooseFBConfig         = (glXChooseFBConfigProc        )glXGetProcAddress((const GLubyte *)"glXChooseFBConfig");
    gl.XCreateNewContext       = (glXCreateNewContextProc      )glXGetProcAddress((const GLubyte *)"glXCreateNewContext");
    gl.XCreateWindow           = (glXCreateWindowProc          )glXGetProcAddress((const GLubyte *)"glXCreateWindow");
    gl.XDestroyContext         = (glXDestroyContextProc        )glXGetProcAddress((const GLubyte *)"glXDestroyContext");
    gl.XDestroyWindow          = (glXDestroyWindowProc         )glXGetProcAddress((const GLubyte *)"glXDestroyWindow");
    gl.XGetFBConfigAttrib      = (glXGetFBConfigAttribProc     )glXGetProcAddress((const GLubyte *)"glXGetFBConfigAttrib");
    gl.XMakeContextCurrent     = (glXMakeContextCurrentProc    )glXGetProcAddress((const GLubyte *)"glXMakeContextCurrent");
    gl.XQueryExtensionsString  = (glXQueryExtensionsStringProc )glXGetProcAddress((const GLubyte *)"glXQueryExtensionsString");
    gl.XSwapBuffers            = (glXSwapBuffersProc           )glXGetProcAddress((const GLubyte *)"glXSwapBuffers");
    
    gl.XSwapIntervalEXT        = (glXSwapIntervalEXTProc       )glXGetProcAddress((const GLubyte *)"glXSwapIntervalEXT");
    gl.XSwapIntervalMESA       = (glXSwapIntervalMESAProc      )glXGetProcAddress((const GLubyte *)"glXSwapIntervalMESA");
    gl.XSwapIntervalSGI        = (glXSwapIntervalSGIProc       )glXGetProcAddress((const GLubyte *)"glXSwapIntervalSGI");
    
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
