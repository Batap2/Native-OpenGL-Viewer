message(STATUS "CMakeLists.txt in ${CMAKE_CURRENT_SOURCE_DIR} is being processed")

#project(VCGLib_Helper)

set(VGCLib_HelperSources
        "src/cmesh.cpp"
        "src/VCG_CMesh0_Helper.cpp"
        "src/quadric_simp.cpp"
        "src/LODMaker.cpp"
)

set(VGCLib_HelperHeaders
        "cmesh.h"
        "VCG_CMesh0_Helper.h"
        "quadric_simp.h"
        "LODMaker.h"
)

add_library(VCGLib_Helper ${VGCLib_HelperSources})

target_link_libraries(VCGLib_Helper PUBLIC vcglib)