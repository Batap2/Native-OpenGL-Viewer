//
// Created by batap on 23/02/2024.
//

#include "../VCG_CMesh0_Helper.h"
#include "vcg/complex/algorithms/clustering.h"


CMeshO VCG_CMesh0_Helper::constructCMesh(const std::vector<uint32_t> &indices, const std::vector<Point3D> &vertices, const std::vector<Point3D> &faceNormals)
{
    CMeshO m;

    bool hasVNormals = false;
    bool hasFNormals = faceNormals.size() > 0;

    std::vector<CMeshO::VertexPointer> ivp(vertices.size());

    CMeshO::VertexIterator vi = vcg::tri::Allocator<CMeshO>::AddVertices(m, vertices.size());

    for (unsigned int i = 0; i < vertices.size(); ++i, ++vi) {
        ivp[i]  = &*vi;
        vi->P() = CMeshO::CoordType(vertices[i][0],vertices[i][1], vertices[i][2]);
        vi->Base().EnableMark();
        vi->Base().EnableVFAdjacency();
        if (hasVNormals) {
            vi->N() = CMeshO::CoordType(faceNormals[i][0], faceNormals[i][1], faceNormals[i][2]);
        }
    }

    CMeshO::FaceIterator fi = vcg::tri::Allocator<CMeshO>::AddFaces(m, indices.size()/3);
    for (unsigned int i = 0; i < indices.size()/3; ++i, ++fi) {

        fi->V(0) = ivp[indices[i*3]];
        fi->V(1) = ivp[indices[i*3+1]];
        fi->V(2) = ivp[indices[i*3+2]];
        fi->Base().EnableVFAdjacency();


        if (hasFNormals) {
            fi->N() =
                    CMeshO::CoordType(faceNormals[i][0], faceNormals[i][1], faceNormals[i][2]);
        }
    }


    std::unordered_map<Edge, uint32_t, EdgeHash, EdgeEqual> edgeMap;
    std::vector<Edge> edges;

    for (int i = 0; i < indices.size()/3; ++i) {
        for (int e = 0; e < 3; ++e) {
            int v1 = indices[i*3 + e % 3];
            int v2 = indices[i*3 + (e + 1) % 3];
            Edge edge = std::make_pair(std::min(v1, v2), std::max(v1, v2));

            if (edgeMap.find(edge) == edgeMap.end()) {
                edgeMap[edge] = 0;
            }
        }
    }


    for (const auto &edge: edgeMap) {
        edges.push_back(edge.first);
    }

    CMeshO::EdgeIterator ei = vcg::tri::Allocator<CMeshO>::AddEdges(m, edges.size());
    for (unsigned int i = 0; i < edges.size(); ++i, ++ei)
    {
        ei->V(0) = ivp[edges[i].first];
        ei->V(1) = ivp[edges[i].second];
    }

    if (!hasFNormals) {
        vcg::tri::UpdateNormal<CMeshO>::PerFace(m);
    }
    if (!hasVNormals) {
        vcg::tri::UpdateNormal<CMeshO>::PerVertex(m);
    }


    return m;
}

void VCG_CMesh0_Helper::retrieveCMeshData(CMeshO &mesh, std::vector<uint32_t> &indices, std::vector<Point3D> &vertices, std::vector<Point3D> &faceNormals)
{

    vertices.resize(mesh.VN());
    indices.resize(mesh.FN()*3);
    faceNormals.resize(mesh.VN() * 3);

    for (int i = 0; i < mesh.VN(); i++) {
        Point3D v = {mesh.vert[i].P()[0], mesh.vert[i].P()[1],mesh.vert[i].P()[2]};
        vertices[i] = v;
    }

    for (int i = 0; i < mesh.FN(); i++) {
        for (int j = 0; j < 3; j++) {
            indices[i*3 + j] = (uint32_t) vcg::tri::Index(mesh, mesh.face[i].V(j));
        }
    }

    for (int i = 0; i < mesh.FN(); i++) {
        Point3D n = {mesh.face[i].N()[0], mesh.face[i].N()[1],mesh.face[i].N()[2]};
        faceNormals[i] = n;
    }
}