#ifndef LODMAKER_H
#define LODMAKER_H

#include "quadric_simp.h"
#include "vcg/complex/algorithms/clean.h"
#include "vcg/complex/algorithms/clustering.h"
#include "VCG_CMesh0_Helper.h"

struct vcgLibHelperLOD{
    std::vector<Point3D> vertices;
    std::vector<uint32_t> indices;
};

struct LODMaker
{
    static void decimateMesh(int targetFaceNb, CMeshO &mesh);

    static void decimatePreparedMesh(int targetFaceNb, CMeshO &mesh);

    static std::vector<vcgLibHelperLOD> makeLOD(CMeshO &mesh, int maxLODNumber, float reduction);

    static void repairAndPrepareForDecimation(CMeshO &mesh);
};


#endif //LODMAKER_H
