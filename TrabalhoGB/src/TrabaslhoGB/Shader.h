// src/Shader.h
#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    unsigned int ID;
    Shader() = default;
    void load(const char* vertexPath, const char* fragmentPath);
    void use();
    void setMat4(const std::string &name, const glm::mat4 &mat);
    void setVec3(const std::string &name, const glm::vec3 &value);
    void setVec2(const std::string &name, const glm::vec2 &value);
    void setInt(const std::string &name, int value);
    void setFloat(const std::string &name, float value);
private:
    void checkCompileErrors(unsigned int shader, std::string type);
};
