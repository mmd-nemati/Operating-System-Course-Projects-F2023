CC = g++ --std=c++17
BUILD_DIR := build

EXECUTABLE = ImageFilters.out
all: ${BUILD_DIR}  ${EXECUTABLE}

${EXECUTABLE}: ${BUILD_DIR}/main.o
	${CC} -o ImageFilters.out ${BUILD_DIR}/main.o

${BUILD_DIR}/main.o: main.cpp
	${CC} -c $< -o $@

${BUILD_DIR}: ; mkdir -p $@

.PHONY: clean
clean:
	rm -rf ${BUILD_DIR} *.out output.bmp