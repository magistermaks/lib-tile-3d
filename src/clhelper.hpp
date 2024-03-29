#pragma once

#include <core.hpp>

namespace CLHelper {

	/// used to initialize OpenCL
	bool init();

	/// used for loading of OpenCL kernels
	cl::Kernel loadKernel( const std::string& name );

}
