MAKEFLAGS += -rR
CXXFLAGS += -Wall

%/sol: %/main.cc common.hh
	$(CXX) $(CXXFLAGS) $< -o $@
