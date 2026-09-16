install: 
	conan install . --build=missing -pr:h ./conan-profiles -pr:b ./conan-profiles

build:
	conan build . -pr ./conan-profiles --profile:build=conan-profiles

run: build
	./build/Debug/main/main

.PHONY: install run build
