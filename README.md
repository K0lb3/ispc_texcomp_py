# ispc_texcomp_py

[![Win/Mac/Linux](https://img.shields.io/badge/platform-windows%20%7C%20macos%20%7C%20linux-informational)]()
[![MIT](https://img.shields.io/github/license/K0lb3/ispc_texcomp_py)](https://github.com/K0lb3/ispc_texcomp_py/blob/master/LICENSE)
[![Docs](https://github.com/K0lb3/ispc_texcomp_py/actions/workflows/doc.yml/badge.svg?branch=master)](k0lb3.github.io/ispc_texcomp_py/)


Python bindings for ISPCTextureCompressor.


## Installation

``pip install ispc_texcomp``

## Building

ispc has to be available in the PATH for setup.py to work.

## Usage

```python
from PIL import Image
import ispc_texcomp_py

# get the rgba data (of an image you want to compress)
img = Image.open(fp)
rgba = Image.tobytes("raw", "RGBA")

# create a RGBASurface
stride = img.width * 4
surface = ispc_texcomp_py.RGBASurface(rgba, img.width, img.height, stride)


# compress the surface (no profile)

# BC1
bc1_compressed: bytes = ispc_texcomp_py.CompressBlocksBC1(surface)

# BC3
bc3_compressed: bytes = ispc_texcomp_py.CompressBlocksBC3(surface)

# BC3
bc4_compressed: bytes = ispc_texcomp_py.CompressBlocksBC4(surface)

# BC5
bc5_compressed: bytes = ispc_texcomp_py.CompressBLocksBC5(surface)


# compress the surface (with profile)

# BC6h
# profile options:
#   veryfast, fast, basic, slow, veryslow
profile = ispc_texcomp_py.BC6HEncSettings.from_profile("fast")
bc6h_compressed: bytes = ispc_texcomp_py.CompressBlocksBC6H(surface, profile)

# BC7
# profile options:
#   ultrafast, veryfast, fast, basic, slow,
#   alpha_ultrafast, alpha_veryfast, alpha_fast, alpha_basic, alpha_slow
profile = ispc_texcomp_py.BC7EncSettings.from_profile("fast")
bc7_compressed: bytes = ispc_texcomp_py.CompressBlocksBC7(surface, profile)

# ETC1
# profile options:
#   slow
profile = ispc_texcomp_py.ETCEncSettings.from_profile("slow")
etc1_compressed: bytes = ispc_texcomp_py.CompressBlocksETC1(surface, profile)

# ASTC
# profile options:
#   fast, alpha_fast, alpha_slow
profile = ispc_texcomp_py.ASTCEncSettings.from_profile("fas", 8, 8)
astc_compressed: bytes = ispc_texcomp_py.CompressBlocksASTC(surface, profile)
```
