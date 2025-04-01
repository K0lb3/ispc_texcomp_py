import os
import platform
import sys

from setuptools import Extension, setup
from setuptools.command.bdist_wheel import bdist_wheel
from setuptools.command.build_ext import build_ext

ISPC_ARCH_MAP = (
    # --arch={x86, x86-64, arm, aarch64, xe64}]
    (("amd64", "x86_64"), "x86-64"),
    (("win32", "i686", "x86"), "x86"),
    (("arm64", "aarch64"), "aarch64"),
    (
        (
            "armv6l",
            "armv7l",
        ),
        "arm",
    ),
)


class build_ext_ispc(build_ext):
    def build_extension(self, ext: Extension):
        # remove ispc files from sources
        ispc_files: list[str] = []
        i = 0
        while i < len(ext.sources):
            if ext.sources[i].endswith(".ispc"):
                ispc_files.append(ext.sources.pop(i))
            else:
                i += 1

        # check for precompiled ispc input dir
        ispc_extra_objects: list[str]
        ispc_include_dir: str

        argv = sys.argv
        if os.environ.get("CIBUILDWHEEL"):
            plat_name: str = self.plat_name
            for plat_archs, ispc_arch in ISPC_ARCH_MAP:
                if plat_name.endswith(plat_archs):
                    argv.append(f"--ispc_prebuild_dir=ispc_texcomp_{ispc_arch}")
                    break
            else:
                raise ValueError("Couldn't identify the target architecture!")

        for argv in argv:
            if argv.startswith("--ispc_prebuild_dir"):
                ispc_build_dir = argv.split("=", 1)[1].strip("\"'")
                ispc_extra_objects = [
                    os.path.join(ispc_build_dir, obj)
                    for obj in os.listdir(ispc_build_dir)
                    if obj.endswith(".o")
                ]
                ispc_include_dir = ispc_build_dir
                break
        else:
            # compile ispc files
            ispc_build_dir = os.path.join(self.build_temp, "ispc")
            ispc_extra_objects = self.build_ispc(ispc_files, ispc_build_dir)
            ispc_include_dir = os.path.realpath(ispc_build_dir)

        # add ispc objects to extra_objects for linking
        ext.extra_objects.extend(ispc_extra_objects)
        # add build_temp to include_dirs to include generated .h files
        ext.include_dirs.append(ispc_include_dir)

        super().build_extension(ext)  # type: ignore

    def build_ispc(self, ispc_files: list[str], build_dir: str) -> list[str]:
        extra_objects: list[str] = []
        for source in ispc_files:
            name = os.path.basename(source)[:-5]
            source = os.path.realpath(source)
            output = os.path.realpath(os.path.join(build_dir, f"{name}.o"))
            header = os.path.realpath(os.path.join(build_dir, f"{name}_ispc.h"))

            self.run_ispc(source, output, header)
            extra_objects.append(output)
        return extra_objects

    def run_ispc(self, src_fp: str, out_fp: str, header_fp: str):
        os.makedirs(os.path.dirname(out_fp), exist_ok=True)
        os.makedirs(os.path.dirname(header_fp), exist_ok=True)

        args = [
            "ispc",
            "-O2",
            src_fp,
            "-o",
            out_fp,
            "-h",
            header_fp,
            "--opt=fast-math",
            "--pic",
        ]

        plat_name: str = self.plat_name
        for plat_archs, ispc_arch in ISPC_ARCH_MAP:
            if plat_name.endswith(plat_archs):
                args.append(f"--arch={ispc_arch}")
        else:
            # let's just see if ispc can handle it....
            # the arch selection is basically for wheel building
            print("Warning:", "failed to detect local arch")

        self.spawn(args)


class bdist_wheel_abi3(bdist_wheel):
    def get_tag(self) -> tuple[str, str, str]:
        python, abi, plat = super().get_tag()

        if python.startswith("cp"):
            # on CPython, our wheels are abi3 and compatible back to 3.7
            return "cp311", "abi3", plat

        return python, abi, plat


def get_extra_compile_args():
    system = platform.system()
    if system == "Windows":
        return ["/std:c++17"]
    elif system == "Darwin":
        return ["-std=c++17"]
    else:
        return ["-std=c++17"]


setup(
    name="ispc_texcomp",
    packages=["ispc_texcomp"],
    package_data={"ispc_texcomp": ["*.py", "*.pyi", "py.typed"]},
    include_package_data=True,
    ext_modules=[
        Extension(
            name="ispc_texcomp._ispc_texcomp",
            sources=[
                "src/ispc_texcomp_py.cpp",
                "src/ISPCTextureCompressor/ispc_texcomp/ispc_texcomp.cpp",
                "src/ISPCTextureCompressor/ispc_texcomp/ispc_texcomp_astc.cpp",
                "src/ISPCTextureCompressor/ispc_texcomp/kernel.ispc",
                "src/ISPCTextureCompressor/ispc_texcomp/kernel_astc.ispc",
            ],
            depends=[
                "src/rgba_surface_py.hpp",
                "src/settings.hpp",
                "src/ISPCTextureCompressor/ispc_texcomp/ispc_texcomp.h",
                "src/ISPCTextureCompressor/ispc_texcomp/ispc_texcomp.def",
            ],
            language="c++",
            include_dirs=["src/ISPCTextureCompressor/ispc_texcomp"],
            extra_compile_args=get_extra_compile_args(),
            define_macros=[
                ("Py_LIMITED_API", "0x030b0000"),
            ],
            py_limited_api=True,
        ),
    ],
    cmdclass={"build_ext": build_ext_ispc, "bdist_wheel": bdist_wheel_abi3},
    zip_safe=False,
)
