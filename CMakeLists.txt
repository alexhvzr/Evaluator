cmake_minimum_required(VERSION 3.13)
project(Evaluator)

set(CMAKE_CXX_STANDARD 14)

add_executable(Evaluator main.cpp evaluator.h tokenizer.h tokenizer.cpp.h token.h evaluator.cpp.h)