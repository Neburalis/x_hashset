CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -msse4.2 \
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

PERF_SRCS := perf_main.cpp \
             src/hashset.cpp \
             src/hashfuncs.cpp \
             external/string_and_thong/stringNthong.cpp \
             external/x_list/src/list.cpp \
             external/x_list/src/list_dump.cpp \
             external/x_list/src/list_verifier.cpp

PERF_TARGET  := hashset_perf.out
PERF_FLAGS   := -O2 -g -fno-omit-frame-pointer -DX_LIST_NO_VERIFY

PERF_SOA_SRCS   := perf_main_soa.cpp \
                   src/hashset_soa.cpp \
                   src/bucket_soa.cpp \
                   src/hashfuncs.cpp \
                   external/string_and_thong/stringNthong.cpp

PERF_SOA_TARGET := hashset_perf_soa.out
PERF_SOA_FLAGS  := -O2 -g -fno-omit-frame-pointer -mavx2

.PHONY: all clean run analyze perf perf_build perf_soa_build perf_soa

all: $(TARGET)

$(TARGET): $(OBJS) | data assets
	$(CXX) $(CXXFLAGS) $^ -o $@ -lm

data assets:
	mkdir -p $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(PERF_TARGET) perf.data perf.data.old

run:
	./hashset.out

analyze:
	python3 scripts/analyze.py

perf_build: | data assets
	$(CXX) $(CXXFLAGS) $(PERF_FLAGS) $(PERF_SRCS) -o $(PERF_TARGET) -lm

perf: perf_build
	perf record -g -F 999 ./$(PERF_TARGET)
	perf report --stdio | head -60

perf_soa_build: | data assets
	$(CXX) $(CXXFLAGS) $(PERF_SOA_FLAGS) $(PERF_SOA_SRCS) -o $(PERF_SOA_TARGET) -lm

perf_soa: perf_soa_build
	perf record -g -F 999 -o /tmp/perf_soa.data ./$(PERF_SOA_TARGET)
	perf report --stdio -i /tmp/perf_soa.data | head -80
