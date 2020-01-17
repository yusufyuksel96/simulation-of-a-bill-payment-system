CXX = g++
LDFLAGS= -lpthread

TARGET = simulation
SOURCE = simulation.cpp


all : $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(SOURCE) -o $(TARGET) $(LDFLAGS)

clean :
	$(RM) $(TARGET)

