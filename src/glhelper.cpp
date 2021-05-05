#include "glhelper.hpp"

GLFWwindow* windowHandle = NULL;

GLFWwindow* GLHelper::window() {
    return windowHandle;
}

bool GLHelper::init(int width, int height, const char* name) {

	if( glfwInit() != true ) {
		logger::fatal( "Failed to initialize GLFW!" );
		return false;
	}

	glfwSetErrorCallback( [] (int err, const char* msg) -> void {
		logger::warn( "GLFW Error: " + std::string(msg) );
	} );

	//glfwWindowHint(GLFW_SAMPLES, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	windowHandle = glfwCreateWindow( width, height, name, NULL, NULL);

	if( windowHandle == NULL ) {
		logger::fatal( "Failed to open GLFW window!" );
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(windowHandle);
	glewExperimental = true;

	if( glewInit() != GLEW_OK ) {
		logger::fatal( "Failed to initialize GLEW!" );
		glfwTerminate();
		return false;
	}

	// clear invalid error
	glGetError();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// not fully supported (breaks 50% of the time)
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	srand( time(0) );

	return true;

}

void GLHelper::getError() {
	GLenum err = glGetError();

	std::string name;

	switch( err ) {
		case GL_NO_ERROR: name = "GL_NO_ERROR"; break;
		case GL_INVALID_ENUM: name = "GL_INVALID_ENUM"; break;
		case GL_INVALID_VALUE: name = "GL_INVALID_VALUE"; break;
		case GL_INVALID_OPERATION: name = "GL_INVALID_OPERATION"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: name = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
		case GL_OUT_OF_MEMORY: name = "GL_OUT_OF_MEMORY"; break;
		case GL_STACK_UNDERFLOW: name = "GL_STACK_UNDERFLOW"; break;
		case GL_STACK_OVERFLOW: name = "GL_STACK_OVERFLOW"; break;
		default: name = "UNKNOWN";
	}

	if( err != GL_NO_ERROR ) {
		logger::warn( "OpenGL Error: " + std::to_string(err) + ", " + name + "!" );
	}
}

void GLHelper::vertexAttribute( GLint index, GLint length, GLenum type, GLsizei stride, GLsizei offset, GLsizei size, GLboolean normalize ) {
	glVertexAttribPointer(index, length, type, normalize, stride * size, (GLvoid*) (long) (offset * size));
	glEnableVertexAttribArray(index);
}

GLHelper::ShaderProgram GLHelper::loadShaders() {

	auto builder = GLHelper::ShaderProgram::make();
	builder.compileFile( "shader/vertex.glsl", GL_VERTEX_SHADER );
	builder.compileFile( "shader/fragment.glsl", GL_FRAGMENT_SHADER );
	builder.link();

	if( !builder.isOk() ) {
		glfwTerminate();
		exit(-1);
	}

	// let's hope that the return will be optimized, otherwise things will break (the destructor must not be called!)
	return GLHelper::ShaderProgram( builder.get() );

}

void GLHelper::ShaderProgramBuilder::compileSource( std::string source, std::string identifier, GLenum type ) {
    GLint shader = glCreateShader(type);
    GLint result = GL_FALSE;
	int log_length;

    logger::info( "Compiling shader: '" + identifier + "'..." );

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
        logger::error( "Shader compilation error!" );         
		printf("%s\n", &log[0]);
        failed = true;
	}else{
        this->shaders.push_back( shader );
    }
}

void GLHelper::ShaderProgramBuilder::compileFile( std::string path, GLenum type ) {

    std::string source;
	std::ifstream ifile(path, std::ios::in);

	if( ifile.is_open() ) {
		std::stringstream sstr;
		sstr << ifile.rdbuf();
		source = sstr.str();
		ifile.close();

        // compile shader source
        this->compileSource( source, path, type );
	}else{
		logger::error( "Failed to read shader file: '" + path + "'!" );
        failed = true;
	}

}

void GLHelper::ShaderProgramBuilder::link() {

    logger::info("Linking shader program...");

    if( this->failed ) {
        logger::error( "Unable to link, compilation incomplete!" );
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
        logger::error( "Program linking error!" );         
		printf("%s\n", &log[0]);
        failed = true;
	}

    for( GLint shader : this->shaders ) {
        glDetachShader(program, shader);
	    glDeleteShader(shader);
    }

    if( !this->failed ) this->id = program;
}

bool GLHelper::ShaderProgramBuilder::isOk() {
    return !this->failed;
}

GLint GLHelper::ShaderProgramBuilder::get() {
    return this->id;
}

GLHelper::ShaderProgram::ShaderProgram(GLuint id) {
    this->program = id;
}

GLHelper::ShaderProgram::ShaderProgram(GLHelper::ShaderProgramBuilder& builder) {
    this->program = builder.get();
}

GLHelper::ShaderProgram::~ShaderProgram() {
    glDeleteProgram(this->program);
}


GLHelper::ShaderProgramBuilder GLHelper::ShaderProgram::make() {
    return GLHelper::ShaderProgramBuilder();
}

GLuint GLHelper::ShaderProgram::location( const char* name ) {
    return glGetUniformLocation(this->program, name);
}

GLuint GLHelper::ShaderProgram::id() {
    return this->program;
}

void GLHelper::ShaderProgram::bind() {
    glUseProgram(this->program);
}

