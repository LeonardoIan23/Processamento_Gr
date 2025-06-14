#include "DiamondView.h"
#include <cmath>

void DiamondView::computeDrawPosition(int col, int row, float tileWidth, float tileHeight, float& outX, float& outY) const {
    outX = (float)(col - row) * (tileWidth / 2.0f);
    outY = (float)(col + row) * (tileHeight / 2.0f);
}

void DiamondView::computeMouseMap(int& outCol, int& outRow, float tileWidth, float tileHeight, float mouseX, float mouseY) const {
    float isoX = mouseX;
    float isoY = mouseY;

    isoX /= (tileWidth / 2.0f);
    isoY /= (tileHeight / 2.0f);

    outCol = (int)floor((isoX + isoY) / 2.0f);
    outRow = (int)floor((isoY - isoX) / 2.0f);
}

void DiamondView::computeTileWalking(int& col, int& row, TileDirection direction) const {
    switch (direction) {
        case DIRECTION_NORTH:
            col++;
            row++;
            break;
        case DIRECTION_SOUTH:
            col--;
            row--;
            break;
        case DIRECTION_EAST:
            col++;
            row--;
            break;
        case DIRECTION_WEST:
            col--;
            row++;
            break;
        case DIRECTION_NORTHEAST:
            col++;
            break;
        case DIRECTION_NORTHWEST:
            row++;
            break;
        case DIRECTION_SOUTHEAST:
            row--;
            break;
        case DIRECTION_SOUTHWEST:
            col--;
            break;
        default:
            break;
    }
}