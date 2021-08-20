
#include "shader.hpp"

ShaderProgram::ShaderProgram(GLuint id) {
    this->program = id;
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(this->program);
}

GLuint ShaderProgram::location( const char* name ) {
    return glGetUniformLocation(this->program, name);
}

GLuint ShaderProgram::id() {
    return this->program;
}

void ShaderProgram::bind() {
    glUseProgram(this->program);
}

bool ShaderProgramBuilder::compileSource( std::string source, std::string identifier, GLenum type ) {
    GLint shader = glCreateShader(type);
    GLint result = GL_FALSE;
	int log_length;

    // compile shader
	const char* cstr = source.c_str();
	glShaderSource(shader, 1, &cstr, NULL);
	glCompileShader(shader);

	// check compilation status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

    // print status
	if( !result && log_length > 0 ) {
		std::vector<char> log(log_length + 1);
		glGetShaderInfoLog(shader, log_length, NULL, &log[0]);
        logger::error( "Failed to compile OpenGL shader: '" + identifier + "'!" );         
		printf("%s\n", &log[0]);
		return false;
	}
        
	this->shaders.push_back( shader );
	GLHelper::getError("compile() end");

	return true;
}

bool ShaderProgramBuilder::compileFile( std::string path, GLenum type ) {

    std::string source;
	std::ifstream ifile(path, std::ios::in);

	if( ifile.is_open() ) {
		std::stringstream sstr;
		sstr << ifile.rdbuf();
		source = sstr.str();
		ifile.close();

        // compile shader source
        return this->compileSource( source, path, type );
	}else{
		logger::error( "Failed to open OpenGL shader file: '" + path + "'!" );
        return false;
	}

}

bool ShaderProgramBuilder::link() {

    if( this->shaders.size() < 2 ) {
        logger::error( "Failed to link OpenGL shader program, expected at least two shaders!" );
		return false;
    }

	GLuint program = glCreateProgram();
    GLint result = GL_FALSE;
	int log_length;

    for( GLint shader : this->shaders ) {
        glAttachShader(program, shader);
    }

    glLinkProgram(program);

    // check the program
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

	if ( !result && log_length > 0 ){
		std::vector<char> log(log_length + 1);
		glGetProgramInfoLog(program, log_length, NULL, &log[0]);
        logger::error( "Failed to link OpenGL shader program!" );         
		printf("%s\n", &log[0]);
        return false;
	}

    for( GLint shader : this->shaders ) {
        glDetachShader(program, shader);
	    glDeleteShader(shader);
    }

	this->id = program;

    return true;
}

ShaderProgram* ShaderProgramBuilder::build() {
    return new ShaderProgram(this->id);
}

