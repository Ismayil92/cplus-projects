cmake_minimum_required(VERSION 3.10)
project(camera VERSION 0.1.0 LANGUAGES C CXX)

find_package(Threads REQUIRED)
find_package(Boost 1.82.0 REQUIRED)

add_executable(${PROJECT_NAME}_receiver src/subscriber.cpp)
target_include_directories(${PROJECT_NAME}_receiver PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}_receiver ${Boost_LIBRARIES} Threads::Threads)

add_executable(${PROJECT_NAME}_sender src/publisher.cpp)
target_include_directories(${PROJECT_NAME}_receiver PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}_receiver ${Boost_LIBRARIES} Threads::Threads)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
