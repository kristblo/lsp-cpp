cmake_minimum_required(VERSION 3.14)
project(Lsp)

set(CMAKE_CXX_STANDARD 14)

#Generation of compile_commands.json is handled by the main program
#set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

include_directories(include)
include_directories(src)
aux_source_directory(src/ sources)
set(lsp_include_dirs ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "" )
add_executable(LspClientTest ${sources})
