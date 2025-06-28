// src/Tilemap.h
#pragma once
#include <string>
#include <vector>
#include <set>
#include <glm/glm.hpp>
#include "Texture.h"
#include "Renderer.h"

struct MapObject { glm::ivec2 pos; int type; };

class Tilemap {
public:
    int mapWidth = 0, mapHeight = 0;
    int tileWidth = 0, tileHeight = 0;
    int tileLavaID = -1, tilePisadoID = -1;
    int tileAguaID = -1;
    glm::ivec2 playerStartPos;
    std::vector<MapObject> objects;

    Tilemap() = default;
    void load(const char* file);
    void draw(Renderer &renderer);
    bool isPositionWalkable(glm::ivec2 pos) const;
    int getObjectAt(glm::ivec2 pos);
    void removeObject(int objectIndex);
    void setTileAt(glm::ivec2 pos, int tileID);
    int getTileAt(glm::ivec2 pos) const;
    int getHeightAt(glm::ivec2 pos) const;
    glm::vec2 getScreenPos(glm::ivec2 gridPos, float heightOffset = 0.0f) const;

private:
    Texture2D tilesheetTexture;
    Texture2D objectTexture;
    std::vector<int> mapData;
    std::vector<int> heightData;
    std::set<int> walkableTiles;
    int tilesetWidthInTiles = 0;
    int tilesetTotalRows = 0;
};