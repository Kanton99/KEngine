install: 
	conan install . --build=missing

build:
	conan build .

.PHONY: install build
