#pragma once

#include <application.hpp>

class ClearColorState: public our::State {
    // onInitialize() function is called once before the state starts
    void onInitialize() override {
        //TODO: Read the color from the configuration file and use it to set the clear color for the window
        //HINT: you can the configuration for the whole application using "getApp()->getConfig()"
        //To see how the clear color is written in the json files, see "config/blue-screen.json"
        //To know how read data from a nlohmann::json object, 
        //look at the following documentation: https://json.nlohmann.me/features/element_access/

        nlohmann::json clearColor = getApp()->getConfig().at("scene").at("clear-color");
        float r = clearColor["r"].get<float>();
        float g = clearColor["g"].get<float>();
        float b = clearColor["b"].get<float>();
        float a = clearColor["a"].get<float>();
        
        // set color you want to clear the buffers with
        glClearColor(r,g,b,a);
    }

    // onDraw(deltaTime) function is called every frame 
    void onDraw(double deltaTime) override {
        //At the start of frame we want to clear the screen. Otherwise we would still see the results from the previous frame.
        glClear(GL_COLOR_BUFFER_BIT);
    }
};