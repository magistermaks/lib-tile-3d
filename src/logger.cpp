
#include "logger.hpp"

void logger::info( std::string msg ) {
	std::cout << (getTimestamp() + " INFO: " + msg + "\n");
}

void logger::warn( std::string msg ) {
	std::cout << (getTimestamp() + " WARN: " + msg + "\n");
}

void logger::error( std::string msg ) {
	std::cout << (getTimestamp() + " ERROR: " + msg + "\n");
}

void logger::fatal( std::string msg ) {
	std::cout << (getTimestamp() + " FATAL: " + msg + "\n");
}

// stolen from Sequensa
std::string logger::getTimestamp( const char* pattern ) {
	time_t rawtime = time(0); 

	struct tm ts;
	char buf[255];

	ts = *gmtime(&rawtime);
	strftime(buf, sizeof(buf), pattern, &ts);

	return std::string(buf);
}
