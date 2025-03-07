# ispc_texcomp_py

Python bindings for ISPCTextureCompressor


## Installation


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
profile = ispc_texcomp_py.BC6HEncSettings(profile="fast")
bc6h_compressed: bytes = ispc_texcomp_py.CompressBlocksBC6H(surface, profile)

# BC7
# profile options:
#   ultrafast, veryfast, fast, basic, slow,
#   alpha_ultrafast, alpha_veryfast, alpha_fast, alpha_basic, alpha_slow
profile = ispc_texcomp_py.BC7EncSettings(profile="fast")
bc7_compressed: bytes = ispc_texcomp_py.CompressBlocksBC7(surface, profile)

# ETC1
# profile options:
#   slow
profile = ispc_texcomp_py.ETCEncSettings(profile="slow")
etc1_compressed: bytes = ispc_texcomp_py.CompressBlocksETC1(surface, profile)

# ASTC
# profile options:
#   fast, alpha_fast, alpha_slow
profile = ispc_texcomp_py.ASTCEncSettings(block_width=8, block_height=8, profile="fast")
astc_compressed: bytes = ispc_texcomp_py.CompressBlocksASTC(surface, profile)
```

### detailed profile settings

*TODO*