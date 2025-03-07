#define PY_SSIZE_T_CLEAN /* Make "s#" use Py_ssize_t rather than int. */
#include <Python.h>
#include "ispc_texcomp.h"

#include "rgba_surface_py.hpp"
#include "settings.hpp"

template <auto compress_func, size_t ratio>
PyObject *py_compress(PyObject *self, PyObject *args) noexcept
{
    RGBASurfaceObject *py_src;
    if (!PyArg_ParseTuple(args, "O!", RGBASurfaceObjectType, &py_src))
        return nullptr;

    const auto &src = py_src->surf;
    size_t size = src.width * src.height;
    if (ratio > 1)
    {
        size /= ratio;
    }
    PyObject *result = PyBytes_FromStringAndSize(nullptr, size);
    if (!result)
        return nullptr;
    uint8_t *dst = (uint8_t *)PyBytes_AsString(result);
    Py_BEGIN_ALLOW_THREADS
        compress_func(&src, dst);
    Py_END_ALLOW_THREADS return result;
}

template <auto compress_func, class SettingsObject, PyTypeObject **SettingsObjectType>
PyObject *py_compress_s(PyObject *self, PyObject *args) noexcept
{
    RGBASurfaceObject *py_src;
    SettingsObject *py_settings;
    if (!PyArg_ParseTuple(args, "O!O!", RGBASurfaceObjectType, &py_src, *SettingsObjectType, &py_settings))
        return nullptr;

    const auto &src = py_src->surf;
    size_t size = src.width * src.height;
    PyObject *result = PyBytes_FromStringAndSize(nullptr, size);
    if (!result)
        return nullptr;
    uint8_t *dst = (uint8_t *)PyBytes_AsString(result);
    Py_BEGIN_ALLOW_THREADS
        compress_func(&src, dst, &py_settings->settings);
    Py_END_ALLOW_THREADS return result;
}

// Exported methods are collected in a table
constexpr PyMethodDef method_table[] = {
    {"compress_blocks_bc1", py_compress<CompressBlocksBC1, 1>, METH_VARARGS, "compress a rgba_surface to bc1"},
    {"compress_blocks_bc3", py_compress<CompressBlocksBC3, 1>, METH_VARARGS, "compress a rgba_surface to bc3"},
    {"compress_blocks_bc4", py_compress<CompressBlocksBC4, 2>, METH_VARARGS, "compress a rgba_surface to bc4"},
    {"compress_blocks_bc5", py_compress<CompressBlocksBC5, 1>, METH_VARARGS, "compress a rgba_surface to bc5"},
    {"compress_blocks_bc6h", py_compress_s<CompressBlocksBC6H, BC6HEncSettingsObject, &BC6HEncSettingsObjectType>, METH_VARARGS, "compress a rgba_surface to bc6h"},
    {"compress_blocks_bc7", py_compress_s<CompressBlocksBC7, BC7EncSettingsObject, &BC7EncSettingsObjectType>, METH_VARARGS, "compress a rgba_surface to bc7"},
    {"compress_blocks_etc1", py_compress_s<CompressBlocksETC1, ETCEncSettingsObject, &ETCEncSettingsObjectType>, METH_VARARGS, "compress a rgba_surface to etc1"},
    {"compress_blocks_astc", py_compress_s<CompressBlocksASTC, ASTCEncSettingsObject, &ASTCEncSettingsObjectType>, METH_VARARGS, "compress a rgba_surface to astc"},
    {NULL, NULL, 0, NULL} // Sentinel value ending the table
};

// A struct contains the definition of a module
PyModuleDef ispc_texcomp_module = {
    PyModuleDef_HEAD_INIT,
    "ispc_texcomp._ispc_texcomp", // Module name
    "Python bindings for ISPCTextureCompressor",
    -1, // Optional size of the module state memory
    const_cast<PyMethodDef *>(method_table),
    NULL, // Optional slot definitions
    NULL, // Optional traversal function
    NULL, // Optional clear function
    NULL  // Optional module deallocation function
};

bool register_type(PyObject *module, PyTypeObject *type, const char *name) noexcept
{
    if (PyType_Ready(type) < 0)
        return false;
    Py_IncRef((PyObject *)type);
    return PyModule_AddObject(module, name, reinterpret_cast<PyObject *>(type)) == 0;
}

PyMODINIT_FUNC PyInit__ispc_texcomp()
{
    auto *m = PyModule_Create(&ispc_texcomp_module);
    if (!m)
        return nullptr;

    auto create_type = [&](PyType_Spec *spec, PyTypeObject **type, const char *name)
    {
        *type = reinterpret_cast<PyTypeObject *>(PyType_FromSpec(spec));
        return *type && register_type(m, *type, name);
    };

    bool success = true;
    success &= create_type(&BC6HEncSettingsType_Spec, &BC6HEncSettingsObjectType, "BC6HEncSettings");
    success &= create_type(&BC7EncSettingsType_Spec, &BC7EncSettingsObjectType, "BC7EncSettings");
    success &= create_type(&ETCEncSettingsType_Spec, &ETCEncSettingsObjectType, "ETCEncSettings");
    success &= create_type(&ASTCEncSettingsType_Spec, &ASTCEncSettingsObjectType, "ASTCEncSettings");
    success &= create_type(&RGBASurfaceType_Spec, &RGBASurfaceObjectType, "RGBASurface");

    if (!success)
    {
        Py_DECREF(m);
        return nullptr;
    }
    return m;
}