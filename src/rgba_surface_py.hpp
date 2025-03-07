#include <Python.h>
#include "structmember.h"

PyTypeObject *RGBASurfaceObjectType = nullptr;

typedef struct
{
    PyObject_HEAD
        Py_buffer view;
    rgba_surface surf;
} RGBASurfaceObject;

void RGBASurface_dealloc(RGBASurfaceObject *self)
{
    if (self->view.buf != nullptr)
    {
        PyBuffer_Release(&self->view);
    }
    PyObject_Del((PyObject *)self);
}

int RGBASurface_init(RGBASurfaceObject *self, PyObject *args, PyObject *kwds)
{
    self->view = {};
    self->surf = {};

    static const char *kwlist[] = {
        "src",
        "width",
        "height",
        "stride",
        nullptr};

    // Clear existing buffer if reinitialized
    if (self->view.buf)
        PyBuffer_Release(&self->view);

    // Parse arguments
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "y*ii|i",
                                     const_cast<char **>(kwlist),
                                     &self->view,
                                     &self->surf.width,
                                     &self->surf.height,
                                     &self->surf.stride))
    {
        return -1;
    }

    // Auto-calculate stride if not provided
    if (self->surf.stride == 0)
    {
        self->surf.stride = self->surf.width * 4; // RGBA
    }

    // Validate geometry
    const size_t expected_size = self->surf.stride * self->surf.height;
    if (self->view.len < expected_size)
    {
        PyErr_Format(PyExc_ValueError,
                     "Buffer too small (need %zu bytes, got %zd)",
                     expected_size, self->view.len);
        return -1;
    }

    self->surf.ptr = static_cast<uint8_t *>(self->view.buf);
    return 0;
};

PyMemberDef RGBASurface_members[] = {
    {"width", T_INT, offsetof(RGBASurfaceObject, surf.width), 0, "width"},
    {"height", T_INT, offsetof(RGBASurfaceObject, surf.height), 0, "height"},
    {"stride", T_SHORT, offsetof(RGBASurfaceObject, surf.stride), 0, "stride"},
    {NULL} /* Sentinel */
};

PyObject *RGBASurface_getData(PyObject *self, void *closure)
{
    // leveraging the buffer protocol
    return PyBytes_FromObject(self);
}

PyGetSetDef RGBASurface_getsetters[] = {
    {"data", (getter)RGBASurface_getData, NULL, "data", NULL},
    {NULL} /* Sentinel */
};

PyObject *
RGBASurface_repr(PyObject *self)
{
    RGBASurfaceObject *node = (RGBASurfaceObject *)self;
    return PyUnicode_FromFormat(
        "<RGBA_Surface (w:%d, h:%d, s:%d)>",
        // node->obj->__repr__(node->obj),
        node->surf.width,
        node->surf.height,
        node->surf.stride);
}

static int RGBASurface_getbuffer(RGBASurfaceObject *self, Py_buffer *view, int flags)
{
    if (!view)
    {
        PyErr_SetString(PyExc_ValueError, "NULL view");
        return -1;
    }

    view->buf = self->view.buf;
    view->obj = reinterpret_cast<PyObject *>(self);
    Py_INCREF(view->obj); // Retain ownership

    view->len = self->view.len;
    view->readonly = 0;
    view->itemsize = sizeof(uint8_t);
    view->format = "BBBB"; // RGBA components
    view->ndim = 3;
    view->shape = new Py_ssize_t[3]{
        static_cast<Py_ssize_t>(self->surf.height),
        static_cast<Py_ssize_t>(self->surf.width),
        4 // RGBA
    };
    view->strides = new Py_ssize_t[3]{
        static_cast<Py_ssize_t>(self->surf.stride),
        static_cast<Py_ssize_t>(4 * sizeof(uint8_t)), // Pixel stride
        sizeof(uint8_t)                               // Component stride
    };
    return 0;
}

static void RGBASurface_releasebuffer(PyObject *, Py_buffer *view)
{
    if (view)
    {
        delete[] view->shape;
        delete[] view->strides;
        view->shape = nullptr;
        view->strides = nullptr;
    }
}

PyType_Slot RGBASurfaceType_slots[] = {
    {Py_tp_new, reinterpret_cast<void *>(PyType_GenericNew)},
    {Py_tp_init, reinterpret_cast<void *>(RGBASurface_init)},
    {Py_tp_dealloc, reinterpret_cast<void *>(RGBASurface_dealloc)},
    {Py_tp_members, RGBASurface_members},
    {Py_tp_getset, reinterpret_cast<void *>(RGBASurface_getsetters)},
    {Py_bf_getbuffer, reinterpret_cast<void *>(RGBASurface_getbuffer)},
    {Py_bf_releasebuffer, reinterpret_cast<void *>(RGBASurface_releasebuffer)},
    {Py_tp_repr, reinterpret_cast<void *>(RGBASurface_repr)},
    {0, NULL},
};

PyType_Spec RGBASurfaceType_Spec = {
    "ispc_texcomp.RGBASurface",               // const char* name;
    sizeof(RGBASurfaceObject),                // int basicsize;
    0,                                        // int itemsize;
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, // unsigned int flags;
    RGBASurfaceType_slots,                    // PyType_Slot *slots;
};