CXX = g++
CXXFLAGS = -std=c++11 -O2
LDFLAGS = -lm -lpthread -lX11
EXEC = exec

PCH_SRC = CImg.h
PCH_OUT = CImg.h.gch

all:main.o
	$(CXX) -o exec $< $(LDFLAGS)

$(PCH_OUT):$(PCH_SRC)
	$(CXX) $(CXXFLAGS) -o $(PCH_OUT) $(PCH_SRC)

main.o: *.cpp $(PCH_OUT)
	$(CXX) $(CXXFLAGS) -include $(PCH_SRC) -c -o $@ $<
