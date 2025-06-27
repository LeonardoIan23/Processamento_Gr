#ifndef MAP_H
#define MAP_H

#include <string>
#include <vector>
#include <glad/glad.h> // Usaremos o tipo GLuint para o ID da textura do OpenGL

// Struct para guardar as informações do Tileset, conforme pedido no enunciado.
struct TilesetInfo {
    std::string filename; // Nome do arquivo de imagem (ex: "tilesetIso.png")
    int num_tiles;        // Quantidade de tiles no arquivo
    int tile_width;       // Largura de cada tile em pixels
    int tile_height;      // Altura de cada tile em pixels
    GLuint texture_id;    // ID da textura no OpenGL (será preenchido na fase gráfica)
};

// Struct para representar um item no mapa (moeda, lava, etc.).
struct Item {
    int type; // Um número para identificar o tipo do item
    int x;    // Posição x no grid do mapa
    int y;    // Posição y no grid do mapa
};

// Struct principal que guarda todas as informações do mapa carregadas do arquivo.
struct MapData {
    TilesetInfo tileset;            // Informações do tileset usado
    int width;                      // Largura do mapa em tiles
    int height;                     // Altura do mapa em tiles
    std::vector<int> tile_grid;     // O grid com os IDs de cada tile
    std::vector<bool> walkable_tiles; // Um vetor para saber se um tile ID é caminhável
    std::vector<Item> items;        // Uma lista com todos os itens do mapa
};

// Declaração da nossa futura função que vai carregar o mapa.
// Ela vai ler um arquivo e preencher um objeto MapData.
bool loadMap(const std::string& filename, MapData& outMapData);

#endif // MAP_H