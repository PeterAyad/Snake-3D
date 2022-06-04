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
    const int UP = 0;
    const int DOWN = 1;
    const int LEFT = 2;
    const int RIGHT = 3;
    int speed = 8;

    std::map<int, int> cornersToBeRemoved;
    std::map<std::string, float> cornerRotations;
    std::vector<std::pair<glm::vec3, std::string>> cornerPositions;

    class UserMovementController
    {
        bool updateDirection = false;
        Application *app;
        std::chrono::_V2::system_clock::time_point lastTime;
        int lastDirection;
        State *state;

    public:
        void enter(Application *app,
                   State *state)
        {
            this->app = app;
            lastTime = std::chrono::high_resolution_clock::now();
            this->state = state;
            lastDirection = UP;

            cornerRotations["upright"] = 0.0f;
            cornerRotations["downright"] = 90.0f;
            cornerRotations["downleft"] = 180.0f;
            cornerRotations["upleft"] = 270.0f;
        }

        void update(World *world, float deltaTime)
        {
            /**
             * take user input
             * specify motion direction based on input
             * move the first part of the snake in this direction on step
             * change all the othe parts of the snake to follow the head where
             * each part becomes in the place of the previous one
             *
             */

            std::vector<Entity *> snakeParts;
            Entity *head = nullptr;
            Entity *tail = nullptr;

            for (auto entity : world->getEntities())
            {
                if (entity->name == "snake_head")
                {
                    head = entity;
                }
                else if (entity->name == "snake_tail")
                {
                    tail = entity;
                }
                else if (entity->name.rfind("snake", 0) == 0)
                {
                    snakeParts.push_back(entity);
                }
            }
            snakeParts.insert(snakeParts.begin(), head);
            snakeParts.push_back(tail);

            if (snakeParts.size() == 0)
            {
                return;
            }

            auto now = std::chrono::high_resolution_clock::now();

            /* Getting number of milliseconds as an integer. */
            int ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime).count();

            if (ms_int > (1000-speed*100))
            {
                updateDirection = true;
                int newDirection = lastDirection;
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

                updatePositions(newDirection, lastDirection, snakeParts);

                if (updateDirection)
                {
                    lastDirection = newDirection;
                }

                updateMesh(snakeParts);
                lastTime = now;
            }
        }

        void updatePositions(int newDirection, int lastDirection, std::vector<Entity *> snakeParts)
        {
            // std::cout << lastDirection << " ";
            // std::cout << newDirection << std::endl;

            for (int i = snakeParts.size() - 1; i > 0; i--)
            {
                snakeParts[i]->getComponent<MovementComponent>()->linearVelocity = snakeParts[i - 1]->getComponent<MovementComponent>()->linearVelocity;
                snakeParts[i]->localTransform.position = snakeParts[i - 1]->localTransform.position;
            }
            switch (lastDirection)
            {
            case UP:
                if (newDirection == UP || newDirection == DOWN)
                {
                    snakeParts[0]->getComponent<MovementComponent>()->linearVelocity = glm::vec3(0.0f, 0.0f, -1.0f);
                    snakeParts[0]->localTransform.position = snakeParts[0]->localTransform.position + glm::vec3(0.0f, 0.0f, -2.0f);
                    updateDirection = false;
                }
                else if (newDirection == LEFT)
                {
                    snakeParts[0]->getComponent<MovementComponent>()->linearVelocity = glm::vec3(-1.0f, 0.0f, 0.0f);
                    std::cout << "Corner Added" << std::endl;
                    addCorner(snakeParts[0]->localTransform.position, "upleft");
                    snakeParts[0]->localTransform.position = snakeParts[0]->localTransform.position + glm::vec3(-2.0f, 0.0f, 0.0f);
                }
                else if (newDirection == RIGHT)
                {
                    snakeParts[0]->getComponent<MovementComponent>()->linearVelocity = glm::vec3(1.0f, 0.0f, 0.0f);
                    std::cout << "Corner Added" << std::endl;
                    addCorner(snakeParts[0]->localTransform.position, "upright");
                    snakeParts[0]->localTransform.position = snakeParts[0]->localTransform.position + glm::vec3(2.0f, 0.0f, 0.0f);
                    snakeParts[0]->localTransform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
                }
                break;
            case DOWN:
                if (newDirection == UP || newDirection == DOWN)
                {
                    snakeParts[0]->getComponent<MovementComponent>()->linearVelocity = glm::vec3(0.0f, 0.0f, 1.0f);
                    snakeParts[0]->localTransform.position = snakeParts[0]->localTransform.position + glm::vec3(0.0f, 0.0f, 2.0f);
                    updateDirection = false;
                }
                else if (newDirection == LEFT)
                {
                    snakeParts[0]->getComponent<MovementComponent>()->linearVelocity = glm::vec3(-1.0f, 0.0f, 0.0f);
                    std::cout << "Corner Added" << std::endl;
                    addCorner(snakeParts[0]->localTransform.position, "downleft");
                    snakeParts[0]->localTransform.position = snakeParts[0]->localTransform.position + glm::vec3(-2.0f, 0.0f, 0.0f);
                }
                else if (newDirection == RIGHT)
                {
                    snakeParts[0]->getComponent<MovementComponent>()->linearVelocity = glm::vec3(1.0f, 0.0f, 0.0f);
                    std::cout << "Corner Added" << std::endl;
                    addCorner(snakeParts[0]->localTransform.position, "downright");
                    snakeParts[0]->localTransform.position = snakeParts[0]->localTransform.position + glm::vec3(2.0f, 0.0f, 0.0f);
                }
                break;
            case RIGHT:
                if (newDirection == LEFT || newDirection == RIGHT)
                {
                    snakeParts[0]->getComponent<MovementComponent>()->linearVelocity = glm::vec3(1.0f, 0.0f, 0.0f);
                    snakeParts[0]->localTransform.position = snakeParts[0]->localTransform.position + glm::vec3(2.0f, 0.0f, 0.0f);
                    updateDirection = false;
                }
                else if (newDirection == UP)
                {
                    snakeParts[0]->getComponent<MovementComponent>()->linearVelocity = glm::vec3(0.0f, 0.0f, -1.0f);
                    std::cout << "Corner Added" << std::endl;
                    addCorner(snakeParts[0]->localTransform.position, "downleft");
                    snakeParts[0]->localTransform.position = snakeParts[0]->localTransform.position + glm::vec3(0.0f, 0.0f, -2.0f);
                }
                else if (newDirection == DOWN)
                {
                    snakeParts[0]->getComponent<MovementComponent>()->linearVelocity = glm::vec3(0.0f, 0.0f, 1.0f);
                    std::cout << "Corner Added" << std::endl;
                    addCorner(snakeParts[0]->localTransform.position, "upleft");
                    snakeParts[0]->localTransform.position = snakeParts[0]->localTransform.position + glm::vec3(0.0f, 0.0f, 2.0f);
                }
                break;
            case LEFT:
                if (newDirection == LEFT || newDirection == RIGHT)
                {
                    snakeParts[0]->getComponent<MovementComponent>()->linearVelocity = glm::vec3(-1.0f, 0.0f, 0.0f);
                    snakeParts[0]->localTransform.position = snakeParts[0]->localTransform.position + glm::vec3(-2.0f, 0.0f, 0.0f);
                    updateDirection = false;
                }
                else if (newDirection == UP)
                {
                    snakeParts[0]->getComponent<MovementComponent>()->linearVelocity = glm::vec3(0.0f, 0.0f, -1.0f);
                    std::cout << "Corner Added" << std::endl;
                    addCorner(snakeParts[0]->localTransform.position, "downright");
                    snakeParts[0]->localTransform.position = snakeParts[0]->localTransform.position + glm::vec3(0.0f, 0.0f, -2.0f);
                }
                else if (newDirection == DOWN)
                {
                    snakeParts[0]->getComponent<MovementComponent>()->linearVelocity = glm::vec3(0.0f, 0.0f, 1.0f);
                    std::cout << "Corner Added" << std::endl;
                    addCorner(snakeParts[0]->localTransform.position, "upright");
                    snakeParts[0]->localTransform.position = snakeParts[0]->localTransform.position + glm::vec3(0.0f, 0.0f, 2.0f);
                }
                break;
            }
        }

        void addCorner(glm::vec3 position, std::string type)
        {
            for (std::pair<glm::vec3, std::string> i : cornerPositions)
            {
                if (i.first.x == position.x && i.first.y == position.y && i.first.z == position.z)
                {
                    i.second = type;
                    return;
                }
            }
            cornerPositions.push_back(std::make_pair(position, type));
        }

        void updateMesh(std::vector<Entity *> snakeParts)
        {
            // update mesh according to velocity
            for (int i = 1; i < snakeParts.size() - 1; i++)
            {
                snakeParts[i]->getComponent<MeshRendererComponent>()->setMesh("body");
                if (snakeParts[i]->getComponent<MovementComponent>()->linearVelocity == glm::vec3(0.0f, 0.0f, -1.0f))
                    snakeParts[i]->localTransform.rotation = glm::vec3(0.0f, glm::radians(90.0f), 0.0f);
                else if (snakeParts[i]->getComponent<MovementComponent>()->linearVelocity == glm::vec3(0.0f, 0.0f, 1.0f))
                    snakeParts[i]->localTransform.rotation = glm::vec3(0.0f, glm::radians(90.0f), 0.0f);
                else if (snakeParts[i]->getComponent<MovementComponent>()->linearVelocity == glm::vec3(-1.0f, 0.0f, 0.0f))
                    snakeParts[i]->localTransform.rotation = glm::vec3(0.0f, glm::radians(180.0f), 0.0f);
                else if (snakeParts[i]->getComponent<MovementComponent>()->linearVelocity == glm::vec3(1.0f, 0.0f, 0.0f))
                    snakeParts[i]->localTransform.rotation = glm::vec3(0.0f, glm::radians(180.0f), 0.0f);
            }

            // std::cout<<"cornerPositions size: "<<cornerPositions.size()<<std::endl;
            // update corner positions
            // if a corner is not found in snake remove it from the vector

            for (int i = 0; i < cornerPositions.size(); i++)
            {
                if (cornerPositions[i].first == snakeParts[0]->localTransform.position)
                {
                    cornerPositions.erase(cornerPositions.begin() + i);
                    break;
                }
            }

            for (int i = 0; i < cornerPositions.size(); i++)
            {
                for (int j = 1; j < snakeParts.size() - 1; j++)
                {
                    if (snakeParts[j]->localTransform.position == cornerPositions[i].first)
                    {
                        // Change its mesh
                        snakeParts[j]->getComponent<MeshRendererComponent>()->setMesh("curve");
                        snakeParts[j]->localTransform.rotation = glm::vec3(0.0f, glm::radians(cornerRotations[cornerPositions[i].second]), 0.0f);
                        break;
                    }
                }
                // if (cornersToBeRemoved.find(i) == cornersToBeRemoved.end())
                // {
                //     cornersToBeRemoved[i] = 1;
                // }
                // else
                // {
                //     cornersToBeRemoved[i]++;
                // }
            }

            // for (int i = cornerPositions.size() - 1; i >= 0; i--)
            // {
            //     if (cornersToBeRemoved.find(i) != cornersToBeRemoved.end() && cornersToBeRemoved[i] > ((snakeParts.size()-2)*1000))
            //     {
            //         auto it_del = cornersToBeRemoved.find(i);
            //         cornersToBeRemoved.erase(it_del);
            //         cornerPositions.erase(cornerPositions.begin() + i);
            //     }
            // }

            // update tail direction
            snakeParts[snakeParts.size() - 1]->getComponent<MeshRendererComponent>()->setMesh("tail");
            if (snakeParts[snakeParts.size() - 2]->getComponent<MovementComponent>()->linearVelocity == glm::vec3(0.0f, 0.0f, -1.0f))
                snakeParts[snakeParts.size() - 1]->localTransform.rotation = glm::vec3(0.0f, glm::radians(90.0f), 0.0f);
            else if (snakeParts[snakeParts.size() - 2]->getComponent<MovementComponent>()->linearVelocity == glm::vec3(0.0f, 0.0f, 1.0f))
                snakeParts[snakeParts.size() - 1]->localTransform.rotation = glm::vec3(0.0f, glm::radians(270.0f), 0.0f);
            else if (snakeParts[snakeParts.size() - 2]->getComponent<MovementComponent>()->linearVelocity == glm::vec3(-1.0f, 0.0f, 0.0f))
                snakeParts[snakeParts.size() - 1]->localTransform.rotation = glm::vec3(0.0f, glm::radians(180.0f), 0.0f);
            else if (snakeParts[snakeParts.size() - 2]->getComponent<MovementComponent>()->linearVelocity == glm::vec3(1.0f, 0.0f, 0.0f))
                snakeParts[snakeParts.size() - 1]->localTransform.rotation = glm::vec3(0.0f, glm::radians(0.0f), 0.0f);

            // update head direction
            snakeParts[0]->getComponent<MeshRendererComponent>()->setMesh("head");
            if (snakeParts[0]->getComponent<MovementComponent>()->linearVelocity == glm::vec3(0.0f, 0.0f, -1.0f))
                snakeParts[0]->localTransform.rotation = glm::vec3(0.0f, glm::radians(90.0f), 0.0f);
            else if (snakeParts[0]->getComponent<MovementComponent>()->linearVelocity == glm::vec3(0.0f, 0.0f, 1.0f))
                snakeParts[0]->localTransform.rotation = glm::vec3(0.0f, glm::radians(270.0f), 0.0f);
            else if (snakeParts[0]->getComponent<MovementComponent>()->linearVelocity == glm::vec3(-1.0f, 0.0f, 0.0f))
                snakeParts[0]->localTransform.rotation = glm::vec3(0.0f, glm::radians(180.0f), 0.0f);
            else if (snakeParts[0]->getComponent<MovementComponent>()->linearVelocity == glm::vec3(1.0f, 0.0f, 0.0f))
                snakeParts[0]->localTransform.rotation = glm::vec3(0.0f, glm::radians(0.0f), 0.0f);
        }
    };
}
