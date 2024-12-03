MAKEFLAGS += -rR
CXXFLAGS += -Wall -std=c++17

%/sol: %/main.cc main.cc common.hh
	$(CXX) $(CXXFLAGS) $< main.cc -o $@
