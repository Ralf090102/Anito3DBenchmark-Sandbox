#include "MeshEntity.hpp"
#include <iostream>

namespace Anito3D {
    bool MeshEntity::LoadMesh(const std::string& filePath) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filePath,
            aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
            return false;
        }

        // Process the first mesh (extend for multiple meshes if needed)
        if (scene->mNumMeshes > 0) {
            ProcessMesh(scene->mMeshes[0], scene);
        }

        return true;
    }

    void MeshEntity::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
        meshData.Clear();

        // Load vertices, normals, and texture coordinates
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            meshData.vertices.emplace_back(
                mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            if (mesh->mNormals) {
                meshData.normals.emplace_back(
                    mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            }
            if (mesh->mTextureCoords[0]) {
                meshData.texCoords.emplace_back(
                    mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            }
        }

        // Load indices
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                meshData.indices.push_back(face.mIndices[j]);
            }
        }

        // Load basic PBR material properties (simplified)
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            aiColor3D color;
            if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
                meshData.material.albedo = glm::vec3(color.r, color.g, color.b);
            }
            // Add metallic/roughness loading if needed
        }
    }
}