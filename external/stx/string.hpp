
#include <string>
#include <vector>
#include <algorithm>

namespace stx {

	/// split string at the given delimeter
	std::vector<std::string> split( const std::string& str, const std::string& delim = "\n" );

	/// test if string includes given substring
	bool includes( const std::string& str, const std::string& needle );

	/// count the number of occurrences of given substring in a string
	size_t count( const std::string& str, const std::string& needle );

}
	

