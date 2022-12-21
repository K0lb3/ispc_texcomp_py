from io import BytesIO
import json
import os
import platform
import re
import tarfile
from urllib.request import urlopen, Request
import zipfile

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext


__version__ = "0.0.1"

COMPILER_ZIP_REGEX = {
    "Windows": r"ispc-v(.+?)-windows.zip",
    "Darwin": r"ispc-v(.+?)-macOS.tar.gz",
    "Linux": r"ispc-v(.+?)-linux.tar.gz",
}


LOCAL = os.path.dirname(os.path.abspath(__file__))
ISPC_TEXCOMP_DIR = os.path.join(LOCAL, "src", "ISPCTextureCompressor", "ispc_texcomp")


def build():
    setup(
        name="ispc_texcomp_py",
        version=__version__,
        ext_modules=[
            Extension(
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


class build_ext_ispc(build_ext):
    def build_extension(self, ext):
        build_temp = os.path.realpath(self.build_temp)
        # fetch ispc compiler
        ispc = fetch_ispc_compiler(self.build_temp)
        # remove ispc files from sources
        ispc_files = []
        i = 0
        while i < len(ext.sources):
            if ext.sources[i].endswith(".ispc"):
                ispc_files.append(ext.sources.pop(i))
            else:
                i += 1

        # compile ispc files
        extra_objects = self.build_ispc(ispc, ispc_files)
        # add ispc objects to extra_objects for linking
        ext.extra_objects.extend(extra_objects)
        # add build_temp to include_dirs to include generated .h files
        ext.include_dirs.append(build_temp)

        super().build_extension(ext)

    # def __del__(self):
        # super().__del__()
        # cleanup build dirs
        # import shutil

        # if os.path.exists(self.build_temp):
        #     shutil.rmtree(self.build_temp)
        # if os.path.exists(self.build_lib):
        #     shutil.rmtree(self.build_lib)

    def build_ispc(self, ispc, ispc_files):
        system = platform.system()
        machine = platform.machine()
        arch = platform.architecture()[0]

        if "arm" in machine:
            if arch == "32bit":
                arch = "arm32"
            elif arch == "64bit":
                arch = "arm64"

        targets = []
        if arch in ["arm64", "aarch64"]:
            targets = ["neon-i32x4"]
            arch = "aarch64"
        elif arch == "arm32":
            raise NotImplementedError("Building for arm32 is not supported.")
        elif system == "Windows":
            targets = ["sse2", "sse4", "avx", "avx2"]
            if arch == "32bit":
                arch = "x86"
            else:
                arch = "x86-64"
        else:
            targets = ["sse2", "avx"]
            if arch == "32bit":
                arch = "x86"
            elif arch == "64bit":
                arch = "x86_64"
            else:
                raise ValueError("Unknown architecture")
        print(f"ISPC arch: {arch}")
        extra_objects = []
        for source in ispc_files:
            extra_objects.extend(
                self.compile_ispc(
                    ispc,
                    source,
                    arch,
                    targets,
                )
            )
        return extra_objects

    def compile_ispc(self, ispc, source, arch, targets):
        name = os.path.basename(source)
        if name.endswith(".ispc"):
            name = name[:-5]
        else:
            raise ValueError("ISPC file must end with .ispc")
        outputs = [
            os.path.join(self.build_temp, f"{name}.o"),
            *[
                os.path.join(self.build_temp, f"{name}_{target}.o")
                for target in targets
            ],
        ]

        if all(os.path.exists(output) for output in outputs):
            print(f"ISPC file {name} already built")
            return outputs

        arguments = [
            "-O2",
            source,
            "-o",
            f"{self.build_temp}/{name}.o",
            "-h",
            f"{self.build_temp}/{name}_ispc.h",
            f"--arch={arch}" if arch else "",
            f'--target={",".join(targets)}',
            "--opt=fast-math",
            "--pic",
        ]
        result = self.spawn([ispc] + arguments)
        print(result)
        return outputs


def fetch_ispc_compiler(target_dir: str = None):
    """Fetches the ispc compiler from the official repository."""
    system = platform.system()
    if system == "Windows":
        ispc_fp = os.path.join(target_dir, "ISPC", "win", "ispc.exe")
    else:
        ispc_fp = os.path.join(
            target_dir, "ISPC", "linux" if system == "Linux" else "osx", "ispc"
        )

    if os.path.exists(ispc_fp):
        print("ISPC compiler already exists")
        return ispc_fp

    print("Fetching ISPC compiler")
    releases = json.loads(
        urlopen("https://api.github.com/repos/ispc/ispc/releases").read()
    )

    pattern = re.compile(COMPILER_ZIP_REGEX[system])

    url = None
    version = None
    for release in releases:
        if release["prerelease"] or release["draft"]:
            continue
        for asset in release["assets"]:
            match = pattern.match(asset["name"])
            if match:
                url = asset["browser_download_url"]
                version = match.group(1)
                break
        if url:
            break
    else:
        raise RuntimeError("No release found")

    print("Downloading ispc compiler version {}".format(version))
    raw_package = urlopen(Request(url, headers={"User-Agent": "Mozilla/5.0"})).read()

    if system == "Windows":
        ispc_fp = os.path.join(target_dir, "ISPC", "win", "ispc.exe")
        os.makedirs(os.path.dirname(ispc_fp), exist_ok=True)
        with zipfile.ZipFile(BytesIO(raw_package)) as zip_file:
            for name in zip_file.namelist():
                if name.endswith("bin/ispc.exe"):
                    with open(ispc_fp, "wb") as f:
                        f.write(zip_file.read(name))
                    break
            else:
                raise RuntimeError("No ispc.exe found in the zip file")
    else:
        ispc_fp = os.path.join(
            target_dir, "ISPC", "linux" if system == "Linux" else "osx", "ispc"
        )
        os.makedirs(os.path.dirname(ispc_fp), exist_ok=True)
        with tarfile.open(fileobj=BytesIO(raw_package)) as tar_file:
            for name in tar_file.getnames():
                if name.endswith("bin/ispc"):
                    with open(ispc_fp, "wb") as f:
                        f.write(tar_file.extractfile(name).read())
                    break
            else:
                raise RuntimeError("No ispc found in the tar file")
        print("Making ispc executable")
        os.system("chmod +x {}".format(ispc_fp))

    return ispc_fp


if __name__ == "__main__":
    build()
