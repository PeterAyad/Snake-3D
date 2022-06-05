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
    // This class detects if collision happened between different parts of the game if so it handles the consequences of the collision
    class CollisionHandler
    {
        int score;        // The score of the player
        State *state;     // Pointer to play-state
        Application *app; // Pointer to the application
        bool *game_over;  // Pointer to the game_over boolean in the play-state

    public:
        void enter(State *state, Application *app, bool &gameOver)
        {
            // Initialize the score to 0
            score = 0;
            // Initialize the pointers to the state, the application and the game_over boolean
            this->state = state;
            this->app = app;
            this->game_over = &gameOver;
        }

        void update(World *world, float deltaTime)
        {
            // Detect if snake head collides with an apple (snake ate an apple)
            handleSnakeEatingApple(world);
            
            // Detect if snake head collides with itself (snake ate itself)
            handleSnakeEatingSelf(world);
        }

        // Detect if snake head collides with itself (snake ate itself)
        void
        handleSnakeEatingSelf(World *world)
        {
            // Get all the snake parts (entities)
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
                else if (entity->name.rfind("snake_part", 0) == 0)
                {
                    snakeParts.push_back(entity);
                }
            }

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

            // Add the tail to the end of the snake
            snakeParts.push_back(tail);

            // Get the boundaries of the snake's head
            std::vector<glm::vec3> headBoundaries = head->getBoundariesInWorldSpace(app->getWindowSize());

            // Loop on each part of the snake and check if it collides with the head
            for (int i = 2; i < snakeParts.size(); i++)
            {
                // Get the boundaries of the current snake part
                std::vector<glm::vec3> snakePartBoundaries = snakeParts[i]->getBoundariesInWorldSpace(app->getWindowSize());
                // Check if the head collides with the current snake part
                if (checkCollision(headBoundaries, snakePartBoundaries))
                {
                    // If so, set the game_over boolean to true and return
                    *game_over = true;
                    return;
                }
            }
        }

        // Detect if snake head collides with an apple (snake ate an apple)
        void handleSnakeEatingApple(World *world)
        {

            // Find the snake head entity and the apple entity
            Entity *snakeHead = nullptr;
            Entity *apple = nullptr;
            for (auto entity : world->getEntities())
            {
                if (entity->name == "snake_head")
                    snakeHead = entity;
                else if (entity->name == "apple")
                    apple = entity;
                if (snakeHead != nullptr && apple != nullptr)
                    break;
            }

            // If the snake head and the apple are null, return
            if (snakeHead == nullptr || apple == nullptr)
                return;

            // Get the snake head's boundaries and the apple's boundaries
            std::vector<glm::vec3> snakeBoundaries = snakeHead->getBoundariesInWorldSpace(app->getWindowSize());
            std::vector<glm::vec3> appleBoundaries = apple->getBoundariesInWorldSpace(app->getWindowSize());

            // Check if the snake head collides with the apple
            if (checkCollision(snakeBoundaries, appleBoundaries))
            {
                // If so, move the apple
                checkApplePosition(apple);
                // Add increase snakes length
                addSnakePart(snakeHead);
                // Increase the score
                updateScore(world);
            }
        }

        // This function updates the score
        // and so it updates the score mesh on the screen
        void updateScore(World *world)
        {
            // Increase the score
            score++;
            // Find the score mesh entities
            Entity *number1 = nullptr;
            Entity *number2 = nullptr;
            for (Entity *entity : world->getEntities())
            {
                if (entity->name == "number1")
                    number1 = entity;
                if (entity->name == "number2")
                    number2 = entity;
                if (number1 && number2)
                    break;
            }

            // Update the score mesh
            number1->getComponent<MeshRendererComponent>()->setMesh("num" + std::to_string(score / 10));
            number2->getComponent<MeshRendererComponent>()->setMesh("num" + std::to_string(score % 10));
        }

        // This function generates a new random position for the apple and checks if it is within coordinates of the ground
        // Then it sets the apple's position to the new random position
        void checkApplePosition(Entity *apple)
        {
            // Set the random seed
            srand(static_cast<unsigned>(time(0)));
            // Generate a random position for the apple
            float x = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * -100;
            while (x < -30.0f || x > 30.0f)
                x = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * -100;
            float y = -11.0f;
            float z = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * -100;
            while (z < -30.0f || z > -5.0f)
                z = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * -100;
            // Update the apple position
            apple->localTransform.position = glm::vec3(x, y, z);
        }

        // This function adds another snake part to the snake
        // by creating a new entity and adding it to the world just before the head
        // and move the head one step forward
        void addSnakePart(Entity *snakeHead)
        {
            // Create a new entity
            our::Entity *newpart = snakeHead->getWorld()->add();
            // Set the name of the entity to "snake_part" + the number of the snake part (score)
            newpart->name = "snake_part" + std::to_string(score);
            // Set the new part position equal to the position of the head
            newpart->localTransform = snakeHead->localTransform;
            // Add a mesh renderer component to the new part
            newpart->addComponent<MeshRendererComponent>();
            // Set the mesh of the new part to "body"
            newpart->getComponent<MeshRendererComponent>()->setMesh("body");
            // Set the texture of the new part to "skin"
            newpart->getComponent<MeshRendererComponent>()->setMaterial("skin");

            // Add a MovementComponent to the new part to hold the motion direction
            newpart->addComponent<MovementComponent>();
            // Set the movement direction to the same direction as the head
            newpart->getComponent<MovementComponent>()->linearVelocity = snakeHead->getComponent<MovementComponent>()->linearVelocity;

            // update head position using its direction (linearVelocity)
            if (snakeHead->getComponent<MovementComponent>()->linearVelocity == glm::vec3(0.0f, 0.0f, -1.0f))
                snakeHead->localTransform.position += glm::vec3(0.0f, 0.0f, -2.0f);
            else if (snakeHead->getComponent<MovementComponent>()->linearVelocity == glm::vec3(0.0f, 0.0f, 1.0f))
                snakeHead->localTransform.position += glm::vec3(0.0f, 0.0f, 2.0f);
            else if (snakeHead->getComponent<MovementComponent>()->linearVelocity == glm::vec3(-1.0f, 0.0f, 0.0f))
                snakeHead->localTransform.position += glm::vec3(-2.0f, 0.0f, 0.0f);
            else if (snakeHead->getComponent<MovementComponent>()->linearVelocity == glm::vec3(1.0f, 0.0f, 0.0f))
                snakeHead->localTransform.position += glm::vec3(2.0f, 0.0f, 0.0f);
        }

        // This function checks that min variable is not swapped with max variable
        void checkMinMax(float &min, float &max)
        {
            // If min is greater than max, swap them
            if (min > max)
                std::swap(min, max);
        }

        // This function checks collision between any two entities assuming they have a bounding box (AABB collision)
        bool checkCollision(std::vector<glm::vec3> object1Boundaries, std::vector<glm::vec3> object2Boundaries)
        {
            // Extract the extreme planes of both objects from the extreme vertices
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

            //  Check that minimum and maximum variables are in the correct order as they may get swapped as the object rotates
            checkMinMax(min_x1, max_x1);
            checkMinMax(min_y1, max_y1);
            checkMinMax(min_z1, max_z1);
            checkMinMax(min_x2, max_x2);
            checkMinMax(min_y2, max_y2);
            checkMinMax(min_z2, max_z2);

            // If the objects are not colliding, return false
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
            // Else return true
            return true;
        }
    };
}
