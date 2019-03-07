CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

project(state_machine)

file(GLOB_RECURSE HEADER_FILES "include/*.h")
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
add_library(StateMachine STATIC ${HEADER_FILES} ${SOURCE_FILES})
target_include_directories(StateMachine PUBLIC include)

install(TARGETS StateMachine DESTINATION lib)
install(FILES ${HEADER_FILES} DESTINATION include/StateMachine)
