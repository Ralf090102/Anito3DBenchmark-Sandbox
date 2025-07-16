#pragma once
#include <vector>
#include <glm/glm.hpp>

namespace Anito3D {
    struct MeshData {
        std::vector<glm::vec3> vertices;  // Vertex positions
        std::vector<glm::vec3> normals;   // Vertex normals
        std::vector<glm::vec2> texCoords; // Texture coordinates
        std::vector<uint32_t> indices;     // Triangle indices

        // Add material properties for PBR (simplified)
        struct Material {
            glm::vec3 albedo{ 1.0f };    // Base color
            float metallic{ 0.0f };      // Metallic factor
            float roughness{ 0.5f };     // Roughness factor
        } material;

        MeshData() = default;
        void Clear() {
            vertices.clear();
            normals.clear();
            texCoords.clear();
            indices.clear();
        }
    };
}