#ifndef LODMAKER_H
#define LODMAKER_H

#include "quadric_simp.h"
#include "vcg/complex/algorithms/clean.h"
#include "VCG_CMesh0_Helper.h"

struct LODMaker
{
    static void decimateMesh(int targetFaceNb, CMeshO &mesh);
};


#endif //LODMAKER_H
