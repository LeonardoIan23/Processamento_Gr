#ifndef LTMATH_H
#define LTMATH_H

// Calcula a área de um triângulo dado três pontos (x1, y1), (x2, y2), (x3, y3)
float calculateTriangleArea(float x1, float y1, float x2, float y2, float x3, float y3);

// Verifica se um ponto (px, py) está dentro de um triângulo ABC
// O array 'abc' deve conter 6 floats: {Ax, Ay, Bx, By, Cx, Cy}
// O array 'point' deve conter 2 floats: {Px, Py}
bool triangleCollidePoint2D(float* abc, float* point);

// Enumeração para direções de navegação, útil para o tile walking
enum TileDirection {
    DIRECTION_NONE = 0,
    DIRECTION_NORTH,
    DIRECTION_SOUTH,
    DIRECTION_EAST,
    DIRECTION_WEST,
    DIRECTION_NORTHEAST,
    DIRECTION_NORTHWEST,
    DIRECTION_SOUTHEAST,
    DIRECTION_SOUTHWEST
};

#endif // LTMATH_H