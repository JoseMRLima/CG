ENGINE    := build/bin/engine
GENERATOR := build/bin/generator
TEST_DIR  := test
DEMO_DIR  := scenes
PATCH_DIR := patches

.PHONY: setup compile clean test1 test2 demo all

all: compile

setup:
	cmake -S . -B build

compile:
	cmake --build build

clean:
	rm -rf build
	rm -f *.3d

test1: compile
	@./$(GENERATOR) patch teapot.patch 10 bezier_10.3d
	@./$(ENGINE) $(TEST_DIR)/test_3_1.xml

test2: compile
	@./$(GENERATOR) patch teapot.patch 10 bezier_10.3d
	@./$(ENGINE) $(TEST_DIR)/test_3_2.xml

demo: compile
	@./$(GENERATOR) ring 4 0.5 10 20 saturn_ring.3d
	@./$(GENERATOR) sphere 1.0 15 15 sphere.3d
	@./$(GENERATOR) patch $(PATCH_DIR)/teapot.patch 10 teapot.3d
	@./$(ENGINE) $(DEMO_DIR)/solar_system.xml
