CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra \
            -I src \
            -I external/string_and_thong \
            -I external/x_list/src

TARGET := hashset.out

SRCS := main.cpp \
        src/hashset.cpp \
        src/hashfuncs.cpp \
        external/string_and_thong/stringNthong.cpp \
        external/x_list/src/list.cpp \
        external/x_list/src/list_dump.cpp \
        external/x_list/src/list_verifier.cpp

OBJS := $(SRCS:.cpp=.o)

.PHONY: all clean run analyze

all: $(TARGET)

$(TARGET): $(OBJS) | data assets
	$(CXX) $(CXXFLAGS) $^ -o $@ -lm

data assets:
	mkdir -p $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run:
	./hashset.out

analyze:
	python3 scripts/analyze.py
