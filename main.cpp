#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>

struct Point {
    double x, y, z;

    Point(double x, double y, double z) : x(x), y(y), z(z) {}
};

// Функция для вычисления расстояния между двумя точками
double distance(const Point& a, const Point& b) {
    return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2) + std::pow(a.z - b.z, 2));
}

struct VoronoiCell {
    Point site; // Точка, к которой принадлежит ячейка
    std::vector<Point> vertices; // Вершины ячейки

    VoronoiCell(const Point& site) : site(site) {}
};

// Функция для разделения пространства
void voronoiDivide(std::vector<Point>& points, int start, int end, std::vector<VoronoiCell>& cells) {
    if (end - start <= 1) {
        return; // Базовый случай
    }

    // Разделение на две части
    int mid = (start + end) / 2;

    // Рекурсивный вызов для левой и правой половин
    voronoiDivide(points, start, mid, cells);
    voronoiDivide(points, mid, end, cells);

    // Здесь добавляем логику для построения ячеек Вороного
    // Для простоты, создадим ячейку для каждой точки
    for (int i = start; i < end; ++i) {
        VoronoiCell cell(points[i]);
        // Здесь можно добавить логику для вычисления вершин ячейки
        // Например, добавление случайных вершин для демонстрации
        cell.vertices.push_back(Point(points[i].x + 1, points[i].y, points[i].z));
        cell.vertices.push_back(Point(points[i].x, points[i].y + 1, points[i].z));
        cell.vertices.push_back(Point(points[i].x, points[i].y, points[i].z + 1));
        cells.push_back(cell);
    }
}

// Основная функция
int main() {
    // Пример точек в 3D пространстве
    std::vector<Point> points = {
        Point(1.0, 2.0, 3.0),
        Point(4.0, 5.0, 6.0),
        Point(7.0, 8.0, 9.0),
        // Добавьте больше точек по необходимости
    };

    std::vector<VoronoiCell> cells;

    // Вызов функции разделения
    voronoiDivide(points, 0, points.size(), cells);

    // Вывод результатов
    for (const auto& cell : cells) {
        std::cout << "Voronoi Cell for Point (" << cell.site.x << ", " << cell.site.y << ", " << cell.site.z << "):\n";
        for (const auto& vertex : cell.vertices) {
            std::cout << "  Vertex: (" << vertex.x << ", " << vertex.y << ", " << vertex.z << ")\n";
        }
    }

    return 0;
}
