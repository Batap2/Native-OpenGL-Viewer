#include <iostream>
#include <fstream>
#include <vector>


#ifndef DECIMATIONATTEMPT2_ASSIMPHELPER_H
#define DECIMATIONATTEMPT2_ASSIMPHELPER_H

namespace ObjIO{
    static void writeObj(const std::vector<uint32_t>& indices, const std::vector<Point3D>& vertices, const char* outputPath) {
        std::ofstream objFile(outputPath);

        if (!objFile.is_open()) {
            std::cerr << "Error: Unable to open output file " << outputPath << std::endl;
            return;
        }

        // Write vertices
        for (const Point3D& vertex : vertices) {
            objFile << "v " << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
        }

        // Write indices
        for (size_t i = 0; i < indices.size(); i += 3) {
            objFile << "f " << indices[i] + 1 << " " << indices[i + 1] + 1 << " " << indices[i + 2] + 1 << "\n";
        }

        objFile.close();
        std::cout << "Successfully wrote .obj file: " << outputPath << std::endl;
    }

    static void readObj(const char* inputPath, std::vector<uint32_t>& indices, std::vector<Point3D>& vertices) {
        std::ifstream file(inputPath);
        if (!file.is_open()) {
            std::cerr << "Error: Couldn't open the file " << inputPath << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string token;
            iss >> token;

            if (token == "v") {
                Point3D vertex;
                iss >> vertex.x >> vertex.y >> vertex.z;
                vertices.push_back(vertex);
            } else if (token == "f") {
                std::string faceToken;
                while (iss >> faceToken) {
                    size_t pos = faceToken.find('/');
                    uint32_t index = std::stoi(faceToken.substr(0, pos)) - 1;
                    indices.push_back(index);
                }
            }
        }

        file.close();
    }
}

#endif //DECIMATIONATTEMPT2_ASSIMPHELPER_H
