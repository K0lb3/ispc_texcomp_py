"""Python bindings for ISPCTextureCompressor."""

__version__ = "1.0.0"

from ._ispc_texcomp import (
    RGBASurface,
    BC7EncSettings,
    BC6HEncSettings,
    ETCEncSettings,
    ASTCEncSettings,
    compress_blocks_bc1,
    compress_blocks_bc3,
    compress_blocks_bc4,
    compress_blocks_bc5,
    compress_blocks_bc6h,
    compress_blocks_bc7,
    compress_blocks_etc1,
    compress_blocks_astc,
)
