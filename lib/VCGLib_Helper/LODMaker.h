#ifndef LODMAKER_H
#define LODMAKER_H

#include "quadric_simp.h"
#include "vcg/complex/algorithms/clean.h"
#include "VCG_CMesh0_Helper.h"

struct LODMaker {


    static void decimateMesh(int targetFaceNb, const std::vector<uint32_t> &inIndices, const std::vector<Point3D> &inVertices, const std::vector<Point3D> &inNormals, std::vector<uint32_t> &outIndices, std::vector<Point3D> &outVertices, std::vector<Point3D> &outNormals);
    static void decimateMesh(int targetFaceNb, const std::vector<uint32_t> &inIndices, const std::vector<Point3D> &inVertices, std::vector<uint32_t> &outIndices, std::vector<Point3D> &outVertices);
};


#endif //LODMAKER_H
