from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout


class MyProjectConan(ConanFile):
    name = "KEngine"
    version = "0.0.1"
    settings = "os", "compiler", "build_type", "arch"
    package_type = "application"
    generators = "CMakeDeps", "CMakeToolchain"
    requires = [
        "fastgltf/0.9.0",
        "glm/1.0.1",
        "tinyobjloader/2.0.0-rc10",
        "volk/1.4.313.0",
        "vulkan-memory-allocator/3.3.0",
    ]

    # def requirements(self):
    #     self.requires(
    #         "sdl/3.4.0",
    #         headers=True,
    #         options={
    #             "alsa": False,
    #             "pulseaudio": False,
    #             "sndio": False,
    #             "opengl": False,
    #             "opengles": False,
    #             "x11": True,
    #             "xcursor": False,
    #             "xdbe": False,
    #             "xinput": True,
    #             "xfixes": False,
    #             "xrandr": False,
    #             "xscrnsaver": False,
    #             "xshape": False,
    #             "wayland": True,
    #             "vulkan": True,
    #             "libudev": False,
    #             "dbus": False,
    #             "libusb": False,
    #             "libiconv": False,
    #         },
    #     )

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
