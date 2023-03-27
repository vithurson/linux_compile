all : comp

CFLAGS = -Wall -Wno-unused-function -pthread

CXXFLAGS += `pkg-config opencv4 --cflags`

LDFLAGS += `pkg-config opencv4 --libs`


comp :
	g++ $(CFLAGS) $(CXXFLAGS) $(LDFLAGS) emu.cpp -o emu -std=c++11 
