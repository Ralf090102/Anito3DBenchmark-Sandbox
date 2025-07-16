#pragma once

#include <glm/glm.hpp>

namespace Anito3D {
    class Entity {
    public:
        Entity() : position(0.0f), rotation(0.0f), scale(1.0f) {}
        virtual ~Entity() = default;

        // Getters/Setters for transform
        glm::vec3 GetPosition() const { return position; }
        void SetPosition(const glm::vec3& pos) { position = pos; }
        glm::vec3 GetRotation() const { return rotation; }
        void SetRotation(const glm::vec3& rot) { rotation = rot; }
        glm::vec3 GetScale() const { return scale; }
        void SetScale(const glm::vec3& scl) { scale = scl; }

        virtual void Update(float deltaTime) {}

    protected:
        glm::vec3 position; // Position in 3D space
        glm::vec3 rotation; // Euler angles (degrees)
        glm::vec3 scale;    // Scale factor
    };
}