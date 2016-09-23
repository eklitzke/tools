CXXFLAGS := -O2 -flto -Wall -std=c++11
TARGETS := compress_cwd gocd loaded_keys

all: $(TARGETS)

install: all
	mkdir -p ~/local/bin
	cp $(TARGETS) ~/local/bin

%: %.cc
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(TARGETS)

.PHONY: all clean install
