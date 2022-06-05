#include "entity.hpp"
#include "../deserialize-utils.hpp"
#include "../components/component-deserializer.hpp"
#include "world.hpp"

#include <glm/gtx/euler_angles.hpp>

namespace our
{

    // This function returns the transformation matrix from the entity's local space to the world space
    // Remember that you can get the transformation matrix from this entity to its parent from "localTransform"
    // To get the local to world matrix, you need to combine this entities matrix with its parent's matrix and
    // its parent's parent's matrix and so on till you reach the root.
    glm::mat4 Entity::getLocalToWorldMatrix() const
    {
        // TODO: (Req 7) Write this function
        /*
         *here I will check if:
         *   i have a parent entity then i will return my localTransform multiplied
         *   with  my parent's getLocalToWorldMatrix() function returned value
         *else:
         *   if will only return my localTransform matrix.
         *
         */
        if (this->parent != nullptr)
            return (this->parent)->getLocalToWorldMatrix() * this->localTransform.toMat4();

        return this->localTransform.toMat4();
    }

    // Deserializes the entity data and components from a json object
    void Entity::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object())
            return;
        name = data.value("name", name);
        localTransform.deserialize(data);
        if (data.contains("components"))
        {
            if (const auto &components = data["components"]; components.is_array())
            {
                for (auto &component : components)
                {
                    deserializeComponent(component, this);
                }
            }
        }
    }


    // This function returns the boundaries of the entity in world space
    // Assuming the function is a cube (for AABB collision)
    std::vector<glm::vec3> Entity::getBoundariesInWorldSpace(glm::ivec2 windowSize)
    {
        // 1. Get the camera entity
        CameraComponent *camera = nullptr;
        for (auto entity : world->getEntities())
        {
            // If we hadn't found a camera yet, we look for a camera in this entity
            if (!camera)
                camera = entity->getComponent<CameraComponent>();
        }

        // 2. Get the view projection matrix
        glm::mat4 VP = camera->getProjectionMatrix(windowSize) * camera->getViewMatrix();

        // 3. Get the extreme vertices of the entity in its local space
        std::vector<glm::vec3> boundaries = getComponent<MeshRendererComponent>()->mesh->boundaries;

        // 4. Transform the vertices to world space using MVP matrices
        for (int i = 0; i < boundaries.size(); i++)
        {
            boundaries[i] = glm::vec3(VP * getLocalToWorldMatrix() * glm::vec4(boundaries[i], 1.0f));
        }
        return boundaries;
    }

}