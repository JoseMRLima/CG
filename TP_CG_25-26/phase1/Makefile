ENGINE    := build/bin/engine
GENERATOR := build/bin/generator
TEST_DIR  := test

.PHONY: setup compile clean test1 test2 test3 test4 test5 all

all: compile

setup:
	cmake -S . -B build

compile:
	cmake --build build

clean:
	rm -rf build
	rm -f *.3d

test1: compile
	@./$(GENERATOR) cone 1 2 4 3 cone_1_2_4_3.3d
	@./$(ENGINE) $(TEST_DIR)/test_1_1.xml

test2: compile
	@./$(GENERATOR) cone 1 2 4 3 cone_1_2_4_3.3d
	@./$(ENGINE) $(TEST_DIR)/test_1_2.xml

test3: compile
	@./$(GENERATOR) sphere 1 10 10 sphere_1_10_10.3d
	@./$(ENGINE) $(TEST_DIR)/test_1_3.xml

test4: compile
	@./$(GENERATOR) box 2 3 box_2_3.3d
	@./$(ENGINE) $(TEST_DIR)/test_1_4.xml

test5: compile
	@./$(GENERATOR) plane 2 3 plane_2_3.3d
	@./$(GENERATOR) sphere 1 10 10 sphere_1_10_10.3d
	@./$(ENGINE) $(TEST_DIR)/test_1_5.xml
