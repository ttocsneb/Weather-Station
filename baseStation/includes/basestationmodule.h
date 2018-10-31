#include  <Python.h>


////////////// Commands Wrapper //////////////

/**
 * Send the GetEEPROM command
 */
static PyObject* cmd_get_eeprom_wrapper(PyObject* self, PyObject* args);

/**
 * Send the SetEEPROM command
 */
static PyObject* cmd_set_eeprom_wrapper(PyObject* self, PyObject* args);

/**
 * Send the GetStatus command
 */
static PyObject* cmd_get_status_wrapper(PyObject* self, PyObject* args);

/**
 * Send the resetWeather command
 */
static PyObject* cmd_reset_weather_wrapper(PyObject* self, PyObject* args);

/**
 * Send the resetStatus command
 */
static PyObject* cmd_reset_status_wrapper(PyObject* self, PyObject* args);

/**
 * Send the resetEEPROM command
 */
static PyObject* cmd_reset_eeprom_wrapper(PyObject* self, PyObject* args);

/**
 * Send the restart command
 */
static PyObject* cmd_restart_wrapper(PyObject* self, PyObject* args);


////////////// Radio Wrapper //////////////

/**
 * Setup the Radio
 */
static PyObject* rdo_begin_wrapper(PyObject* self, PyObject* args);

/**
 * Update the Radio
 */
static PyObject* rdo_update_wrapper(PyObject* self, PyObject* args);

////////////// Weather Wrapper //////////////

/**
 * Update the weather after the raw weather has been updated
 */
static PyObject* wtr_update_wrapper(PyObject* self, PyObject* args);

/**
 * Get the current weather
 */
static PyObject* wtr_get_weather_wrapper(PyObject* self, PyObject* args);

/**
 * Initialize the BaseStation
 */
PyMODINIT_FUNC initBaseStation(void);