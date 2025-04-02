#include <unordered_map>
#include <string>
#include "Python.h"

PyTypeObject *BC7EncSettingsObjectType = nullptr;
PyTypeObject *BC6HEncSettingsObjectType = nullptr;
PyTypeObject *ETCEncSettingsObjectType = nullptr;
PyTypeObject *ASTCEncSettingsObjectType = nullptr;

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

template <class SettingsObject, auto &ProfileMap>
static PyObject *settings_from_profile(PyObject *cls, PyObject *args)
{
    char *profile = NULL;

    if (!PyArg_ParseTuple(args, "s", &profile))
    {
        return NULL;
    }

    auto it = ProfileMap.find(profile);
    if (it != ProfileMap.end())
    {
        PyObject *settings_py = PyType_GenericNew((PyTypeObject *)cls, NULL, NULL);
        if (!settings_py)
        {
            return NULL;
        }
        it->second(&(reinterpret_cast<SettingsObject *>(settings_py)->settings));
        return settings_py;
    }
    else
    {
        PyErr_SetString(PyExc_ValueError, "Invalid profile");
        return nullptr;
    }
}

///////////////////////////////////////////////////////////////////////////////////
// BC7EncSettings
typedef void (*GetProfileFunc)(bc7_enc_settings *settings);
static std::unordered_map<std::string, GetProfileFunc> bc7_profile_map = {
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
    static const char *kwlist[] = {
        "mode_selection",
        "refine_iterations",
        "skip_mode2",
        "fast_skip_threshold_mode1",
        "fast_skip_threshold_mode3",
        "fast_skip_threshold_mode7",
        "mode45_channel0",
        "refine_iterations_channel",
        "channels",
        NULL};
    int skip_mode2 = 0;
    PyObject *mode_selection = NULL;
    PyObject *refineIterations = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OObiiiiii", const_cast<char **>(kwlist),
                                     &mode_selection,
                                     &refineIterations,
                                     &skip_mode2,
                                     &self->settings.fastSkipTreshold_mode1,
                                     &self->settings.fastSkipTreshold_mode3,
                                     &self->settings.fastSkipTreshold_mode7,
                                     &self->settings.mode45_channel0,
                                     &self->settings.refineIterations_channel,
                                     &self->settings.channels))
        return -1;

    self->settings.skip_mode2 = skip_mode2 == 1;

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
            self->settings.mode_selection[i] = PyObject_IsTrue(item);
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
            PyErr_SetString(PyExc_ValueError, "refineIterations must be a list of 8 integers");
            return -1;
        }
        for (int i = 0; i < 8; i++)
        {
            PyObject *item = PyList_GetItem(refineIterations, i);
            long long value = PyLong_AsLongLong(item);
            if (value == -1 && PyErr_Occurred())
                return -1;
            self->settings.refineIterations[i] = static_cast<int>(value);
        }
    }
    return 0;
}

PyMemberDef BC7EncSettingsObject_members[] = {
    {"skip_mode2", T_BOOL, offsetof(BC7EncSettingsObject, settings.skip_mode2), 0, "skip_mode2"},
    {"fast_skip_threshold_mode1", T_INT, offsetof(BC7EncSettingsObject, settings.fastSkipTreshold_mode1), 0, "fastSkipTreshold_mode1"},
    {"fast_skip_threshold_mode3", T_INT, offsetof(BC7EncSettingsObject, settings.fastSkipTreshold_mode3), 0, "fastSkipTreshold_mode3"},
    {"fast_skip_threshold_mode7", T_INT, offsetof(BC7EncSettingsObject, settings.fastSkipTreshold_mode7), 0, "fastSkipTreshold_mode7"},
    {"mode45_channel0", T_INT, offsetof(BC7EncSettingsObject, settings.mode45_channel0), 0, "mode45_channel0"},
    {"refine_iterations_channel", T_INT, offsetof(BC7EncSettingsObject, settings.refineIterations_channel), 0, "refineIterations_channel"},
    {"channels", T_INT, offsetof(BC7EncSettingsObject, settings.channels), 0, "channels"},
    {NULL} /* Sentinel */
};

// TODO getset for mode_selection and refineIterations
PyObject *BC7EncSettings_repr(BC7EncSettingsObject *self)
{
    return PyUnicode_FromFormat("BC7EncSettings(mode_selection=[%s, %s, %s, %s], refineIterations=[%d, %d, %d, %d, %d, %d, %d, %d], skip_mode2=%s, fastSkipTreshold_mode1=%d, fastSkipTreshold_mode3=%d, fastSkipTreshold_mode7=%d, mode45_channel0=%s, refineIterations_channel=%d, channels=%d)",
                                self->settings.mode_selection[0] ? "True" : "False", self->settings.mode_selection[1] ? "True" : "False", self->settings.mode_selection[2] ? "True" : "False", self->settings.mode_selection[3] ? "True" : "False",
                                self->settings.refineIterations[0], self->settings.refineIterations[1], self->settings.refineIterations[2], self->settings.refineIterations[3], self->settings.refineIterations[4], self->settings.refineIterations[5], self->settings.refineIterations[6], self->settings.refineIterations[7],
                                self->settings.skip_mode2 ? "True" : "False",
                                self->settings.fastSkipTreshold_mode1,
                                self->settings.fastSkipTreshold_mode3,
                                self->settings.fastSkipTreshold_mode7,
                                self->settings.mode45_channel0 ? "True" : "False",
                                self->settings.refineIterations_channel,
                                self->settings.channels);
}

static PyMethodDef BC7EncSettingsMethods[] = {
    {"from_profile", settings_from_profile<BC7EncSettingsObject, bc7_profile_map>, METH_O | METH_CLASS, ""},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

PyType_Slot BC7EncSettingsType_slots[] = {
    {Py_tp_new, reinterpret_cast<void *>(PyType_GenericNew)},
    {Py_tp_init, reinterpret_cast<void *>(BC7EncSettings_init)},
    {Py_tp_members, reinterpret_cast<void *>(BC7EncSettingsObject_members)},
    {Py_tp_repr, reinterpret_cast<void *>(BC7EncSettings_repr)},
    {Py_tp_methods, reinterpret_cast<void *>(BC7EncSettingsMethods)},
    {0, NULL},
};

PyType_Spec BC7EncSettingsType_Spec = {
    "ispc_texcomp.BC7EncSettings",            // const char* name;
    sizeof(BC7EncSettingsObject),             // int basicsize;
    0,                                        // int itemsize;
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, // unsigned int flags;
    BC7EncSettingsType_slots,                 // PyType_Slot *slots;
};

///////////////////////////////////////////////////////////////////////////////////
// BC6HEncSettings

typedef void (*GetProfile_bc6h)(bc6h_enc_settings *settings);
std::unordered_map<std::string, GetProfile_bc6h> bc6h_profile_map = {
    {"veryfast", GetProfile_bc6h_veryfast},
    {"fast", GetProfile_bc6h_fast},
    {"basic", GetProfile_bc6h_basic},
    {"slow", GetProfile_bc6h_slow},
    {"veryslow", GetProfile_bc6h_veryslow},
};

int BC6HEncSettings_init(BC6HEncSettingsObject *self, PyObject *args, PyObject *kwds)
{
    static const char *kwlist[] = {"slow_mode", "fast_mode", "refine_iterations_1p", "refine_iterations_2p", "fast_skip_treshold", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "bbiii", const_cast<char **>(kwlist),
                                     &self->settings.slow_mode,
                                     &self->settings.fast_mode,
                                     &self->settings.refineIterations_1p,
                                     &self->settings.refineIterations_2p,
                                     &self->settings.fastSkipTreshold))
        return -1;
    return 0;
}

PyMemberDef BC6HEncSettingsObject_members[] = {
    {"slow_mode", T_BOOL, offsetof(BC6HEncSettingsObject, settings.slow_mode), 0, "slow_mode"},
    {"fast_mode", T_BOOL, offsetof(BC6HEncSettingsObject, settings.fast_mode), 0, "fast_mode"},
    {"refine_iterations_1p", T_INT, offsetof(BC6HEncSettingsObject, settings.refineIterations_1p), 0, "refineIterations_1p"},
    {"refine_iterations_2p", T_INT, offsetof(BC6HEncSettingsObject, settings.refineIterations_2p), 0, "refineIterations_2p"},
    {"fast_skip_treshold", T_INT, offsetof(BC6HEncSettingsObject, settings.fastSkipTreshold), 0, "fastSkipTreshold"},
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

static PyMethodDef BC6HEncSettingsMethods[] = {
    {"from_profile", settings_from_profile<BC6HEncSettingsObject, bc6h_profile_map>, METH_O | METH_CLASS, ""},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

PyType_Slot BC6HEncSettingsType_slots[] = {
    {Py_tp_new, reinterpret_cast<void *>(PyType_GenericNew)},
    {Py_tp_init, reinterpret_cast<void *>(BC6HEncSettings_init)},
    {Py_tp_members, reinterpret_cast<void *>(BC6HEncSettingsObject_members)},
    {Py_tp_repr, reinterpret_cast<void *>(BC6HEncSettings_repr)},
    {Py_tp_methods, reinterpret_cast<void *>(BC6HEncSettingsMethods)},
    {0, NULL},
};

PyType_Spec BC6HEncSettingsType_Spec = {
    "ispc_texcomp.BC6HEncSettings",           // const char* name;
    sizeof(BC6HEncSettingsObject),            // int basicsize;
    0,                                        // int itemsize;
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, // unsigned int flags;
    BC6HEncSettingsType_slots,                // PyType_Slot *slots;
};

///////////////////////////////////////////////////////////////////////////////////
// ETCEncSettings

typedef void (*GetProfile_etc)(etc_enc_settings *settings);
std::unordered_map<std::string, GetProfile_etc> etc_profile_map = {
    {"slow", GetProfile_etc_slow},
};

int ETCEncSettings_init(ETCEncSettingsObject *self, PyObject *args, PyObject *kwds)
{
    static const char *kwlist[] = {"fast_skip_treshold", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "i", const_cast<char **>(kwlist),
                                     &self->settings.fastSkipTreshold))
        return -1;
    return 0;
}

PyMemberDef ETCEncSettingsObject_members[] = {
    {"fast_skip_treshold", T_INT, offsetof(ETCEncSettingsObject, settings.fastSkipTreshold), 0, "fastSkipTreshold"},
    {NULL} /* Sentinel */
};

PyObject *ETCEncSettings_repr(ETCEncSettingsObject *self)
{
    return PyUnicode_FromFormat("ETCEncSettings(fastSkipTreshold=%d)",
                                self->settings.fastSkipTreshold);
}

static PyMethodDef ETCEncSettingsMethods[] = {
    {"from_profile", settings_from_profile<ETCEncSettingsObject, etc_profile_map>, METH_O | METH_CLASS, ""},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

PyType_Slot ETCEncSettingsType_slots[] = {
    {Py_tp_new, reinterpret_cast<void *>(PyType_GenericNew)},
    {Py_tp_init, reinterpret_cast<void *>(ETCEncSettings_init)},
    {Py_tp_members, reinterpret_cast<void *>(ETCEncSettingsObject_members)},
    {Py_tp_repr, reinterpret_cast<void *>(ETCEncSettings_repr)},
    {Py_tp_methods, reinterpret_cast<void *>(ETCEncSettingsMethods)},
    {0, NULL},
};

PyType_Spec ETCEncSettingsType_Spec = {
    "ispc_texcomp.ETCEncSettings",            // const char* name;
    sizeof(ETCEncSettingsObject),             // int basicsize;
    0,                                        // int itemsize;
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, // unsigned int flags;
    ETCEncSettingsType_slots,                 // PyType_Slot *slots;
};

///////////////////////////////////////////////////////////////////////////////////
// ASTCEncSettings

typedef void (*GetProfile_astc)(astc_enc_settings *settings, int block_width, int block_height);
std::unordered_map<std::string, GetProfile_astc> astc_profile_map = {
    {"fast", GetProfile_astc_fast},
    {"alpha_fast", GetProfile_astc_alpha_fast},
    {"alpha_slow", GetProfile_astc_alpha_slow},
};

int ASTCEncSettings_init(ASTCEncSettingsObject *self, PyObject *args, PyObject *kwds)
{
    static const char *kwlist[] = {"block_width", "block_height", "channels", "fast_skip_treshold", "refine_iterations", NULL};
    self->settings.block_width = 4;
    self->settings.block_height = 4;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "iiiii", const_cast<char **>(kwlist),
                                     &self->settings.block_width,
                                     &self->settings.block_height,
                                     &self->settings.channels,
                                     &self->settings.fastSkipTreshold,
                                     &self->settings.refineIterations))
        return -1;

    // Validate block size
    if (self->settings.block_width < 4 || self->settings.block_height < 4 || self->settings.block_width > 8 || self->settings.block_height > 8)
    {
        PyErr_SetString(PyExc_ValueError, "Invalid block dimensions (4-8 allowed)");
        return -1;
    }
    return 0;
}

PyMemberDef ASTCEncSettingsObject_members[] = {
    {"block_width", T_INT, offsetof(ASTCEncSettingsObject, settings.block_width), 0, "block_width"},
    {"block_height", T_INT, offsetof(ASTCEncSettingsObject, settings.block_height), 0, "block_height"},
    {"channels", T_INT, offsetof(ASTCEncSettingsObject, settings.channels), 0, "channels"},
    {"fast_skip_treshold", T_INT, offsetof(ASTCEncSettingsObject, settings.fastSkipTreshold), 0, "fastSkipTreshold"},
    {"refine_iterations", T_INT, offsetof(ASTCEncSettingsObject, settings.refineIterations), 0, "refineIterations"},
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

static PyObject *ASTC_settings_from_profile(PyObject *cls, PyObject *args)
{
    char *profile = NULL;
    int block_width;
    int block_height;

    if (!PyArg_ParseTuple(args, "sii", &profile, &block_width, &block_height))
    {
        return NULL;
    }

    // Validate block size
    if (block_width < 4 || block_height < 4 || block_width > 8 || block_height > 8)
    {
        PyErr_SetString(PyExc_ValueError, "Invalid block dimensions (4-8 allowed)");
        return nullptr;
    }

    auto it = astc_profile_map.find(profile);
    if (it != astc_profile_map.end())
    {
        PyObject *settings_py = PyType_GenericNew((PyTypeObject *)cls, NULL, NULL);
        if (!settings_py)
        {
            return NULL;
        }
        it->second(&(reinterpret_cast<ASTCEncSettingsObject *>(settings_py)->settings), block_width, block_height);
        return settings_py;
    }
    else
    {
        PyErr_SetString(PyExc_ValueError, "Invalid profile");
        return nullptr;
    }
}

static PyMethodDef ASTCEncSettingsMethods[] = {
    {"from_profile", ASTC_settings_from_profile, METH_VARARGS | METH_CLASS, ""},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

PyType_Slot ASTCEncSettingsType_slots[] = {
    {Py_tp_new, reinterpret_cast<void *>(PyType_GenericNew)},
    {Py_tp_init, reinterpret_cast<void *>(ASTCEncSettings_init)},
    {Py_tp_members, reinterpret_cast<void *>(ASTCEncSettingsObject_members)},
    {Py_tp_repr, reinterpret_cast<void *>(ASTCEncSettings_repr)},
    {Py_tp_methods, reinterpret_cast<void *>(ASTCEncSettingsMethods)},
    {0, NULL},
};

PyType_Spec ASTCEncSettingsType_Spec = {
    "ispc_texcomp.ASTCEncSettings",           // const char* name;
    sizeof(ASTCEncSettingsObject),            // int basicsize;
    0,                                        // int itemsize;
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, // unsigned int flags;
    ASTCEncSettingsType_slots,                // PyType_Slot *slots;
};