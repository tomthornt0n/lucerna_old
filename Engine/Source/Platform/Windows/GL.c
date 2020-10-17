/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 17 Oct 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

struct
{
    PFNGLATTACHSHADERPROC             AttachShader;
    PFNGLBINDBUFFERPROC               BindBuffer;
    PFNGLBINDTEXTUREPROC              BindTexture;
    PFNGLBINDVERTEXARRAYPROC          BindVertexArray;
    PFNGLBUFFERDATAPROC               BufferData;
    PFNGLBUFFERSUBDATAPROC            BufferSubData;
    PFNGLCLEARPROC                    Clear;
    PFNGLCLEARCOLORPROC               ClearColor;
    PFNGLCOMPILESHADERPROC            CompileShader;
    PFNGLCREATEPROGRAMPROC            CreateProgram;
    PFNGLCREATESHADERPROC             CreateShader;
    PFNGLDELETEBUFFERSPROC            DeleteBuffers;
    PFNGLDELETEPROGRAMPROC            DeleteProgram;
    PFNGLDELETESHADERPROC             DeleteShader;
    PFNGLDELETEVERTEXARRAYSPROC       DeleteVertexArrays;
    PFNGLDETACHSHADERPROC             DetachShader;
    PFNGLDRAWELEMENTSPROC             DrawElements;
    PFNGLENABLEVERTEXATTRIBARRAYPROC  EnableVertexAttribArray;
    PFNGLGENBUFFERSPROC               GenBuffers;
    PFNGLGENTEXTURESPROC              GenTextures;
    PFNGLGENVERTEXARRAYSPROC          GenVertexArrays;
    PFNGLGETERRORPROC                 GetError;
    PFNGLGETPROGRAMIVPROC             GetProgramiv;
    PFNGLGETUNIFORMLOCATIONPROC       GetUniformLocation;
    PFNGLGETSHADERINFOLOGPROC         GetShaderInfoLog;
    PFNGLGETSHADERIVPROC              GetShaderiv;
    PFNGLLINKPROGRAMPROC              LinkProgram;
    PFNGLSHADERSOURCEPROC             ShaderSource;
    PFNGLTEXIMAGE2DPROC               TexImage2D;
    PFNGLTEXPARAMETERIPROC            TexParameteri;
    PFNGLUNIFORMMATRIX4FVPROC         UniformMatrix4fv;
    PFNGLUSEPROGRAMPROC               UseProgram;
    PFNGLVERTEXATTRIBPOINTERPROC      VertexAttribPointer;
    PFNGLVIEWPORTPROC                 Viewport;

    PFNWGLSWAPINTERVALEXTPROC         wglSwapIntervalEXT;
    PFNWGLCHOOSEPIXELFORMATARBPROC    wglChoosePixelFormatARB;
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
    PFNWGLGETEXTENSIONSSTRINGARBPROC  wglGetExtensionsStringARB;
} gl;

static bool
lcGLIsExtensionSupported(HDC deviceContext, char *extension)
{
    const char *start = gl.wglGetExtensionsStringARB(deviceContext);
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

static void *
lcGLLoadFunctionWindows(char *name, HMODULE opengl32)
{
    void *p = wglGetProcAddress(name);

    if(p == NULL        ||
      (p == (void*)0x1) ||
      (p == (void*)0x2) ||
      (p == (void*)0x3) ||
      (p == (void*)-1) )
    {
        LC_CORE_LOG_INFO("wglGetProcAddress returned NULL - "
                         "trying to load '%s' from opengl32.dll...",
                         name);

        p = (void *)GetProcAddress(opengl32, name);
        if (p) LC_CORE_LOG_INFO("Success!");
        else LC_CORE_LOG_ERROR("\x1b[31mCould not load OpenGL function: %s", name);
    }

    return p;
}

static void
lcGLLoad(void)
{
    HMODULE opengl32 = LoadLibrary(TEXT("opengl32.dll"));
    gl.AttachShader            = (PFNGLATTACHSHADERPROC           )lcGLLoadFunctionWindows("glAttachShader",            opengl32);
    gl.BindBuffer              = (PFNGLBINDBUFFERPROC             )lcGLLoadFunctionWindows("glBindBuffer",              opengl32);
    gl.BindTexture             = (PFNGLBINDTEXTUREPROC            )lcGLLoadFunctionWindows("glBindTexture",             opengl32);
    gl.BindVertexArray         = (PFNGLBINDVERTEXARRAYPROC        )lcGLLoadFunctionWindows("glBindVertexArray",         opengl32);
    gl.BufferData              = (PFNGLBUFFERDATAPROC             )lcGLLoadFunctionWindows("glBufferData",              opengl32);
    gl.BufferSubData           = (PFNGLBUFFERSUBDATAPROC          )lcGLLoadFunctionWindows("glBufferSubData",           opengl32);
    gl.Clear                   = (PFNGLCLEARPROC                  )lcGLLoadFunctionWindows("glClear",                   opengl32);
    gl.ClearColor              = (PFNGLCLEARCOLORPROC             )lcGLLoadFunctionWindows("glClearColor",              opengl32);
    gl.CompileShader           = (PFNGLCOMPILESHADERPROC          )lcGLLoadFunctionWindows("glCompileShader",           opengl32);
    gl.CreateProgram           = (PFNGLCREATEPROGRAMPROC          )lcGLLoadFunctionWindows("glCreateProgram",           opengl32);
    gl.CreateShader            = (PFNGLCREATESHADERPROC           )lcGLLoadFunctionWindows("glCreateShader",            opengl32);
    gl.DeleteBuffers           = (PFNGLDELETEBUFFERSPROC          )lcGLLoadFunctionWindows("glDeleteBuffers",           opengl32);
    gl.DeleteProgram           = (PFNGLDELETEPROGRAMPROC          )lcGLLoadFunctionWindows("glDeleteProgram",           opengl32);
    gl.DeleteShader            = (PFNGLDELETESHADERPROC           )lcGLLoadFunctionWindows("glDeleteShader",            opengl32);
    gl.DeleteVertexArrays      = (PFNGLDELETEVERTEXARRAYSPROC     )lcGLLoadFunctionWindows("glDeleteVertexArrays",      opengl32);
    gl.DetachShader            = (PFNGLDETACHSHADERPROC           )lcGLLoadFunctionWindows("glDetachShader",            opengl32);
    gl.DrawElements            = (PFNGLDRAWELEMENTSPROC           )lcGLLoadFunctionWindows("glDrawElements",            opengl32);
    gl.EnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)lcGLLoadFunctionWindows("glEnableVertexAttribArray", opengl32);
    gl.GenBuffers              = (PFNGLGENBUFFERSPROC             )lcGLLoadFunctionWindows("glGenBuffers",              opengl32);
    gl.GenTextures             = (PFNGLGENTEXTURESPROC            )lcGLLoadFunctionWindows("glGenTextures",             opengl32);
    gl.GenVertexArrays         = (PFNGLGENVERTEXARRAYSPROC        )lcGLLoadFunctionWindows("glGenVertexArrays",         opengl32);
    gl.GetError                = (PFNGLGETERRORPROC               )lcGLLoadFunctionWindows("glGetError",                opengl32);
    gl.GetProgramiv            = (PFNGLGETPROGRAMIVPROC           )lcGLLoadFunctionWindows("glGetProgramiv",            opengl32);
    gl.GetUniformLocation      = (PFNGLGETUNIFORMLOCATIONPROC     )lcGLLoadFunctionWindows("glGetUniformLocation",      opengl32);
    gl.GetShaderInfoLog        = (PFNGLGETSHADERINFOLOGPROC       )lcGLLoadFunctionWindows("glGetShaderInfoLog",        opengl32);
    gl.GetShaderiv             = (PFNGLGETSHADERIVPROC            )lcGLLoadFunctionWindows("glGetShaderiv",             opengl32);
    gl.LinkProgram             = (PFNGLLINKPROGRAMPROC            )lcGLLoadFunctionWindows("glLinkProgram",             opengl32);
    gl.ShaderSource            = (PFNGLSHADERSOURCEPROC           )lcGLLoadFunctionWindows("glShaderSource",            opengl32);
    gl.TexImage2D              = (PFNGLTEXIMAGE2DPROC             )lcGLLoadFunctionWindows("glTexImage2D",              opengl32);
    gl.TexParameteri           = (PFNGLTEXPARAMETERIPROC          )lcGLLoadFunctionWindows("glTexParameteri",           opengl32);
    gl.UniformMatrix4fv        = (PFNGLUNIFORMMATRIX4FVPROC       )lcGLLoadFunctionWindows("glUniformMatrix4fv",        opengl32);
    gl.UseProgram              = (PFNGLUSEPROGRAMPROC             )lcGLLoadFunctionWindows("glUseProgram",              opengl32);
    gl.VertexAttribPointer     = (PFNGLVERTEXATTRIBPOINTERPROC    )lcGLLoadFunctionWindows("glVertexAttribPointer",     opengl32);
    gl.Viewport                = (PFNGLVIEWPORTPROC               )lcGLLoadFunctionWindows("glViewport",                opengl32);
    FreeModule(opengl32);
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
