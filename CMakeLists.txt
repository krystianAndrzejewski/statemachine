CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

project(State_machine_sample)

add_subdirectory(StateMachine)

add_executable(app main.cpp)
target_link_libraries(app StateMachine)

install(TARGETS app DESTINATION bin)

