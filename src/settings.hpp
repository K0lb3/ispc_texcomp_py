#include <map>
#include <string>
#include "Python.h"

typedef struct
{
    PyObject_HEAD
        bc7_enc_settings settings;
} BC7EncSettingsObject;

typedef struct
{
    PyObject_HEAD
        bc6h_enc_settings settings;
} BC6HEncSettingsObject;

typedef struct
{
    PyObject_HEAD
        etc_enc_settings settings;
} ETCEncSettingsObject;

typedef struct
{
    PyObject_HEAD
        astc_enc_settings settings;
} ASTCEncSettingsObject;

///////////////////////////////////////////////////////////////////////////////////
// BC7EncSettings
typedef void (*GetProfileFunc)(bc7_enc_settings *settings);
std::map<std::string, GetProfileFunc> bc7_profile_map = {
    {"ultrafast", GetProfile_ultrafast},
    {"veryfast", GetProfile_veryfast},
    {"fast", GetProfile_fast},
    {"basic", GetProfile_basic},
    {"slow", GetProfile_slow},
    {"alpha_ultrafast", GetProfile_alpha_ultrafast},
    {"alpha_veryfast", GetProfile_alpha_veryfast},
    {"alpha_fast", GetProfile_alpha_fast},
    {"alpha_basic", GetProfile_alpha_basic},
    {"alpha_slow", GetProfile_alpha_slow},
};

int BC7EncSettings_init(BC7EncSettingsObject *self, PyObject *args, PyObject *kwds)
{
    const char *kwlist[] = {
        "mode_selection",
        "refineIterations",
        "skip_mode2",
        "fastSkipTreshold_mode1",
        "fastSkipTreshold_mode3",
        "fastSkipTreshold_mode7",
        "mode45_channel0",
        "refineIterations_channel",
        "channels",
        "profile",
        NULL};
    char *profile = NULL;
    PyObject *mode_selection = NULL;
    PyObject *refineIterations = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|OObiiiiiis", (char **)kwlist,
                                     &mode_selection,
                                     &refineIterations,
                                     &self->settings.skip_mode2,
                                     &self->settings.fastSkipTreshold_mode1,
                                     &self->settings.fastSkipTreshold_mode3,
                                     &self->settings.fastSkipTreshold_mode7,
                                     &self->settings.mode45_channel0,
                                     &self->settings.refineIterations_channel,
                                     &self->settings.channels,
                                     &profile))
        return -1;

    if (profile != NULL)
    {
        auto it = bc7_profile_map.find(profile);
        if (it != bc7_profile_map.end())
        {
            it->second(&self->settings);
        }
        else
        {
            PyErr_SetString(PyExc_ValueError, "Invalid profile");
            return -1;
        }
    }

    if (mode_selection != NULL)
    {
        if (!PyList_Check(mode_selection))
        {
            PyErr_SetString(PyExc_ValueError, "mode_selection must be a list");
            return -1;
        }
        if (PyList_Size(mode_selection) != 4)
        {
            PyErr_SetString(PyExc_ValueError, "mode_selection must be a list of 4 booleans");
            return -1;
        }
        for (int i = 0; i < 4; i++)
        {
            PyObject *item = PyList_GetItem(mode_selection, i);
            if (!PyBool_Check(item))
            {
                PyErr_SetString(PyExc_ValueError, "mode_selection must be a list of 4 booleans");
                return -1;
            }
            self->settings.mode_selection[i] = (item == Py_True);
        }
    }
    if (refineIterations != NULL)
    {
        if (!PyList_Check(refineIterations))
        {
            PyErr_SetString(PyExc_ValueError, "refineIterations must be a list");
            return -1;
        }
        if (PyList_Size(refineIterations) != 8)
        {
            PyErr_SetString(PyExc_ValueError, "refineIterations must be a list of 4 integers");
            return -1;
        }
        for (int i = 0; i < 8; i++)
        {
            PyObject *item = PyList_GetItem(refineIterations, i);
            if (!PyLong_Check(item))
            {
                PyErr_SetString(PyExc_ValueError, "refineIterations must be a list of 4 integers");
                return -1;
            }
            self->settings.refineIterations[i] = PyLong_AsLong(item);
        }
    }
    return 0;
}

PyMemberDef BC7EncSettingsObject_members[] = {
    {"skip_mode2", T_BOOL, offsetof(BC7EncSettingsObject, settings.skip_mode2), 0, "skip_mode2"},
    {"fastSkipTreshold_mode1", T_INT, offsetof(BC7EncSettingsObject, settings.fastSkipTreshold_mode1), 0, "fastSkipTreshold_mode1"},
    {"fastSkipTreshold_mode3", T_INT, offsetof(BC7EncSettingsObject, settings.fastSkipTreshold_mode3), 0, "fastSkipTreshold_mode3"},
    {"fastSkipTreshold_mode7", T_INT, offsetof(BC7EncSettingsObject, settings.fastSkipTreshold_mode7), 0, "fastSkipTreshold_mode7"},
    {"mode45_channel0", T_BOOL, offsetof(BC7EncSettingsObject, settings.mode45_channel0), 0, "mode45_channel0"},
    {"refineIterations_channel", T_INT, offsetof(BC7EncSettingsObject, settings.refineIterations_channel), 0, "refineIterations_channel"},
    {"channels", T_INT, offsetof(BC7EncSettingsObject, settings.channels), 0, "channels"},
    {NULL} /* Sentinel */
};

// TODO getset for mode_selection and refineIterations
PyObject *BC7EncSettings_repr(BC7EncSettingsObject *self)
{
    return PyUnicode_FromFormat("BC7EncSettings(mode_selection=%s, refineIterations=%s, skip_mode2=%s, fastSkipTreshold_mode1=%d, fastSkipTreshold_mode3=%d, fastSkipTreshold_mode7=%d, mode45_channel0=%s, refineIterations_channel=%d, channels=%d)",
                                PyUnicode_FromFormat("[%s, %s, %s, %s]", self->settings.mode_selection[0] ? "True" : "False", self->settings.mode_selection[1] ? "True" : "False", self->settings.mode_selection[2] ? "True" : "False", self->settings.mode_selection[3] ? "True" : "False"),
                                PyUnicode_FromFormat("[%d, %d, %d, %d, %d, %d, %d, %d]", self->settings.refineIterations[0], self->settings.refineIterations[1], self->settings.refineIterations[2], self->settings.refineIterations[3], self->settings.refineIterations[4], self->settings.refineIterations[5], self->settings.refineIterations[6], self->settings.refineIterations[7]),
                                self->settings.skip_mode2 ? "True" : "False",
                                self->settings.fastSkipTreshold_mode1,
                                self->settings.fastSkipTreshold_mode3,
                                self->settings.fastSkipTreshold_mode7,
                                self->settings.mode45_channel0 ? "True" : "False",
                                self->settings.refineIterations_channel,
                                self->settings.channels);
}

PyTypeObject BC7EncSettingsType = []() -> PyTypeObject
{
    PyTypeObject type = {PyObject_HEAD_INIT(NULL) 0};
    type.tp_name = "BC7EncSettings";
    type.tp_basicsize = sizeof(BC7EncSettingsObject);
    type.tp_itemsize = 0;
    type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    type.tp_doc = "BC7EncSettings objects";
    type.tp_new = PyType_GenericNew;
    type.tp_init = (initproc)BC7EncSettings_init;
    type.tp_members = BC7EncSettingsObject_members;
    type.tp_repr = (reprfunc)BC7EncSettings_repr;
    return type;
}();

///////////////////////////////////////////////////////////////////////////////////
// BC6HEncSettings

typedef void (*GetProfile_bc6h)(bc6h_enc_settings *settings);
std::map<std::string, GetProfile_bc6h> bc6h_profile_map = {
    {"veryfast", GetProfile_bc6h_veryfast},
    {"fast", GetProfile_bc6h_fast},
    {"basic", GetProfile_bc6h_basic},
    {"slow", GetProfile_bc6h_slow},
    {"veryslow", GetProfile_bc6h_veryslow},
};

int BC6HEncSettings_init(BC6HEncSettingsObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"slow_mode", "fast_mode", "refineIterations_1p", "refineIterations_2p", "fastSkipTreshold", "profile", NULL};
    char *profile = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|bbiiis", kwlist,
                                     &self->settings.slow_mode,
                                     &self->settings.fast_mode,
                                     &self->settings.refineIterations_1p,
                                     &self->settings.refineIterations_2p,
                                     &self->settings.fastSkipTreshold,
                                     &profile))
        return -1;
    if (profile != NULL)
    {
        auto it = bc6h_profile_map.find(profile);
        if (it != bc6h_profile_map.end())
        {
            it->second(&self->settings);
        }
        else
        {
            PyErr_SetString(PyExc_ValueError, "Invalid profile");
            return -1;
        }
    }
    return 0;
}

PyMemberDef BC6HEncSettingsObject_members[] = {
    {"slow_mode", T_BOOL, offsetof(BC6HEncSettingsObject, settings.slow_mode), 0, "slow_mode"},
    {"fast_mode", T_BOOL, offsetof(BC6HEncSettingsObject, settings.fast_mode), 0, "fast_mode"},
    {"refineIterations_1p", T_INT, offsetof(BC6HEncSettingsObject, settings.refineIterations_1p), 0, "refineIterations_1p"},
    {"refineIterations_2p", T_INT, offsetof(BC6HEncSettingsObject, settings.refineIterations_2p), 0, "refineIterations_2p"},
    {"fastSkipTreshold", T_INT, offsetof(BC6HEncSettingsObject, settings.fastSkipTreshold), 0, "fastSkipTreshold"},
    {NULL} /* Sentinel */
};

PyObject *BC6HEncSettings_repr(BC6HEncSettingsObject *self)
{
    return PyUnicode_FromFormat("BC6HEncSettings(slow_mode=%s, fast_mode=%s, refineIterations_1p=%d, refineIterations_2p=%d, fastSkipTreshold=%d)",
                                self->settings.slow_mode ? "True" : "False",
                                self->settings.fast_mode ? "True" : "False",
                                self->settings.refineIterations_1p,
                                self->settings.refineIterations_2p,
                                self->settings.fastSkipTreshold);
}

PyTypeObject BC6HEncSettingsType = []() -> PyTypeObject
{
    PyTypeObject type = {PyObject_HEAD_INIT(NULL) 0};
    type.tp_name = "BC6HEncSettings";
    type.tp_basicsize = sizeof(BC6HEncSettingsObject);
    type.tp_itemsize = 0;
    type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    type.tp_doc = "BC6HEncSettings objects";
    type.tp_new = PyType_GenericNew;
    type.tp_init = (initproc)BC6HEncSettings_init;
    type.tp_members = BC6HEncSettingsObject_members;
    type.tp_repr = (reprfunc)BC6HEncSettings_repr;
    return type;
}();

///////////////////////////////////////////////////////////////////////////////////
// ETCEncSettings

typedef void (*GetProfile_etc)(etc_enc_settings *settings);
std::map<std::string, GetProfile_etc> etc_profile_map = {
    {"slow", GetProfile_etc_slow},
};

int ETCEncSettings_init(ETCEncSettingsObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"fastSkipTreshold", "profile", NULL};
    char *profile = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|is", kwlist,
                                     &self->settings.fastSkipTreshold, &profile))
        return -1;
    if (profile != NULL)
    {
        auto it = etc_profile_map.find(profile);
        if (it != etc_profile_map.end())
        {
            it->second(&self->settings);
        }
        else
        {
            PyErr_SetString(PyExc_ValueError, "Invalid profile");
            return -1;
        }
    }
    return 0;
}

PyMemberDef ETCEncSettingsObject_members[] = {
    {"fastSkipTreshold", T_INT, offsetof(ETCEncSettingsObject, settings.fastSkipTreshold), 0, "fastSkipTreshold"},
    {NULL} /* Sentinel */
};

PyObject *ETCEncSettings_repr(ETCEncSettingsObject *self)
{
    return PyUnicode_FromFormat("ETCEncSettings(fastSkipTreshold=%d)",
                                self->settings.fastSkipTreshold);
}

PyTypeObject ETCEncSettingsType = []() -> PyTypeObject
{
    PyTypeObject type = {PyObject_HEAD_INIT(NULL) 0};
    type.tp_name = "ETCEncSettings";
    type.tp_basicsize = sizeof(ETCEncSettingsObject);
    type.tp_itemsize = 0;
    type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    type.tp_doc = "ETCEncSettings objects";
    type.tp_new = PyType_GenericNew;
    type.tp_init = (initproc)ETCEncSettings_init;
    type.tp_members = ETCEncSettingsObject_members;
    type.tp_repr = (reprfunc)ETCEncSettings_repr;
    return type;
}();

///////////////////////////////////////////////////////////////////////////////////
// ASTCEncSettings

typedef void (*GetProfile_astc)(astc_enc_settings *settings, int block_width, int block_height);
std::map<std::string, GetProfile_astc> astc_profile_map = {
    {"fast", GetProfile_astc_fast},
    {"alpha_fast", GetProfile_astc_alpha_fast},
    {"alpha_slow", GetProfile_astc_alpha_slow},
};

int ASTCEncSettings_init(ASTCEncSettingsObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"block_width", "block_height", "channels", "fastSkipTreshold", "refineIterations", "profile", NULL};
    char *profile = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|iiiiis", kwlist,
                                     &self->settings.block_width,
                                     &self->settings.block_height,
                                     &self->settings.channels,
                                     &self->settings.fastSkipTreshold,
                                     &self->settings.refineIterations,
                                     &profile))
        return -1;

    if (profile != NULL)
    {
        auto it = astc_profile_map.find(profile);
        if (it != astc_profile_map.end())
        {
            it->second(&self->settings, self->settings.block_width, self->settings.block_height);
        }
        else
        {
            PyErr_SetString(PyExc_ValueError, "Invalid profile");
            return -1;
        }
    }

    return 0;
}

PyMemberDef ASTCEncSettingsObject_members[] = {
    {"block_width", T_INT, offsetof(ASTCEncSettingsObject, settings.block_width), 0, "block_width"},
    {"block_height", T_INT, offsetof(ASTCEncSettingsObject, settings.block_height), 0, "block_height"},
    {"channels", T_INT, offsetof(ASTCEncSettingsObject, settings.channels), 0, "channels"},
    {"fastSkipTreshold", T_INT, offsetof(ASTCEncSettingsObject, settings.fastSkipTreshold), 0, "fastSkipTreshold"},
    {"refineIterations", T_INT, offsetof(ASTCEncSettingsObject, settings.refineIterations), 0, "refineIterations"},
    {NULL} /* Sentinel */
};

PyObject *ASTCEncSettings_repr(ASTCEncSettingsObject *self)
{
    return PyUnicode_FromFormat("ASTCEncSettings(block_width=%d, block_height=%d, channels=%d, fastSkipTreshold=%d, refineIterations=%d)",
                                self->settings.block_width,
                                self->settings.block_height,
                                self->settings.channels,
                                self->settings.fastSkipTreshold,
                                self->settings.refineIterations);
}

PyTypeObject ASTCEncSettingsType = []() -> PyTypeObject
{
    PyTypeObject type = {PyObject_HEAD_INIT(NULL) 0};
    type.tp_name = "ASTCEncSettings";
    type.tp_basicsize = sizeof(ASTCEncSettingsObject);
    type.tp_itemsize = 0;
    type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    type.tp_doc = "ASTCEncSettings objects";
    type.tp_new = PyType_GenericNew;
    type.tp_init = (initproc)ASTCEncSettings_init;
    type.tp_members = ASTCEncSettingsObject_members;
    type.tp_repr = (reprfunc)ASTCEncSettings_repr;
    return type;
}();
