INCLUDES_DIR = -I/usr/local/include/opencv4 -I.
LIBS = -lpthread -lopencv_core -lopencv_tracking -lopencv_video -lopencv_highgui -lopencv_imgproc -lopencv_videoio -lopencv_objdetect -lopenblas -lopencv_dnn -ldlib
CXXFLAGS = $(INCLUDES_DIR) $(LIBS) -O3 -march=native -std=c++17 -g
all:
	g++ main.cpp $(CXXFLAGS)
