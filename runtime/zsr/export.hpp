#pragma once

#if defined(_MSC_VER)
    #if defined(ZSR_BUILD)
        #define ZSR_EXPORT __declspec(dllexport)
    #else
        #define ZSR_EXPORT __declspec(dllimport)
    #endif
#else
    #define ZSR_EXPORT __attribute__ ((visibility ("default")))
#endif
