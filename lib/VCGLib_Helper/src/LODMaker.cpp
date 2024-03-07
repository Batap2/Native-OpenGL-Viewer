#include "../LODMaker.h"


void LODMaker::decimateMesh(int targetFaceNb, CMeshO &mesh)
{
    mesh.vert.EnableVFAdjacency();
    mesh.face.EnableVFAdjacency();
    vcg::tri::UpdateTopology<CMeshO>::VertexFace(mesh);
    mesh.vert.EnableMark();

    //vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromVF(mesh);
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
    params.PreserveBoundary = false;
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

    QuadricSimplification(mesh, targetFaceNb, false, params, vcg::DummyCallBackPos);

    {
        int nullFaces=vcg::tri::Clean<CMeshO>::RemoveFaceOutOfRangeArea(mesh, 0);
        int deldupvert=vcg::tri::Clean<CMeshO>::RemoveDuplicateVertex(mesh);
        int delvert=vcg::tri::Clean<CMeshO>::RemoveUnreferencedVertex(mesh);
        mesh.face.DisableFFAdjacency();
        vcg::tri::Allocator<CMeshO>::CompactVertexVector(mesh);
        vcg::tri::Allocator<CMeshO>::CompactFaceVector(mesh);
    }

    vcg::tri::UpdateBounding<CMeshO>::Box(mesh);
    if(mesh.fn > 0) {
        vcg::tri::UpdateNormal<CMeshO>::PerFaceNormalized(mesh);
        vcg::tri::UpdateNormal<CMeshO>::PerVertexAngleWeighted(mesh);
    }

    vcg::tri::UpdateNormal<CMeshO>::NormalizePerFace(mesh);
    vcg::tri::UpdateNormal<CMeshO>::PerVertexFromCurrentFaceNormal(mesh);
    vcg::tri::UpdateNormal<CMeshO>::NormalizePerVertex(mesh);
}