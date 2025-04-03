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

// Структура для представления 3D-точки
struct Point3D {
    double x, y, z;

    
    Point3D(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}

    // Оператор < для сортировки точек по координатам
    bool operator<(const Point3D& other) const {
        if (x != other.x) return x < other.x;
        if (y != other.y) return y < other.y;
        return z < other.z;
    }

    // Оператор == для сравнения точек
    bool operator==(const Point3D& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

// Функция для чтения рёбер Делоне из файла
vector<pair<Point3D, Point3D>> readDelaunayEdges(const string& filename) {
    vector<pair<Point3D, Point3D>> edges; // Вектор для хранения рёбер
    ifstream file(filename); 
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return edges;
    }

    double x1, y1, z1, x2, y2, z2;
    while (file >> x1 >> y1 >> z1 >> x2 >> y2 >> z2) {
        edges.emplace_back(Point3D(x1, y1, z1), Point3D(x2, y2, z2)); // Чтение рёбер и добавление в список
    }
    return edges; 
}

// Функция для поиска тетраэдров, образованных точками и рёбрами
set<vector<Point3D>> findTetrahedra(const set<Point3D>& points, const vector<pair<Point3D, Point3D>>& edges) {
    set<vector<Point3D>> tetrahedra; // Множество для хранения уникальных тетраэдров
    vector<Point3D> pointList(points.begin(), points.end()); // Преобразуем множество точек в вектор
    map<Point3D, int> pointIndices; // мапа для индексации точек
    int n = pointList.size(); // Количество точек

    // Индексация точек
    for (int i = 0; i < n; ++i) {
        pointIndices[pointList[i]] = i;
    }

    // Создаём матрицу смежности, где true означает, что точки соединены рёбром
    vector<vector<bool>> adjMatrix(n, vector<bool>(n, false));
    for (const auto& edge : edges) {
        int i = pointIndices[edge.first];
        int j = pointIndices[edge.second];
        adjMatrix[i][j] = adjMatrix[j][i] = true; // Заполняем матрицу смежности
    }

    // Оптимизированный поиск тетраэдров
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (!adjMatrix[i][j]) continue; // Пропускаем, если между точками нет ребра

            for (int k = j + 1; k < n; ++k) {
                if (!adjMatrix[i][k] || !adjMatrix[j][k]) continue; // Пропускаем, если ребра нет

                for (int l = k + 1; l < n; ++l) {
                    if (adjMatrix[i][l] && adjMatrix[j][l] && adjMatrix[k][l]) {
                        // Если все 4 точки соединены рёбрами, то это тетраэдр
                        vector<Point3D> tet = { pointList[i], pointList[j], pointList[k], pointList[l] };
                        sort(tet.begin(), tet.end()); // Сортируем точки для унификации
                        tetrahedra.insert(tet); // Добавляем тетраэдр в множество
                    }
                }
            }
        }
    }

    return tetrahedra; // Возвращаем найденные тетраэдры
}

// Функция для вычисления циркумцентра тетраэдра
Point3D computeCircumcenter(const vector<Point3D>& tet) {
    Matrix3d A; // Матрица для системы линейных уравнений
    Vector3d b; // Вектор правых частей уравнений
    const Point3D& p0 = tet[0]; // Первая точка тетраэдра

    // Заполнение матрицы и вектора для решения системы
    for (int i = 1; i < 4; ++i) {
        const Point3D& p = tet[i];
        A.row(i - 1) << p.x - p0.x, p.y - p0.y, p.z - p0.z;
        b(i - 1) = (pow(p.x, 2) - pow(p0.x, 2) + pow(p.y, 2) - pow(p0.y, 2) + pow(p.z, 2) - pow(p0.z, 2));
        b(i - 1) *= 0.5; // Разделяем на 2, чтобы решить систему
    }

    Vector3d x = A.colPivHouseholderQr().solve(b); // Решаем систему линейных уравнений
    Point3D center(x(0) + p0.x, x(1) + p0.y, x(2) + p0.z); // Вычисляем центр

    return center; 
}

// Функция для построения рёбер диаграммы Вороного и записи их в файл
void buildVoronoiEdges(const set<vector<Point3D>>& tetrahedra, const string& outputFilename) {
    ofstream out(outputFilename); 
    if (!out.is_open()) {
        cerr << "Error opening output file: " << outputFilename << endl; 
        return;
    }

    map<vector<Point3D>, Point3D> tetToCenter; // Карта для хранения циркумцентров тетраэдров
    vector<vector<Point3D>> tetrahedraList(tetrahedra.begin(), tetrahedra.end()); // Список тетраэдров
    int n = tetrahedraList.size(); 

    // Вычисление циркумцентров для всех тетраэдров
    for (const auto& tet : tetrahedra) {
        tetToCenter[tet] = computeCircumcenter(tet);
    }

    // Проверка соседства тетраэдров
    for (int i = 0; i < n; ++i) {
        const auto& tet1 = tetrahedraList[i];
        const Point3D& c1 = tetToCenter[tet1]; // Циркумцентр первого тетраэдра

        for (int j = i + 1; j < n; ++j) {
            const auto& tet2 = tetrahedraList[j];
            int shared = 0; // Счётчик общих точек

            // Проверяем, сколько общих точек у двух тетраэдров
            for (const auto& p : tet1) {
                if (find(tet2.begin(), tet2.end(), p) != tet2.end()) {
                    shared++;
                }
            }

            // Если 3 общие точки, то тетраэдры соседние и нужно записать их циркумцентры
            if (shared == 3) {
                const Point3D& c2 = tetToCenter[tet2];
                out << c1.x << " " << c1.y << " " << c1.z << " "
                    << c2.x << " " << c2.y << " " << c2.z << "\n";
            }
        }
    }
}


int main() {
    string inputFile = "delaunay_edges.txt"; 
    auto edges = readDelaunayEdges(inputFile); 

    if (edges.empty()) {
        cerr << "No edges read from file. Exiting." << endl; 
        return 1;
    }

    set<Point3D> points; 
    for (const auto& edge : edges) {
        points.insert(edge.first); 
        points.insert(edge.second); 
    }

    cout << "Points count: " << points.size() << endl; 
    cout << "Edges count: " << edges.size() << endl;

    auto tetrahedra = findTetrahedra(points, edges); 
    cout << "Found tetrahedra: " << tetrahedra.size() << endl; 

    if (tetrahedra.empty()) {
        cerr << "No tetrahedra found. Check input data." << endl; 
        return 1;
    }

    string outputFile = "voronoi_edges.txt"; 
    buildVoronoiEdges(tetrahedra, outputFile); 
    cout << "Voronoi diagram saved to: " << outputFile << endl; 

    return 0; 
}
