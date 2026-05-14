CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -msse4.2 \
            -I src \
            -I external/string_and_thong \
            -I external/x_list/src \
            -I external/io_utils/include

TARGET := hashset.out

SRCS := main.cpp \
        src/hashset.cpp \
        src/hashfuncs.cpp \
        external/string_and_thong/stringNthong.cpp \
        external/x_list/src/list.cpp \
        external/x_list/src/list_dump.cpp \
        external/x_list/src/list_verifier.cpp \
        external/io_utils/src/io_utils.cpp \
        external/io_utils/src/memdump.cpp \

OBJS := $(SRCS:.cpp=.o)

PERF_FLAGS := -Ofast -g -fno-omit-frame-pointer -mavx2

BASE_SRCS := perf_main.cpp \
             src/hashset.cpp \
             src/hashfuncs.cpp \
             external/string_and_thong/stringNthong.cpp \
             external/x_list/src/list.cpp \
             external/x_list/src/list_dump.cpp \
             external/x_list/src/list_verifier.cpp

SOA_SRCS := perf_soa.cpp \
            src/hashset_soa.cpp \
            src/bucket_soa.cpp \
            src/hashfuncs.cpp \
            external/io_utils/src/io_utils.cpp \
            external/io_utils/src/memdump.cpp

ALIGNED_SRCS := perf_aligned.cpp \
                src/hashset_soa.cpp \
                src/bucket_soa.cpp \
                src/hashfuncs.cpp \
                external/io_utils/src/io_utils.cpp \
                external/io_utils/src/memdump.cpp

.PHONY: all clean run analyze \
        perf_base_verify_build perf_base_build perf_soa_build perf_soa_hw_build perf_aligned_build \
        perf_base_verify perf_base perf_soa perf_soa_hw perf_aligned

all: $(TARGET)

$(TARGET): $(OBJS) | data assets
	$(CXX) $(CXXFLAGS) $^ -o $@ -lm

data assets:
	mkdir -p $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) \
	      hashset_perf_base_verify.out hashset_perf_base.out \
	      hashset_perf_soa.out hashset_perf_soa_hw.out \
	      hashset_perf_aligned.out \
	      perf.data perf.data.old

run:
	./hashset.out

analyze:
	python3 scripts/analyze.py

perf_base_verify_build: | data assets
	$(CXX) $(CXXFLAGS) $(PERF_FLAGS) $(BASE_SRCS) -o hashset_perf_base_verify.out -lm

perf_base_build: | data assets
	$(CXX) $(CXXFLAGS) $(PERF_FLAGS) -DNDEBUG -DX_LIST_NO_VERIFY $(BASE_SRCS) -o hashset_perf_base.out -lm

perf_soa_build: | data assets
	$(CXX) $(CXXFLAGS) $(PERF_FLAGS) $(SOA_SRCS) -o hashset_perf_soa.out -lm

perf_soa_hw_build: | data assets
	$(CXX) $(CXXFLAGS) $(PERF_FLAGS) -DUSE_HW_CRC32 $(SOA_SRCS) -o hashset_perf_soa_hw.out -lm

perf_aligned_build: | data assets
	$(CXX) $(CXXFLAGS) $(PERF_FLAGS) -DUSE_ALIGNED_STRCMP $(ALIGNED_SRCS) -o hashset_perf_aligned.out -lm

perf_base_verify: perf_base_verify_build
	perf record -g -F 999 -o /tmp/perf_base_verify.data ./hashset_perf_base_verify.out
	perf report --no-children -i /tmp/perf_base_verify.data

perf_base: perf_base_build
	perf record -g -F 999 -o /tmp/perf_base.data ./hashset_perf_base.out
	perf report --no-children -i /tmp/perf_base.data

perf_soa: perf_soa_build
	perf record -g -F 999 -o /tmp/perf_soa.data ./hashset_perf_soa.out
	perf report --no-children -i /tmp/perf_soa.data

perf_soa_hw: perf_soa_hw_build
	perf record -g -F 999 -o /tmp/perf_soa_hw.data ./hashset_perf_soa_hw.out
	perf report --no-children -i /tmp/perf_soa_hw.data

perf_aligned: perf_aligned_build
	perf record -g -F 999 -o /tmp/perf_aligned.data ./hashset_perf_aligned.out
	perf report --no-children -i /tmp/perf_aligned.data
