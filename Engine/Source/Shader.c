/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 23 Oct 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static lcShader_t lcRendererBoundShader;

lcShader_t
lcShaderCreate(char *vertexSrc,
               char *fragmentSrc)
{
    lcShader_t program;
    uint32_t vertexID, fragmentID;
    int status;

    program = gl.CreateProgram();
    
    vertexID = gl.CreateShader(GL_VERTEX_SHADER);

    gl.ShaderSource(vertexID, 1, (const char * const *)&vertexSrc, NULL);
    gl.CompileShader(vertexID);

    gl.GetShaderiv(vertexID, GL_COMPILE_STATUS, &status);

    if (status == GL_FALSE)
    {
        int maxLength;
        char *msg;

        gl.GetShaderiv(vertexID, GL_INFO_LOG_LENGTH, &maxLength);
        msg = malloc(maxLength);
        gl.GetShaderInfoLog(vertexID, maxLength, NULL, msg);

        gl.DeleteShader(vertexID);

        LC_ASSERT(0, "Vertex shader compilation failure! '%s'", msg);
    }

    gl.AttachShader(program, vertexID);
    
    fragmentID = gl.CreateShader(GL_FRAGMENT_SHADER);

    gl.ShaderSource(fragmentID, 1, (const char * const *)&fragmentSrc, NULL);

    gl.CompileShader(fragmentID);

    gl.GetShaderiv(fragmentID, GL_COMPILE_STATUS, &status);

    if (status == GL_FALSE)
    {
        int maxLength;
        char *msg;

        gl.GetShaderiv(fragmentID, GL_INFO_LOG_LENGTH, &maxLength);
        msg = malloc(maxLength);
        gl.GetShaderInfoLog(fragmentID, maxLength, NULL, msg);

        gl.DeleteShader(fragmentID);

        LC_ASSERT(0, "Fragment shader compilation failure! %s", msg);
    }

    gl.AttachShader(program, fragmentID);

    gl.LinkProgram(program);

    gl.GetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        int maxLength;
        char *msg;

        gl.GetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
        msg = malloc(maxLength);
        gl.GetShaderInfoLog(program, maxLength, NULL, msg);

        gl.DeleteProgram(program);

        gl.DeleteShader(vertexID);
        gl.DeleteShader(fragmentID);

        LC_ASSERT(0, "Shader link failure! %s", msg);
    }

    gl.DetachShader(program, vertexID);
    gl.DetachShader(program, fragmentID);

    gl.DeleteShader(vertexID);
    gl.DeleteShader(fragmentID);

    free(vertexSrc);
    free(fragmentSrc);

    gl.UseProgram(program);

    return program;
}

void
lcShaderDestroy(lcShader_t shader)
{
    gl.DeleteProgram(shader);
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

