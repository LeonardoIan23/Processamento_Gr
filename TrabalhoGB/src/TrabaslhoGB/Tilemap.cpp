#include "Tilemap.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

void Tilemap::load(const char* file) {
    this->objectTexture.load("items.png", true);
    std::ifstream f(file);
    if (!f.is_open()) { std::cerr << "\n!!! ERRO FATAL: Nao foi possivel abrir o arquivo '" << file << "'" << std::endl; return; }
    std::string line, currentSection = "", tilesetFilename;
    while (std::getline(f, line)) {
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);
        if (line.empty() || line[0] == '#') continue;
        if (line[0] == '[') {
            currentSection = line.substr(1, line.find(']') - 1);
            continue;
        }
        if (!currentSection.empty()) {
            if (currentSection == "TILESHEET") {
                std::stringstream ss(line);
                std::string key, eq, value;
                if (ss >> key >> eq >> value && eq == "=") {
                    if (key == "ARQUIVO") tilesetFilename = value;
                    else if (key == "LARGURA_TILE") tileWidth = std::stoi(value);
                    else if (key == "ALTURA_TILE") tileHeight = std::stoi(value);
                    else if (key == "TILES_POR_LINHA") tilesetWidthInTiles = std::stoi(value);
                    else if (key == "TILE_LAVA") tileLavaID = std::stoi(value);
                    else if (key == "TILE_PISADO") tilePisadoID = std::stoi(value);
                    else if (key == "TILE_AGUA") tileAguaID = std::stoi(value);
                }
            } else if (currentSection == "MAPA") {
                std::stringstream ss(line);
                std::string key, eq;
                ss >> key >> eq;
                if (key == "DIMENSOES" && eq == "=") { ss >> mapWidth >> mapHeight; } 
                else if (key == "CAMINHAVEL" && eq == "=") { int id; while(ss >> id) walkableTiles.insert(id); }
                else if (key == "DADOS" && eq == "=") {
                    mapData.assign(mapWidth * mapHeight, 0);
                    for (int i = 0; i < mapHeight; ++i) { if (std::getline(f, line)) { std::stringstream ss_grid(line); for (int j = 0; j < mapWidth; ++j) { ss_grid >> mapData[i * mapWidth + j]; } } }
                }
            } else if (currentSection == "OBJETOS") {
                char type; int x, y; std::stringstream ss_obj(line);
                ss_obj >> type >> x >> y;
                if (type == 'P') { playerStartPos = glm::ivec2(x,y); }
                else if (type == 'C') { objects.push_back({glm::ivec2(x,y), 0}); }
            } else if (currentSection == "ALTURAS") {
                 std::stringstream ss(line);
                std::string key, eq;
                ss >> key >> eq;
                 if (key == "DADOS" && eq == "=") {
                    heightData.assign(mapWidth * mapHeight, 0);
                    for (int i = 0; i < mapHeight; ++i) { if (std::getline(f, line)) { std::stringstream ss_grid(line); for (int j = 0; j < mapWidth; ++j) { ss_grid >> heightData[i * mapWidth + j]; } } }
                }
            }
        }
    }
    f.close();
    if (!tilesetFilename.empty()) { this->tilesheetTexture.load(tilesetFilename.c_str(), false); } 
    else { std::cerr << "!!! ERRO: Nome do ficheiro do tileset (ARQUIVO) nao encontrado no map.txt!" << std::endl; }
    if (this->tileHeight > 0 && this->tilesheetTexture.Height > 0) { this->tilesetTotalRows = (int)ceil((float)this->tilesheetTexture.Height / (float)this->tileHeight); }
    else { if(this->tileHeight <= 0) std::cerr << "!!! ERRO: Dimensoes do tile (ALTURA_TILE) invalidas ou em falta no map.txt!" << std::endl; this->tilesetTotalRows = 1; }
}

void Tilemap::draw(Renderer &renderer) {
    if (tilesetWidthInTiles == 0 || mapData.empty() || tilesetTotalRows == 0) return;
    float uv_tile_w = 1.0f / (float)this->tilesetWidthInTiles;
    float uv_tile_h = 1.0f / (float)this->tilesetTotalRows;
    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            int tileID = mapData.at(y * mapWidth + x);
            if (tileID < 0) continue;
            
            glm::vec2 screenPos = getScreenPos({x, y});
            
            int tileGridX = tileID % tilesetWidthInTiles;
            int tileGridY = tileID / tilesetWidthInTiles;
            glm::vec2 uvOffset = { tileGridX * uv_tile_w, tileGridY * uv_tile_h };
            glm::vec2 uvScale = { uv_tile_w, uv_tile_h };
            
            // A chamada para desenhar volta a ser a original, que desenha um quadrado
            renderer.draw(this->tilesheetTexture, screenPos, glm::vec2(this->tileWidth, this->tileHeight), 0.0f, glm::vec3(1.0f), 1.0f, uvOffset, uvScale);
        }
    }
    for (const auto& obj : objects) {
        glm::vec2 screenPos = getScreenPos(obj.pos, 0.25f);
        renderer.draw(this->objectTexture, screenPos, glm::vec2(this->tileWidth * 0.5f, this->tileHeight * 0.5f));
    }
}

// ESTA É A FUNÇÃO CORRIGIDA PARA O ÂNGULO
glm::vec2 Tilemap::getScreenPos(glm::ivec2 gridPos, float heightOffset) const {
    float originX = 1280.0f / 2.0f;
    float originY = 200.0f;

    glm::vec2 world_pos;
    
    // A matemática do DiamondView.h
    world_pos.x = (gridPos.x - gridPos.y) * (this->tileWidth / 2.0f);
    world_pos.y = (gridPos.x + gridPos.y) * (this->tileHeight / 2.0f);
    
    // A altura do terreno
    world_pos.y -= this->getHeightAt(gridPos) * (this->tileHeight * 0.75f);
    
    // Offset de objetos
    world_pos.y -= heightOffset * (this->tileHeight);
    
    // Adiciona o offset da origem e ajusta para o canto do tile
    glm::vec2 screen_pos;
    screen_pos.x = originX + world_pos.x - (this->tileWidth / 2.0f);
    screen_pos.y = originY + world_pos.y;
    
    return screen_pos;
}

bool Tilemap::isPositionWalkable(glm::ivec2 pos) const {
    if (pos.x < 0 || pos.x >= mapWidth || pos.y < 0 || pos.y >= mapHeight || mapData.empty()) return false;
    int tileID = mapData.at(pos.y * mapWidth + pos.x);
    return walkableTiles.count(tileID) > 0;
}
int Tilemap::getTileAt(glm::ivec2 pos) const {
    if (pos.x < 0 || pos.x >= mapWidth || pos.y < 0 || pos.y >= mapHeight || mapData.empty()) return -1;
    return mapData.at(pos.y * mapWidth + pos.x);
}
int Tilemap::getHeightAt(glm::ivec2 pos) const {
    if (pos.x < 0 || pos.x >= mapWidth || pos.y < 0 || pos.y >= mapHeight || heightData.empty()) return 0;
    return heightData.at(pos.y * mapWidth + pos.x);
}
int Tilemap::getObjectAt(glm::ivec2 pos) {
    for (size_t i = 0; i < objects.size(); ++i) { if (objects[i].pos == pos) return i; }
    return -1;
}
void Tilemap::removeObject(int objectIndex) {
    if(objectIndex >= 0 && (size_t)objectIndex < objects.size()) { objects.erase(objects.begin() + objectIndex); }
}
void Tilemap::setTileAt(glm::ivec2 pos, int tileID) {
    if (pos.x >= 0 && pos.x < mapWidth && pos.y >= 0 && pos.y < mapHeight && !mapData.empty()) {
        int currentTile = mapData.at(pos.y * mapWidth + pos.x);
        if (walkableTiles.count(currentTile) > 0) { mapData[pos.y * mapWidth + pos.x] = tileID; }
    }
}