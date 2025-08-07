cmake_minimum_required(VERSION 3.18)

project(godbot CXX)

set(CMAKE_CXX_STANDARD 20)

set (Boost_NO_SYSTEM_PATHS ON)
set (Boost_USE_MULTITHREADED ON)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_STATIC_RUNTIME OFF)
set (BOOST_ALL_DYN_LINK OFF)

find_package(Boost 1.74 COMPONENTS program_options REQUIRED)

include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp src/GodBot.cpp src/binance_api.cpp src/Kline.cpp src/GB_SimpleStrategy.cpp 
  src/GB_LogData.cpp src/GB_Logger.cpp src/curl_object.cpp src/base_api.cpp src/GB_RSIStrategy.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC hpp/)

target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${Boost_LIBRARIES})

target_link_libraries(${PROJECT_NAME} LINK_PUBLIC Threads::Threads)
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC CURL::libcurl)
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC OpenSSL::Crypto)
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC OpenSSL::SSL)

