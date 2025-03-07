from typing import ByteString, List, Literal

__version__: str

class RGBASurface:
    width: int
    height: int
    stride: int
    obj: ByteString

    def __init__(
        self, src: ByteString, width: int, height: int, stride: int = 0
    ) -> None: ...

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
    skip_mode2: bool
    fast_skip_threshold_mode1: int
    fast_skip_threshold_mode2: int
    fast_skip_threshold_mode7: int
    mode45_channel0: bool
    refine_iterations_channel: int
    channels: int

    def __init__(
        self,
        mode_selection: List[bool],
        refine_iterations: List[int],
        skip_mode2: bool,
        fast_skip_threshold_mode1: int,
        fast_skip_threshold_mode2: int,
        fast_skip_threshold_mode7: int,
        mode45_channel0: int,
        refine_iterations_channel: int,
        channels: int,
        profile: BC7EncProfile,
    ) -> None:
        """BC7EncSettings constructor.

        Parameters
        ----------
        mode_selection : List[bool]
            a list of 4 bools
        refine_iterations : List[int]
            a list of 8 ints
        """
        ...

BC6EncProfile = Literal[
    "fast",
    "veryfast",
    "basic",
    "slow",
    "veryslow",
]

class BC6HEncSettings:
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
    ) -> None: ...

class ETCEncSettings:
    fast_skip_threshold: int

    def __init__(self, fast_skip_threshold: int = 0, profile: str = "slow") -> None: ...

ASTCEncProfile = Literal[
    "fast",
    "alpha_fast",
    "alpha_slow",
]

class ASTCEncSettings:
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
        profile: ASTCEncProfile,
    ) -> None: ...

def compress_blocks_bc1(rgba: RGBASurface) -> bytes: ...
def compress_blocks_bc3(rgba: RGBASurface) -> bytes: ...
def compress_blocks_bc4(rgba: RGBASurface) -> bytes: ...
def compress_blocks_bc5(rgba: RGBASurface) -> bytes: ...
def compress_blocks_bc6h(rgba: RGBASurface, settings: BC6HEncSettings) -> bytes: ...
def compress_blocks_bc7(rgba: RGBASurface, settings: BC7EncSettings) -> bytes: ...
def compress_blocks_etc1(rgba: RGBASurface, settings: ETCEncSettings) -> bytes: ...
def compress_blocks_astc(rgba: RGBASurface, settings: ASTCEncSettings) -> bytes: ...
