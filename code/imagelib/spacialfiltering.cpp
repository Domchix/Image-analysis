#include "image.hpp"

float applyMask(uint32 filterWidth, unsigned char* imgData, uint32 imgWidth, uint32 imgHeight, int x, int y, int* W){
  float maskProduct = 0;
  int a = (filterWidth - 1) / 2;

  for (int t = -a; t <= a; t++)
    for (int s = -a; s <= a; s++){
      int fx = x+s;
      int fy = y+t;
      if (fx > 0 && fy > 0 && fx < imgWidth && fy < imgHeight){
        maskProduct += W[ (t+a)*filterWidth + (s+a) ] * (int)imgData[ fy*imgWidth + fx ];
      }
      else {
        maskProduct += 0;
      }
    }
  return maskProduct;
}

void Image::imageBlurring(uint32 filterWidth){
  if (filterWidth % 2 == 0){
    std::cout << "Filter width must be an odd number";
    return;
  }

  uint32 imgSize = _height * _width * _channels;
  uint32 filterSize = filterWidth * filterWidth;

  int* W = new int[filterSize];
  for (uint32 i = 0; i < filterSize; i++){
    W[i] = 1;
  }
  float* tempImg = new float[imgSize];
  for (int y = 0; y < _height; y++)
    for (int x = 0; x < _width; x++){
      int a = (filterWidth - 1) / 2;
      uint32 sumNeighbourhood = applyMask(filterWidth, _data, _width, _height, x, y, W);
      float avg = (float)sumNeighbourhood / (float)filterSize;
      tempImg[y*_width + x] = avg;
    }

  for (uint32 i = 0; i < imgSize; i++){
    _data[i] = static_cast<unsigned char>((int)round(tempImg[i]));
  }
  delete(tempImg);
  updateHistogram();
}

void Image::sharpeningUnsharpMask(uint16 blurringFilterWidth, uint8 k){
  uint16 L = pow(2, _bps);
  uint32 imgSize = _height * _width * _channels;
  float* original = new float[imgSize];
  float* tempImg = new float[imgSize];

  for(uint32 i = 0; i < imgSize; i++){
    original[i] = static_cast<float>(_data[i]);
  }

  float max = std::numeric_limits<float>::min();
  float min = std::numeric_limits<float>::min();
  imageBlurring(blurringFilterWidth);
  for(uint32 i = 0; i < imgSize; i++){
    tempImg[i] = original[i] - _data[i];
    // tempImg[i] = original[i] + k*tempImg[i];
    if (tempImg[i] > max) max = tempImg[i];
    if (tempImg[i] < min) min = tempImg[i];
  }
  for(uint32 i = 0; i < imgSize; i++){
    tempImg[i] = ((L-1)*(tempImg[i] - min)) / (max - min);
    // _data[i] = static_cast<unsigned char>((int)round(tempImg[i]));
  }
  
  max = std::numeric_limits<float>::min();
  min = std::numeric_limits<float>::max();
  imageBlurring(blurringFilterWidth);
  for(uint32 i = 0; i < imgSize; i++){
    tempImg[i] = original[i] + k*tempImg[i];
    if (tempImg[i] > max) max = tempImg[i];
    if (tempImg[i] < min) min = tempImg[i];
  }
  for(uint32 i = 0; i < imgSize; i++){
    tempImg[i] = ((L-1)*(tempImg[i] - min)) / (max - min);
    _data[i] = static_cast<unsigned char>((int)round(tempImg[i]));
  }
  delete(tempImg);
  updateHistogram();
}

void Image::sharpeningLaplacian(bool useN8, bool getOnlyLaplacian){
  uint16 L = pow(2, _bps);
  uint32 imgSize = _height * _width * _channels;
  float* tempImg = new float[imgSize];
  float d2f;

  float max = std::numeric_limits<float>::min();
  float min = std::numeric_limits<float>::min();

  if (!useN8){
    int* W = new int[9]{0,1,0,1,-4,1,0,1,0};
    for (int y = 2; y < _height; y++)
      for (int x = 2; x < _width; x++){

        d2f = applyMask(3, _data, _width, _height, x, y, W);

        if (getOnlyLaplacian) tempImg[ y*_width + x ] = -d2f;
        if (!getOnlyLaplacian) tempImg[ y*_width + x ] = (float)_data[ y*_width + x ] - d2f;
        
        if (tempImg[ y*_width + x ] > max) max = tempImg[ y*_width + x ];
        if (tempImg[ y*_width + x ] < min) min = tempImg[ y*_width + x ];
      }

    for(uint32 i = 0; i < imgSize; i++){
      tempImg[i] = ((L-1)*(tempImg[i] - min)) / (max - min);
      _data[i] = static_cast<unsigned char>((int)round(tempImg[i]));
    }
  }
  else{
    int* W = new int[9]{1,1,1,1,-8,1,1,1,1};
    for (int y = 0; y < _height; y++)
      for (int x = 0; x < _width; x++){

        d2f = applyMask(3, _data, _width, _height, x, y, W);

        if (getOnlyLaplacian) tempImg[ y*_width + x ] = -d2f;
        else tempImg[ y*_width + x ] = (float)_data[ y*_width + x ] - d2f;
        
        if (tempImg[ y*_width + x ] > max) max = tempImg[ y*_width + x ];
        if (tempImg[ y*_width + x ] < min) min = tempImg[ y*_width + x ];
      }
    
    for(uint32 i = 0; i < imgSize; i++){
      tempImg[i] = ((L-1)*(tempImg[i] - min)) / (max - min);
      _data[i] = static_cast<unsigned char>((int)round(tempImg[i]));
    }
  }
  delete(tempImg);
  updateHistogram();
}

void Image::sobelOperator(){
  uint16 L = pow(2, _bps);
  uint32 imgSize = _height * _width * _channels;
  float* tempImg = new float[imgSize];
  float Mxy, gx, gy;
  int* Wx = new int[9]{-1,0,1,-2,0,2,-1,0,1};
  int* Wy = new int[9]{-1,-2,-1,0,0,0,1,2,1};

  float max = std::numeric_limits<float>::min();
  float min = std::numeric_limits<float>::min();

  for (int y = 0; y < _height; y++)
    for (int x = 0; x < _width; x++){
      gx = applyMask(3, _data, _width, _height, x, y, Wx);
      gy = applyMask(3, _data, _width, _height, x, y, Wy);
      
      Mxy = sqrt(pow(gx, 2) + pow(gy, 2));

      tempImg[ y*_width + x ] = Mxy;
      if (tempImg[ y*_width + x ] > max) max = tempImg[ y*_width + x ];
      if (tempImg[ y*_width + x ] < min) min = tempImg[ y*_width + x ];
    }
    
  for(uint32 i = 0; i < imgSize; i++){
    tempImg[i] = ((L-1)*(tempImg[i] - min)) / (max - min);
    _data[i] = static_cast<unsigned char>((int)round(tempImg[i]));
  }

  updateHistogram();
}

void Image::Fig3_43(char imgLetter){
  if (imgLetter == 'a') return;

  uint32 imgSize = _width * _height * _channels;
  uint16 L = pow(2, _bps);
  float* tempImg = new float[imgSize];
  unsigned char* a = new unsigned char[imgSize];
  for (uint32 i = 0; i < imgSize; i++){
    a[i] = _data[i];
  }

  sharpeningLaplacian(false, true);
  if (imgLetter == 'b'){
    delete(a);
    return;
  }
  unsigned char* b = new unsigned char[imgSize];
  for (uint32 i = 0; i < imgSize; i++){
    b[i] = _data[i];
  }
  for (uint32 i = 0; i < imgSize; i++){
    _data[i] = a[i];
  }
  
  sharpeningLaplacian(false, false);
  if (imgLetter == 'c'){
    delete(a);
    delete(b);
    return;
  }
  unsigned char* c = new unsigned char[imgSize];
  for (uint32 i = 0; i < imgSize; i++){
    c[i] = _data[i];
  }
  for (uint32 i = 0; i < imgSize; i++){
    _data[i] = a[i];
  }

  sobelOperator();
  if (imgLetter == 'd'){
    delete(a);
    delete(b);
    delete(c);
    return;
  } 
  unsigned char* d = new unsigned char[imgSize];
  for (uint32 i = 0; i < imgSize; i++){
    d[i] = _data[i];
  }

  imageBlurring(5);
  if (imgLetter == 'e'){
    delete(d);
    return;
  } 
  unsigned char* e = new unsigned char[imgSize];
  for (uint32 i = 0; i < imgSize; i++){
    e[i] = _data[i];
  }
  for (uint32 i = 0; i < imgSize; i++){
    _data[i] = a[i];
  }

  unsigned char* f = new unsigned char[imgSize];
  float max = std::numeric_limits<float>::min();
  float min = std::numeric_limits<float>::min();

  for (uint32 i = 0; i < imgSize; i++){
    tempImg[i] = c[i] * e[i];
    if (tempImg[i] > max) max = tempImg[i];
    if (tempImg[i] < min) min = tempImg[i];
  }
  for(uint32 i = 0; i < imgSize; i++){
    tempImg[i] = ((L-1)*(tempImg[i] - min)) / (max - min);
    f[i] = static_cast<unsigned char>((int)round(tempImg[i]));
  }
  if (imgLetter == 'f'){
    for (uint32 i = 0; i < imgSize; i++){
      _data[i] = f[i];
    }
    delete(a);
    delete(b);
    delete(c);
    delete(d);
    delete(e);
    delete(f);
    updateHistogram();
    return;
  }

  unsigned char* g = new unsigned char[imgSize];
  max = std::numeric_limits<float>::min();
  min = std::numeric_limits<float>::min();

  for (uint32 i = 0; i < imgSize; i++){
    tempImg[i] = a[i] + f[i];
    if (tempImg[i] > max) max = tempImg[i];
    if (tempImg[i] < min) min = tempImg[i];
  }
  for(uint32 i = 0; i < imgSize; i++){
    tempImg[i] = ((L-1)*(tempImg[i] - min)) / (max - min);
    g[i] = static_cast<unsigned char>((int)round(tempImg[i]));
  }
  if (imgLetter == 'g'){
    for (uint32 i = 0; i < imgSize; i++){
      _data[i] = g[i];
    }
    delete(a);
    delete(b);
    delete(c);
    delete(d);
    delete(e);
    delete(f);
    delete(g);
    updateHistogram();
    return;
  }

  intensityPowerLaw(0.5);
  if (imgLetter == 'h') return;
  unsigned char* h = new unsigned char[imgSize];
  for (uint32 i = 0; i < imgSize; i++){
    h[i] = _data[i];
  }
  delete(a);
  delete(b);
  delete(c);
  delete(d);
  delete(e);
  delete(f);
  delete(g);
  delete(h);
}