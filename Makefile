project-configure-with-executable-on-fresh:
	rm -rf build
	mkdir build
	cmake -B build -S . -DCOMPILE_EXECUTABLE=ON

project-configure-with-executable-on-fresh-debug:
	rm -rf build
	mkdir build
	cmake -B build -S . -DCOMPILE_EXECUTABLE=ON -DCMAKE_BUILD_TYPE=Debug

project-build:
	cmake --build build

project-clean-build:
	cmake --build build --clean-first