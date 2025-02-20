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
    }
    
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

  return 0;
}
