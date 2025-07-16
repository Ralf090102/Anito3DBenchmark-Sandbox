#pragma once

#include "Entity.hpp"
#include "MeshData.hpp"
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Anito3D {
    class MeshEntity : public Entity {
    public:
        MeshEntity() = default;
        ~MeshEntity() override = default;

        bool LoadMesh(const std::string& filePath);

        const MeshData& GetMeshData() const { return meshData; }

    private:
        MeshData meshData;

        void ProcessMesh(aiMesh* mesh, const aiScene* scene);
    };
}