# ispc_texcomp_py

[![Win/Mac/Linux](https://img.shields.io/badge/platform-windows%20%7C%20macos%20%7C%20linux-informational)]()
[![MIT](https://img.shields.io/github/license/K0lb3/ispc_texcomp_py)](https://github.com/K0lb3/ispc_texcomp_py/blob/master/LICENSE)
[![Docs](https://github.com/K0lb3/ispc_texcomp_py/actions/workflows/doc.yml/badge.svg?branch=master)](https://k0lb3.github.io/ispc_texcomp_py/ispc_texcomp.html)

Python bindings for ISPCTextureCompressor.


## Installation

``pip install ispc_texcomp``

## Building

ispc has to be available in the PATH for setup.py to work.

## Example
## Example: Compressing Textures with Quality Profiles

```python
from pathlib import Path
from PIL import Image
import ispc_texcomp_py as itc

# 1. Load Image and Prepare Surface
img_path = Path("texture.png")
with Image.open(img_path) as img:
    # Convert to RGBA if necessary (library requires 32bpp RGBA)
    if img.mode != "RGBA":
        img = img.convert("RGBA")
        
    # Create surface with automatic stride calculation
    surface = itc.RGBASurface(
        img.tobytes("raw", "RGBA"),
        img.width,
        img.height,
        stride = img.width * 4  # 4 bytes per pixel (RGBA)
    )

# 2. Basic Compression (No Profiles)
# ------------------------------------------------------------------
print("Compressing with basic formats...")

# BC1 (DXT1 equivalent) - 4bpp RGB with 1-bit alpha
bc1_data = itc.compress_blocks_bc1(surface)
print(f"BC1 size: {len(bc1_data)//1024} KB")

# BC3 (DXT5 equivalent) - 8bpp RGBA with explicit alpha
bc3_data = itc.compress_blocks_bc3(surface)
print(f"BC3 size: {len(bc3_data)//1024} KB")

# BC4 (RGTC R-channel) - 4bpp single channel
bc4_data = itc.compress_blocks_bc4(surface)
print(f"BC4 size: {len(bc4_data)//1024} KB")

# BC5 (RGTC RG-channels) - 8bpp two channels
bc5_data = itc.compress_blocks_bc5(surface)
print(f"BC5 size: {len(bc5_data)//1024} KB")

# 3. Advanced Compression with Quality Profiles
# ------------------------------------------------------------------
print("\nCompressing with quality profiles...")

# ETC1
# ETC1 Profiles: slow
etc_profile = itc.ETCEncSettings.from_profile("slow")
bc6h_data = itc.compress_blocks_etc1(surface, etc_profile)
print(f"ETC size: {len(bc6h_data)//1024} KB")

# BC6H (HDR Format)
# BC6H Profiles: veryfast | fast | basic | slow | veryslow
bc6h_profile = itc.BC6HEncSettings.from_profile("fast")
# the surface has to contain FP16 data instead of U8 RGBA!
bc6h_data = itc.compress_blocks_bc6h(surface, bc6h_profile)
print(f"BC6H size: {len(bc6h_data)//1024} KB")

# BC7 (High Quality RGBA)
# BC7 Profiles: ultrafast | veryfast | fast | basic | slow
#   for alpha support prepend alpha_
bc7_profile = itc.BC7EncSettings.from_profile("alpha_fast")
bc7_data = itc.compress_blocks_bc7(surface, bc7_profile)
print(f"BC7 size: {len(bc7_data)//1024} KB")

# ASTC (LDR, 4x4 to 8x8 blocks)
# ASTC Profiles: fast, alpha_fast, alpha_slow
# ASTC Block Sizes: 4x4, 5x5, 6x6, 8x8, etc. (must be valid combinations)
astc_profile = itc.ASTCEncSettings.from_profile("fast", 8, 8)
astc_data = itc.compress_blocks_astc(surface, astc_profile)
print(f"ASTC 8x8 size: {len(astc_data)//1024} KB")
```
