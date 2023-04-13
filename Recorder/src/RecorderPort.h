#pragma once

// The way how the function is called
#if !defined(RECORD_CALL)
#if defined(_WIN32)
#define RECORD_CALL __stdcall
#else
#define RECORD_CALL
#endif // _WIN32 
#endif // RECORD_CALL

// The function exported symbols
#if defined _WIN32 || defined __CYGWIN__
#define RECORD_DLL_IMPORT __declspec(dllimport)
#define RECORD_DLL_EXPORT __declspec(dllexport)
#define RECORD_LIB
#else
#if __GNUC__ >= 4
#define RECORD_DLL_IMPORT __attribute__((visibility("default")))
#define RECORD_DLL_EXPORT __attribute__((visibility("default")))
#define RECORD_LIB __attribute__((visibility("hidden")))
#else
#define RECORD_DLL_IMPORT
#define RECORD_DLL_EXPORT
#define RECORD_LIB
#endif
#endif

#if defined(RECORD_LIBS)
#define RECORDAPI_PORT RECORD_LIB
#elif RECORD_DLL_EXPORTS
#define RECORDAPI_PORT RECORD_DLL_EXPORT
#else
#define RECORDAPI_PORT RECORD_DLL_IMPORT
#endif // RECORD_DLL_EXPORTS

#define RECORDAPI RECORDAPI_PORT RECORD_CALL
