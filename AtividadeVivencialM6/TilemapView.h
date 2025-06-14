#ifndef TILEMAPVIEW_H
#define TILEMAPVIEW_H

#include "ltMath.h" // <<<--- ESSA LINHA É CRÍTICA!

class TilemapView {
public:
    virtual ~TilemapView() {}

    // ATENÇÃO: Verifique cada caractere aqui.
    virtual void computeDrawPosition(int col, int row, float tileWidth, float tileHeight, float& outX, float& outY) const = 0;
    virtual void computeMouseMap(int& outCol, int& outRow, float tileWidth, float tileHeight, float mouseX, float mouseY) const = 0;
    virtual void computeTileWalking(int& col, int& row, TileDirection direction) const = 0; // <<<--- ASSINATURA EXATA
};

#endif // TILEMAPVIEW_H