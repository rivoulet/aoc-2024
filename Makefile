MAKEFLAGS += -rR
CXXFLAGS += -Wall -std=c++17 -g

%/sol: %/main.cc main.cc common.hh
	$(CXX) $(CXXFLAGS) $< main.cc -o $@

.PHONY: clean
clean:
	-find . -name 'sol' -or -name 'sol.dSYM' -maxdepth 2 | xargs rm -r
