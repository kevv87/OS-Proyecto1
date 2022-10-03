cmake_minimum_required(VERSION 3.10)

project(Decoder)

add_subdirectory("${PROJECT_SOURCE_DIR}/../common/" "${PROJECT_BINARY_DIR}/common")

# Setting sources files for decoder
set(SOURCES decoder.c)

# Adding sources al executable
add_executable( ${PROJECT_NAME} ${SOURCES} )

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
	#export PKG_CONFIG_PATH="$(brew --prefix libffi)/lib/pkgconfig/"
    set(ENV{PKG_CONFIG_PATH} /usr/local/opt/libffi/lib/pkgconfig)
endif()

find_package(PkgConfig REQUIRED)
	pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

message(STATUS "----- GKT3_LIBRARIES: ${GTK3_LIBRARIES}")
message(STATUS "----- GTK3_INCLUDE_DIRS: ${GTK3_INCLUDE_DIRS}")
message(STATUS "----- GTK3_CFLAGS_OTHER: ${GTK3_CFLAGS_OTHER}")


include_directories(${GTK3_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PUBLIC Common m ${GTK3_LIBRARIES})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GTK3_CFLAGS_OTHER}")

target_include_directories(
	${PROJECT_NAME} PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/../common/include"
	"${GTK3_INCLUDE_DIRS}"
)