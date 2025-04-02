import imagehash
import ispc_texcomp
import texture2ddecoder
from PIL import Image, ImageDraw

CUTOFF = 0.1
SAMPLE_IMG = Image.new("RGBA", (256, 256), (0, 0, 0, 255))
draw = ImageDraw.Draw(SAMPLE_IMG)
draw.ellipse((0, 0, 256, 256), fill=(255, 0, 0, 255))
draw.ellipse((32, 32, 256 - 32, 256 - 32), fill=(0, 255, 0, 255))
draw.ellipse((64, 64, 256 - 64, 256 - 64), fill=(0, 0, 255, 255))
draw.ellipse((96, 96, 256 - 96, 256 - 96), fill=(255, 255, 255, 255))
SURFACE = ispc_texcomp.RGBASurface(
    SAMPLE_IMG.tobytes("raw", "RGBA"), SAMPLE_IMG.width, SAMPLE_IMG.height
)


def check_type_check():
    try:
        ispc_texcomp.compress_blocks_bc1(None)
        assert False, "failed to check type"
    except TypeError:
        pass


def check_decompressed(bgra):
    assert SAMPLE_IMG.width * SAMPLE_IMG.height * 4 == len(bgra)
    dec_img = Image.frombytes(
        "RGBA", (SAMPLE_IMG.width, SAMPLE_IMG.height), bgra, "raw", ("BGRA")
    )

    assert (
        abs(imagehash.average_hash(SAMPLE_IMG) - imagehash.average_hash(dec_img))
        < CUTOFF
    ), "Decompressed image is not similar enough to original"


def test_astc():
    block_size = (8, 8)
    profile = ispc_texcomp.ASTCEncSettings.from_profile("fast", 8, 8)
    raw = ispc_texcomp.compress_blocks_astc(SURFACE, profile)
    bgra = texture2ddecoder.decode_astc(
        raw, SAMPLE_IMG.width, SAMPLE_IMG.height, *block_size
    )
    check_decompressed(bgra)


def test_etc1():
    profile = ispc_texcomp.ETCEncSettings.from_profile("slow")
    raw = ispc_texcomp.compress_blocks_etc1(SURFACE, profile)
    bgra = texture2ddecoder.decode_etc1(raw, SAMPLE_IMG.width, SAMPLE_IMG.height)
    check_decompressed(bgra)


def test_bc1():
    raw = ispc_texcomp.compress_blocks_bc1(SURFACE)
    bgra = texture2ddecoder.decode_bc1(raw, SAMPLE_IMG.width, SAMPLE_IMG.height)
    check_decompressed(bgra)


def test_bc3():
    raw = ispc_texcomp.compress_blocks_bc3(SURFACE)
    bgra = texture2ddecoder.decode_bc3(raw, SAMPLE_IMG.width, SAMPLE_IMG.height)
    check_decompressed(bgra)


# def test_bc5():
#     raw = ispc_texcomp.compress_blocks_BC5(SURFACE)
#     bgra = texture2ddecoder.decode_bc5(raw, SAMPLE_IMG.width, SAMPLE_IMG.height)
#     check_decompressed(bgra)


# def test_bc6h():
#     profile = ispc_texcomp.GetProfile_bc6h_fast()
#     raw = ispc_texcomp.compress_blocks_BC6H(SURFACE, profile)
#     bgra = texture2ddecoder.decode_bc6(raw, SAMPLE_IMG.width, SAMPLE_IMG.height)
#     check_decompressed(bgra)


def test_bc7():
    profile = ispc_texcomp.BC7EncSettings.from_profile("fast")
    raw = ispc_texcomp.compress_blocks_bc7(SURFACE, profile)
    bgra = texture2ddecoder.decode_bc7(raw, SAMPLE_IMG.width, SAMPLE_IMG.height)
    check_decompressed(bgra)


if __name__ == "__main__":
    for item in dir():
        if item.startswith("test_"):
            try:
                print("Running test: " + item)
                globals()[item]()
            except Exception as e:
                print("Test failed: " + item, e)
