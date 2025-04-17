#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <map>
#include <set>
#include <array>

using namespace std;


string input_filename = "input.txt";
string output_filename = "output.off";


typedef CGAL::Exact_predicates_exact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_3<K> Delaunay;
typedef Delaunay::Point Point;
typedef Delaunay::Cell_handle Cell_handle;
typedef Delaunay::Vertex_handle Vertex_handle;

// Чтение точек
void read_points(vector<Point>& Points) {
    ifstream input(input_filename);
    if (!input) {
        cout << "Ошибка: файл input.txt не найден!" << endl;
        return;
    }
    double x, y, z;
    while (input >> x >> y >> z) {
        Points.push_back(Point(x, y, z));
    }
    input.close();

    if (Points.empty()) {
        cout << "Ошибка: файл input.txt пуст!" << endl;
    }
}

int main() {
    vector<Point> Points;
    read_points(Points);
    if (Points.empty()) return 1;

    Delaunay dt(Points.begin(), Points.end());

    cout << "Введено точек: " << Points.size() << endl;
    int finite_vertex_count = 0;
    for (auto v = dt.finite_vertices_begin(); v != dt.finite_vertices_end(); ++v) {
        ++finite_vertex_count;
    }
    cout << "Финитных вершин в триангуляции: " << finite_vertex_count << endl;


    // Сопоставление вершина -> индекс
    map<Vertex_handle, int> vertex_indices;
    vector<Point> vertices;
    int index = 0;
    for (auto v = dt.finite_vertices_begin(); v != dt.finite_vertices_end(); ++v) {
        vertex_indices[v] = index++;
        vertices.push_back(v->point());
    }

    // Грани тетраэдров
    set<array<int, 3>> faces;
    for (auto cell = dt.finite_cells_begin(); cell != dt.finite_cells_end(); ++cell) {
        for (int i = 0; i < 4; ++i) {
            array<int, 3> face;
            int k = 0;
            for (int j = 0; j < 4; ++j) {
                if (j != i) {
                    face[k++] = vertex_indices[cell->vertex(j)];
                }
            }
            sort(face.begin(), face.end());
            faces.insert(face);
        }
    }

    // Запись в OFF
    ofstream output(output_filename);
    if (!output) {
        cout << "Ошибка: не удалось создать output.off" << endl;
        return 1;
    }

    output << "OFF\n";
    output << vertices.size() << " " << faces.size() << " 0\n";

    for (const auto& p : vertices) {
        // CGAL с точным кернелом использует типы как CGAL::Gmpq
        output << CGAL::to_double(p.x()) << " "
            << CGAL::to_double(p.y()) << " "
            << CGAL::to_double(p.z()) << "\n";
    }

    for (const auto& f : faces) {
        output << "3 " << f[0] << " " << f[1] << " " << f[2] << "\n";
    }

    output.close();
    cout << "Файл " << output_filename << " успешно создан!" << endl;

    return 0;
}
