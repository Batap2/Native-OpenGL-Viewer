#ifndef VCG_CMESH0_HELPER_H
#define VCG_CMESH0_HELPER_H

#include <vector>
#include "cmesh.h"
#include "../../src/Point3D.h"
#include "../../src/Point3D.inl.h"
#include <unordered_map>

struct VCG_CMesh0_Helper {

    typedef std::pair<unsigned int, unsigned int> Edge;

    // For edgeMap
    struct EdgeHash {
        template<class T1, class T2>
        std::size_t operator()(const std::pair<T1, T2> &p) const {
            auto h1 = std::hash<T1>{}(p.first);
            auto h2 = std::hash<T2>{}(p.second);
            return h1 ^ h2;
        }
    };

    struct EdgeEqual {
        template<class T1, class T2>
        bool operator()(const std::pair<T1, T2> &lhs, const std::pair<T1, T2> &rhs) const {
            return lhs.first == rhs.first && lhs.second == rhs.second;
        }
    };

    static CMeshO constructCMesh(const std::vector<uint32_t> &indices, const std::vector<Point3D> &vertices, const std::vector<Point3D> &normals);

    static void retrieveCMeshData(CMeshO &mesh, std::vector<uint32_t> &indices, std::vector<Point3D> &vertices, std::vector<Point3D> &normals);
};


#endif //VCG_CMESH0_HELPER_H
