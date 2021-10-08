
get_filename_component(name ${CMAKE_CURRENT_SOURCE_DIR} NAME)

message("-- Found LT3D example/test directory '${name}'")

file(GLOB_RECURSE SOURCES RELATIVE 
	${CMAKE_CURRENT_SOURCE_DIR} 
	"*.cpp"
)

add_executable(${name}
	${SOURCES}
)

target_link_libraries(${name}
	LT3D
)

set_target_properties(${name}
	PROPERTIES OUTPUT_NAME main
)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY "${CMAKE_SOURCE_DIR}/assets" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

