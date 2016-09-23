CXXFLAGS := -O2 -flto -Wall -std=c++11
TARGETS := _compress_cwd _loaded_keys _smartcd

all: $(TARGETS)

install: all
	mkdir -p ~/local/bin
	cp $(TARGETS) ~/local/bin

_%: %.cc
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(TARGETS)

.PHONY: all clean install
