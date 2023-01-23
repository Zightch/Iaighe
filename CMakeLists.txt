cmake_minimum_required(VERSION 3.23)
project(Iaighe)

set(CMAKE_CXX_STANDARD 23)
#add_compile_options("$<$<C_COMPILER_ID:GNU>:-fexec-charset=GBK>")
#add_compile_options("$<$<CXX_COMPILER_ID:GNU>:-fexec-charset=GBK>")
#add_compile_options("$<$<C_COMPILER_ID:GNU>:-finput-charset=utf-8>")
#add_compile_options("$<$<CXX_COMPILER_ID:GNU>:-finput-charset=utf-8>")

add_executable(Iaighe
        main.cpp
        #class/Trie/Trie.hpp
        #class/Exception/ElementNotFoundException/ElementNotFoundException.cpp
        #class/Exception/KeyDisagreeStandardException/KeyDisagreeStandardException.cpp
        #class/Exception/UnknownErrorException/UnknownErrorException.cpp
        #namespace/Symbol/Symbol.cpp
        define.cpp
        #Exception/LexicalAnalysisErrorException/LexicalAnalysisErrorException.cpp
        #lexer_old.cpp
        lexer.cpp
        readFile.cpp
        branchBreaker.cpp
        error.cpp
        #parser.cpp
        )
