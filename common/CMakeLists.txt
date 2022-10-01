set(SOURCES helloWorld.c imageChunk.c)
set(HEADERS include/helloWorld.h include/imageChunk.h)

add_library(Common ${SOURCES} ${HEADERS})
target_include_directories(Common PUBLIC "./")