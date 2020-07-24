typedef uint32_t lcShader_t;

lcShader_t _lc_RendererBoundShader;

lcShader_t
lc_ShaderCreate(const char *vertexPath,
                const char *fragmentPath)
{
	lcShader_t program;

	uint8_t *vertexSrc;
	LC_ASSERT(lc_LoaderReadFile(vertexPath, &vertexSrc) != -1, "Could not load vertex src!");
	uint8_t *fragmentSrc;
	LC_ASSERT(lc_LoaderReadFile(fragmentPath, &fragmentSrc) != -1, "Could not load fragment src!");
    
	program = glCreateProgram();
	
	uint32_t vertexID;
	uint32_t fragmentID;

	vertexID = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexID, 1, (const char**)&vertexSrc, 0);
	glCompileShader(vertexID);

	int isCompiled;
	glGetShaderiv(vertexID, GL_COMPILE_STATUS, &isCompiled);

	if (isCompiled == GL_FALSE)
	{
	    int maxLength;
		glGetShaderiv(vertexID, GL_INFO_LOG_LENGTH, &maxLength);
		char *msg = malloc(sizeof(char) * maxLength);
		glGetShaderInfoLog(vertexID, maxLength, &maxLength, msg);

		glDeleteShader(vertexID);

		LC_ASSERT(0, "Vertex shader compilation failure! %s", msg);

		free(msg);
	}

	glAttachShader(program, vertexID);
	
	fragmentID = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragmentID, 1, (const char**)&fragmentSrc, 0);

	glCompileShader(fragmentID);

	isCompiled;
	glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		int maxLength;
		glGetShaderiv(fragmentID, GL_INFO_LOG_LENGTH, &maxLength);

		char *msg = malloc(sizeof(char) * maxLength);
		glGetShaderInfoLog(fragmentID, maxLength, &maxLength, msg);

		glDeleteShader(fragmentID);

		LC_ASSERT(0, "Fragment shader compilation failure! %s", msg);

		free(msg);
	}

	glAttachShader(program, fragmentID);

	glLinkProgram(program);

	GLint isLinked;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE)
	{
		int maxLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		char *msg = malloc(sizeof(char) * maxLength);
		glGetShaderInfoLog(program, maxLength, &maxLength, msg);

		glDeleteProgram(program);

		glDeleteShader(vertexID);
		glDeleteShader(fragmentID);

		LC_ASSERT(0, "Shader link failure! %s", msg);

		free(msg);
	}

	glDetachShader(program, vertexID);
	glDetachShader(program, fragmentID);

	free(vertexSrc);
	free(fragmentSrc);

    glUseProgram(program);

	return program;
}

void lc_ShaderDestroy(lcShader_t shader)
{
	glDeleteProgram(shader);
}

/*-----------------------------uniforms-----------------------------*/

/* matrices */
void
lc_ShaderUploadUniformMatrix4(char *name,
                              float *matrix)
{
	GLint location = glGetUniformLocation(_lc_RendererBoundShader, name);
	glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
}

/* floats */
void
lc_ShaderUploadUniformFloat(char *name,
                            float value)
{
	GLint location = glGetUniformLocation(_lc_RendererBoundShader, name);
	glUniform1f(location, value);
}

void
lc_ShaderUploadUniformFloat2(char *name,
                             float *value)
{
	GLint location = glGetUniformLocation(_lc_RendererBoundShader, name);
	glUniform2f(location, value[0], value[1]);
}

void
lc_ShaderUploadUniformFloat3(char *name,
                             float *value)
{
	GLint location = glGetUniformLocation(_lc_RendererBoundShader, name);
	glUniform3f(location, value[0], value[1], value[2]);
}

void
lc_ShaderUploadUniformFloat4(char *name,
                             float *value)
{
	GLint location = glGetUniformLocation(_lc_RendererBoundShader, name);
	glUniform4f(location, value[0], value[1], value[2], value[3]);
}

/* integers */
void
lc_ShaderUploadUniformInt(char *name,
                          int value)
{
	GLint location = glGetUniformLocation(_lc_RendererBoundShader, name);
	glUniform1i(location, value);
}

void
lc_ShaderUploadUniformIntArray(char *name,
                               int count,
                               int *value)
{
	GLint location = glGetUniformLocation(_lc_RendererBoundShader, name);
	glUniform1iv(location, count, value);
}
