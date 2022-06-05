#pragma once

#include <iostream>

#include "../ecs/world.hpp"
#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/ext.hpp>
#include <iostream>

#include <chrono>
#include <map>

namespace our
{
    // Define constants for the four directions of motion
    const int UP = 0;
    const int DOWN = 1;
    const int LEFT = 2;
    const int RIGHT = 3;

    class UserMovementController
    {
        //  Define vectors that holds corners, their ages and their directions
        //  Corners are the point of intersection of two directions of motion
        std::map<std::string, float> cornerRotations;
        std::vector<std::pair<glm::vec3, std::string>> cornerPositions;
        std::vector<int> cornersAge;

        // This variable is used to determine if the direction of motion is allowed to change or not
        bool updateDirection = false;
        Application *app;                                    // Pointer to the application
        std::chrono::_V2::system_clock::time_point lastTime; // Last time the user moved
        int lastDirection;                                   // Last direction the user moved
        State *state;                                        // Pointer to the state of the application
        bool *game_over;                                     // Pointer to the game over flag
        int speed;                                           // Speed of the snake

    public:
        void enter(State *state, Application *app,
                   bool &gameOver)
        {
            speed = 9;                                            // Set the speed of the snake
            this->app = app;                                      // Set the application pointer
            lastTime = std::chrono::high_resolution_clock::now(); // Set the last time the user moved as now
            this->state = state;                                  // Set the state pointer
            lastDirection = UP;                                   // Set the last direction the user moved as up (as this initial motion direction)
            this->game_over = &gameOver;                          // Set the game over flag pointer

            // Define the required rotation of snake corner object according to the direction of corner
            cornerRotations["upright"] = 0.0f;
            cornerRotations["downright"] = 90.0f;
            cornerRotations["downleft"] = 180.0f;
            cornerRotations["upleft"] = 270.0f;
        }

        void update(World *world, float deltaTime)
        {
            // Summary to what the function does:
            // take user input
            // specify motion direction based on input
            // move the first part of the snake in this direction on step
            // change all the othe parts of the snake to follow the head where
            // each part becomes in the place of the previous one

            // Find the snake parts (Entities)
            std::vector<Entity *> snakeParts;
            Entity *head = nullptr;
            Entity *tail = nullptr;

            for (auto entity : world->getEntities())
            {
                if (entity->name == "snake_head")
                    head = entity;
                else if (entity->name == "snake_tail")
                    tail = entity;
                else if (entity->name.rfind("snake_part", 0) == 0)
                    snakeParts.push_back(entity);
            }

            // If non was found, return
            if (snakeParts.size() == 0)
                return;

            // Sort the snake parts by their name (they are sorted by their order in the snake)
            std::sort(snakeParts.begin(), snakeParts.end(), [](const Entity *a, const Entity *b)
                      {   std::string strA= a->name.substr(10);
                          std::string strB= b->name.substr(10);
                          if(strB.empty())
                              return true;
                          if(strA.empty())
                              return false;
                          else
                              return ((std::stoi(strA))>(std::stoi(strB))); });

            // Insert the head to the beginning of the vector
            snakeParts.insert(snakeParts.begin(), head);
            // Insert the tail to the end of the vector
            snakeParts.push_back(tail);

            // Calculate the time difference between the last time the user moved and now
            auto now = std::chrono::high_resolution_clock::now();
            int ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime).count();

            // If the time difference is greater close to the speed of the snake and the game is not over, then update the direction of motion
            if (ms_int > (1000 - speed * 100) && !(*game_over))
            {

                // Initially assume the direction of motion is allowed to change
                updateDirection = true;

                // If no user input is detected, then set the direction of motion to the last direction the user moved
                int newDirection = lastDirection;

                // If user gave an input, then set the direction of motion to the input
                if (app->getKeyboard().isPressed(GLFW_KEY_UP))
                {
                    newDirection = UP;
                }
                else if (app->getKeyboard().isPressed(GLFW_KEY_DOWN))
                {
                    newDirection = DOWN;
                }
                else if (app->getKeyboard().isPressed(GLFW_KEY_LEFT))
                {
                    newDirection = LEFT;
                }
                else if (app->getKeyboard().isPressed(GLFW_KEY_RIGHT))
                {
                    newDirection = RIGHT;
                }

                // Update the positions of snake parts according to the direction of motion
                updatePositions(newDirection, lastDirection, snakeParts);
                // Update the meshes of snake parts according to the direction of motion
                updateMesh(snakeParts);
                // Remove the oldest corners
                removeOldCorners(snakeParts);

                // If the direction of motion is allowed to change, then update the direction of motion
                if (updateDirection)
                    lastDirection = newDirection;
                lastTime = now;
            }
        }

        // This function guarantees infinite plane
        glm::vec3 checkPosition(glm::vec3 position)
        {
            float x = position.x;
            if (x > 29.0f)
                x = -29.0f;
            else if (x < -29.0f)
                x = 29.0f;
            else
                x = x;
            float y = -10.0f;
            float z = position.z;
            if (z > -2.0f)
                z = -35.0f;
            else if(z<-35.0f)
                z = -2.0f;
            else
                z = z;
            return glm::vec3(x, y, z);
        }

        // This function updates the corners ages and remove the oldest ones assuming they are no longer needed
        void removeOldCorners(std::vector<Entity *> snakeParts)
        {
            // Update the ages of the corners
            for (int i = 0; i < cornersAge.size(); i++)
                cornersAge[i]++;
            // Remove the corners that are older than the maximum age
            for (int i = cornersAge.size() - 1; i >= 0; i--)
            {
                if (cornersAge[i] > (snakeParts.size() * 3))
                {
                    cornerPositions.erase(cornerPositions.begin() + i);
                    cornersAge.erase(cornersAge.begin() + i);
                }
            }
        }

        // This function updates the position, direction of the snake head according to the direction of motion
        // It also add corners if needed
        void updateSnakeHeadChanges(int newDirection, int lastDirection, std::vector<Entity *> snakeParts, Entity *snakeHead, glm::vec3 linearVelocity, glm::vec3 positionChange, std::string cornerDirection = "")
        {
            snakeHead->getComponent<MovementComponent>()->linearVelocity = linearVelocity;
            if (cornerDirection != "")
                addCorner(snakeHead->localTransform.position, cornerDirection);
            snakeHead->localTransform.position = checkPosition(snakeHead->localTransform.position + positionChange);
        }

        // This function updates the positions of the snake parts according to the direction of motion (direction of motion is stored in the linearVelocity variable of MovementComponent)
        void updatePositions(int newDirection, int lastDirection, std::vector<Entity *> snakeParts)
        {
            // For all snake parts except the head, each part takes the position and direction of the previous one
            for (int i = snakeParts.size() - 1; i > 0; i--)
            {
                // Get the direction of the previous part
                snakeParts[i]->getComponent<MovementComponent>()->linearVelocity = snakeParts[i - 1]->getComponent<MovementComponent>()->linearVelocity;
                // Get the position of the previous part
                snakeParts[i]->localTransform.position = snakeParts[i - 1]->localTransform.position;
            }

            // Updating the direction and position of the head depends on the last direction and the new direction
            // If the new direction is the same as the last direction, then the head moves in the same direction
            // If the new direction is opposite to the last direction, then the head moves in the same direction (neglect the new direction) and don't update the direction of motion
            // If the new direction is orthogonal to the last direction, then the head moves in the new direction and update the direction of motion AND add a corner in this postion with the properties of this turn
            switch (lastDirection)
            {
            case UP:
                if (newDirection == UP || newDirection == DOWN)
                {
                    updateSnakeHeadChanges(newDirection, lastDirection, snakeParts, snakeParts[0], glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
                    updateDirection = false;
                }
                else if (newDirection == LEFT)
                    updateSnakeHeadChanges(newDirection, lastDirection, snakeParts, snakeParts[0], glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-2.0f, 0.0f, 0.0f), "upleft");
                else if (newDirection == RIGHT)
                    updateSnakeHeadChanges(newDirection, lastDirection, snakeParts, snakeParts[0], glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f), "upright");
                break;
            case DOWN:
                if (newDirection == UP || newDirection == DOWN)
                {
                    updateSnakeHeadChanges(newDirection, lastDirection, snakeParts, snakeParts[0], glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 2.0f));
                    updateDirection = false;
                }
                else if (newDirection == LEFT)
                    updateSnakeHeadChanges(newDirection, lastDirection, snakeParts, snakeParts[0], glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-2.0f, 0.0f, 0.0f), "downleft");
                else if (newDirection == RIGHT)
                    updateSnakeHeadChanges(newDirection, lastDirection, snakeParts, snakeParts[0], glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f), "downright");
                break;
            case RIGHT:
                if (newDirection == LEFT || newDirection == RIGHT)
                {
                    updateSnakeHeadChanges(newDirection, lastDirection, snakeParts, snakeParts[0], glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f));
                    updateDirection = false;
                }
                else if (newDirection == UP)
                    updateSnakeHeadChanges(newDirection, lastDirection, snakeParts, snakeParts[0], glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -2.0f), "downleft");
                else if (newDirection == DOWN)
                    updateSnakeHeadChanges(newDirection, lastDirection, snakeParts, snakeParts[0], glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 2.0f), "upleft");
                break;
            case LEFT:
                if (newDirection == LEFT || newDirection == RIGHT)
                {
                    updateSnakeHeadChanges(newDirection, lastDirection, snakeParts, snakeParts[0], glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-2.0f, 0.0f, 0.0f));
                    updateDirection = false;
                }
                else if (newDirection == UP)
                    updateSnakeHeadChanges(newDirection, lastDirection, snakeParts, snakeParts[0], glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -2.0f), "downright");
                else if (newDirection == DOWN)
                    updateSnakeHeadChanges(newDirection, lastDirection, snakeParts, snakeParts[0], glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 2.0f), "upright");
                break;
            }
        }

        // This function adds corner to the cornerPositions vector
        // and sets its age to 0
        void addCorner(glm::vec3 position, std::string type)
        {
            cornerPositions.push_back(std::make_pair(position, type));
            cornersAge.push_back(0);
        }

        // This function updates all snake parts mesh according to direction of motion (linearVelocity)
        void updateMesh(std::vector<Entity *> snakeParts)
        {
            // For all the parts of the snake except the head and the tail, update the mesh according to the direction of motion (stored in linearVelocity)
            for (int i = 1; i < snakeParts.size() - 1; i++)
            {
                glm::vec3 linearVelocity = snakeParts[i]->getComponent<MovementComponent>()->linearVelocity;
                snakeParts[i]->getComponent<MeshRendererComponent>()->setMesh("body");
                if (linearVelocity == glm::vec3(0.0f, 0.0f, -1.0f))
                    snakeParts[i]->localTransform.rotation = glm::vec3(0.0f, glm::radians(90.0f), 0.0f);
                else if (linearVelocity == glm::vec3(0.0f, 0.0f, 1.0f))
                    snakeParts[i]->localTransform.rotation = glm::vec3(0.0f, glm::radians(90.0f), 0.0f);
                else if (linearVelocity == glm::vec3(-1.0f, 0.0f, 0.0f))
                    snakeParts[i]->localTransform.rotation = glm::vec3(0.0f, glm::radians(180.0f), 0.0f);
                else if (linearVelocity == glm::vec3(1.0f, 0.0f, 0.0f))
                    snakeParts[i]->localTransform.rotation = glm::vec3(0.0f, glm::radians(180.0f), 0.0f);
            }

            // If The snake's head is in the position of a corner, delete this corner (as it is an old corner) on condition that this corner is not the last one
            if (cornerPositions.size() > 2)
                for (int i = 0; i < cornerPositions.size() - 1; i++)
                {
                    if (cornerPositions[i].first == snakeParts[0]->localTransform.position)
                    {
                        cornerPositions.erase(cornerPositions.begin() + i);
                        break;
                    }
                }

            // If any of these parts is in the position of a corner, update its mesh to be a corner
            for (int i = 0; i < cornerPositions.size(); i++)
            {
                for (int j = 1; j < snakeParts.size() - 1; j++)
                {
                    if (snakeParts[j]->localTransform.position == cornerPositions[i].first)
                    {
                        // Change its mesh
                        snakeParts[j]->getComponent<MeshRendererComponent>()->setMesh("curve");
                        // And change its rotation according to the type of corner
                        snakeParts[j]->localTransform.rotation = glm::vec3(0.0f, glm::radians(cornerRotations[cornerPositions[i].second]), 0.0f);
                        break;
                    }
                }
            }

            // Update tail direction to be in the same direction of the last part of snakes body
            updateTailRotation(snakeParts[snakeParts.size() - 1], snakeParts[snakeParts.size() - 2]);
            // Update head rotation according to direction of motion (linearVelocity)
            updateHeadRotation(snakeParts[0]);
        }

        // Update tail direction to be in the same direction of the last part of snakes body
        void updateTailRotation(Entity *snakeTail, Entity *beforeTail)
        {
            snakeTail->getComponent<MeshRendererComponent>()->setMesh("tail");

            glm::vec3 linearVelocity = beforeTail->getComponent<MovementComponent>()->linearVelocity;
            if (linearVelocity == glm::vec3(0.0f, 0.0f, -1.0f))
                snakeTail->localTransform.rotation = glm::vec3(0.0f, glm::radians(90.0f), 0.0f);
            else if (linearVelocity == glm::vec3(0.0f, 0.0f, 1.0f))
                snakeTail->localTransform.rotation = glm::vec3(0.0f, glm::radians(270.0f), 0.0f);
            else if (linearVelocity == glm::vec3(-1.0f, 0.0f, 0.0f))
                snakeTail->localTransform.rotation = glm::vec3(0.0f, glm::radians(180.0f), 0.0f);
            else if (linearVelocity == glm::vec3(1.0f, 0.0f, 0.0f))
                snakeTail->localTransform.rotation = glm::vec3(0.0f, glm::radians(0.0f), 0.0f);
        }

        // Update head rotation according to direction of motion (linearVelocity)
        void updateHeadRotation(Entity *snakeHead)
        {
            snakeHead->getComponent<MeshRendererComponent>()->setMesh("head");
            glm::vec3 linearVelocity = snakeHead->getComponent<MovementComponent>()->linearVelocity;
            if (linearVelocity == glm::vec3(0.0f, 0.0f, -1.0f))
                snakeHead->localTransform.rotation = glm::vec3(0.0f, glm::radians(90.0f), 0.0f);
            else if (linearVelocity == glm::vec3(0.0f, 0.0f, 1.0f))
                snakeHead->localTransform.rotation = glm::vec3(0.0f, glm::radians(270.0f), 0.0f);
            else if (linearVelocity == glm::vec3(-1.0f, 0.0f, 0.0f))
                snakeHead->localTransform.rotation = glm::vec3(0.0f, glm::radians(180.0f), 0.0f);
            else if (linearVelocity == glm::vec3(1.0f, 0.0f, 0.0f))
                snakeHead->localTransform.rotation = glm::vec3(0.0f, glm::radians(0.0f), 0.0f);
        }
    };
}
