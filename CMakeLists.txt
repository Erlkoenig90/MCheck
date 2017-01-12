cmake_minimum_required(VERSION 3.5)


# set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX
#	${CMAKE_CURRENT_SOURCE_DIR}/cxx_flag_overrides.cmake)

project(MCheck)

if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG				"/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1 /MP /W4")
    set(CMAKE_CXX_FLAGS_MINSIZEREL			"/MT /O1 /Ob1 /D DNDEBUG /MP /W4")
    set(CMAKE_CXX_FLAGS_RELEASE				"/MT /O2 /Ob2 /D DNDEBUG /MP /W4")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO		"/MT /Zi /O2 /Ob1 /D DNDEBUG /MP /W4")
endif()
if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS						"${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -ffunction-sections -fdata-sections")
	set(CMAKE_EXE_LINKER_FLAGS				"${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")

	set(CMAKE_CXX_FLAGS_MINSIZEREL			"${CMAKE_CXX_FLAGS_MINSIZEREL} -flto")
	set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL	"${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL} -s")

	set(CMAKE_CXX_FLAGS_RELEASE				"${CMAKE_CXX_FLAGS_RELEASE} -flto")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE		"${CMAKE_EXE_LINKER_FLAGS_RELEASE} -static -static-libgcc -static-libstdc++ -s")
	
#	set(CMAKE_CXX_FLAGS_DEBUG			"")
#	set(CMAKE_EXE_LINKER_FLAGS_DEBUG	"")
	
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO		"${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -flto")
	set(CMAKE_LINKER_FLAGS_RELWITHDEBINFO	"${CMAKE_LINKER_FLAGS_RELWITHDEBINFO} -static -static-libgcc -static-libstdc++")
endif()



set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost 1.63.0 REQUIRED filesystem system)

if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS}) 
	add_executable(MCheck src/main.cc src/base_grammar.cc src/formula_ast.cc src/ctl/ctl.cc src/ctl/ctl_grammar.cc src/ltl/ltl.cc src/ltl/ltl_output.cc src/ltl/ltl_grammar.cc src/ts/grammar.cc src/ts/sys.cc src/ts/ts_ast.cc src/sysstring.cc) 
	target_link_libraries(MCheck ${Boost_LIBRARIES})

	set_property(TARGET MCheck PROPERTY CXX_STANDARD 14)
else()
	message(STATUS "Boost not found!")
endif()

