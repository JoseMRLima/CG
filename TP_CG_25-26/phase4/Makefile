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
	@./$(GENERATOR) box 2 3 box_2_3.3d
	@./$(ENGINE) $(TEST_DIR)/test_4_1.xml

test2: compile
	@./$(GENERATOR) box 2 3 box_2_3.3d
	@./$(ENGINE) $(TEST_DIR)/test_4_2.xml

test3: compile
	@./$(GENERATOR) plane 2 3 plane_2_3.3d
	@./$(GENERATOR) cone 1 2 4 3 cone_1_2_4_3.3d
	@./$(GENERATOR) sphere 1 8 8 sphere_1_8_8.3d
	@./$(GENERATOR) patch $(PATCH_DIR)/teapot.patch 10 bezier_10.3d
	@./$(GENERATOR) box 2 3 box_2_3.3d
	@./$(ENGINE) $(TEST_DIR)/test_4_3.xml

test4: compile
	@./$(GENERATOR) plane 2 3 plane_2_3.3d
	@./$(GENERATOR) cone 1 2 4 3 cone_1_2_4_3.3d
	@./$(GENERATOR) sphere 1 8 8 sphere_1_8_8.3d
	@./$(GENERATOR) patch $(PATCH_DIR)/teapot.patch 10 bezier_10.3d
	@./$(GENERATOR) box 2 3 box_2_3.3d
	@./$(ENGINE) $(TEST_DIR)/test_4_4.xml

test5: compile
	@./$(GENERATOR) plane 2 3 plane_2_3.3d
	@./$(GENERATOR) cone 1 2 4 3 cone_1_2_4_3.3d
	@./$(GENERATOR) sphere 1 8 8 sphere_1_8_8.3d
	@./$(GENERATOR) patch $(PATCH_DIR)/teapot.patch 10 bezier_10.3d
	@./$(GENERATOR) box 2 3 box_2_3.3d
	@./$(ENGINE) $(TEST_DIR)/test_4_5.xml

test6: compile
	@./$(GENERATOR) plane 2 3 plane_2_3.3d
	@./$(GENERATOR) cone 1 2 4 3 cone_1_2_4_3.3d
	@./$(GENERATOR) sphere 1 8 8 sphere_1_8_8.3d
	@./$(GENERATOR) patch $(PATCH_DIR)/teapot.patch 10 bezier_10.3d
	@./$(GENERATOR) box 2 3 box_2_3.3d
	@./$(ENGINE) $(TEST_DIR)/test_4_6.xml

demo: compile
	@./$(GENERATOR) ring 4 0.5 10 20 saturn_ring.3d
	@./$(GENERATOR) sphere 1.0 15 15 sphere.3d
	@./$(GENERATOR) patch $(PATCH_DIR)/teapot.patch 10 teapot.3d
	@./$(ENGINE) $(DEMO_DIR)/solar_system.xml
