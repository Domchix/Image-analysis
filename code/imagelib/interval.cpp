#include "image.hpp"

Interval::Interval(Eigen::Vector2f singleVec, std::string side, uint16 L){
  if (side == "left"){
    _left = singleVec;
    _right = Eigen::Vector2f{(float)(L - 1), (float)(L - 1)};
  }
  else if (side == "right"){
    _right = singleVec;
    _left = Eigen::Vector2f{0, 0};
  }
  else {
    std::cout << "Side selection must be \"left\" or \"right\"" << std::endl;
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
  //std::cout << x << " " << x0 << " " << y0 << " " << x1 << " " << y1 << " ";

  if (x == x0) { 
    //std::cout << y0 << std::endl;
    return y0; 
  }
  else if ( x == x1 ) { 
    //std::cout << y1 << std::endl;
    return y1; 
  }
  else { 
    //std::cout << y0 + ((y1-y0)/(x1-x0)) * (x - x0) << std::endl;
    return (y0 + ((y1-y0)/(x1-x0)) * (x - x0)); 
  }
}

unsigned int Interval::threshold(uint32 x){
  // float x0 = _left(0);
  // float x1 = _right(0);
  // float y0 = _left(1);
  // float y1 = _right(1);
  // std::cout << x << " " << x0 << " " << y0 << " " << x1 << " " << y1 << " " << y1 << std::endl;
  return _left(1);
}

Eigen::Vector2f Interval::getLeft(){ return _left; }
Eigen::Vector2f Interval::getRight(){ return _right; }