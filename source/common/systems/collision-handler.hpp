#pragma once

#include "../ecs/world.hpp"
#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <iostream>
#include <random>

namespace our
{
    class CollisionHandler
    {
        int score = 0;
        State *state;
        Application *app;

    public:
        void enter(State *state, Application *app)
        {
            this->state = state;
            this->app = app;
        }

        void update(World *world, float deltaTime)
        {
            Entity *snake = nullptr;
            Entity *apple = nullptr;
            for (auto entity : world->getEntities())
            {
                if (entity->name == "snake_head")
                {
                    snake = entity;
                }
                else if (entity->name == "apple")
                {
                    apple = entity;
                }
                if (snake != nullptr && apple != nullptr)
                    break;
            }

            if (snake == nullptr || apple == nullptr)
                return;

            std::vector<glm::vec3> snakeBoundaries = snake->getBoundariesInWorldSpace(app->getWindowSize());
            std::vector<glm::vec3> appleBoundaries = apple->getBoundariesInWorldSpace(app->getWindowSize());

            if (checkCollision(snakeBoundaries, appleBoundaries))
            {
                checkApplePosition(apple);
                addSnakePart(snake);
                updateScore(world);
            }
        }

        void updateScore(World *world)
        {
            score++;
            Entity *number1 = nullptr;
            Entity *number2 = nullptr;
            // Entity *camera = nullptr;
            // for (Entity *entity : world->getEntities())
            // {
            //     if (entity->getComponent<CameraComponent>())
            //         camera = entity;
            //     if (camera)
            //         break;
            // }
            for (Entity *entity : world->getEntities())
            {
                if (entity->name == "number1")
                    number1 = entity;
                if (entity->name == "number2")
                    number2 = entity;
                if (number1 && number2)
                    break;
            }

            number1->getComponent<MeshRendererComponent>()->setMesh("num" + std::to_string(score / 10));
            number2->getComponent<MeshRendererComponent>()->setMesh("num" + std::to_string(score % 10));
        }

        void checkApplePosition(Entity *apple)
        {
            srand(static_cast<unsigned>(time(0)));
            float x = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * -100;
            while (x < -30.0f || x > 30.0f)
            {
                std::cout << "x: " << x << std::endl;
                x = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * -100;
            }
            float y = -11.0f;
            float z = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * -100;
            while (z < -30.0f || z > -5.0f)
            {
                std::cout << "z: " << z << std::endl;
                z = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * -100;
            }

            apple->localTransform.position = glm::vec3(x, y, z);
        }

        void addSnakePart(Entity *snakeHead)
        {
            std::cout << "Snake ate an apple" << std::endl;
            our::Entity *newpart = snakeHead->getWorld()->add();
            newpart->name = "snake_part";
            newpart->localTransform = snakeHead->localTransform;

            newpart->addComponent<MeshRendererComponent>();
            newpart->getComponent<MeshRendererComponent>()->setMesh("body");
            newpart->getComponent<MeshRendererComponent>()->setMaterial("skin");

            newpart->addComponent<MovementComponent>();
            newpart->getComponent<MovementComponent>()->linearVelocity = snakeHead->getComponent<MovementComponent>()->linearVelocity;

            // update head direction
            if (snakeHead->getComponent<MovementComponent>()->linearVelocity == glm::vec3(0.0f, 0.0f, -1.0f))
                snakeHead->localTransform.position += glm::vec3(0.0f, 0.0f, -2.0f);
            else if (snakeHead->getComponent<MovementComponent>()->linearVelocity == glm::vec3(0.0f, 0.0f, 1.0f))
                snakeHead->localTransform.position += glm::vec3(0.0f, 0.0f, 2.0f);
            else if (snakeHead->getComponent<MovementComponent>()->linearVelocity == glm::vec3(-1.0f, 0.0f, 0.0f))
                snakeHead->localTransform.position += glm::vec3(-2.0f, 0.0f, 0.0f);
            else if (snakeHead->getComponent<MovementComponent>()->linearVelocity == glm::vec3(1.0f, 0.0f, 0.0f))
                snakeHead->localTransform.position += glm::vec3(2.0f, 0.0f, 0.0f);
        }

        void checkMinMax(float &min, float &max)
        {
            if (min > max)
                std::swap(min, max);
        }

        bool checkCollision(std::vector<glm::vec3> object1Boundaries, std::vector<glm::vec3> object2Boundaries)
        {
            float min_x1 = object1Boundaries[0].x;
            float min_y1 = object1Boundaries[1].y;
            float min_z1 = object1Boundaries[2].z;
            float max_x1 = object1Boundaries[3].x;
            float max_y1 = object1Boundaries[4].y;
            float max_z1 = object1Boundaries[5].z;

            float min_x2 = object2Boundaries[0].x;
            float min_y2 = object2Boundaries[1].y;
            float min_z2 = object2Boundaries[2].z;
            float max_x2 = object2Boundaries[3].x;
            float max_y2 = object2Boundaries[4].y;
            float max_z2 = object2Boundaries[5].z;

            checkMinMax(min_x1, max_x1);
            checkMinMax(min_y1, max_y1);
            checkMinMax(min_z1, max_z1);
            checkMinMax(min_x2, max_x2);
            checkMinMax(min_y2, max_y2);
            checkMinMax(min_z2, max_z2);

            if (min_x1 > max_x2)
                return false;
            if (max_x1 < min_x2)
                return false;
            if (min_y1 > max_y2)
                return false;
            if (max_y1 < min_y2)
                return false;
            if (min_z1 > max_z2)
                return false;
            if (max_z1 < min_z2)
                return false;
            return true;
        }
    };
}
