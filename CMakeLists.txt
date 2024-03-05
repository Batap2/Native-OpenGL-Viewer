cmake_minimum_required(VERSION 3.26)
project(DecimationAttempt2)

set(CMAKE_CXX_STANDARD 17)


add_subdirectory(lib)

add_executable(DecimationAttempt2 src/3dview.cpp
        src/Matrix3x3.h
        src/Matrix3x3.inl.h
        src/Matrix4x4.h
        src/Matrix4x4.inl.h
        src/Point3D.h
        src/Point3D.inl.h
        src/ObjIO.h
)

target_link_directories(DecimationAttempt2 PUBLIC ${PROJECT_SOURCE_DIR}/lib)
target_include_directories(DecimationAttempt2 PUBLIC ${PROJECT_SOURCE_DIR}/lib)

if(WIN32)
    target_link_libraries(DecimationAttempt2 freeglut opengl32 vcglib VCGLib_Helper)
endif(WIN32)

if(UNIX)
    target_link_libraries(DecimationAttempt2 GL glut vcglib VCGLib_Helper)
endif (UNIX)