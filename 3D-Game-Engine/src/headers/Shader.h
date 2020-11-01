#ifndef SHADER_H
#define SHADER_H
#include "glew.h"
#include <string>
#include <vector>
#include "glm.hpp"

    //TODO: Add safetys around Binary cache
    //https://www.khronos.org/opengl/wiki/Shader_Compilation#Binary_upload
    class Shader
    {
    public:
        const GLuint& GetHandle()
        {
            return m_handle;
        }
        void Bind();
        void Unbind();
        void SetMat4(std::string uniformName, glm::mat4);
        void SetVec3(std::string uniformName, glm::vec3);
        void SetVec3(std::string uniformName, float vecX, float vecY, float vecZ);
        Shader() {}
        Shader(const std::vector<GLenum>& types, const std::vector<std::string>& filepaths);
        ~Shader();
    private:
        GLuint m_handle;

        GLuint GenerateBinary(GLuint program);
        GLuint CompileAndLink(const std::vector<GLenum>& types, const std::vector<std::string>& filepaths);
        void testShaderCompilation(GLuint shaderHandle);
        void testProgramLinkage(GLuint programHandle);
        GLuint compileShader(GLenum shaderType, const std::string& filepath);
        GLuint linkProgram(std::vector<GLuint> shaderHandles);
    };
#endif 
