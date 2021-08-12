
#include "string.hpp"

// based on: https://stackoverflow.com/a/44495206
std::vector<std::string> stx::split( const std::string& str, const std::string& delim ) {
	std::vector<std::string> arr;
	size_t pos = 0, size = delim.size();

	if(size == 0) {
		for( char c : str ) {
			arr.emplace_back(std::string() + c);
		}
	}else{
		// intentional str copy
		std::string s = str;

		while( (pos = s.find(delim)) != std::string::npos ) {
			arr.emplace_back( s.substr(0, pos) );
			s.erase(0, pos + size);
		}

		arr.push_back(s);
	}
				
	return arr;
}

bool stx::includes( const std::string& str, const std::string& needle ) {
	return str.find(needle) != std::string::npos;
}

size_t stx::count( const std::string& str, const std::string& needle ) {
	const size_t size = needle.size();

	if( size == 0 ) {
		return str.size() + 1;
	}else{
		size_t pos = 0, count = 0;

		while( (pos = str.find(needle, pos)) != std::string::npos ) {
			pos += size;
			count ++;
		}

		return count;
	}

}

