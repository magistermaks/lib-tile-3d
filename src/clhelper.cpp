
#include "clhelper.hpp"

void CLHelper::KernelProgramBuilder::addSource( std::string source ) {
	this->sources.push_back( {source.c_str(), source.length()} );
}

void CLHelper::KernelProgramBuilder::addFile( std::string path ) {

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

		// compile shader source
		this->addSource( source );
	}else{
		logger::error( "Failed to read kernel file: '" + path + "'!" );
		failed = true;
	}

}

std::string CLHelper::KernelProgramBuilder::build() {

	if( failed ) {
		return "Resource loading failed!";
	}

	this->program = cl::Program(sources);
	if( program.build( {cl::Device::getDefault()} ) != CL_SUCCESS) {
		return program.getBuildInfo<CL_PROGRAM_BUILD_LOG>( cl::Device::getDefault() );
	}

	return "";

}

cl::Kernel CLHelper::KernelProgramBuilder::get( const char* name ) {
	return cl::Kernel( program, name );
}

bool CLHelper::init() {

	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	cl::Platform platform;

	// search for platform supporting OpenCL >=2.0
	for( auto &plat : platforms ) {

		std::string version = plat.getInfo<CL_PLATFORM_VERSION>();
		std::smatch match;

		if( std::regex_search(version, match, std::regex("OpenCL (\\d+)", std::regex_constants::icase)) && std::stoi(match[1]) > 1 ) {
			logger::info("Found OpenCL platform: '" + version + "'");
			platform = plat;
			break;
		}
	}

	// check is any platform was found
	if( platform() == 0 ) {
		logger::fatal("No OpenCL platform found!");
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

	KernelProgramBuilder builder;
	builder.addFile( "shader/opencl/" + path );
	std::string log = builder.build();

	if( !log.empty() ) {
		logger::fatal( "Failed to load kernel: " + log );
	}else{
		logger::info( "Loaded OpenCL kernel: '" + kernel + "' from: 'shader/opencl/" + path + "'" );
	}

	return builder.get( kernel.c_str() );

}

