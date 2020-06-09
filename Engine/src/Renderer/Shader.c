typedef uint32_t lc_Shader;

lc_Shader lc_CreateShader(const char* vertexPath, const char* fragmentPath)
{
	//create a shader program ID to return
	lc_Shader program;

	char* vertexSrc;
	LC_ASSERT(lc_LoadFile(vertexPath, &vertexSrc) != -1, "Could not load vertex src!");
	char* fragmentSrc;
	LC_ASSERT(lc_LoadFile(fragmentPath, &fragmentSrc) != -1, "Could not load fragment src!");
    
	program = glCreateProgram();
	
	uint32_t vertexID;
	uint32_t fragmentID;

	//compile the vertex shader and attach it to the program
	vertexID = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexID, 1, (const char**)&vertexSrc, 0);
	glCompileShader(vertexID);

	int isCompiled;
	glGetShaderiv(vertexID, GL_COMPILE_STATUS, &isCompiled);

	if (isCompiled == GL_FALSE)
	{
	    int maxLength;
		glGetShaderiv(vertexID, GL_INFO_LOG_LENGTH, &maxLength);
		char* msg = malloc(sizeof(char) * maxLength);
		glGetShaderInfoLog(vertexID, maxLength, &maxLength, msg);

		glDeleteShader(vertexID);

		LC_ASSERT(false, "Vertex shader compilation failure! %s", msg);

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

		char* msg = malloc(sizeof(char) * maxLength);
		glGetShaderInfoLog(fragmentID, maxLength, &maxLength, msg);

		glDeleteShader(fragmentID);

		LC_ASSERT(false, "Fragment shader compilation failure! %s", msg);

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

		char* msg = malloc(sizeof(char) * maxLength);
		glGetShaderInfoLog(program, maxLength, &maxLength, msg);

		glDeleteProgram(program);

		glDeleteShader(vertexID);
		glDeleteShader(fragmentID);

		LC_ASSERT(false, "Shader link failure! %s", msg);

		free(msg);
	}

	glDetachShader(program, vertexID);
	glDetachShader(program, fragmentID);

	free(vertexSrc);
	free(fragmentSrc);

	return program;
}

void lc_DestroyShader(lc_Shader shader)
{
	glDeleteProgram(shader);
}

//----uniforms-------------------------------------------------------------------------------

//floats
void lc_ShaderUploadUniformFloat(lc_Shader shader, const char* name, float value)
{
	GLint location = glGetUniformLocation(shader, name);
	glUniform1f(location, value);
}
void lc_ShaderUploadUniformFloat2(lc_Shader shader, const char* name, const float* value)
{
	GLint location = glGetUniformLocation(shader, name);
	glUniform2f(location, value[0], value[1]);
}
void lc_ShaderUploadUniformFloat3(lc_Shader shader, const char* name, const float* value)
{
	GLint location = glGetUniformLocation(shader, name);
	glUniform3f(location, value[0], value[1], value[2]);
}
void lc_ShaderUploadUniformFloat4(lc_Shader shader, const char* name, const float* value)
{
	GLint location = glGetUniformLocation(shader, name);
	glUniform4f(location, value[0], value[1], value[2], value[3]);
}

//integers
void lc_ShaderUploadUniformInt(lc_Shader shader, const char* name, int value)
{
	GLint location = glGetUniformLocation(shader, name);
	glUniform1i(location, value);
}

void lc_ShaderUploadUniformIntArray(lc_Shader shader, const char* name, int count, int* value)
{
	GLint location = glGetUniformLocation(shader, name);
	glUniform1iv(location, count, value);
}
