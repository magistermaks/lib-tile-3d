#pragma once

#include <config.hpp>

namespace logger {
	void info( std::string );
	void warn( std::string );
	void error( std::string );
	void fatal( std::string );
	std::string getTimestamp( const char* pattern = "%H:%M:%S" );
}
