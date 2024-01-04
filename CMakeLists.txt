cmake_minimum_required(VERSION 3.11)

project(GodBot CXX)

set(CMAKE_CXX_STANDARD 20)

set (Boost_NO_SYSTEM_PATHS ON)
set (Boost_USE_MULTITHREADED ON)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_STATIC_RUNTIME OFF)
set (BOOST_ALL_DYN_LINK OFF)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

conan_basic_setup()

find_package(Boost 1.78.0 REQUIRED COMPONENTS program_options)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

add_executable(godbot src/main.cpp src/GodBot.cpp src/binance_api.cpp src/Kline.cpp src/GB_SimpleStrategy.cpp 
  src/GB_LogData.cpp src/GB_Logger.cpp src/curl_object.cpp src/base_api.cpp)

target_include_directories(godbot PUBLIC hpp/)

# target_include_directories(godbot INTERFACE ${Boost_INCLUDE_DIR})
target_link_libraries(godbot LINK_PUBLIC ${Boost_LIBRARIES})

target_link_libraries(godbot LINK_PUBLIC Threads::Threads)
target_link_libraries(godbot LINK_PUBLIC CURL::libcurl)
target_link_libraries(godbot LINK_PUBLIC OpenSSL::Crypto)
target_link_libraries(godbot LINK_PUBLIC OpenSSL::SSL)

