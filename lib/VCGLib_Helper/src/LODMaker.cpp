#include "../LODMaker.h"


void LODMaker::decimateMesh(int targetFaceNb, const std::vector<uint32_t> &inIndices, const std::vector<Point3D> &inVertices, const std::vector<Point3D> &inNormals, std::vector<uint32_t> &outIndices, std::vector<Point3D> &outVertices, std::vector<Point3D> &outNormals)
{
    CMeshO m1 = VCG_CMesh0_Helper::constructCMesh(inIndices, inVertices, inNormals);

    int nullFaces=vcg::tri::Clean<CMeshO>::RemoveFaceOutOfRangeArea(m1,0);
    //int deldupvert=vcg::tri::Clean<CMeshO>::RemoveDuplicateVertex(m1);
    //int delvert=vcg::tri::Clean<CMeshO>::RemoveUnreferencedVertex(m1);

    m1.vert.EnableVFAdjacency();
    m1.face.EnableVFAdjacency();
    vcg::tri::UpdateTopology<CMeshO>::VertexFace(m1);
    m1.vert.EnableMark();

    //vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromVF(m1);
    vcg::tri::TriEdgeCollapseQuadricParameter params;

    params.BoundaryQuadricWeight = 0.5;
    params.FastPreserveBoundary = false;
    params.AreaCheck = false;
    params.HardQualityCheck = false;
    params.HardQualityThr = 0.1;
    params.HardNormalCheck = false;
    params.NormalCheck = false;
    //params.NormalThrRad = M_PI/2.0;
    //params.CosineThr             = 0 ; // ~ cos(pi/2)
    params.OptimalPlacement =true;
    //params.SVDPlacement = false;
    params.PreserveTopology = false;
    params.PreserveBoundary = true;
    //params.QuadricEpsilon = 1e-15;
    params.QualityCheck = true;
    params.QualityThr =.3;     // Collapsed that generate faces with quality LOWER than this value are penalized. So higher the value -> better the quality of the accepted triangles
    params.QualityQuadric = false; // planar simplification
    params.QualityQuadricWeight = 0.001f; // During the initialization manage all the edges as border edges adding a set of additional quadrics that are useful mostly for keeping face aspect ratio good.
    params.QualityWeight=false;
    params.QualityWeightFactor=100;
    //params.ScaleFactor=1.0;
    //params.ScaleIndependent=true;
    //params.UseArea =true;
    //params.UseVertexWeight=false;

    //QuadricSimplification(m1, targetFaceNb, false, params, vcg::DummyCallBackPos);

    {
        int nullFaces=vcg::tri::Clean<CMeshO>::RemoveFaceOutOfRangeArea(m1,0);
        int deldupvert=vcg::tri::Clean<CMeshO>::RemoveDuplicateVertex(m1);
        int delvert=vcg::tri::Clean<CMeshO>::RemoveUnreferencedVertex(m1);
        m1.face.DisableFFAdjacency();
        vcg::tri::Allocator<CMeshO>::CompactVertexVector(m1);
        vcg::tri::Allocator<CMeshO>::CompactFaceVector(m1);
    }

    vcg::tri::UpdateBounding<CMeshO>::Box(m1);
    if(m1.fn>0) {
        vcg::tri::UpdateNormal<CMeshO>::PerFaceNormalized(m1);
        vcg::tri::UpdateNormal<CMeshO>::PerVertexAngleWeighted(m1);
    }

    vcg::tri::UpdateNormal<CMeshO>::NormalizePerFace(m1);
    vcg::tri::UpdateNormal<CMeshO>::PerVertexFromCurrentFaceNormal(m1);
    vcg::tri::UpdateNormal<CMeshO>::NormalizePerVertex(m1);

    VCG_CMesh0_Helper::retrieveCMeshData(m1, outIndices, outVertices, outNormals);
}

void LODMaker::decimateMesh(int targetFaceNb, const std::vector<uint32_t> &inIndices, const std::vector<Point3D> &inVertices, std::vector<uint32_t> &outIndices, std::vector<Point3D> &outVertices)
{
    std::vector<Point3D> empty;
    decimateMesh(targetFaceNb, inIndices, inVertices, empty, outIndices, outVertices, empty);
}