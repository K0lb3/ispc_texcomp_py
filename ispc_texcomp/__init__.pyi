from __future__ import annotations

from typing import ByteString, Literal

class RGBASurface:
    """
    Represents a RGBA image surface for texture compression.

    Attributes
    ----------
    width : int
        The width of the image in pixels (must be > 0)
    height : int
        The height of the image in pixels (must be > 0)
    stride : int
        The number of bytes per row (calculated as width*4 if 0)

    Methods
    -------
    __init__(src, width, height, stride=0)
        Initialize from raw pixel data
    data()
        Get raw bytes of the surface
    __buffer__()
        Get memoryview of pixel data
    """

    width: int
    height: int
    stride: int

    def __init__(
        self, src: ByteString, width: int, height: int, stride: int = 0
    ) -> None:
        """
        Initialize an RGBA surface from raw pixel data.

        Parameters
        ----------
        src : ByteString
            Raw RGBA pixel data (length should be height*stride or width*height*4)
        width : int
            Surface width in pixels (>0)
        height : int
            Surface height in pixels (>0)
        stride : int, optional
            Bytes per row (0 = width*4)

        Notes
        -----
        Data layout is assumed to be 8-bit per channel RGBA (32bpp)
        """
        ...

    @property
    def data(self) -> bytes:
        """bytes: Raw pixel data bytes of the surface."""
        ...

    def __buffer__(self) -> memoryview:
        """memoryview: Memoryview interface to pixel data."""
        ...

BC7EncProfile = Literal[
    "ultrafast",
    "veryfast",
    "fast",
    "basic",
    "slow",
    "alpha_ultrafast",
    "alpha_veryfast",
    "alpha_fast",
    "alpha_basic",
    "alpha_slow",
]

class BC7EncSettings:
    """
    Configuration settings for BC7 texture compression.

    Attributes
    ----------
    skip_mode2 : bool
        Skip BC7 mode 2 during compression
    fast_skip_threshold_mode1 : int
        Early exit threshold for mode 1 (0-4)
    fast_skip_threshold_mode2 : int
        Early exit threshold for mode 2 (0-4)
    fast_skip_threshold_mode7 : int
        Early exit threshold for mode 7 (0-4)
    mode45_channel0 : bool
        Use modes 4/5 for channel 0 (RGB)
    refine_iterations_channel : int
        Number of refinement iterations per channel
    channels : int
        Number of channels (3=RGB, 4=RGBA)

    Methods
    -------
    from_profile(profile)
        Create settings from predefined profile
    """

    skip_mode2: bool
    fast_skip_threshold_mode1: int
    fast_skip_threshold_mode2: int
    fast_skip_threshold_mode7: int
    mode45_channel0: bool
    refine_iterations_channel: int
    channels: int

    def __init__(
        self,
        mode_selection: list[bool],
        refine_iterations: list[int],
        skip_mode2: bool,
        fast_skip_threshold_mode1: int,
        fast_skip_threshold_mode2: int,
        fast_skip_threshold_mode7: int,
        mode45_channel0: int,
        refine_iterations_channel: int,
        channels: int,
    ) -> None:
        """
        Create custom BC7 compression settings.

        Parameters
        ----------
        mode_selection : list[bool]
            4-element list enabling compression modes
        refine_iterations : list[int]
            8-element list of refinement iterations per mode
        skip_mode2 : bool
            Skip mode 2 during compression
        fast_skip_threshold_mode1 : int
            Mode 1 early exit threshold (0-4)
        fast_skip_threshold_mode2 : int
            Mode 2 early exit threshold (0-4)
        fast_skip_threshold_mode7 : int
            Mode 7 early exit threshold (0-4)
        mode45_channel0 : int
            Use modes 4/5 for RGB channel
        refine_iterations_channel : int
            Refinement iterations per channel
        channels : int
            Number of color channels (3 or 4)
        """
        ...

    @classmethod
    def from_profile(cls, profile: BC7EncProfile) -> BC7EncSettings:
        """
        Create settings from a predefined BC7 compression profile.

        Parameters
        ----------
        profile : BC7EncProfile
            Compression profile name. Valid options:
            - ultrafast/veryfast/fast/basic/slow: For opaque textures
            - alpha_* variants: For textures with alpha channel

        Returns
        -------
        BC7EncSettings
            Preconfigured settings instance
        """
        ...

BC6HEncProfile = Literal[
    "fast",
    "veryfast",
    "basic",
    "slow",
    "veryslow",
]

class BC6HEncSettings:
    """
    Configuration settings for BC6H texture compression (HDR format).

    Attributes
    ----------
    slow_mode : bool
        Enable thorough but slow search for best endpoints
    fast_mode : bool
        Enable fast optimizations (mutually exclusive with slow_mode)
    refine_iterations_1p : int
        Refinement iterations for 1-partition modes
    refine_iterations_2p : int
        Refinement iterations for 2-partition modes
    fast_skip_threshold : int
        Threshold for early termination (0-4)

    Methods
    -------
    from_profile(profile)
        Create settings from predefined profile
    """

    slow_mode: bool
    fast_mode: bool
    refine_iterations_1p: int
    refine_iterations_2p: int
    fast_skip_threshold: int

    def __init__(
        self,
        slow_mode: bool = False,
        fast_mode: bool = False,
        refine_iterations_1p: int = 0,
        refine_iterations_2p: int = 0,
        fast_skip_threshold: int = 0,
    ) -> None:
        """
        Initialize BC6H compression settings.

        Parameters
        ----------
        slow_mode : bool, optional
            Default False. Enables exhaustive search
        fast_mode : bool, optional
            Default False. Enables performance optimizations
        refine_iterations_1p : int, optional
            Default 0. Iterations for 1-partition refinement
        refine_iterations_2p : int, optional
            Default 0. Iterations for 2-partition refinement
        fast_skip_threshold : int, optional
            Default 0. Early exit threshold (0=disabled)
        """
        ...

    @classmethod
    def from_profile(cls, profile: BC6HEncProfile) -> BC6HEncSettings:
        """
        Create BC6H settings from a named profile.

        Parameters
        ----------
        profile : BC6HEncProfile
            Compression profile. Valid options:
            - 'veryfast': Fastest compression
            - 'fast': Balanced speed/quality
            - 'basic': Standard quality
            - 'slow': High quality
            - 'veryslow': Best quality

        Returns
        -------
        BC6HEncSettings
            Preconfigured settings instance
        """
        ...

ETCEncProfile = Literal["slow",]

class ETCEncSettings:
    """
    Configuration settings for ETC1 texture compression.

    Attributes
    ----------
    fast_skip_threshold : int
        Threshold for early termination (0-4)

    Methods
    -------
    from_profile(profile)
        Create settings from predefined profile
    """

    fast_skip_threshold: int

    def __init__(self, fast_skip_threshold: int = 0) -> None:
        """
        Initialize ETC1 compression settings.

        Parameters
        ----------
        fast_skip_threshold : int, optional
            Default 0. Early exit threshold (0=disabled)
        """
        ...
    @classmethod
    def from_profile(cls, profile: ETCEncProfile) -> ETCEncSettings:
        """
        Create ETC1 settings from a named profile.

        Parameters
        ----------
        profile : ETCEncProfile
            'slow' is the only preset profile

        Returns
        -------
        ETCEncSettings
            Preconfigured settings instance
        """
        ...

ASTCEncProfile = Literal[
    "fast",
    "alpha_fast",
    "alpha_slow",
]

class ASTCEncSettings:
    """
    Configuration settings for ASTC texture compression.

    Attributes
    ----------
    block_width : int
        ASTC block width (4-12)
    block_height : int
        ASTC block height (4-12)
    channels : int
        Color channels (3=RGB, 4=RGBA)
    fast_skip_threshold : int
        Early termination threshold (0-4)
    refine_iterations : int
        Number of endpoint refinement iterations

    Methods
    -------
    from_profile(profile, block_width, block_height)
        Create settings from predefined profile
    """

    block_width: int
    block_height: int
    channels: int
    fast_skip_threshold: int
    refine_iterations: int

    def __init__(
        self,
        block_width: int,
        block_height: int,
        channels: int,
        fast_skip_threshold: int,
        refine_iterations: int,
    ) -> None:
        """
        Initialize ASTC compression settings.

        Parameters
        ----------
        block_width : int
            ASTC block width (4-12)
        block_height : int
            ASTC block height (4-12)
        channels : int
            Color channels (3 or 4)
        fast_skip_threshold : int
            Early exit threshold (0=disabled)
        refine_iterations : int
            Endpoint refinement iterations
        """
        ...
    @classmethod
    def from_profile(
        cls, profile: ASTCEncProfile, block_width: int, block_height: int
    ) -> ASTCEncSettings:
        """
        Create ASTC settings from a named profile.

        Parameters
        ----------
        profile : ASTCEncProfile
            Compression profile. Valid options:
            - 'fast': Opaque textures
            - 'alpha_fast': Textures with Alpha channel (fast)
            - 'alpha_slow': Textures with Alpha channel (high quality)
        block_width : int
            ASTC block width (4-8)
        block_height : int
            ASTC block height (4-8)

        Returns
        -------
        ASTCEncSettings
            Preconfigured settings instance
        """
        ...

def compress_blocks_bc1(rgba: RGBASurface) -> bytes:
    """
    Compress to BC1 format (DXT1 equivalent).

    Parameters
    ----------
    rgba : RGBASurface
        Input RGBA surface (alpha channel ignored)

    Returns
    -------
    bytes
        Compressed BC1 texture data

    Notes
    -----
    - 4x4 blocks, 4bpp
    - Supports RGB with 1-bit alpha
    """
    ...

def compress_blocks_bc3(rgba: RGBASurface) -> bytes:
    """
    Compress to BC3 format (DXT5 equivalent).

    Parameters
    ----------
    rgba : RGBASurface
        Input RGBA surface

    Returns
    -------
    bytes
        Compressed BC3 texture data

    Notes
    -----
    - 4x4 blocks, 8bpp
    - RGBA format with explicit alpha
    """
    ...

def compress_blocks_bc4(rgba: RGBASurface) -> bytes:
    """
    Compress to BC4 format (single-channel).

    Parameters
    ----------
    rgba : RGBASurface
        Input surface (uses red channel)

    Returns
    -------
    bytes
        Compressed BC4 texture data

    Notes
    -----
    - 4x4 blocks, 4bpp
    - Stores single channel (typically red)
    """
    ...

def compress_blocks_bc5(rgba: RGBASurface) -> bytes:
    """
    Compress to BC5 format (dual-channel).

    Parameters
    ----------
    rgba : RGBASurface
        Input surface (uses red/green channels)

    Returns
    -------
    bytes
        Compressed BC5 texture data

    Notes
    -----
    - 4x4 blocks, 8bpp
    - Stores two channels (typically red/green)
    """
    ...

def compress_blocks_bc6h(rgba: RGBASurface, settings: BC6HEncSettings) -> bytes:
    """
    Compress an RGBA surface to BC6 texture blocks.

    Parameters
    ----------
    rgba : RGBASurface
        Input RGBA surface to compress
    settings : BC6HEncSettings
        Compression configuration settings

    Returns
    -------
    bytes
        Compressed texture data in BC6 format
    """
    ...

def compress_blocks_bc7(rgba: RGBASurface, settings: BC7EncSettings) -> bytes:
    """
    Compress an RGBA surface to BC7 texture blocks.

    Parameters
    ----------
    rgba : RGBASurface
        Input RGBA surface to compress
    settings : BC7EncSettings
        Compression configuration settings

    Returns
    -------
    bytes
        Compressed texture data in BC7 format
    """
    ...

def compress_blocks_etc1(rgba: RGBASurface, settings: ETCEncSettings) -> bytes:
    """
    Compress to ETC1 format.

    Parameters
    ----------
    rgba : RGBASurface
        Input RGBA surface (alpha channel ignored)
    settings : ETCEncSettings
        Compression settings

    Returns
    -------
    bytes
        Compressed ETC1 texture data

    Notes
    -----
    - 4x4 blocks, 4bpp
    - RGB format only
    """
    ...

def compress_blocks_astc(rgba: RGBASurface, settings: ASTCEncSettings) -> bytes:
    """
    Compress to ASTC format.

    Parameters
    ----------
    rgba : RGBASurface
        Input RGBA surface
    settings : ASTCEncSettings
        Compression settings with block configuration

    Returns
    -------
    bytes
        Compressed ASTC texture data

    Notes
    -----
    - 4x4 to 8x8 block sizes
    """
    ...
