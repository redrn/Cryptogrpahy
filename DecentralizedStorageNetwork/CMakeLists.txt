cmake_minimum_required(VERSION 3.10)

#set(CMAKE_TOOLCHAIN_FILE C:/Users/RedRN/Documents/c++/vcpkg/scripts/buildsystems/vcpkg.cmake)
set(CMAKE_TOOLCHAIN_FILE /mnt/c/Users/RedRN/Documents/c++/vcpkg/scripts/buildsystems/vcpkg.cmake)

project(CryptoStorage)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

find_package(OpenSSL REQUIRED)
find_package(libssh CONFIG REQUIRED)

add_executable(main ${CMAKE_CURRENT_SOURCE_DIR}/src/file_block_encrypt.cpp)
target_link_libraries(main PRIVATE OpenSSL::SSL OpenSSL::Crypto)

add_executable(ssh_connect ${CMAKE_CURRENT_SOURCE_DIR}/src/ssh_connect.cpp)
target_link_libraries(ssh_connect PRIVATE ssh)