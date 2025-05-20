#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

GLuint createTriangle(float x0, float y0, float x1, float y1, float x2, float y2) {
    GLuint VAO, VBO;
    float vertices[] = {
        x0, y0, 0.0f,
        x1, y1, 0.0f,
        x2, y2, 0.0f
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return VAO;
}

int main() {
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(800, 600, "Ex1", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    const char* vShader = "#version 400\nlayout(location=0) in vec3 position;\nvoid main(){ gl_Position = vec4(position, 1.0); }";
    const char* fShader = "#version 400\nout vec4 color;\nvoid main(){ color = vec4(0.3,0.5,1.0,1.0); }";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vShader, NULL);
    glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fShader, NULL);
    glCompileShader(fs);
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    GLuint vao = createTriangle(-0.5f, -0.5f, 0.5f, -0.5f, 0.0f, 0.5f);
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
