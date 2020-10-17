/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 17 Oct 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static lcShader_t lcRendererBoundShader;

static bool
lcShaderLoadFile(char *path,
                 char **result)
{
    FILE *f = fopen(path, "rb");
    if (!f) return false;

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    rewind(f);
    
    *result = malloc(fsize + 1);
    if (*result == NULL) return false;
    int status = fread(*result, 1, fsize, f);
    if (status != fsize) return false;
    fclose(f);
    
    (*result)[fsize] = 0;
    return true;
}

lcShader_t
lcShaderCreate(char *vertexPath,
               char *fragmentPath)
{
    lcShader_t program;

    char *vertexSrc;
    LC_ASSERT(lcShaderLoadFile(vertexPath, &vertexSrc) != false, "Could not load vertex src!");
    char *fragmentSrc;
    LC_ASSERT(lcShaderLoadFile(fragmentPath, &fragmentSrc) != false, "Could not load fragment src!");

    program = gl.CreateProgram();
    
    uint32_t vertexID, fragmentID;

    vertexID = gl.CreateShader(GL_VERTEX_SHADER);

    gl.ShaderSource(vertexID, 1, (const char * const *)&vertexSrc, NULL);
    gl.CompileShader(vertexID);

    int isCompiled;
    gl.GetShaderiv(vertexID, GL_COMPILE_STATUS, &isCompiled);

    if (isCompiled == GL_FALSE)
    {
        int maxLength;
        gl.GetShaderiv(vertexID, GL_INFO_LOG_LENGTH, &maxLength);
        char *msg = malloc(maxLength);
        gl.GetShaderInfoLog(vertexID, maxLength, NULL, msg);

        gl.DeleteShader(vertexID);

        LC_ASSERT(0, "Vertex shader compilation failure! '%s'", msg);
    }

    gl.AttachShader(program, vertexID);
    
    fragmentID = gl.CreateShader(GL_FRAGMENT_SHADER);

    gl.ShaderSource(fragmentID, 1, (const char * const *)&fragmentSrc, NULL);

    gl.CompileShader(fragmentID);

    gl.GetShaderiv(fragmentID, GL_COMPILE_STATUS, &isCompiled);

    if (isCompiled == GL_FALSE)
    {
        int maxLength;
        gl.GetShaderiv(fragmentID, GL_INFO_LOG_LENGTH, &maxLength);

        char *msg = malloc(maxLength);
        gl.GetShaderInfoLog(fragmentID, maxLength, NULL, msg);

        gl.DeleteShader(fragmentID);

        LC_ASSERT(0, "Fragment shader compilation failure! %s", msg);
    }

    gl.AttachShader(program, fragmentID);

    gl.LinkProgram(program);

    GLint isLinked;
    gl.GetProgramiv(program, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE)
    {
        int maxLength;
        gl.GetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        char *msg = malloc(maxLength);
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

