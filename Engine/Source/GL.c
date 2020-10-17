/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 17 Oct 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if defined(LC_PLATFORM_LINUX)
#include "Platform/Linux/GL.c"
#elif defined(LC_PLATFORM_WINDOWS)
#include "Platform/Windows/GL.c"
#else
#error "Platform macro not defined"
#endif

static void
lcGLGetErrors(void)
{
#ifdef LC_DEBUG
    GLenum error;
    while ((error = gl.GetError()) != GL_NO_ERROR)
    {
        LC_CORE_LOG_ERROR("OpenGL error: %s", error == GL_INVALID_ENUM ? "GL_INVALID_ENUM" :
                                              error == GL_INVALID_VALUE ? "GL_INVALID_VALUE":
                                              error == GL_INVALID_OPERATION ? "GL_INVALID_OPERATION":
                                              error == GL_INVALID_FRAMEBUFFER_OPERATION ? "GL_INVALID_FRAMEBUFFER_OPERATION":
                                              error == GL_OUT_OF_MEMORY ? "GL_OUT_OF_MEMORY":
                                              error == GL_STACK_UNDERFLOW ? "GL_STACK_UNDERFLOW":
                                              error == GL_STACK_OVERFLOW ? "GL_STACK_OVERFLOW":
                                              NULL);
    }
#endif
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

