
/*
 * Copyright 2017, Technische Universitaet Dresden, Germany, all rights reserved.
 * Author: Andreas Gocht
 *
 * Permission to use, copy, modify, and distribute this Python software and
 * its associated documentation for any purpose without fee is hereby
 * granted, provided that the above copyright notice appears in all copies,
 * and that both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of TU Dresden is not used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 */

#include <Python.h>
#include <iostream>
#include <map>
#include <scorep/SCOREP_User_Functions.h>
#include <scorep/SCOREP_User_Variables.h>
#include <string>

namespace scorep
{
// SCOREP_User_RegionHandle handle = SCOREP_USER_INVALID_REGION

struct region_handle
{
    region_handle() = default;
    ~region_handle() = default;
    SCOREP_User_RegionHandle value = SCOREP_USER_INVALID_REGION;
};

static std::map<std::string, region_handle> regions;

void region_begin(std::string region_name, std::string file_name, std::uint64_t line_number)
{
    auto& handle = regions[region_name];
    SCOREP_User_RegionBegin(&handle.value, NULL, &SCOREP_User_LastFileHandle, region_name.c_str(),
                            SCOREP_USER_REGION_TYPE_FUNCTION, file_name.c_str(), line_number);
}

void region_end(std::string region_name)
{
    auto& handle = regions[region_name];
    SCOREP_User_RegionEnd(handle.value);
}

void parameter_int(std::string name, int64_t value)
{
    static SCOREP_User_ParameterHandle scorep_param = SCOREP_USER_INVALID_PARAMETER;
    SCOREP_User_ParameterInt64(&scorep_param, name.c_str(), value);
}

void parameter_uint(std::string name, uint64_t value)
{
    static SCOREP_User_ParameterHandle scorep_param = SCOREP_USER_INVALID_PARAMETER;
    SCOREP_User_ParameterUint64(&scorep_param, name.c_str(), value);
}

void parameter_string(std::string name, std::string value)
{
    static SCOREP_User_ParameterHandle scorep_param = SCOREP_USER_INVALID_PARAMETER;
    SCOREP_User_ParameterString(&scorep_param, name.c_str(), value.c_str());
}

void oa_region_begin(std::string region_name, std::string file_name, std::uint64_t line_number)
{
    auto& handle = regions[region_name];
    SCOREP_User_OaPhaseBegin(&handle.value, &SCOREP_User_LastFileName, &SCOREP_User_LastFileHandle,
                             region_name.c_str(), SCOREP_USER_REGION_TYPE_FUNCTION,
                             file_name.c_str(), line_number);
}

void oa_region_end(std::string region_name)
{
    auto& handle = regions[region_name];
    SCOREP_User_OaPhaseEnd(handle.value);
}
}

extern "C" {

extern const char* SCOREP_GetExperimentDirName(void);

static PyObject* enable_recording(PyObject* self, PyObject* args)
{
    SCOREP_User_EnableRecording();
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* disable_recording(PyObject* self, PyObject* args)
{

    SCOREP_User_DisableRecording();
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* region_begin(PyObject* self, PyObject* args)
{
    const char* region;
    const char* file_name;
    std::uint64_t line_number = 0;

    if (!PyArg_ParseTuple(args, "ssK", &region, &file_name, &line_number))
        return NULL;

    scorep::region_begin(region, file_name, line_number);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* region_end(PyObject* self, PyObject* args)
{
    const char* region;

    if (!PyArg_ParseTuple(args, "s", &region))
        return NULL;

    scorep::region_end(region);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* oa_region_begin(PyObject* self, PyObject* args)
{
    const char* region;
    const char* file_name;
    std::uint64_t line_number = 0;

    if (!PyArg_ParseTuple(args, "ssK", &region, &file_name, &line_number))
        return NULL;

    scorep::oa_region_begin(region, file_name, line_number);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* oa_region_end(PyObject* self, PyObject* args)
{
    const char* region;

    if (!PyArg_ParseTuple(args, "s", &region))
        return NULL;

    scorep::oa_region_end(region);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* parameter_string(PyObject* self, PyObject* args)
{
    const char* name;
    const char* value;

    if (!PyArg_ParseTuple(args, "ss", &name, &value))
        return NULL;

    scorep::parameter_string(name, value);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* parameter_int(PyObject* self, PyObject* args)
{
    const char* name;
    long long value;

    if (!PyArg_ParseTuple(args, "sL", &name, &value))
        return NULL;

    scorep::parameter_int(name, value);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* parameter_uint(PyObject* self, PyObject* args)
{
    const char* name;
    unsigned long long value;

    if (!PyArg_ParseTuple(args, "sK", &name, &value))
        return NULL;

    scorep::parameter_uint(name, value);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* get_expiriment_dir_name(PyObject* self, PyObject* args)
{

    return PyUnicode_FromString(SCOREP_GetExperimentDirName());
}

static PyMethodDef ScorePMethods[] = {
    { "region_begin", region_begin, METH_VARARGS, "enter a region." },
    { "region_end", region_end, METH_VARARGS, "exit a region." },
    { "oa_region_begin", oa_region_begin, METH_VARARGS, "enter an online access region." },
    { "oa_region_end", oa_region_end, METH_VARARGS, "exit an online access region." },
    { "enable_recording", enable_recording, METH_VARARGS, "disable scorep recording." },
    { "disable_recording", disable_recording, METH_VARARGS, "disable scorep recording." },
    { "parameter_int", parameter_int, METH_VARARGS, "User parameter int." },
    { "parameter_uint", parameter_uint, METH_VARARGS, "User parameter uint." },
    { "parameter_string", parameter_string, METH_VARARGS, "User parameter string." },
    { "get_expiriment_dir_name", get_expiriment_dir_name, METH_VARARGS,
      "Get the Score-P experiment dir." },
    { NULL, NULL, 0, NULL } /* Sentinel */
};

#if PY_VERSION_HEX < 0x03000000
#ifndef USE_MPI
PyMODINIT_FUNC init_scorep(void)
{
    (void)Py_InitModule("_scorep", ScorePMethods);
}
#else  /*USE_MPI*/
PyMODINIT_FUNC init_scorep_mpi(void)
{
    (void)Py_InitModule("_scorep_mpi", ScorePMethods);
}
#endif /*USE_MPI*/
#else  /*python 3*/
#ifndef USE_MPI
static struct PyModuleDef scorepmodule = { PyModuleDef_HEAD_INIT, "_scorep", /* name of module */
                                           NULL, /* module documentation, may be NULL */
                                           -1,   /* size of per-interpreter state of the module,
                                                    or -1 if the module keeps state in global
                                                    variables. */
                                           ScorePMethods };
PyMODINIT_FUNC PyInit__scorep(void)
{
    return PyModule_Create(&scorepmodule);
}
#else  /*USE_MPI*/
static struct PyModuleDef scorepmodule_mpi = { PyModuleDef_HEAD_INIT,
                                               "_scorep_mpi", /* name of module */
                                               NULL, /* module documentation, may be NULL */
                                               -1,   /* size of per-interpreter state of the module,
                                                        or -1 if the module keeps state in global
                                                        variables. */
                                               ScorePMethods };
PyMODINIT_FUNC PyInit__scorep_mpi(void)
{
    return PyModule_Create(&scorepmodule_mpi);
}
#endif /*USE_MPI*/
#endif /*python 3*/
}