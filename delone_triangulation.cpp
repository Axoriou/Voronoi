#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

struct Point{
  
  double x,y;
  
};

void points_bubble_sort (vector<Point>& points ){ 

    for (int i=0;i<points.size()-1;i++){
      for (int j=i+1;j<points.size();j++){
        if (points[i].x>points[j].x || (points[i].x==points[j].x && points[i].y>points[j].y)){
          swap(points[i], points[j]);
        }
      }
    }#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

struct Point{
  
  double x,y;
  
};

void points_bubble_sort (vector<Point>& points ){ 

    for (int i=0;i<points.size()-1;i++){
      for (int j=i+1;j<points.size();j++){
        if (points[i].x>points[j].x || (points[i].x==points[j].x && points[i].y>points[j].y)){
          swap(points[i], points[j]);
        }
      }
    }
    
}

vector<std::vector<Point>> divideIntoTriplets(const vector<Point>& points) {
    vector<vector<Point>> triplets;

    // Проходим по вектору точек с шагом 3
    for (size_t i = 0; i < points.size(); i += 3) {
        vector<Point> triplet;

        // Добавляем до трех точек в текущую группу
        for (size_t j = 0; j < 3 && (i + j) < points.size(); ++j) {
            triplet.push_back(points[i + j]);
        }

        triplets.push_back(triplet);
    }

    // Если остались точки, добавляем их в новую группу
    if (points.size() % 3 != 0) {
        vector<Point> remainingPoints;
        for (size_t i = triplets.size() * 3; i < points.size(); ++i) {
            remainingPoints.push_back(points[i]);
        }
        triplets.push_back(remainingPoints);
    }

    return triplets;
}



int main() {

  vector <Point> hull;

  Point point;

  while (cin>>point.x>>point.y){
    
    hull.push_back(point);
    
  }

  points_bubble_sort (hull);

  for(Point point: hull){
    cout<<point.x<<" "<<point.y<<endl;
  }

  vector<vector<Point>> triplets = divideIntoTriplets(hull);

  for (const auto& triplet : triplets) {
    cout << "Triplet: ("
    << triplet[0].x << ", " << triplet[0].y << "), ("
    << triplet[1].x << ", " << triplet[1].y << "), ("
    << triplet[2].x << ", " << triplet[2].y << ")\n";
  }

  return 0;
}
    
}

vector<vector<Point>> divideIntoTriplets(vector<Point>& points) {
    vector<vector<Point>> triplets;

    // Проходим по вектору точек с шагом 3
    for (size_t i = 0; i + 2 < points.size(); i += 3) {
        // Создаем систему из трех точек
        vector<Point> triplet = { points[i], points[i + 1], points[i + 2] };
        triplets.push_back(triplet);
    }

    return triplets;
}


int main() {

  vector <Point> hull;

  Point point;

  while (cin>>point.x>>point.y){
    
    hull.push_back(point);
    
  }

  points_bubble_sort (hull);

  for(Point point: hull){
    cout<<point.x<<" "<<point.y<<endl;
  }

  vector<vector<Point>> triplets = divideIntoTriplets(hull);

  for (const auto& triplet : triplets) {
    cout << "Triplet: ("
    << triplet[0].x << ", " << triplet[0].y << "), ("
    << triplet[1].x << ", " << triplet[1].y << "), ("
    << triplet[2].x << ", " << triplet[2].y << ")\n";
  }

  return 0;
}
