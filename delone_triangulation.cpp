#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

struct Point{
  
  double x,y;
  
};

Point middle (Point p1, Point p2, Point p3){

    Point MID;

    double D=2*(p1.x*(p2.y-p3.y)+p2.x*(p3.y-p1.y)+p3.x*(p1.y-p2.y));

    MID.x=((p1.x**2+p1.y**2)*(p2.y-p3.y)+(p2.x**2+p2.y**2)*(p3.y-p1.y)+(p3.x**2+p3.y**2)*(p1.y-p2.y))/D;

    MID.y=((p1.x**2+p1.y**2)*(p3.x-p2.x)+(p2.x**2+p2.y**2)*(p1.x-p3.x)+(p3.x**2+p3.y**2)*(p2.x-p1.x))/D;

    return MID;
    
}

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
