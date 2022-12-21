#define PY_SSIZE_T_CLEAN /* Make "s#" use Py_ssize_t rather than int. */
#include <Python.h>
#include "ispc_texcomp.h"

#include "rgba_surface_py.hpp"
#include "settings.hpp"

typedef void (*compress_f)(const rgba_surface *, uint8_t *dst);

template <compress_f F, char ratio>
static PyObject *py_compress(PyObject *self, PyObject *args)
{
    printf("py_compress\n");
    PyObject *py_src;
    if (!PyArg_ParseTuple(args, "O", &py_src))
        return NULL;

    if (!PyObject_TypeCheck(py_src, &RGBASurfaceType))
    {
        PyErr_SetString(PyExc_TypeError, "src must be a RGBASurface");
        return NULL;
    }

    rgba_surface *src = &((RGBASurfaceObject *)py_src)->surf;
    size_t size = src->width * src->height;
    if (ratio > 1)
    {
        size /= ratio;
    }
    uint8_t *dst = (uint8_t *)PyMem_Malloc(size);
    F(src, dst);
    PyObject *result = PyBytes_FromStringAndSize((const char *)dst, size);
    PyMem_Free(dst);
    printf("compress done\n");
    return result;
}

#define COMPRESS_W_SETTINGS(F, ratio, settings_object, settings_object_type, settings_type, name) \
    static PyObject *py_compress_##name(PyObject *self, PyObject *args)                           \
    {                                                                                             \
        PyObject *py_src;                                                                         \
        PyObject *py_settings;                                                                    \
        if (!PyArg_ParseTuple(args, "OO", &py_src, &py_settings))                                 \
            return NULL;                                                                          \
        if (!PyObject_TypeCheck(py_src, &RGBASurfaceType))                                        \
        {                                                                                         \
            PyErr_SetString(PyExc_TypeError, "src must be a RGBASurface");                        \
            return NULL;                                                                          \
        }                                                                                         \
        if (!PyObject_TypeCheck(py_settings, &##settings_object_type))                            \
        {                                                                                         \
            PyErr_SetString(PyExc_TypeError, "src must be a " #settings_object_type);             \
            return NULL;                                                                          \
        }                                                                                         \
        rgba_surface *src = &((RGBASurfaceObject *)py_src)->surf;                                 \
        size_t size = src->width * src->height;                                                   \
        if (##ratio > 1)                                                                          \
        {                                                                                         \
            size /= ratio;                                                                        \
        }                                                                                         \
        uint8_t *dst = (uint8_t *)PyMem_Malloc(size);                                             \
        ##settings_type *settings = &((##settings_object *)py_settings)->settings;                \
        ##F(src, dst, settings);                                                                  \
        PyObject *result = PyBytes_FromStringAndSize((const char *)dst, size);                    \
        PyMem_Free(dst);                                                                          \
        return result;                                                                            \
    };

COMPRESS_W_SETTINGS(CompressBlocksBC6H, 1, BC6HEncSettingsObject, BC6HEncSettingsType, bc6h_enc_settings, BC6H)
COMPRESS_W_SETTINGS(CompressBlocksBC7, 1, BC7EncSettingsObject, BC7EncSettingsType, bc7_enc_settings, BC7)
COMPRESS_W_SETTINGS(CompressBlocksETC1, 1, ETCEncSettingsObject, ETCEncSettingsType, etc_enc_settings, ETC1)
COMPRESS_W_SETTINGS(CompressBlocksASTC, 1, ASTCEncSettingsObject, ASTCEncSettingsType, astc_enc_settings, ASTC)

// Exported methods are collected in a table
static struct PyMethodDef method_table[] = {
    {"CompressBlocksBC1", (PyCFunction)py_compress<CompressBlocksBC1, 1>, METH_VARARGS, "compress a rgba_surface to "},
    {"CompressBlocksBC3", (PyCFunction)py_compress<CompressBlocksBC3, 1>, METH_VARARGS, "compress a rgba_surface to "},
    {"CompressBlocksBC4", (PyCFunction)py_compress<CompressBlocksBC4, 2>, METH_VARARGS, "compress a rgba_surface to "},
    {"CompressBlocksBC5", (PyCFunction)py_compress<CompressBlocksBC5, 1>, METH_VARARGS, "compress a rgba_surface to "},
    {"CompressBlocksBC6H", (PyCFunction)py_compress_BC6H, METH_VARARGS, "compress a rgba_surface to "},
    {"CompressBlocksBC7", (PyCFunction)py_compress_BC7, METH_VARARGS, "compress a rgba_surface to "},
    {"CompressBlocksETC1", (PyCFunction)py_compress_ETC1, METH_VARARGS, "compress a rgba_surface to "},
    {"CompressBlocksASTC", (PyCFunction)py_compress_ASTC, METH_VARARGS, "compress a rgba_surface to "},
    {NULL,
     NULL,
     0,
     NULL} // Sentinel value ending the table
};

// A struct contains the definition of a module
static PyModuleDef ispc_texcomp_py_module = {
    PyModuleDef_HEAD_INIT,
    "ispc_texcomp_py", // Module name
    "a python wrapper for Perfare's ispc_texcomp_py",
    -1, // Optional size of the module state memory
    method_table,
    NULL, // Optional slot definitions
    NULL, // Optional traversal function
    NULL, // Optional clear function
    NULL  // Optional module deallocation function
};

static void add_type(PyObject *m, PyTypeObject *obj, const char *name)
{
    if (PyType_Ready(obj) < 0)
        return;
    Py_INCREF(obj);
    PyModule_AddObject(m, name, (PyObject *)obj);
}

// The module init function
PyMODINIT_FUNC PyInit_ispc_texcomp_py(void)
{
    PyObject *m = PyModule_Create(&ispc_texcomp_py_module);
    if (m == NULL)
        return NULL;
    add_type(m, &BC6HEncSettingsType, "BC6HEncSettings");
    add_type(m, &BC7EncSettingsType, "BC7EncSettings");
    add_type(m, &ETCEncSettingsType, "ETCEncSettings");
    add_type(m, &ASTCEncSettingsType, "ASTCEncSettings");
    add_type(m, &RGBASurfaceType, "RGBASurface");
    return m;
}