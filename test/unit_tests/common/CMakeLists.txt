cmake_minimum_required(VERSION 3.10)

project(Test)

# Adding the projects to test
add_subdirectory("${PROJECT_SOURCE_DIR}/../../../common/" "${PROJECT_BINARY_DIR}/common")

set(SOURCES linked_list_test.c)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC Common)

target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/../../../")

# Adding cmocka
add_subdirectory("${PROJECT_SOURCE_DIR}/../../../cmocka" "${PROJECT_BINARY_DIR}/cmocka")
target_link_libraries(${PROJECT_NAME} PUBLIC cmocka)