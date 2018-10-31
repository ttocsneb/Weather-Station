#include "basestationmodule.h"

#include "main.h"
#include "radio.h"
#include "commands.h"


#define Py_bool(x) (x ? Py_True : Py_False)
#define Py_bool_new(x) Py_bool(x); Py_INCREF(Py_bool(x)); 


////////////// Commands Wrapper //////////////


PyObject* callback_get_eeprom;
/**
 * A python function pointer wrapper for get_status
 */
void eeprom_got_wrapper() {

    PyObject* status = PyDict_New();

    PyDict_SetItem(status, PyLong_FromLong(EEPROM_REFRESH_TIME), PyLong_FromLong(commands::eeprom::refresh_time));
    PyDict_SetItem(status, PyLong_FromLong(EEPROM_LISTEN_TIME), PyLong_FromLong(commands::eeprom::listen_time));
    PyDict_SetItem(status, PyLong_FromLong(EEPROM_ALTITUDE), PyLong_FromLong(commands::eeprom::pressure::altitude));
    PyDict_SetItem(status, PyLong_FromLong(EEPROM_AVG_WIND_UPDATE), PyLong_FromLong(commands::eeprom::wind::avg_wind_update));
    PyDict_SetItem(status, PyLong_FromLong(EEPROM_AVG_WIND_STORAGE), PyLong_FromLong(commands::eeprom::wind::avg_wind_storage));
    PyDict_SetItem(status, PyLong_FromLong(EEPROM_WIND_UPDATE), PyLong_FromLong(commands::eeprom::wind::wind_update));
    PyDict_SetItem(status, PyLong_FromLong(EEPROM_WIND_STORAGE), PyLong_FromLong(commands::eeprom::wind::wind_storage));

    PyObject_CallObject(callback_get_eeprom, PyTuple_Pack(1, status));
}

static PyObject* cmd_get_eeprom_wrapper(PyObject* self, PyObject* args) {

    PyObject* temp;

    if(PyArg_ParseTuple(args, "O", temp)) {
        // Raise an exception if the object is not callable
        if(!PyCallable_Check(temp)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }

        Py_XINCREF(temp);
        Py_XDECREF(callback_get_eeprom);
        callback_get_eeprom = temp;
        commands::getEEPROM(&eeprom_got_wrapper);

        Py_INCREF(Py_None);
        return Py_None;
    }

    commands::getEEPROM();
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* cmd_set_eeprom_wrapper(PyObject* self, PyObject* args) {

    uint8_t variable;
    uint32_t value;

    if(!PyArg_ParseTuple(args, "BI", &variable, &value))
        return NULL;

    const EEPROM_Variable VARIABLES[] = {
        EEPROM_REFRESH_TIME, EEPROM_LISTEN_TIME, EEPROM_ALTITUDE,
        EEPROM_AVG_WIND_UPDATE, EEPROM_AVG_WIND_STORAGE,
        EEPROM_WIND_UPDATE, EEPROM_WIND_STORAGE
    };

    EEPROM_Variable selected_var;

    // Check if the variable is valid
    bool isEEPROM_Variable = false;
    for(const EEPROM_Variable var : VARIABLES) {
        if(variable == var) {
            selected_var = var;
            isEEPROM_Variable = true;
            break;
        }
    }
    if(!isEEPROM_Variable) {
        PyErr_SetString(PyExc_TypeError, "This is not a valid variable");
        return NULL;
    }

    commands::setEEPROM(selected_var, value);

    Py_INCREF(Py_None);
    return Py_None;
}


PyObject* callback_get_status;
/**
 * A python function pointer wrapper for get_status
 */
void status_got_wrapper() {

    PyObject* status = PyDict_New();


    PyObject* just_started = Py_bool_new(commands::status::just_started);
    PyObject* charging = Py_bool_new(commands::status::charging);

    PyDict_SetItemString(status, "just_started", just_started);
    PyDict_SetItemString(status, "charging", charging);
    PyDict_SetItemString(status, "battery", PyFloat_FromDouble(commands::status::battery));
    PyDict_SetItemString(status, "battery_temp", PyFloat_FromDouble(commands::status::battery_temp));
    PyDict_SetItemString(status, "lost_packets", PyLong_FromLong(commands::status::lost_packets));

    PyObject_CallObject(callback_get_status, PyTuple_Pack(1, status));
}

static PyObject* cmd_get_status_wrapper(PyObject* self, PyObject* args) {

    PyObject* temp;

    if(PyArg_ParseTuple(args, "O", temp)) {

        // Raise an exception if the object is not callable
        if(!PyCallable_Check(temp)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }

        Py_XINCREF(temp);
        Py_XDECREF(callback_get_status);
        callback_get_status = temp;
        commands::getStatus(&status_got_wrapper);

        Py_INCREF(Py_None);
        return Py_None;
    }

    commands::getStatus();
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* cmd_reset_weather_wrapper(PyObject* self, PyObject* args) {
    commands::resetWeather();
    
    // Return None
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* cmd_reset_status_wrapper(PyObject* self, PyObject* args) {
    commands::resetStatus();
    
    // Return None
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* cmd_reset_eeprom_wrapper(PyObject* self, PyObject* args) {
    commands::resetEEPROM();

    // Return None
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* cmd_restart_wrapper(PyObject* self, PyObject* args) {
    commands::restart();

    // Return None
    Py_INCREF(Py_None);
    return Py_None;
}


////////////// Radio Wrapper //////////////


static PyObject* rdo_begin_wrapper(PyObject* self, PyObject* args) {
    global::begin();
    radio::begin();

    // Return None
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* rdo_update_wrapper(PyObject* self, PyObject* args) {
    int delay = radio::update();

    bool success = delay != -1;
    delay = success ? delay : 0;

    PyObject* succeeded = Py_bool_new(success);

    return PyTuple_Pack(2, success, PyLong_FromLong(delay));
}

////////////// Weather Wrapper //////////////


static PyObject* wtr_get_weather_wrapper(PyObject* self, PyObject* args) {
    PyObject* dict = PyDict_New();

    if(!dict) {
        return NULL;
    }


    if(PyDict_SetItemString(dict, "windSpeed", PyLong_FromLong(commands::weather::rawWindSpeed)) == -1)
        return NULL;
    if(PyDict_SetItemString(dict, "windDir", PyLong_FromLong(commands::weather::rawWindDirection)) == -1)
        return NULL;
    if(PyDict_SetItemString(dict, "maxWindSpeed", PyLong_FromLong(commands::weather::rawMaxWindSpeed)) == -1)
        return NULL;
    if(PyDict_SetItemString(dict, "maxWindDir", PyLong_FromLong(commands::weather::rawMaxWindDirection)) == -1)
        return NULL;
    if(PyDict_SetItemString(dict, "avgWindSpeed", PyLong_FromLong(commands::weather::rawAverageWindSpeed)) == -1)
        return NULL;
    if(PyDict_SetItemString(dict, "avgWindDir", PyLong_FromLong(commands::weather::rawAverageWindDirection)) == -1);

    if(PyDict_SetItemString(dict, "humidity", PyLong_FromLong(commands::weather::rawHumidity)) == -1)
        return NULL;
    if(PyDict_SetItemString(dict, "temperature", PyLong_FromLong(commands::weather::rawTemperature)) == -1)
        return NULL;
    if(PyDict_SetItemString(dict, "rain", PyLong_FromLong(commands::weather::rawRainFall)) == -1)
        return NULL;
    if(PyDict_SetItemString(dict, "pressure", PyLong_FromLong(commands::weather::rawPressure)) == -1)
        return NULL;

    return dict;
}


static PyMethodDef BasestationMethods[] = {
    {
        "get_EEPROM", cmd_get_eeprom_wrapper, METH_VARARGS | METH_STATIC, "Get the Saved EEPROM from the weather station"
    },
    {
        "set_EEPROM", cmd_set_eeprom_wrapper, METH_VARARGS | METH_STATIC, "Set an EEPROM Variable on the weather Station"
    },
    {
        "reset_EEPROM", cmd_reset_eeprom_wrapper, METH_NOARGS | METH_STATIC, "Reset the EEPROM to its defaults"
    },
    {
        "get_status", cmd_get_status_wrapper, METH_VARARGS | METH_STATIC, "Get the current status of the weather Station"
    },
    {
        "reset_weather", cmd_reset_weather_wrapper, METH_NOARGS | METH_STATIC, "Reset the weather Counters"
    },
    {
        "reset_status", cmd_reset_status_wrapper, METH_NOARGS | METH_STATIC, "Reset the status Counters"
    },
    {
        "restart", cmd_restart_wrapper, METH_NOARGS | METH_STATIC, "Restart the weather station"
    },

    {
        "begin_radio", rdo_begin_wrapper, METH_NOARGS | METH_STATIC, "Start the radio"
    },
    {
        "update_radio", rdo_update_wrapper, METH_NOARGS | METH_STATIC, "Update the radio, should be called synchronously with the weather Station"
    },

    {
        "get_raw_weather", wtr_get_weather_wrapper, METH_NOARGS | METH_STATIC, "Get the raw weather"
    }
};

static struct PyModuleDef basestationModule = {
    PyModuleDef_HEAD_INIT,
    "basestation",
    NULL,
    -1,
    BasestationMethods
};

PyMODINIT_FUNC initBaseStation(void) {
    return PyModule_Create(&basestationModule);
}