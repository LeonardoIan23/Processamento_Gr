#include "gl_utils.h"
#include <iostream>
#include <fstream> // Este include pode ser removido se parse_file_into_str for eliminada/não usada.

FILE* g_gl_log_file = NULL;

#ifndef GL_LOG_FILE
#define GL_LOG_FILE "gl.log"
#endif

void start_gl() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    g_window = glfwCreateWindow(g_gl_width, g_gl_height, "Isometric Tilemap", NULL, NULL);
    if (g_window == NULL) {
        fprintf(stderr, "ERROR: could not open GLFW window with current GL context\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(g_window);

    glfwSetFramebufferSizeCallback(g_window, glfw_framebuffer_size_callback);
    glfwSetKeyCallback(g_window, glfw_key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "ERROR: Failed to initialize GLAD\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "OpenGL Version: %s\n", glGetString(GL_VERSION));
    fprintf(stdout, "GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    fprintf(stdout, "Renderer: %s\n", glGetString(GL_RENDERER));
}

void restart_gl_log() {
    g_gl_log_file = fopen(GL_LOG_FILE, "w");
    if (!g_gl_log_file) {
        fprintf(stderr, "WARNING: could not open GL_LOG_FILE log file for writing\n");
        return;
    }
    time_t now = time(NULL);
    char* dt = ctime(&now);
    fprintf(g_gl_log_file, "%s localtime: %s\n", GL_LOG_FILE, dt);
    fclose(g_gl_log_file);
}

void _update_fps_counter(GLFWwindow* window) {
    static double previous_seconds = glfwGetTime();
    static int frame_count;
    double current_seconds = glfwGetTime();
    double elapsed_seconds = current_seconds - previous_seconds;
    if (elapsed_seconds > 0.25) {
        previous_seconds = current_seconds;
        double fps = (double)frame_count / elapsed_seconds;
        char tmp[128];
        sprintf(tmp, "OpenGL @ fps: %.2f", fps);
        glfwSetWindowTitle(window, tmp);
        frame_count = 0;
    }
    frame_count++;
}

bool parse_file_into_str(const char* file_name, char* shader_str, int max_len) {
    // Esta função não é mais estritamente necessária se os shaders forem embutidos.
    // Mas a mantenho por completude ou se houver outras chamadas a ela.
    std::ifstream file(file_name, std::ios::in);
    if (!file.is_open()) {
        std::cerr << "ERROR: Nao foi possivel abrir o arquivo " << file_name << std::endl;
        return false;
    }

    file.read(shader_str, max_len);
    if (file.gcount() < max_len) {
        shader_str[file.gcount()] = '\0';
    } else {
        shader_str[max_len - 1] = '\0';
        std::cerr << "AVISO: Conteudo do arquivo " << file_name << " truncado. Aumente max_len." << std::endl;
    }
    file.close();
    return true;
}

void print_shader_info_log(GLuint shader_idx) {
    int max_length = 2048;
    int actual_length = 0;
    char log_text[max_length];
    glGetShaderInfoLog(shader_idx, max_length, &actual_length, log_text);
    fprintf(stderr, "Shader log for GL index %u:\n%s\n", shader_idx, log_text);
}

void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    g_gl_width = width;
    g_gl_height = height;
    glViewport(0, 0, width, height);
}

void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
}