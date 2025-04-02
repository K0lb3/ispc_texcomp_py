"""
ISPCTextureCompressor Python Bindings.

This module provides Python bindings for the GameTechDev/ISPCTextureCompressor library.
"""

__version__ = version = "1.0.1"

from ._ispc_texcomp import (
    BC6HEncSettings,
    BC7EncSettings,
    ETCEncSettings,
    ASTCEncSettings,
    RGBASurface,
    compress_blocks_bc1,
    compress_blocks_bc3,
    compress_blocks_bc4,
    compress_blocks_bc5,
    compress_blocks_bc6h,
    compress_blocks_bc7,
    compress_blocks_etc1,
    compress_blocks_astc,
)

# Add module-level documentation
__doc__ = """
ISPCTextureCompressor Python Bindings

A Python interface for the GameTechDev/ISPCTextureCompressor library,
providing GPU texture compression functionality.

This module contains compiled extensions with type information provided
through stub files (.pyi).
"""

# Explicitly list public symbols
__all__ = [
    "__version__",
    "RGBASurface",
    "BC6HEncSettings",
    "BC7EncSettings",
    "ETCEncSettings",
    "ASTCEncSettings",
    "compress_blocks_bc1",
    "compress_blocks_bc3",
    "compress_blocks_bc4",
    "compress_blocks_bc5",
    "compress_blocks_bc6h",
    "compress_blocks_bc7",
    "compress_blocks_etc1",
    "compress_blocks_astc",
]
