#ifndef TILEMAP_H
#define TILEMAP_H

#include <vector>
#include <glad/glad.h>

class TileMap {
private:
    int width;
    int height;
    std::vector<unsigned char> tiles;
    GLuint tileSetTextureID;

public:
    TileMap(int w, int h, unsigned char defaultTileID = 0);
    ~TileMap();

    unsigned char getTile(int col, int row) const;
    void setTile(int col, int row, unsigned char tileID);

    int getWidth() const;
    int getHeight() const;

    void setTid(GLuint tid);
    GLuint getTileSet() const;

    float getZ() const;
};

#endif // TILEMAP_H