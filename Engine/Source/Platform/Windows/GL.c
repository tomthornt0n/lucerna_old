/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 27 Oct 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

internal struct
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

internal b8
_lcGLIsExtensionSupported(HDC deviceContext, i8 *extension)
{
    const i8 *start;
    start = gl.wglGetExtensionsStringARB(deviceContext);

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

internal void *
_lcGLLoadFunction(i8 *func,
                  HMODULE module)
{
    void *p;
    p = wglGetProcAddress(func);

    if(p == NULL        ||
      (p == (void*)0x1) ||
      (p == (void*)0x2) ||
      (p == (void*)0x3) ||
      (p == (void*)-1) )
    {
        LC_CORE_LOG_INFO("wglGetProcAddress returned NULL - "
                         "trying to load '%s' from opengl32.dll...",
                         func);

        p = (void *)GetProcAddress(module, func);
        if (p) LC_CORE_LOG_INFO("Success!");
        else LC_CORE_LOG_ERROR("\x1b[31mCould not load OpenGL function: %s",
                               func);
    }

    return p;
}

internal void
_lcGLLoad(void)
{
    HMODULE opengl32;

    opengl32 = LoadLibrary(TEXT("opengl32.dll"));
    gl.AttachShader            = (PFNGLATTACHSHADERPROC           )_lcGLLoadFunction("glAttachShader",            opengl32);
    gl.BindBuffer              = (PFNGLBINDBUFFERPROC             )_lcGLLoadFunction("glBindBuffer",              opengl32);
    gl.BindTexture             = (PFNGLBINDTEXTUREPROC            )_lcGLLoadFunction("glBindTexture",             opengl32);
    gl.BindVertexArray         = (PFNGLBINDVERTEXARRAYPROC        )_lcGLLoadFunction("glBindVertexArray",         opengl32);
    gl.BufferData              = (PFNGLBUFFERDATAPROC             )_lcGLLoadFunction("glBufferData",              opengl32);
    gl.BufferSubData           = (PFNGLBUFFERSUBDATAPROC          )_lcGLLoadFunction("glBufferSubData",           opengl32);
    gl.Clear                   = (PFNGLCLEARPROC                  )_lcGLLoadFunction("glClear",                   opengl32);
    gl.ClearColor              = (PFNGLCLEARCOLORPROC             )_lcGLLoadFunction("glClearColor",              opengl32);
    gl.CompileShader           = (PFNGLCOMPILESHADERPROC          )_lcGLLoadFunction("glCompileShader",           opengl32);
    gl.CreateProgram           = (PFNGLCREATEPROGRAMPROC          )_lcGLLoadFunction("glCreateProgram",           opengl32);
    gl.CreateShader            = (PFNGLCREATESHADERPROC           )_lcGLLoadFunction("glCreateShader",            opengl32);
    gl.DeleteBuffers           = (PFNGLDELETEBUFFERSPROC          )_lcGLLoadFunction("glDeleteBuffers",           opengl32);
    gl.DeleteProgram           = (PFNGLDELETEPROGRAMPROC          )_lcGLLoadFunction("glDeleteProgram",           opengl32);
    gl.DeleteShader            = (PFNGLDELETESHADERPROC           )_lcGLLoadFunction("glDeleteShader",            opengl32);
    gl.DeleteVertexArrays      = (PFNGLDELETEVERTEXARRAYSPROC     )_lcGLLoadFunction("glDeleteVertexArrays",      opengl32);
    gl.DetachShader            = (PFNGLDETACHSHADERPROC           )_lcGLLoadFunction("glDetachShader",            opengl32);
    gl.DrawElements            = (PFNGLDRAWELEMENTSPROC           )_lcGLLoadFunction("glDrawElements",            opengl32);
    gl.EnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)_lcGLLoadFunction("glEnableVertexAttribArray", opengl32);
    gl.GenBuffers              = (PFNGLGENBUFFERSPROC             )_lcGLLoadFunction("glGenBuffers",              opengl32);
    gl.GenTextures             = (PFNGLGENTEXTURESPROC            )_lcGLLoadFunction("glGenTextures",             opengl32);
    gl.GenVertexArrays         = (PFNGLGENVERTEXARRAYSPROC        )_lcGLLoadFunction("glGenVertexArrays",         opengl32);
    gl.GetError                = (PFNGLGETERRORPROC               )_lcGLLoadFunction("glGetError",                opengl32);
    gl.GetProgramiv            = (PFNGLGETPROGRAMIVPROC           )_lcGLLoadFunction("glGetProgramiv",            opengl32);
    gl.GetUniformLocation      = (PFNGLGETUNIFORMLOCATIONPROC     )_lcGLLoadFunction("glGetUniformLocation",      opengl32);
    gl.GetShaderInfoLog        = (PFNGLGETSHADERINFOLOGPROC       )_lcGLLoadFunction("glGetShaderInfoLog",        opengl32);
    gl.GetShaderiv             = (PFNGLGETSHADERIVPROC            )_lcGLLoadFunction("glGetShaderiv",             opengl32);
    gl.LinkProgram             = (PFNGLLINKPROGRAMPROC            )_lcGLLoadFunction("glLinkProgram",             opengl32);
    gl.ShaderSource            = (PFNGLSHADERSOURCEPROC           )_lcGLLoadFunction("glShaderSource",            opengl32);
    gl.TexImage2D              = (PFNGLTEXIMAGE2DPROC             )_lcGLLoadFunction("glTexImage2D",              opengl32);
    gl.TexParameteri           = (PFNGLTEXPARAMETERIPROC          )_lcGLLoadFunction("glTexParameteri",           opengl32);
    gl.UniformMatrix4fv        = (PFNGLUNIFORMMATRIX4FVPROC       )_lcGLLoadFunction("glUniformMatrix4fv",        opengl32);
    gl.UseProgram              = (PFNGLUSEPROGRAMPROC             )_lcGLLoadFunction("glUseProgram",              opengl32);
    gl.VertexAttribPointer     = (PFNGLVERTEXATTRIBPOINTERPROC    )_lcGLLoadFunction("glVertexAttribPointer",     opengl32);
    gl.Viewport                = (PFNGLVIEWPORTPROC               )_lcGLLoadFunction("glViewport",                opengl32);

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
