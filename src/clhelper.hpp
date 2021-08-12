#pragma once

#include "config.hpp"

namespace CLHelper {

	class KernelProgramBuilder {

		private:
			cl::Program::Sources sources;
			cl::Program program;
			bool failed = false;

		public:
			void addSource( std::string );
			void addFile( std::string );

			std::string build();
			cl::Kernel get( const char* name );

			bool isOk();
	};

	bool init();
	cl::Kernel loadKernel( std::string, std::string );

}
