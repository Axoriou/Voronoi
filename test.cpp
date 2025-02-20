#include <iostream>
#include <vector>
#include <algorithm>

// Структура для представления точки в 3D пространстве
struct Point {
    double x, y, z;

    Point(double x, double y, double z) : x(x), y(y), z(z) {}
};

// Функция для преобразования точки из одного базиса в другой
Point transformPoint(const Point& p, const std::vector<std::vector<double>>& transformationMatrix) {
    double newX = transformationMatrix[0][0] * p.x + transformationMatrix[0][1] * p.y + transformationMatrix[0][2] * p.z;
    double newY = transformationMatrix[1][0] * p.x + transformationMatrix[1][1] * p.y + transformationMatrix[1][2] * p.z;
    double newZ = transformationMatrix[2][0] * p.x + transformationMatrix[2][1] * p.y + transformationMatrix[2][2] * p.z;

    return Point(newX, newY, newZ);
}

// Функция для разбиения пространства на ячейки Вороного
void voronoiDivide(std::vector<Point>& points, std::vector<std::vector<Point>>& voronoiCells) {
    // Реализация алгоритма "разделяй и властвуй" для построения ячеек Вороного
    // Здесь будет ваша логика для разбиения пространства
}

// Основная функция
int main() {
    // Пример точек, которые будут использоваться для построения ячеек Вороного
    std::vector<Point> points = {
        Point(1.0, 2.0, 3.0),
        Point(4.0, 5.0, 6.0),
        Point(7.0, 8.0, 9.0)
    };

    // Вектор для хранения ячеек Вороного
    std::vector<std::vector<Point>> voronoiCells;

    // Разбиение пространства на ячейки Вороного
    voronoiDivide(points, voronoiCells);

    // Пример матрицы преобразования (замените на ваши значения)
    std::vector<std::vector<double>> transformationMatrix = {
        {1, 0, 0},  // Вектор A
        {0, 1, 0},  // Вектор B
        {0, 0, 1}   // Вектор C
    };

    // Преобразование точек в новый базис
    for (const auto& point : points) {
        Point transformedPoint = transformPoint(point, transformationMatrix);
        std::cout << "Transformed Point: (" << transformedPoint.x << ", " << transformedPoint.y << ", " << transformedPoint.z << ")\n";
    }

    return 0;
}
