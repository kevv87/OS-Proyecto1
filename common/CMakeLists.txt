set(SOURCES helloWorld.c imageChunk.c shared_memory.c)
set(HEADERS include/helloWorld.h include/imageChunk.h include/shared_memory.h)

add_library(Common ${SOURCES} ${HEADERS})
target_include_directories(Common PUBLIC "./")