cmake_minimum_required(VERSION 2.8)

project(img_proc)

add_compile_options(-Wall)
add_compile_options(-std=c11)

add_subdirectory(src)
add_subdirectory(test)
