install: 
	conan install . --build=missing -pr ./conan-profiles

build: install
	conan build . -pr ./conan-profiles

.PHONY: install build
