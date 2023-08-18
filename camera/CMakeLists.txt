find_package(Threads REQUIRED)
find_package(Boost 1.82.0 REQUIRED)
find_package(OpenCV REQUIRED)

message(STATUS "OpenCV imported to the Project: " ${OpenCV_INCLUDE_DIRS}/opencv2)
set(OpenCV_INCLUDE_DIR {OpenCV_INCLUDE_DIRS/opencv2})

add_executable(${PROJECT_NAME}_receiver src/subscriber.cpp)
target_include_directories(${PROJECT_NAME}_receiver PRIVATE include ${Boost_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME}_receiver ${Boost_LIBRARIES} Threads::Threads ${OpenCV_LIBS})

add_executable(${PROJECT_NAME}_sender src/publisher.cpp)
target_include_directories(${PROJECT_NAME}_sender PRIVATE include ${Boost_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME}_sender ${Boost_LIBRARIES} Threads::Threads ${OpenCV_LIBS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
