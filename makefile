install: 
	conan install . --build=missing -pr ./conan-profiles

build:
	conan build . -pr ./conan-profiles

run: build
	./build/Debug/main/main

.PHONY: install run build
