
#include "logger.hpp"

void logger::info( std::string msg ) {
	std::cout << "INFO: " << msg << std::endl;
}

void logger::warn( std::string msg ) {
	std::cout << "WARN: " << msg << std::endl;
}

void logger::error( std::string msg ) {
	std::cout << "ERROR: " << msg << std::endl;
}

void logger::fatal( std::string msg ) {
	std::cout << "FATAL: " << msg << std::endl;
}
