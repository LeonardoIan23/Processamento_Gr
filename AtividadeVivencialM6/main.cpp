// STB_IMAGE
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "gl_utils.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <vector>

#include "TileMap.h"
#include "TilemapView.h"
#include "DiamondView.h"
#include "ltMath.h"

using namespace std;

// Variáveis globais
int g_gl_width = 800;
int g_gl_height = 600;
GLFWwindow *g_window = NULL;

float xi = -1.0f;
float xf = 1.0f;
float yi = -1.0f;
float yf = 1.0f;
float w = xf - xi;
float h = yf - yi;

float tw, th, tw2, th2;
int tileSetCols = 7, tileSetRows = 1; // Ajustado para sua textura tilesetIso.png
float tileW, tileW2;
float tileH, tileH2;

int cx = 0, cy = 0; // Coordenadas do tile atualmente selecionado

TilemapView *tview = new DiamondView();
TileMap *tmap = NULL;

// --- Shaders Embutidos como strings ---
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out float Weight;

uniform float offsetx;
uniform float offsety;
uniform float tx;
uniform float ty;
uniform float layer_z;
uniform float weight;

void main()
{
    gl_Position = vec4(aPos.x + tx, aPos.y + ty, layer_z, 1.0);
    TexCoord = vec2(aTexCoord.x + offsetx, aTexCoord.y + offsety);
    Weight = weight;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float Weight;

uniform sampler2D sprite;

void main()
{
    vec4 texColor = texture(sprite, TexCoord);
    if (Weight > 0.0) {
        FragColor = vec4(texColor.rgb * (1.0 + Weight), texColor.a);
    } else {
        FragColor = texColor;
    }
}
)";
// --- Fim dos Shaders Embutidos ---


int loadTexture(unsigned int &texture, const char *filename)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	GLfloat max_aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);

	int width, height, nrChannels;
	unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture: " << filename << std::endl;
	}
	stbi_image_free(data);
    return (data != NULL);
}

void SRD2SRU(double &mx, double &my, float &x, float &y) {
	x = xi + (mx / g_gl_width ) * w;
	y = yi + (1 - (my / g_gl_height)) * h;
}

void mouse(double &mx, double &my) {
    float worldX, worldY;
    SRD2SRU(mx, my, worldX, worldY);

    int c_cand, r_cand;
    tview->computeMouseMap(c_cand, r_cand, tw, th, worldX, worldY);

    float x0, y0;
    tview->computeDrawPosition(c_cand, r_cand, tw, th, x0, y0);

    float point[] = {worldX, worldY};

    float *abc = new float[6];

    bool left = worldX < (x0 + tw/2.0f);

    if (left) {
        abc[0] = x0;             abc[1] = y0 + th/2.0f;
        abc[2] = x0 + tw/2.0f;   abc[3] = y0 + th;
        abc[4] = x0 + tw/2.0f;   abc[5] = y0;
    } else {
        abc[0] = x0 + tw/2.0f;   abc[1] = y0;
        abc[2] = x0 + tw/2.0f;   abc[3] = y0 + th;
        abc[4] = x0 + tw;        abc[5] = y0 + th/2.0f;
    }

    bool collide = triangleCollidePoint2D(abc, point);
    delete[] abc;

    if (!collide) {
        if (left) {
            tview->computeTileWalking(c_cand, r_cand, DIRECTION_WEST);
        } else {
            tview->computeTileWalking(c_cand, r_cand, DIRECTION_EAST);
        }
    }

    if ((c_cand < 0) || (c_cand >= tmap->getWidth()) || (r_cand < 0) || (r_cand >= tmap->getHeight())){
        cout << "Clique fora do mapa: c=" << c_cand << ", r=" << r_cand << endl;
        return;
    }

    cx = c_cand;
    cy = r_cand;
    cout << "SELECIONADO (mouse) c=" << cx << ", r=" << cy << endl;
}


int main()
{
	restart_gl_log();
	start_gl();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

    cout << "Tentando criar tmap" << endl;
    tmap = new TileMap(3, 3, 0);

    tmap->setTile(0, 2, 1); tmap->setTile(1, 2, 1); tmap->setTile(2, 2, 4);
    tmap->setTile(0, 1, 4); tmap->setTile(1, 1, 1); tmap->setTile(2, 1, 4);
    tmap->setTile(0, 0, 4); tmap->setTile(1, 0, 4); tmap->setTile(2, 0, 1);

    cx = 1;
    cy = 1;

    tw = w / (float)tmap->getWidth();
    th = tw / 2.0f;
    tw2 = tw / 2.0f;
    th2 = th / 2.0f;

    tileW = 1.0f / (float) tileSetCols;
    tileH = 1.0f / (float) tileSetRows;
    tileW2 = tileW / 2.0f;
    tileH2 = tileH / 2.0f;

    cout << "tw=" << tw << " th=" << th << " tw2=" << tw2 << " th2=" << th2
         << " tileW=" << tileW << " tileH=" << tileH
         << " tileW2=" << tileW2 << " tileH2=" << tileH2
    << endl;

	GLuint tid;
	loadTexture(tid, "tilesetIso.png");

    tmap->setTid(tid);
    cout << "Tmap inicializado" << endl;

	float vertices[] = {
		// positions        // texture coords
		-tw2 ,  0.0f,       0.0f,  tileH2,
		 0.0f, -th2,        tileW2, 0.0f,
		 tw2 ,  0.0f,       tileW,  tileH2,
		 0.0f,  th2,        tileW2, tileH
	};
	unsigned int indices[] = {
		0, 1, 3,
		3, 1, 2
	};

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

    // --- Compilação dos shaders embutidos ---
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderSource, NULL); // Usando a string do shader
    glCompileShader(vs);

    int params = -1;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &params);
    if (GL_TRUE != params)
    {
        fprintf(stderr, "ERROR: GL vertex shader did not compile\n");
        print_shader_info_log(vs);
        return 1;
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSource, NULL); // Usando a string do shader
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &params);
    if (GL_TRUE != params)
    {
        fprintf(stderr, "ERROR: GL fragment shader did not compile\n");
        print_shader_info_log(fs);
        return 1;
    }

    GLuint shader_programme = glCreateProgram();
    glAttachShader(shader_programme, fs);
    glAttachShader(shader_programme, vs);
    glLinkProgram(shader_programme);

    glGetProgramiv(shader_programme, GL_LINK_STATUS, &params);
    if (GL_TRUE != params)
    {
        fprintf(stderr, "ERROR: could not link shader programme GL index %i\n",
                shader_programme);
        return false;
    }
    // --- Fim da Compilação dos shaders embutidos ---

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (!glfwWindowShouldClose(g_window))
	{
		_update_fps_counter(g_window);
		double current_seconds = glfwGetTime();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, g_gl_width, g_gl_height);

		glUseProgram(shader_programme);
		glBindVertexArray(VAO);

        // Lógica de Navegação por Teclado
        static double lastMoveTime = 0.0;
        double currentTime = glfwGetTime();
        const double moveDelay = 0.15;

        if (currentTime - lastMoveTime > moveDelay) {
            bool moved = false;
            int nextCx = cx;
            int nextCy = cy;

            if (glfwGetKey(g_window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(g_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                tview->computeTileWalking(nextCx, nextCy, DIRECTION_NORTHEAST);
                moved = true;
            }
            else if (glfwGetKey(g_window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(g_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
                tview->computeTileWalking(nextCx, nextCy, DIRECTION_NORTHWEST);
                moved = true;
            }
            else if (glfwGetKey(g_window, GLFW_KEY_DOWN) == GLFW_PRESS && glfwGetKey(g_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                tview->computeTileWalking(nextCx, nextCy, DIRECTION_SOUTHEAST);
                moved = true;
            }
            else if (glfwGetKey(g_window, GLFW_KEY_DOWN) == GLFW_PRESS && glfwGetKey(g_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
                tview->computeTileWalking(nextCx, nextCy, DIRECTION_SOUTHWEST);
                moved = true;
            }
            else if (glfwGetKey(g_window, GLFW_KEY_UP) == GLFW_PRESS) {
                tview->computeTileWalking(nextCx, nextCy, DIRECTION_NORTH);
                moved = true;
            }
            else if (glfwGetKey(g_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                tview->computeTileWalking(nextCx, nextCy, DIRECTION_SOUTH);
                moved = true;
            }
            else if (glfwGetKey(g_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
                tview->computeTileWalking(nextCx, nextCy, DIRECTION_WEST);
                moved = true;
            }
            else if (glfwGetKey(g_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                tview->computeTileWalking(nextCx, nextCy, DIRECTION_EAST);
                moved = true;
            }

            if (moved) {
                if (nextCx >= 0 && nextCx < tmap->getWidth() &&
                    nextCy >= 0 && nextCy < tmap->getHeight()) {
                    cx = nextCx;
                    cy = nextCy;
                    cout << "POSICAO ATUAL (teclado): c=" << cx << ", r=" << cy << endl;
                } else {
                    cout << "Tentativa de mover para fora do mapa. Ignorado." << endl;
                }
                lastMoveTime = currentTime;
            }
        }

        // Desenho dos tiles
        float x, y;
        for(int r = 0; r < tmap->getHeight(); r++) {
            for(int c = 0; c < tmap->getWidth(); c++) {
                int t_id = (int) tmap->getTile(c, r);
                int u = t_id % tileSetCols;
                int v = t_id / tileSetCols;

                tview->computeDrawPosition(c, r, tw, th, x, y);

                glUniform1f(glGetUniformLocation(shader_programme, "offsetx"), u * tileW);
                glUniform1f(glGetUniformLocation(shader_programme, "offsety"), v * tileH);
                glUniform1f(glGetUniformLocation(shader_programme, "tx"), x);
                glUniform1f(glGetUniformLocation(shader_programme, "ty"), y + 1.0f);
                glUniform1f(glGetUniformLocation(shader_programme, "layer_z"), tmap->getZ());
                glUniform1f(glGetUniformLocation(shader_programme, "weight"), (c == cx) && (r == cy) ? 0.5f : 0.0f);

                glBindTexture(GL_TEXTURE_2D, tmap->getTileSet());
                glUniform1i(glGetUniformLocation(shader_programme, "sprite"), 0);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }

		glfwPollEvents();

		if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose(g_window, 1);
		}

        double mx, my;
        glfwGetCursorPos(g_window, &mx, &my);

        static bool mouse_pressed_prev = false;
        const int state = glfwGetMouseButton(g_window, GLFW_MOUSE_BUTTON_LEFT);

        if (state == GLFW_PRESS && !mouse_pressed_prev) {
            mouse(mx, my);
            mouse_pressed_prev = true;
        } else if (state == GLFW_RELEASE) {
            mouse_pressed_prev = false;
        }

		glfwSwapBuffers(g_window);
	}

	glfwTerminate();
    delete tmap;
    delete tview;
	return 0;
}