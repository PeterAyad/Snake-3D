#pragma once

#include <application.hpp>

#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/movement.hpp>
#include <systems/user-movement-controller.hpp>
#include <systems/collision-handler.hpp>
#include <asset-loader.hpp>


// This state shows how to use the ECS framework and deserialization.
class Playstate : public our::State
{
    our::World world;
    our::ForwardRenderer renderer;
    our::FreeCameraControllerSystem cameraController;
    our::UserMovementController userMovementController;
    our::CollisionHandler collisionHandler;
    bool gameOver = false;

    void onInitialize() override
    {
        // First of all, we get the scene configuration from the app config
        auto &config = getApp()->getConfig()["scene"];
        // If we have assets in the scene config, we deserialize them
        if (config.contains("assets"))
        {
            our::deserializeAllAssets(config["assets"]);
        }
        // If we have a world in the scene config, we use it to populate our world
        if (config.contains("world"))
        {
            world.deserialize(config["world"]);
        }
        // We initialize the camera controller system since it needs a pointer to the app
        cameraController.enter(getApp());
        // We initialize the collision handler system since it needs a pointer to the app, the state and a pointer to the game over flag
        collisionHandler.enter(this, getApp(), gameOver);
        // We initialize the user movement controller system since it needs a pointer to the app, the state and a pointer to the game over flag
        userMovementController.enter(this, getApp(), gameOver);
        // Then we initialize the renderer and set the sky element
        auto size = getApp()->getFrameBufferSize();
        renderer.initialize(size, config["renderer"]);
        renderer.deserialize(config["worldSky"]);
    }

    void onDraw(double deltaTime) override
    {
        // Here, we just run a bunch of systems to control the world logic
        userMovementController.update(&world, deltaTime);
        collisionHandler.update(&world, deltaTime);
        cameraController.update(&world, (float)deltaTime);
        // And finally we use the renderer system to draw the scene
        renderer.render(&world, gameOver);
    }

    void onDestroy() override
    {
        // Don't forget to destroy the renderer
        renderer.destroy();
        // On exit, we call exit for the camera controller system to make sure that the mouse is unlocked
        cameraController.exit();
        // and we delete all the loaded assets to free memory on the RAM and the VRAM
        our::clearAllAssets();
    }
};