#include "TileMap.h"
#include <iostream>

TileMap::TileMap(int w, int h, unsigned char defaultTileID)
    : width(w), height(h), tiles(w * h, defaultTileID), tileSetTextureID(0) {
    if (w <= 0 || h <= 0) {
        std::cerr << "ERRO: TileMap criado com dimensoes invalidas (w=" << w << ", h=" << h << ")" << std::endl;
        width = 0;
        height = 0;
        tiles.clear();
    }
    std::cout << "TileMap criado: " << width << "x" << height << std::endl;
}

TileMap::~TileMap() {
    std::cout << "TileMap destruido." << std::endl;
}

unsigned char TileMap::getTile(int col, int row) const {
    if (col < 0 || col >= width || row < 0 || row >= height) {
        return 0;
    }
    return tiles[row * width + col];
}

void TileMap::setTile(int col, int row, unsigned char tileID) {
    if (col < 0 || col >= width || row < 0 || row >= height) {
        std::cerr << "AVISO: Tentativa de definir tile fora dos limites (" << col << "," << row << ") com ID " << (int)tileID << std::endl;
        return;
    }
    tiles[row * width + col] = tileID;
}

int TileMap::getWidth() const {
    return width;
}

int TileMap::getHeight() const {
    return height;
}

void TileMap::setTid(GLuint tid) {
    tileSetTextureID = tid;
}

GLuint TileMap::getTileSet() const {
    return tileSetTextureID;
}

float TileMap::getZ() const {
    return 0.0f;
}