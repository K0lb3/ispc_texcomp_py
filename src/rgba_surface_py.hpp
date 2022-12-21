#include <Python.h>
#include "structmember.h"

typedef struct
{
    PyObject_HEAD
        PyObject *obj;
        rgba_surface surf;
} RGBASurfaceObject;

void RGBASurface_dealloc(RGBASurfaceObject *self)
{
    Py_XDECREF(self->obj);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

PyObject *
RGBASurface_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    printf("RGBASurface_new\n");
    RGBASurfaceObject *self;
    self = (RGBASurfaceObject *)type->tp_alloc(type, 0);
    if (self != NULL)
    {
        self->obj = NULL;
        self->surf = {NULL, 0, 0, 0};
    }
    return (PyObject *)self;
}

int RGBASurface_init(RGBASurfaceObject *self, PyObject *args, PyObject *kwds)
{
    printf("RGBASurface_init\n");
    const char *kwlist[] = {
        "src",
        "width",
        "height",
        "stride",
        NULL};

    PyObject *buf = NULL;
    if (!PyArg_ParseTupleAndKeywords(
            args,
            kwds,
            "Oii|i",
            (char **)kwlist,
            &buf,
            &self->surf.width,
            &self->surf.height,
            &self->surf.stride))
        return -1;
    if (buf == NULL)
    {
        PyErr_SetString(PyExc_TypeError, "The src attribute value must be set!");
        return -1;
    }

    Py_buffer view;
    if (Py_TYPE(buf)->tp_as_buffer && Py_TYPE(buf)->tp_as_buffer->bf_releasebuffer)
    {
        buf = PyMemoryView_FromObject(buf);
        if (buf == NULL)
        {
            return -1;
        }
    }
    else
    {
        Py_INCREF(buf);
    }

    if (PyObject_GetBuffer(buf, &view, PyBUF_SIMPLE) < 0)
    {
        Py_DECREF(buf);
        return -1;
    }

    self->obj = buf;
    self->surf.ptr = (uint8_t *)view.buf;

    if (self->surf.stride == 0)
    {
        self->surf.stride = view.len / self->surf.height;
    }
    else
    {
        if (self->surf.stride * self->surf.height >= view.len)
        {
            PyErr_SetString(PyExc_TypeError, "The stride value is too big!\nIt has to be the size of a single row in bytes. (e.g. for RGBA -> 4 * width)");
            return -1;
        }
    }

    PyBuffer_Release(&view);
    return 0;
};

PyMemberDef RGBASurface_members[] = {
    {"width", T_INT, offsetof(RGBASurfaceObject, surf.width), 0, "width"},
    {"height", T_INT, offsetof(RGBASurfaceObject, surf.height), 0, "height"},
    {"stride", T_SHORT, offsetof(RGBASurfaceObject, surf.stride), 0, "stride"},
    {NULL} /* Sentinel */
};

PyObject *RGBASurface_getObj(RGBASurfaceObject *self, void *closure)
{
    // TODO - is this correct?
    Py_INCREF(self->obj);
    return self->obj;
}

int RGBASurface_setObj(RGBASurfaceObject *self, PyObject *value, void *closure)
{
    if (value == NULL)
    {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the obj attribute");
        return -1;
    }

    Py_DECREF(self->obj);
    Py_INCREF(value);
    self->obj = value;
    return 0;
}

// PyObject *RGBASurface_getData(RGBASurfaceObject *self, void *closure)
// {
//     return PyMemoryView_FromMemory((char *)self->data, self->stride * self->height, PyBUF_READ);
// }

PyGetSetDef RGBASurface_getsetters[] = {
    {"obj", (getter)RGBASurface_getObj, (setter)RGBASurface_setObj,
     "obj", NULL},
    //{"data", (getter)RGBASurface_getData, NULL, "data", NULL},
    // TODO - make obj editable
    // {"m_Type", (getter)RGBASurface_getType, (setter)RGBASurface_setType,
    //  "", NULL},
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

PyTypeObject RGBASurfaceType = []() -> PyTypeObject
{
    PyTypeObject type = {PyObject_HEAD_INIT(NULL) 0};
    type.tp_name = "RGBASurface";
    type.tp_doc = PyDoc_STR("RGBASurface objects");
    type.tp_basicsize = sizeof(RGBASurfaceObject);
    type.tp_itemsize = 0;
    type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    type.tp_new = RGBASurface_new;
    type.tp_init = (initproc)RGBASurface_init;
    type.tp_dealloc = (destructor)RGBASurface_dealloc;
    type.tp_members = RGBASurface_members;
    type.tp_getset = RGBASurface_getsetters;
    type.tp_repr = (reprfunc)RGBASurface_repr;
    return type;
}();
