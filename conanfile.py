from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout

class MyProjectConan(ConanFile):
    name = "KEngine"
    version = "0.0.1"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"
    requires = [
        "fastgltf/0.9.0",  
        "glm/1.0.1",     
        "sdl_image/3.4.0", 
        "tinyobjloader/2.0.0-rc10", 
        "volk/1.4.313.0",       
        "vulkan-memory-allocator/3.3.0", 
    ]

    def requirements(self):
        if self.settings.os == "Linux":
            self.requires("sdl/3.4.0", options={"features": ["vulkan", "wayland", "x11"]})
        elif self.settings.os == "Windows":
            self.requires("sdl/3.4.0", options={"features": ["vulkan"]})

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
