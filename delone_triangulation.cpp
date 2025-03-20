#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>  // Подключение заголовочного файла для работы с строками

using namespace std;

struct Point {
    double x, y, z;  // Добавлена координата z
};

// Функция для вычисления центра описанной окружности в 3D
Point middle(Point p1, Point p2, Point p3) {
    Point MID;
    double D = 2 * (p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y));

    MID.x = ((pow(p1.x, 2) + pow(p1.y, 2) + pow(p1.z, 2)) * (p2.y - p3.y) +
        (pow(p2.x, 2) + pow(p2.y, 2) + pow(p2.z, 2)) * (p3.y - p1.y) +
        (pow(p3.x, 2) + pow(p3.y, 2) + pow(p3.z, 2)) * (p1.y - p2.y)) / D;

    MID.y = ((pow(p1.x, 2) + pow(p1.y, 2) + pow(p1.z, 2)) * (p3.x - p2.x) +
        (pow(p2.x, 2) + pow(p2.y, 2) + pow(p2.z, 2)) * (p1.x - p3.x) +
        (pow(p3.x, 2) + pow(p3.y, 2) + pow(p3.z, 2)) * (p2.x - p1.x)) / D;

    MID.z = ((pow(p1.x, 2) + pow(p1.y, 2) + pow(p1.z, 2)) * (p2.z - p3.z) +
        (pow(p2.x, 2) + pow(p2.y, 2) + pow(p2.z, 2)) * (p3.z - p1.z) +
        (pow(p3.x, 2) + pow(p3.y, 2) + pow(p3.z, 2)) * (p1.z - p2.z)) / D;

    return MID;
}

// Сортировка точек с использованием стандартной сортировки
void points_sort(vector<Point>& points) {
    sort(points.begin(), points.end(), [](const Point& p1, const Point& p2) {
        return (p1.x < p2.x) || (p1.x == p2.x && p1.y < p2.y) || (p1.x == p2.x && p1.y == p2.y && p1.z < p2.z);
        });
}

// Функция для деления на тройки
vector<vector<Point>> divideIntoTriplets(const vector<Point>& points) {
    vector<vector<Point>> triplets;

    for (size_t i = 0; i < points.size(); i += 3) {
        vector<Point> triplet;
        // Добавляем до трех точек в текущую группу
        for (size_t j = 0; j < 3 && (i + j) < points.size(); ++j) {
            triplet.push_back(points[i + j]);
        }
        triplets.push_back(triplet);
    }

    return triplets;
}

// Функция для чтения точек из файла с пропуском первых двух строк
bool readPointsFromFile(const string& filename, vector<Point>& points) {
    ifstream inputFile(filename);

    if (!inputFile) {
        cerr << "Ошибка открытия файла: " << filename << endl;
        return false;
    }

    // Пропускаем первые две строки
    string line;
    getline(inputFile, line);  // Пропускаем первую строку
    getline(inputFile, line);  // Пропускаем вторую строку

    // Считываем оставшиеся точки
    Point point;
    while (inputFile >> point.x >> point.y >> point.z) {
        points.push_back(point);
    }

    inputFile.close();
    return true;
}

int main() {
    vector<Point> points;

    // Чтение точек из файла
    string filename = "points.txt"; // Укажите путь к вашему файлу с точками
    if (!readPointsFromFile(filename, points)) {
        return 1;  // Если ошибка чтения файла, завершаем выполнение программы
    }

    // Сортировка точек
    points_sort(points);

    // Вывод отсортированных точек
    for (const Point& point : points) {
        cout << point.x << " " << point.y << " " << point.z << endl;
    }

    // Разделение точек на тройки
    vector<vector<Point>> triplets = divideIntoTriplets(points);

    // Вывод триплетов (в том числе тех, в которых меньше 3 точек)
    for (const auto& triplet : triplets) {
        cout << "Triplet: ";
        for (size_t i = 0; i < triplet.size(); ++i) {
            cout << "("
                << triplet[i].x << ", " << triplet[i].y << ", " << triplet[i].z << ")";
            if (i < triplet.size() - 1) cout << ", ";
        }
        cout << "\n";
    }

    return 0;
}
