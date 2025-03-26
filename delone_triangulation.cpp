#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <set>
#include <array>
#include <limits>

// Точка в 3D пространстве
struct Point3D {
    double x, y, z;

    Point3D(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}

    bool operator==(const Point3D& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator<(const Point3D& other) const {
        if (x != other.x) return x < other.x;
        if (y != other.y) return y < other.y;
        return z < other.z;
    }
};

// Ребро триангуляции
struct Edge {
    Point3D p1, p2;

    Edge(const Point3D& p1, const Point3D& p2) : p1(p1), p2(p2) {}

    bool operator==(const Edge& other) const {
        return (p1 == other.p1 && p2 == other.p2) || (p1 == other.p2 && p2 == other.p1);
    }

    bool operator<(const Edge& other) const {
        if (!(p1 == other.p1)) return p1 < other.p1;
        return p2 < other.p2;
    }
};

// Тетраэдр (4 точки)
struct Tetrahedron {
    Point3D p[4];

    Tetrahedron(const Point3D& p0, const Point3D& p1,
        const Point3D& p2, const Point3D& p3) {
        p[0] = p0; p[1] = p1; p[2] = p2; p[3] = p3;
    }
};

// Чтение точек из файла
std::vector<Point3D> readPoints(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<Point3D> points;
    double x, y, z;

    while (file >> x >> y >> z) {
        points.emplace_back(x, y, z);
    }

    return points;
}

// Вычисление определителя матрицы 4x4
double determinant4x4(const std::array<std::array<double, 4>, 4>& mat) {
    double det = 0;

    for (int i = 0; i < 4; ++i) {
        double minor[3][3];
        for (int j = 1; j < 4; ++j) {
            int col = 0;
            for (int k = 0; k < 4; ++k) {
                if (k == i) continue;
                minor[j - 1][col++] = mat[j][k];
            }
        }

        double minorDet = minor[0][0] * (minor[1][1] * minor[2][2] - minor[1][2] * minor[2][1])
            - minor[0][1] * (minor[1][0] * minor[2][2] - minor[1][2] * minor[2][0])
            + minor[0][2] * (minor[1][0] * minor[2][1] - minor[1][1] * minor[2][0]);

        det += (i % 2 == 0 ? 1 : -1) * mat[0][i] * minorDet;
    }

    return det;
}

// Проверка, находится ли точка внутри описанной сферы тетраэдра
bool inSphere(const Tetrahedron& tet, const Point3D& point) {
    std::array<std::array<double, 5>, 5> matrix;

    for (int i = 0; i < 4; ++i) {
        const Point3D& p = tet.p[i];
        matrix[i][0] = p.x;
        matrix[i][1] = p.y;
        matrix[i][2] = p.z;
        matrix[i][3] = p.x * p.x + p.y * p.y + p.z * p.z;
        matrix[i][4] = 1;
    }

    const Point3D& p = point;
    matrix[4][0] = p.x;
    matrix[4][1] = p.y;
    matrix[4][2] = p.z;
    matrix[4][3] = p.x * p.x + p.y * p.y + p.z * p.z;
    matrix[4][4] = 1;

    // Вычисляем определитель матрицы 5x5 через разложение по последней строке
    double det = 0;
    for (int col = 0; col < 5; ++col) {
        std::array<std::array<double, 4>, 4> minor;
        for (int i = 0; i < 4; ++i) {
            int minor_col = 0;
            for (int j = 0; j < 5; ++j) {
                if (j == col) continue;
                minor[i][minor_col++] = matrix[i][j];
            }
        }

        double minorDet = determinant4x4(minor);
        det += (col % 2 == 0 ? 1 : -1) * matrix[4][col] * minorDet;
    }

    return det > 0;
}

// Построение выпуклой оболочки (упрощенный QuickHull)
std::vector<Tetrahedron> convexHull3D(const std::vector<Point3D>& points) {
    if (points.size() < 4) return {};

    // Находим начальный тетраэдр
    Tetrahedron initialTet(points[0], points[1], points[2], points[3]);
    if (determinant4x4({ {
        {points[0].x, points[0].y, points[0].z, 1},
        {points[1].x, points[1].y, points[1].z, 1},
        {points[2].x, points[2].y, points[2].z, 1},
        {points[3].x, points[3].y, points[3].z, 1}
    } }) == 0) {
        // Точки копланарны, ищем другую комбинацию
        for (size_t i = 0; i < points.size(); ++i) {
            for (size_t j = i + 1; j < points.size(); ++j) {
                for (size_t k = j + 1; k < points.size(); ++k) {
                    for (size_t l = k + 1; l < points.size(); ++l) {
                        Tetrahedron tet(points[i], points[j], points[k], points[l]);
                        if (determinant4x4({ {
                            {points[i].x, points[i].y, points[i].z, 1},
                            {points[j].x, points[j].y, points[j].z, 1},
                            {points[k].x, points[k].y, points[k].z, 1},
                            {points[l].x, points[l].y, points[l].z, 1}
                        } }) != 0) {
                            initialTet = tet;
                            goto found_tet;
                        }
                    }
                }
            }
        }
        // Если все точки копланарны, возвращаем пустой результат
        return {};
    }

found_tet:
    std::vector<Tetrahedron> hull = { initialTet };
    std::vector<Point3D> outsidePoints;

    // Разделяем точки на внутренние и внешние
    for (const auto& point : points) {
        bool inside = true;
        for (const auto& tet : hull) {
            if (inSphere(tet, point)) {
                inside = false;
                break;
            }
        }
        if (!inside) {
            outsidePoints.push_back(point);
        }
    }

    // Добавляем внешние точки к оболочке
    for (const auto& point : outsidePoints) {
        std::vector<Tetrahedron> newHull;
        std::set<Edge> visibleEdges;

        for (const auto& tet : hull) {
            if (inSphere(tet, point)) {
                // Добавляем грани тетраэдра в видимые рёбра
                for (int i = 0; i < 4; ++i) {
                    for (int j = i + 1; j < 4; ++j) {
                        Edge edge(tet.p[i], tet.p[j]);
                        if (visibleEdges.count(edge)) {
                            visibleEdges.erase(edge);
                        }
                        else {
                            visibleEdges.insert(edge);
                        }
                    }
                }
            }
            else {
                newHull.push_back(tet);
            }
        }

        // Создаем новые тетраэдры из видимых рёбер и точки
        for (const auto& edge : visibleEdges) {
            newHull.emplace_back(edge.p1, edge.p2, point, point);
        }

        hull = newHull;
    }

    return hull;
}

// Основная функция триангуляции Делоне
std::vector<Edge> delaunayTriangulation3D(std::vector<Point3D>& points) {
    if (points.size() < 4) return {};

    // Сортируем точки по x-координате
    std::sort(points.begin(), points.end(), [](const Point3D& a, const Point3D& b) {
        return a.x < b.x;
        });

    // Базовый случай: 4 точки образуют тетраэдр
    if (points.size() == 4) {
        std::vector<Edge> edges;
        edges.emplace_back(points[0], points[1]);
        edges.emplace_back(points[0], points[2]);
        edges.emplace_back(points[0], points[3]);
        edges.emplace_back(points[1], points[2]);
        edges.emplace_back(points[1], points[3]);
        edges.emplace_back(points[2], points[3]);
        return edges;
    }

    // Разделяем точки на две половины
    size_t mid = points.size() / 2;
    std::vector<Point3D> left(points.begin(), points.begin() + mid);
    std::vector<Point3D> right(points.begin() + mid, points.end());

    // Рекурсивно строим триангуляцию для каждой половины
    auto leftEdges = delaunayTriangulation3D(left);
    auto rightEdges = delaunayTriangulation3D(right);

    // Объединяем рёбра
    std::set<Edge> mergedEdges(leftEdges.begin(), leftEdges.end());
    mergedEdges.insert(rightEdges.begin(), rightEdges.end());

    // Строим выпуклые оболочки для левой и правой частей
    auto leftHull = convexHull3D(left);
    auto rightHull = convexHull3D(right);

    // Находим рёбра, соединяющие левую и правую части
    for (const auto& lh : leftHull) {
        for (const auto& rh : rightHull) {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    Edge edge(lh.p[i], rh.p[j]);

                    // Проверяем условие Делоне для нового ребра
                    bool isDelaunay = true;
                    for (const auto& tet : leftHull) {
                        if (inSphere(tet, rh.p[j])) {
                            isDelaunay = false;
                            break;
                        }
                    }
                    for (const auto& tet : rightHull) {
                        if (inSphere(tet, lh.p[i])) {
                            isDelaunay = false;
                            break;
                        }
                    }

                    if (isDelaunay) {
                        mergedEdges.insert(edge);
                    }
                }
            }
        }
    }

    return std::vector<Edge>(mergedEdges.begin(), mergedEdges.end());
}

// Вывод рёбер в консоль
void printEdges(const std::vector<Edge>& edges) {
    for (const auto& edge : edges) {
        std::cout << edge.p1.x << " " << edge.p1.y << " " << edge.p1.z << " "
            << edge.p2.x << " " << edge.p2.y << " " << edge.p2.z << "\n";
    }
}

int main() {
    std::vector<Point3D> points = readPoints("input.txt");

    if (points.size() < 4) {
        std::cerr << "Error: Need at least 4 points for 3D Delaunay triangulation\n";
        return 1;
    }

    auto edges = delaunayTriangulation3D(points);
    printEdges(edges);

    return 0;
}
