set(SOURCES helloWorld.c imageChunk.c shared_memory.c)
set(HEADERS include/helloWorld.h include/imageChunk.h include/shared_memory.h)

set(LIBRARY_NAME Common)

add_library(${LIBRARY_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${LIBRARY_NAME} PUBLIC "./")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
    target_link_libraries(${LIBRARY_NAME} PUBLIC rt)
endif()