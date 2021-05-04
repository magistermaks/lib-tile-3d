#pragma once

#include <string>
#include <iostream>

namespace logger {
    void info( std::string );
    void warn( std::string );
    void error( std::string );
    void fatal( std::string ); 
}
