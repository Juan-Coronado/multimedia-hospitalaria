cmake_minimum_required(VERSION 3.16)
project(SistemaHospitalarioMultimedia LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 🔥 ESTA ES LA LÍNEA MÁGICA QUE SOLUCIONA EL ERROR "byte ambiguo" 🔥
add_compile_definitions(_HAS_STD_BYTE=0)

include_directories(include)

set(SOURCES
    src/main.cpp
    src/MotorAlgoritmico.cpp
    src/FileSystem.cpp
    src/BusinessLogic.cpp
    src/Presentation.cpp
)

add_executable(SistemaHospitalarioMultimedia ${SOURCES})

if(MSVC)
    target_compile_options(SistemaHospitalarioMultimedia PRIVATE /W3)
endif()