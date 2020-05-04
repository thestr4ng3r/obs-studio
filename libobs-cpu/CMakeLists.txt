project(libobs-cpu)

add_definitions(-DLIBOBS_EXPORTS)

set(libobs-cpu_HEADERS
		cpu-subsystem.h)

set(libobs-cpu_SOURCES
		cpu-subsystem.c
		cpu-operations.cpp
		cpu-x11.c)

add_library(libobs-cpu SHARED
	${libobs-cpu_SOURCES}
	${libobs-cpu_HEADERS})

set_target_properties(libobs-cpu
	PROPERTIES
		OUTPUT_NAME obs-cpu
		VERSION 0.0
		SOVERSION 0
		)

target_link_libraries(libobs-cpu libobs)

install_obs_core(libobs-cpu)
