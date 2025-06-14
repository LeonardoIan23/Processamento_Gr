#ifndef DIAMONDVIEW_H
#define DIAMONDVIEW_H

#include "TilemapView.h" // <<<--- ESSA LINHA É CRÍTICA!

class DiamondView : public TilemapView
{
public:
    // ATENÇÃO: Verifique cada caractere aqui, deve ser IDÊNTICO à base (exceto pelo 'virtual' e '=0')
    void computeTileWalking(int &col, int &row, TileDirection direction) const override; // <<<--- ASSINATURA EXATA com 'override'
    void computeDrawPosition(int col, int row, float tileWidth, float tileHeight, float &outX, float &outY) const override;
    void computeMouseMap(int &outCol, int &outRow, float tileWidth, float tileHeight, float mouseX, float mouseY) const override;
};

#endif // DIAMONDVIEW_H