cmake_minimum_required(VERSION 3.17)
project(together_viper C)

set(CMAKE_C_STANDARD 99)

include_directories(/usr/local/Cellar/curl/8.0.1/include)

add_executable(together_viper main.c)

target_link_libraries(together_viper /usr/local/Cellar/curl/8.0.1/lib/libcurl.dylib)
