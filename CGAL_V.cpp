#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <map>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_3<K> Delaunay;
typedef Delaunay::Point Point;
typedef Delaunay::Vertex_handle Vertex_handle;
typedef Delaunay::Cell_handle Cell_handle;

int main() {
    std::vector<Point> points;
    std::ifstream input("input.txt");
    if (!input) {
        std::cerr << "Ошибка: файл input.txt не найден!" << std::endl;
        return 1;
    }

    double x, y, z;
    while (input >> x >> y >> z) {
        points.push_back(Point(x, y, z));
    }
    input.close();

    if (points.empty()) {
        std::cerr << "Ошибка: файл input.txt пуст!" << std::endl;
        return 1;
    }

    //Построение триангуляции Делоне
    Delaunay dt(points.begin(), points.end());

    std::ofstream voro_out("voronoi.off");
    if (!voro_out) {
        std::cerr << "Ошибка создания файла voronoi.off!" << std::endl;
        return 1;
    }

    //Сохранение вершин Вороного (центры тетраэдров Делоне)
    std::map<Cell_handle, size_t> cell_to_index;
    size_t vertex_count = 0;
    std::vector<K::Point_3> voronoi_vertices;

    for (Delaunay::Finite_cells_iterator it = dt.finite_cells_begin();
        it != dt.finite_cells_end(); ++it) {
        K::Point_3 v = dt.dual(it);
        voronoi_vertices.push_back(v);
        cell_to_index[it] = vertex_count++;
    }

    //Построение граней Вороного (ячеек)
    std::vector<std::vector<size_t>> faces;

    for (Delaunay::Finite_vertices_iterator vit = dt.finite_vertices_begin();
        vit != dt.finite_vertices_end(); ++vit) {
        std::vector<Cell_handle> incident_cells;
        dt.finite_incident_cells(vit, std::back_inserter(incident_cells));

        std::vector<size_t> face;
        for (Cell_handle c : incident_cells) {
            face.push_back(cell_to_index[c]);
        }
        faces.push_back(face);
    }

    //Запись в OFF-формат
    voro_out << "OFF\n"
        << voronoi_vertices.size() << " "
        << faces.size() << " 0\n";  // Вершины, грани, рёбра 

    for (const auto& v : voronoi_vertices) {
        voro_out << v.x() << " " << v.y() << " " << v.z() << "\n";
    }

    for (const auto& face : faces) {
        voro_out << face.size();
        for (size_t idx : face) {
            voro_out << " " << idx;
        }
        voro_out << "\n";
    }

    voro_out.close();

    return 0;
}
