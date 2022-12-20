import os

# Available at setup time due to pyproject.toml
from pybind11.setup_helpers import Pybind11Extension
from setuptools import setup

from build_ext_ispc import build_ext_ispc

__version__ = "0.0.1"

LOCAL = os.path.dirname(os.path.abspath(__file__))
ISPC_TEXCOMP_DIR = os.path.join(LOCAL, "src", "ISPCTextureCompressor", "ispc_texcomp")


def build():
    setup(
        name="ispc_texcomp_py",
        ext_modules=[
            Pybind11Extension(
                name="ispc_texcomp_py",
                sources=[
                    "src/ispc_texcomp_py.cpp",
                    os.path.join(ISPC_TEXCOMP_DIR, "ispc_texcomp.cpp"),
                    os.path.join(ISPC_TEXCOMP_DIR, "ispc_texcomp_astc.cpp"),
                    os.path.join(ISPC_TEXCOMP_DIR, "kernel.ispc"),
                    os.path.join(ISPC_TEXCOMP_DIR, "kernel_astc.ispc"),
                ],
                # extra_objects=extra_objects,
                language="c++",
                include_dirs=[ISPC_TEXCOMP_DIR],
                define_macros=[("VERSION_INFO", __version__)],
            ),
        ],
        cmdclass={"build_ext": build_ext_ispc},
        zip_safe=False,
    )


if __name__ == "__main__":
    build()
