
#include "clhelper.hpp"

// helper function used by CLHelper::loadKernel
bool parseKernelFile( cl::Program::Sources& sources, const std::string& path, const std::string& filename ) {
	
	std::string source;
	std::string fullpath = path + filename;
	std::ifstream ifile(fullpath, std::ios::in);
	const std::string command = "#require ";

	// open specified file
	if( ifile.is_open() ) {
		std::stringstream sstr;
		sstr << ifile.rdbuf();
		ifile.close();

		std::string line;

		while( std::getline(sstr, line) ) {
			if( !line.compare(0, command.size(), command) ) {
				if( !parseKernelFile( sources, path, line.substr( command.size() ) ) ) {
					logger::info( "Required from: '" + fullpath + "'" );
					return false;
				}
			} else {
				source += line + "\n";
			}
		}

		// add parsed source
		sources.push_back( {source.c_str(), source.length()} );
	}else{
		logger::error( "Failed to read kernel file: '" + fullpath + "'!" );
		return false;
	}

	return true;

}

bool CLHelper::init() {

	using cl_ctxprop = cl_context_properties;

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

	// OpenCL/OpenGL shared context properties
	cl_ctxprop properties[] = {
		CL_GL_CONTEXT_KHR, GLHelper::getContext(),
		CL_DISPLAY_KHR, GLHelper::getDisplay(),
		CL_CONTEXT_PLATFORM, (cl_ctxprop) (cl_platform_id) platform(),
		0
	};

	// get default device of the default platform
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
	size_t count = devices.size();

	// check device count
	if( count == 0 ) {
		logger::fatal("No OpenCL GPU device found!");
		return false;
	}else{
		logger::info("Found " + std::to_string(count) + " OpenCL GPU device(s)");
	}

	cl::Context context( {devices[0]}, properties );

	if( context() == 0 ) {
		logger::fatal("Failed to create OpenCL context!");
		return false;
	}

	// set created context as default
	if( cl::Context::setDefault(context) != context || cl::Device::setDefault(devices[0]) != devices[0] ) {
		logger::fatal("Failed to set OpenCL defaults!");
		return false;
	}

	// check device extensions
	bool success = true;
	auto extension = [&] ( std::string& str, std::string name ) {
		if( !stx::includes( str, name ) ) {
			logger::error( "Required extension '" + name + "' not supported!" );
			success = false;
		}
	};

	std::string extensions = devices[0].getInfo<CL_DEVICE_EXTENSIONS>();
	extension( extensions, "cl_khr_gl_sharing" );

	if( !success ) {
		logger::fatal( "Some required extensions are missing!" );
		return false;
	}

	return true;

}

cl::Kernel CLHelper::loadKernel( const std::string& name ) {

	cl::Program::Sources sources;
	cl::Program program;

	if( parseKernelFile( sources, "assets/opencl/", name ) ) {
		
		cl::Program program = cl::Program(sources);
		if( program.build( {cl::Device::getDefault()} ) != CL_SUCCESS ) {
			std::string log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>( cl::Device::getDefault() );

			logger::fatal( "Failed to compile OpenCL kernel: '" + name + "'\n" + log );
		}else{
			logger::info( "Loaded OpenCL kernel: '" + name + "'" );
			return cl::Kernel( program, "main" );
		}

	}

	throw std::runtime_error("OpenCL kernel failed to load!");

}

