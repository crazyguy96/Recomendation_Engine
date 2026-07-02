CXX      = g++
CXXFLAGS = -std=c++17 -Wall -O2
TARGET   = engine

all:
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp
	@echo "Done. Run: ./engine"

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET)
