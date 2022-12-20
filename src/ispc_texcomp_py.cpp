#define PY_SSIZE_T_CLEAN /* Make "s#" use Py_ssize_t rather than int. */
#include <Python.h>
#include <pybind11/pybind11.h>
using namespace pybind11::literals;
namespace py = pybind11;

#include "ispc_texcomp.h"

PYBIND11_MODULE(ispc_texcomp_py, m)
{
    m.doc() = "pbrt python bindings"; // optional module docstring
    py::class_<rgba_surface>(m, "rgba_surface")
        .def(py::init<>())
        .def_readwrite("width", &rgba_surface::width)
        .def_readwrite("height", &rgba_surface::height)
        .def_readwrite("stride", &rgba_surface::stride)
        .def_property(
            "ptr", [](rgba_surface &self) -> py::memoryview {
                return py::memoryview::from_buffer(self.ptr, {self.height*self.stride},{sizeof(char)});
            }, [](rgba_surface &self, std::string string){
                if (self.ptr != NULL){
                    free(self.ptr);
                }
                self.ptr = (uint8_t*)malloc(string.size());
                memcpy(self.ptr, string.data(), string.size());
            }
        );

    py::class_<bc7_enc_settings>(m, "bc7_enc_settings")
        .def(py::init<>())
        .def_readwrite("skip_mode2", &bc7_enc_settings::skip_mode2)
        .def_readwrite("fastSkipTreshold_mode1", &bc7_enc_settings::fastSkipTreshold_mode1)
        .def_readwrite("fastSkipTreshold_mode3", &bc7_enc_settings::fastSkipTreshold_mode3)
        .def_readwrite("fastSkipTreshold_mode7", &bc7_enc_settings::fastSkipTreshold_mode7)
        .def_readwrite("mode45_channel0", &bc7_enc_settings::mode45_channel0)
        .def_readwrite("refineIterations_channel", &bc7_enc_settings::refineIterations_channel)
        .def_readwrite("channels", &bc7_enc_settings::channels)
        .def_property(
            "mode_selection", [](bc7_enc_settings &settings) -> py::memoryview
            { return py::memoryview::from_buffer(
                  settings.mode_selection,
                  {4},
                  {sizeof(bool)}); },
            [](bc7_enc_settings &settings, py::object obj)
            {
                py::list list = obj.cast<py::list>();
                if (list.size() != 4)
                    throw pybind11::value_error("mode_selection must be a list of 4 bools");
                for (int i = 0; i < 4; i++)
                {
                    settings.mode_selection[i] = list[i].cast<bool>();
                }
            })
        .def_property(
            "refineIterations", [](bc7_enc_settings &settings) -> py::memoryview
            { return py::memoryview::from_buffer(
                  settings.refineIterations,
                  {8},
                  {sizeof(int)}); },
            [](bc7_enc_settings &settings, py::object obj)
            {
                py::list list = obj.cast<py::list>();
                if (list.size() != 8)
                    throw pybind11::value_error("refineIterations must be a list of 8 ints");
                for (int i = 0; i < 8; i++)
                {
                    settings.refineIterations[i] = list[i].cast<int>();
                }
            });

    py::class_<bc6h_enc_settings>(m, "bc6h_enc_settings")
        .def(py::init<>())
        .def_readwrite("slow_mode", &bc6h_enc_settings::slow_mode)
        .def_readwrite("fast_mode", &bc6h_enc_settings::fast_mode)
        .def_readwrite("refineIterations_1p", &bc6h_enc_settings::refineIterations_1p)
        .def_readwrite("refineIterations_2p", &bc6h_enc_settings::refineIterations_2p)
        .def_readwrite("fastSkipTreshold", &bc6h_enc_settings::fastSkipTreshold);

    py::class_<etc_enc_settings>(m, "etc_enc_settings")
        .def(py::init<>())
        .def_readwrite("fastSkipTreshold", &etc_enc_settings::fastSkipTreshold);

    py::class_<astc_enc_settings>(m, "astc_enc_settings")
        .def(py::init<>())
        .def_readwrite("block_width", &astc_enc_settings::block_width)
        .def_readwrite("block_height", &astc_enc_settings::block_height)
        .def_readwrite("channels", &astc_enc_settings::channels)
        .def_readwrite("fastSkipTreshold", &astc_enc_settings::fastSkipTreshold)
        .def_readwrite("refineIterations", &astc_enc_settings::refineIterations);

    m.def("GetProfile_ultrafast", &GetProfile_ultrafast, "A function which returns a bc7_enc_settings");
    m.def("GetProfile_veryfast", &GetProfile_veryfast, "A function which returns a bc7_enc_settings");
    m.def("GetProfile_fast", &GetProfile_fast, "A function which returns a bc7_enc_settings");
    m.def("GetProfile_basic", &GetProfile_basic, "A function which returns a bc7_enc_settings");
    m.def("GetProfile_slow", &GetProfile_slow, "A function which returns a bc7_enc_settings");

    m.def("GetProfile_alpha_ultrafast", &GetProfile_alpha_ultrafast, "A function which returns a bc7_enc_settings");
    m.def("GetProfile_alpha_veryfast", &GetProfile_alpha_veryfast, "A function which returns a bc7_enc_settings");
    m.def("GetProfile_alpha_fast", &GetProfile_alpha_fast, "A function which returns a bc7_enc_settings");
    m.def("GetProfile_alpha_basic", &GetProfile_alpha_basic, "A function which returns a bc7_enc_settings");
    m.def("GetProfile_alpha_slow", &GetProfile_alpha_slow, "A function which returns a bc7_enc_settings");

    m.def("GetProfile_bc6h_beryfast", &GetProfile_bc6h_veryfast, "A function which returns a bc6h_enc_settings");
    m.def("GetProfile_bc6h_fast", &GetProfile_bc6h_fast, "A function which returns a bc6h_enc_settings");
    m.def("GetProfile_bc6h_basic", &GetProfile_bc6h_basic, "A function which returns a bc6h_enc_settings");
    m.def("GetProfile_bc6h_slow", &GetProfile_bc6h_slow, "A function which returns a bc6h_enc_settings");
    m.def("GetProfile_bc6h_veryslow", &GetProfile_bc6h_veryslow, "A function which returns a bc6h_enc_settings");

    m.def("GetProfile_etc_slow", &GetProfile_etc_slow, "A function which returns a etc_enc_settings");

    m.def("GetProfile_astc_fast", &GetProfile_astc_fast, "A function which returns a astc_enc_settings");
    m.def("GetProfile_astc_alpha_fast", &GetProfile_astc_alpha_fast, "A function which returns a astc_enc_settings");
    m.def("GetProfile_astc_alpha_slow", &GetProfile_astc_alpha_slow, "A function which returns a astc_enc_settings");

    m.def("ReplicateBorders", &ReplicateBorders, "helper function to replicate border pixels for the desired block sizes (bpp = 32 or 64)");

    m.def("CompressBlocksBC1", &CompressBlocksBC1, "Compresses an rgba_surface to BC1");
    m.def("CompressBlocksBC3", &CompressBlocksBC3, "Compresses an rgba_surface to BC3");
    m.def("CompressBlocksBC4", &CompressBlocksBC4, "Compresses an rgba_surface to BC4");
    m.def("CompressBlocksBC5", &CompressBlocksBC5, "Compresses an rgba_surface to BC5");
    m.def("CompressBlocksBC6H", &CompressBlocksBC6H, "Compresses an rgba_surface to BC6H");
    m.def("CompressBlocksBC7", &CompressBlocksBC7, "Compresses an rgba_surface to BC7");
    m.def("CompressBlocksETC1", &CompressBlocksETC1, "Compresses an rgba_surface to ETC1");
    m.def("CompressBlocksASTC", &CompressBlocksASTC, "Compresses an rgba_surface to ASTC");
}