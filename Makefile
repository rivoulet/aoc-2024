MAKEFLAGS += -rR
CXXFLAGS += -Wall -std=c++17 -g

%/sol: %/main.cc main.cc common.hh
	$(CXX) $(CXXFLAGS) $< main.cc -o $@

.PHONY: clean
clean:
	-rm -r */sol */sol.dSYM
