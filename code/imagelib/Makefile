HEADERS = image.hpp
SOURCES = image.cpp imagetiff.cpp imagejpeg.cpp imagetransformation.cpp imageintensity.cpp interval.cpp fouriertransform.cpp filteringfrequency.cpp segmentation.cpp morphology.cpp imageprocessing.cpp
OBJS = $(SOURCES:.cpp=.o)

TARGET = libimagelib.so

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(HEADERS) $(OBJS)
	g++ $(OBJS) -shared -lfftw3 -lm -ldl -o $(TARGET)

# '$@' matches target, '%<' matches source
%.o: %.cpp image.hpp
	g++ -I/usr/include/eigen3 -fPIC -O3 -c $< -o $@ -I./

clean:
	rm *.o;
	rm $(TARGET)
