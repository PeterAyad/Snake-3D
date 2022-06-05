#pragma once

#include <asset-loader.hpp>
#include <ecs/transform.hpp>
#include <application.hpp>
#include <deserialize-utils.hpp>

#include <vector>
#include <glm/gtc/matrix_transform.hpp> 

// This state shows how to use the Material class.
// It also shows how to use the AssetLoader to load assets
class LightTestState: public our::State {

    our::Material* material;
    our::Mesh* mesh;
    std::vector<our::Transform> transforms;
    glm::mat4 VP;
    glm::mat4 camera;
    glm::vec3 cameraeye;

   glm::mat4 getVP (){
            glm::mat4 iVP = VP;
            auto& config = getApp()->getConfig()["scene"];
            if(config.contains("camera")){
            if(auto& camera = config["camera"]; camera.is_object()){
                float angle = (float)glfwGetTime();
                glm::vec3 eye = glm::vec3(2*glm::sin(angle), 1, 2*glm::cos(angle));//camera.value("eye", glm::vec3(0, 0, 0));
                cameraeye = eye;
                glm::vec3 center = camera.value("center", glm::vec3(0, 0, -1));
                glm::vec3 up = camera.value("up", glm::vec3(0, 1, 0));
                glm::mat4 V = glm::lookAt(eye, center, up);

                float fov = glm::radians(camera.value("fov", 90.0f));
                float near = camera.value("near", 0.01f);
                float far = camera.value("far", 1000.0f);

                glm::ivec2 size = getApp()->getFrameBufferSize();
                float aspect = float(size.x)/size.y;
                glm::mat4 P = glm::perspective(fov, aspect, near, far);

                iVP = P * V;
            }
    }
    return iVP;
    }
    void onInitialize() override {
        // First of all, we get the scene configuration from the app config
        auto& config = getApp()->getConfig()["scene"];

        // If we have assets in the scene config, we deserialize them
        if(config.contains("assets")){
            our::deserializeAllAssets(config["assets"]);
        }
        // We get the mesh and the material from AssetLoader 
        mesh = our::AssetLoader<our::Mesh>::get("mesh");
        material = our::AssetLoader<our::Material>::get("material");

        // Then we read a list of transform objects from the shader
        // In draw, we will render a mesh for each of the transforms
        transforms.clear();
        if(config.contains("objects")){
            if(auto& objects = config["objects"]; objects.is_array()){
                for(auto& object : objects){
                    our::Transform transform;
                    transform.deserialize(object);
                    transforms.push_back(transform);
                }
            }
        }
        // Then we read the camera information to compute the VP matrix
        VP = getVP();
        
    }

    void onDraw(double deltaTime) override {
        glClearColor(0.2f, 0.4f, 0.6f, 1.0f);
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // The material setup will use the shader, setup the pipeline state
        // and send the uniforms that are common between objects using the same material
        material->setup();
        for(auto& transform : transforms){
            // For each transform, we compute the MVP matrix and send it to the "transform" uniform
            //material->shader->set("transform", VP * transform.toMat4());
            material->shader->set("eye",cameraeye);
            material->shader->set("M", transform.toMat4());
            material->shader->set("VP", getVP());
            material->shader->set("MIT", glm::transpose(glm::inverse(transform.toMat4())));     
            material->shader->set("sky.top",glm::vec3(0.3, 0.6, 1.0));
            material->shader->set("sky.middle",glm::vec3(0.3, 0.3, 0.3));
            material->shader->set("sky.bottom",glm::vec3(0.1, 0.1, 0.0));
            material->shader->set("light_count", 2);
        
        material->shader->set("lights[0].type", 2);
        material->shader->set( "lights[0].position", glm::vec3(2, 0, 0));
        material->shader->set("lights[0].direction", glm::vec3(-1, 0, 0));
        material->shader->set( "lights[0].diffuse", glm::vec3(1, 0.9, 0.7));
        material->shader->set( "lights[0].specular", glm::vec3(1, 0.9, 0.7));
       material->shader->set( "lights[0].attenuation", glm::vec3(1, 0, 0));
        material->shader->set("cone_angles", glm::vec2(glm::radians(10.0f), glm::radians(11.0f)));

        material->shader->set( "lights[1].type", 1);
        material->shader->set( "lights[1].position", glm::vec3(0, 1.5f, 0));
       material->shader->set( "lights[1].diffuse", glm::vec3(1, 0.2, 0.1));
        material->shader->set( "lights[1].specular", glm::vec3(1, 0.2, 0.1));
        material->shader->set("lights[1].attenuation", glm::vec3(1, 0, 0));


            // Then we draw a mesh instance
            mesh->draw();
        }
    }

    void onDestroy() override {
        our::clearAllAssets();
    }

 
};