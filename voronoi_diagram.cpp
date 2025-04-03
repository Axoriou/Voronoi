//используетсся библиотека eigen для линейной алгебры
//вывод неправильный 
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

// Точка в 3D пространстве
struct Point3D {
    double x, y, z;

    Point3D(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}

    bool operator<(const Point3D& other) const {
        if (abs(x - other.x) > 1e-9) return x < other.x;
        if (abs(y - other.y) > 1e-9) return y < other.y;
        return z < other.z;
    }

    bool operator==(const Point3D& other) const {
        return abs(x - other.x) < 1e-9 && abs(y - other.y) < 1e-9 && abs(z - other.z) < 1e-9;
    }
};

// Чтение рёбер Делоне из файла
vector<pair<Point3D, Point3D>> readDelaunayEdges(const string& filename) {
    vector<pair<Point3D, Point3D>> edges;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return edges;
    }

    double x1, y1, z1, x2, y2, z2;
    while (file >> x1 >> y1 >> z1 >> x2 >> y2 >> z2) {
        edges.emplace_back(Point3D(x1, y1, z1), Point3D(x2, y2, z2));
    }
    return edges;
}

// Поиск всех тетраэдров в триангуляции Делоне
set<vector<Point3D>> findTetrahedra(const set<Point3D>& points, const vector<pair<Point3D, Point3D>>& edges) {
    set<vector<Point3D>> tetrahedra;
    vector<Point3D> pointList(points.begin(), points.end());
    map<Point3D, int> pointIndices;

    // Создаем индекс для каждой точки
    for (int i = 0; i < pointList.size(); ++i) {
        pointIndices[pointList[i]] = i;
    }

    // Создаем матрицу смежности
    vector<vector<bool>> adjMatrix(pointList.size(), vector<bool>(pointList.size(), false));
    for (const auto& edge : edges) {
        int i = pointIndices[edge.first];
        int j = pointIndices[edge.second];
        adjMatrix[i][j] = adjMatrix[j][i] = true;
    }

    // Оптимизированный поиск тетраэдров
    for (int i = 0; i < pointList.size(); ++i) {
        for (int j = i + 1; j < pointList.size(); ++j) {
            if (!adjMatrix[i][j]) continue;

            for (int k = j + 1; k < pointList.size(); ++k) {
                if (!adjMatrix[i][k] || !adjMatrix[j][k]) continue;

                for (int l = k + 1; l < pointList.size(); ++l) {
                    if (adjMatrix[i][l] && adjMatrix[j][l] && adjMatrix[k][l]) {
                        vector<Point3D> tet = { pointList[i], pointList[j], pointList[k], pointList[l] };
                        sort(tet.begin(), tet.end());
                        tetrahedra.insert(tet);
                    }
                }
            }
        }
    }

    return tetrahedra;
}

// Вычисление центра описанной сферы (circumcenter) для тетраэдра
Point3D computeCircumcenter(const vector<Point3D>& tet) {
    Matrix3d A;
    Vector3d b;
    const Point3D& p0 = tet[0];

    for (int i = 1; i < 4; ++i) {
        const Point3D& p = tet[i];
        A.row(i - 1) << p.x - p0.x, p.y - p0.y, p.z - p0.z;
        b(i - 1) = 0.5 * (pow(p.x, 2) - pow(p0.x, 2) +
            pow(p.y, 2) - pow(p0.y, 2) +
            pow(p.z, 2) - pow(p0.z, 2));
    }

    Vector3d x = A.colPivHouseholderQr().solve(b);
    Point3D center(x(0) + p0.x, x(1) + p0.y, x(2) + p0.z);

    cout << "Circumcenter for tetrahedron: ("
        << center.x << "," << center.y << "," << center.z << ")\n";
    return center;
}

// Построение рёбер диаграммы Вороного
void buildVoronoiEdges(const set<vector<Point3D>>& tetrahedra, const string& outputFilename) {
    ofstream out(outputFilename);
    if (!out.is_open()) {
        cerr << "Error opening output file: " << outputFilename << endl;
        return;
    }

    map<vector<Point3D>, Point3D> tetToCenter;

    // Вычисляем центры для всех тетраэдров
    for (const auto& tet : tetrahedra) {
        tetToCenter[tet] = computeCircumcenter(tet);
    }

    // Соединяем центры соседних тетраэдров (имеющих общую грань)
    for (auto it1 = tetrahedra.begin(); it1 != tetrahedra.end(); ++it1) {
        const auto& tet1 = *it1;
        const Point3D& c1 = tetToCenter[tet1];

        for (auto it2 = next(it1); it2 != tetrahedra.end(); ++it2) {
            const auto& tet2 = *it2;

            // Проверяем, что тетраэдры имеют ровно 3 общие точки (общую грань)
            int shared = 0;
            for (const auto& p : tet1) {
                if (find(tet2.begin(), tet2.end(), p) != tet2.end()) {
                    shared++;
                }
            }

            if (shared == 3) {
                const Point3D& c2 = tetToCenter[tet2];
                out << c1.x << " " << c1.y << " " << c1.z << " "
                    << c2.x << " " << c2.y << " " << c2.z << "\n";
            }
        }
    }
}

int main() {
    // Чтение рёбер Делоне
    string inputFile = "delaunay_edges.txt";
    auto edges = readDelaunayEdges(inputFile);

    if (edges.empty()) {
        cerr << "No edges read from file. Exiting." << endl;
        return 1;
    }

    // Извлекаем уникальные точки
    set<Point3D> points;
    for (const auto& edge : edges) {
        points.insert(edge.first);
        points.insert(edge.second);
    }

    cout << "Points count: " << points.size() << endl;
    cout << "Edges count: " << edges.size() << endl;

    // Находим все тетраэдры
    auto tetrahedra = findTetrahedra(points, edges);
    cout << "Found tetrahedra: " << tetrahedra.size() << endl;

    if (tetrahedra.empty()) {
        cerr << "No tetrahedra found. Check input data." << endl;
        return 1;
    }

    // Строим диаграмму Вороного
    string outputFile = "voronoi_edges.txt";
    buildVoronoiEdges(tetrahedra, outputFile);
    cout << "Voronoi diagram saved to: " << outputFile << endl;

    return 0;
}
