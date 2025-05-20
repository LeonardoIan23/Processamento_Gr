#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
using namespace glm;

struct Triangle {
    vec3 position;
    vec3 dimensions;
    vec3 color;
};

GLuint createBaseTriangle() {
    float vertices[] = {
         0.0f, 0.0f, 0.0f,
         0.1f, 0.1f, 0.0f,
         0.2f, 0.0f, 0.0f
    };
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    return VAO;
}

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(800, 600, "Ex3", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    const char* vShader = "#version 400\nlayout(location=0) in vec3 position;\nuniform mat4 model;\nuniform mat4 projection;\nvoid main(){ gl_Position = projection * model * vec4(position, 1.0); }";
    const char* fShader = "#version 400\nuniform vec3 inputColor;\nout vec4 color;\nvoid main(){ color = vec4(inputColor, 1.0); }";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vShader, NULL); glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fShader, NULL); glCompileShader(fs);
    GLuint program = glCreateProgram();
    glAttachShader(program, vs); glAttachShader(program, fs); glLinkProgram(program);

    GLuint VAO = createBaseTriangle();
    std::vector<Triangle> triangles = {
        {{100, 100, 0}, {100, 100, 1}, {1.0f, 0.0f, 0.0f}},
        {{300, 100, 0}, {100, 100, 1}, {0.0f, 1.0f, 0.0f}},
        {{500, 100, 0}, {100, 100, 1}, {0.0f, 0.0f, 1.0f}},
        {{200, 300, 0}, {100, 100, 1}, {1.0f, 1.0f, 0.0f}},
        {{400, 300, 0}, {100, 100, 1}, {1.0f, 0.0f, 1.0f}}
    };

    glUseProgram(program);
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        for (auto& tri : triangles) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, tri.position);
            model = glm::scale(model, tri.dimensions);
            glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(glGetUniformLocation(program, "inputColor"), 1, glm::value_ptr(tri.color));
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
