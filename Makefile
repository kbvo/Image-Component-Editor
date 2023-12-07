CXX=g++
CXXFLAGS=-g -Wall -std=c++11 -I /usr/include/gtest/
GTEST_LD_FLAGS=-lgtest -lgtest_main -lpthread
TARGETS=compedit
all: $(TARGETS)

bmplib.o: bmplib.cpp bmplib.h
	$(CXX) $(CXXFLAGS) -c $< -o $@ 

component.o: component.cpp component.h
	$(CXX) $(CXXFLAGS) -c $< -o $@ 

cimage.o: cimage.cpp cimage.h component.h
	$(CXX) $(CXXFLAGS) -c $< -o $@ 

compedit: compedit.cpp bmplib.o component.o cimage.o
	$(CXX) $(CXXFLAGS) $^ -o $@ 
	
#bigint-ops-gtest: bigint-ops-gtest.cpp ../../bigint.o
#	$(CXX) $(CXXFLAGS) $^ -o $@ $(GTEST_LD_FLAGS)


#../../bigint.o: ../../bigint.cpp ../../bigint.h 
#	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf *.o $(TARGETS)