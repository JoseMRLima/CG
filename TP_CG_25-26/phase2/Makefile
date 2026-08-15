ENGINE    := build/bin/engine
GENERATOR := build/bin/generator
TEST_DIR  := test
DEMO_DIR  := scenes

.PHONY: setup compile clean test1 test2 test3 test4 demo all

all: compile

setup:
	cmake -S . -B build

compile:
	cmake --build build

clean:
	rm -rf build
	rm -f *.3d

test1: compile
	@./$(GENERATOR) box 2 3 box_2_3.3d
	@./$(ENGINE) $(TEST_DIR)/test_2_1.xml

test2: compile
	@./$(GENERATOR) box 2 3 box_2_3.3d
	@./$(GENERATOR) cone 1 2 4 3 cone_1_2_4_3.3d
	@./$(GENERATOR) sphere 1 8 8 sphere_1_8_8.3d
	@./$(ENGINE) $(TEST_DIR)/test_2_2.xml

test3: compile
	@./$(GENERATOR) sphere 1 8 8 sphere_1_8_8.3d
	@./$(GENERATOR) cone 1 2 4 3 cone_1_2_4_3.3d
	@./$(ENGINE) $(TEST_DIR)/test_2_3.xml

test4: compile
	@./$(GENERATOR) box 2 3 box_2_3.3d
	@./$(ENGINE) $(TEST_DIR)/test_2_4.xml

demo: compile
	@./$(GENERATOR) ring 2 0.3 10 20 ring.3d
	@./$(GENERATOR) sphere 1 8 8 sphere.3d
	@./$(ENGINE) $(DEMO_DIR)/solar_system.xml
