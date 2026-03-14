cmake_minimum_required(VERSION 3.28)
project(Pacman LANGUAGES CXX)

find_package(PkgConfig REQUIRED)
pkg_check_modules(NCURSES REQUIRED IMPORTED_TARGET ncurses)

add_executable(main src/main.cpp)

target_link_libraries(main PRIVATE PkgConfig::NCURSES)
target_compile_features(main PRIVATE cxx_std_20)
