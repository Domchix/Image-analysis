#include "image.hpp"

Interval::Interval(Eigen::Vector2f singleVec, std::string side, uint16 L){
  if (side == "l"){
    _left = singleVec;
    _right = Eigen::Vector2f{(float)(L - 1), (float)(L - 1)};
  }
  else if (side == "r"){
    _right = singleVec;
    _left = Eigen::Vector2f{0, 0};
  }
}

Interval::Interval(Eigen::Vector2f left, Eigen::Vector2f right){
  _left = left;
  _right = right;
}

float Interval::linearInterpolation(float x){
  float x0 = _left(0);
  float x1 = _right(0);
  float y0 = _left(1);
  float y1 = _right(1);

  if (x == x0) { 
    return y0; 
  }
  else if ( x == x1 ) { 
    return y1; 
  }
  else { 
    return (y0 + ((y1-y0)/(x1-x0)) * (x - x0)); 
  }
}

unsigned int Interval::threshold(){
  return _left(1);
}

Eigen::Vector2f Interval::getLeft(){ return _left; }
Eigen::Vector2f Interval::getRight(){ return _right; }