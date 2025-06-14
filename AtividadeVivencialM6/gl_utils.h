#ifndef GL_UTILS_H
#define GL_UTILS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <time.h>

extern GLFWwindow *g_window;
extern int g_gl_width;
extern int g_gl_height;

void start_gl();
void restart_gl_log();
void _update_fps_counter(GLFWwindow* window);
// A função parse_file_into_str não será mais necessária no main.cpp se os shaders forem embutidos.
// Mas se outras partes do seu código a usam, você pode mantê-la.
// Para este exemplo, como ela é chamada apenas no main, não precisaria dela se os shaders forem strings.
// Vou mantê-la por enquanto, mas ela pode ser removida junto com o include <fstream> no .cpp se não for mais usada.
bool parse_file_into_str(const char* file_name, char* shader_str, int max_len);
void print_shader_info_log(GLuint shader_idx);
void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height);
void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

#endif // GL_UTILS_H