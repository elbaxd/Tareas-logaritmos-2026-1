#pragma once

#include <algorithm>
#include <cstdint>

// Punto 2D: par de floats (8 bytes en total)
// Coincide con el formato del archivo binario de puntos del enunciado.
// x: Coordenada X
// y: Coordenada Y
struct Point {
    float x;
    float y;
};

// Minimum Bounding Rectangle (MBR): 4 floats (16 bytes)
// Para entradas hoja se cumple x1 == x2 e y1 == y2 (el MBR es un punto)
// x1, x2: limites inferior y superior en X
// y1, y2: limites inferior y superior en Y
struct MBR {
    float x1, x2;
    float y1, y2;
};

// Construye un MBR puntual a partir de un punto (Point)
//  p: punto de entrada.
//  ret: MBR puntual (x1==x2, y1==y2).
inline MBR pointMBR(const Point& p) {
    return MBR{p.x, p.x, p.y, p.y};
}

// Chequea si dos MBR se intersectan
//  a, b: MBRs a comparar
//  ret: true si a y b se intersectan
inline bool intersects(const MBR& a, const MBR& b) {
    return !(a.x2 < b.x1 || b.x2 < a.x1 || a.y2 < b.y1 || b.y2 < a.y1);
}

// Revisa si un punto esta contenido al interior de un MBR
//  m: MBR
//  x, y: coordenadas del punto
//  ret: true si (x, y) esta dentro de m.
inline bool contains(const MBR& m, float x, float y) {
    return x >= m.x1 && x <= m.x2 && y >= m.y1 && y <= m.y2;
}

// Agranda (expande) el MBR en dst para que envuelva tambien al MBR de src
// En otras palabras, es la union de los MBRs
//  dst: MBR a expandir (modificado in-place).
//  src: MBR que debe quedar contenido dentro de dst.
inline void expand(MBR& dst, const MBR& src) {
    dst.x1 = std::min(dst.x1, src.x1);
    dst.x2 = std::max(dst.x2, src.x2);
    dst.y1 = std::min(dst.y1, src.y1);
    dst.y2 = std::max(dst.y2, src.y2);
}
