
#include "clhelper.hpp"

void CLHelper::KernelProgram::addSource( std::string source ) {
	this->sources.push_back( {source.c_str(), source.length()} );
}

void CLHelper::KernelProgram::addFile( std::string path ) {

	std::string source;
	std::ifstream ifile(path, std::ios::in);
	const std::string command = "#require ";

	if( ifile.is_open() ) {
		std::stringstream sstr;
		sstr << ifile.rdbuf();
		ifile.close();

		std::string line;

		while( std::getline(sstr, line) ) {
			if( !line.compare(0, command.size(), command) ) {
				addFile( line.substr( command.size() ) );
			} else {
				source += line + "\n";
			}
		}

		//logger::info( source );

        // compile shader source
        this->addSource( source );
	}else{
		logger::error( "Failed to read kernel file: '" + path + "'!" );
        failed = true;
	}

}

std::string CLHelper::KernelProgram::build() {

	this->program = cl::Program(sources);
    if( program.build( {cl::Device::getDefault()} ) != CL_SUCCESS) {
		failed = true;
        return program.getBuildInfo<CL_PROGRAM_BUILD_LOG>( cl::Device::getDefault() );
    }

}

cl::Kernel CLHelper::KernelProgram::getKernel( const char* name ) {
	return cl::Kernel( program, name );
}

bool CLHelper::KernelProgram::isOk() {
	return !failed;
}

bool CLHelper::init() {

	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	cl::Platform platform;

	// search for platform supporting OpenCL 2.X
	for( auto &p : platforms ) {
		std::string version = p.getInfo<CL_PLATFORM_VERSION>();

		if( version.find("OpenCL 2.") != std::string::npos ) {
			logger::info("Found OpenCL 2.0 platform: '" + version + "'");
			platform = p;
			break;
		}
	}

	// check is any platform was found
	if( platform() == 0 ) {
		logger::fatal("No OpenCL 2.0 platform found!");
		return false;
	}

	// get default device of the default platform
    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
	size_t count = devices.size();

	// check device count
    if( count == 0 ) {
		logger::fatal("No GPU device found!");
        return false;
    }else{
		logger::info("Found " + std::to_string(count) + " OpenCL GPU device(s)");
	}

	cl::Context context( {devices[0]} );

	// set created context as default
	if( cl::Context::setDefault(context) != context || cl::Device::setDefault(devices[0]) != devices[0] ) {
		logger::fatal("Failed to set OpenCL defaults!");
		return false;
	}

	return true;

}

cl::Kernel CLHelper::loadKernel( std::string path, std::string kernel ) {

	KernelProgram program;
	program.addFile( "shader/" + path );
	std::string log = program.build();

	if( !program.isOk() ) {
		logger::error(log);
	}

	return program.getKernel( kernel.c_str() );

}

