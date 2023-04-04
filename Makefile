CC = g++

CFLAGS = -Wall --std=c++17
INLCUDES = -I ./src/include
RAYLIB = `pkg-config --cflags --libs raylib`
OPENCV = `pkg-config --cflags --libs opencv4`
EIGEN = `pkg-config --cflags --libs Eigen3`

output:
	mkdir -p builds
	$(CC) src/main.cpp src/include/read.cpp src/include/utils.cpp src/include/preprocess.cpp src/include/canvas.cpp $(INLCUDES) $(RAYLIB) $(OPENCV) $(EIGEN) $(CFLAGS) -o ./builds/main

clean:
	rm ./builds/*.o ./builds/main
