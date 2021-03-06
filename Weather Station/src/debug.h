#ifndef __DEBUG__
#define __DEBUG__

#ifdef DEBUG
    #undef DEBUG_BEGIN
    #undef DEBUG_PRINT
    #undef DEBUG_PRINTLN

    #define DEBUG_BEGIN(...) Serial.begin(__VA_ARGS__)
    #define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
    #define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else

    #undef DEBUG_BEGIN
    #undef DEBUG_PRINT
    #undef DEBUG_PRINTLN

    #define DEBUG_BEGIN(...)
    #define DEBUG_PRINT(...)
    #define DEBUG_PRINTLN(...)
#endif

#endif