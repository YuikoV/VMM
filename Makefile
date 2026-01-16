CXX = g++
CXXFLAGS = -std=c++11 -Wall -O2

OBJS = page_table.o address_translator.o swap_manager.o \
       page_replacement.o statistics.o vmm.o test.o

TARGET = vmm_test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET) swap.dat

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
