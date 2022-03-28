#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace our {

    class ShaderProgram {

    private:
        //Shader Program Handle
        GLuint program;

    public:
        void create();
        void destroy();

        ShaderProgram(){ program = 0; }
        ~ShaderProgram(){ destroy(); }

        bool attach(const std::string &filename, GLenum type) const;

        bool link() const;

        void use() { 
            //TODO: call opengl to use the program identified by this->program
            // Tell opengl which program (pipeline) we are currently using
            glUseProgram(this->program);
        }

        GLuint getUniformLocation(const std::string &name) {
            //TODO: call opengl to get the uniform location for the uniform defined by name from this->program
            // Get the physical location of a uniform inside this->program with name "name"
            return glGetUniformLocation(this->program, name.c_str());
        }

        void set(const std::string &uniform, GLfloat value) {
            //TODO: call opengl to set the value to the uniform defined by name
            // Set 1 float inside unifrom called "uniform"
            glUniform1f(getUniformLocation(uniform), value);
        }

        void set(const std::string &uniform, glm::vec2 value) {
            //TODO: call opengl to set the value to the uniform defined by name
            // Set 2 float inside unifrom called "uniform" (vector)
            // 1 is the count of vectors we want to set
            glUniform2fv(getUniformLocation(uniform),1,&value[0]);
        }

        void set(const std::string &uniform, glm::vec3 value) {
            //TODO: call opengl to set the value to the uniform defined by name
            // Set 3 float inside unifrom called "uniform" (vector)
            // 1 is the count of vectors we want to set
            glUniform3fv(getUniformLocation(uniform),1,&value[0]);
        }

        void set(const std::string &uniform, glm::vec4 value) {
            //TODO: call opengl to set the value to the uniform defined by name
            // Set 4 float inside unifrom called "uniform" (vector)
            // 1 is the count of vectors we want to set
            glUniform4fv(getUniformLocation(uniform),1,&value[0]);
        }

        //TODO: Delete the copy constructor and assignment operator
        ShaderProgram(const ShaderProgram &x) = delete;
        void operator=(ShaderProgram x) = delete;
        //Question: Why do we do this? Hint: Look at the deconstructor
        /**
         * Because copy constructor and assignment operator do shallow copy 
         * not deep copy meaning that pointers are copied as addresses only 
         * so on deleting one pointer, one of the copies gets an error
         * also on editing the pointer content it gets edited in the other copy
         * 
         * In this case we have a program id which would be copied and on destroying
         * the copy, the original application will have an error because it uses
         * the id of the deleted program
         * 
         * We could have modified them to make deep copy instead
         */
    };

}

#endif