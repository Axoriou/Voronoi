#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/convex_hull_3.h>

#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <array>
#include <iostream>

using namespace std;

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_3<K> Delaunay;
typedef Delaunay::Point Point;
typedef Delaunay::Cell_handle Cell_handle;
typedef Delaunay::Vertex_handle Vertex_handle;
typedef CGAL::Polyhedron_3<K> Polyhedron;

string input_filename = "input.txt";
string output_filename = "output.off";

void read_points(vector<Point>& points) {
    ifstream input(input_filename);
    if (!input) {
        cerr << "Ошибка: не удалось открыть input.txt" << endl;
        return;
    }

    double x, y, z;
    while (input >> x >> y >> z) {
        points.push_back(Point(x, y, z));
    }

    if (points.empty()) {
        cerr << "Ошибка: файл пуст" << endl;
    }
}

int main() {
    vector<Point> points;
    read_points(points);
    if (points.empty()) return 1;

    // Строим триангуляцию
    Delaunay dt;
    dt.insert(points.begin(), points.end());

    vector<Point> voronoi_vertices;
    vector<array<int, 3>> voronoi_faces;
    map<Point, int> vertex_to_index;
    int next_index = 0;

    for (auto vit = dt.finite_vertices_begin(); vit != dt.finite_vertices_end(); ++vit) {
        vector<Cell_handle> incident_cells;
        dt.finite_incident_cells(vit, back_inserter(incident_cells));

        vector<Point> cell_centers;
        for (auto cell : incident_cells) {
            if (!dt.is_infinite(cell)) {
                Point center = dt.dual(cell); // центр описанной сферы
                cell_centers.push_back(center);
            }
        }

        if (cell_centers.size() < 4) continue;

        // Строим выпуклую оболочку из центров
        Polyhedron poly;
        CGAL::convex_hull_3(cell_centers.begin(), cell_centers.end(), poly);

        // Сохраняем вершины
        for (auto v = poly.points_begin(); v != poly.points_end(); ++v) {
            if (!vertex_to_index.count(*v)) {
                vertex_to_index[*v] = next_index++;
                voronoi_vertices.push_back(*v);
            }
        }

        // Сохраняем грани
        for (auto f = poly.facets_begin(); f != poly.facets_end(); ++f) {
            auto h = f->facet_begin();
            vector<int> indices;
            do {
                indices.push_back(vertex_to_index[h->vertex()->point()]);
            } while (++h != f->facet_begin());

            // Триангулируем многоугольную грань
            for (size_t i = 1; i + 1 < indices.size(); ++i) {
                voronoi_faces.push_back({ indices[0], indices[i], indices[i + 1] });
            }
        }
    }

    // Запись в OFF
    ofstream output(output_filename);
    if (!output) {
        cerr << "Ошибка: не удалось создать файл output.off" << endl;
        return 1;
    }

    output << "OFF\n";
    output << voronoi_vertices.size() << " " << voronoi_faces.size() << " 0\n";
    for (const auto& p : voronoi_vertices)
        output << p.x() << " " << p.y() << " " << p.z() << "\n";

    for (const auto& f : voronoi_faces)
        output << "3 " << f[0] << " " << f[1] << " " << f[2] << "\n";

    output.close();
    cout << "Диаграмма Вороного успешно записана в " << output_filename << endl;
    return 0;
}
