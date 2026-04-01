install: 
	conan install . --build=missing -pr ./conan-profiles

build:
	conan build . -pr ./conan-profiles

run:
	@./build/Debug/main/main

.PHONY: install build
