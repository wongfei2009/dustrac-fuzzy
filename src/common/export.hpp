#if defined _WIN32
  #define DUSTCOMMON_HELPER_DLL_IMPORT __declspec(dllimport)
  #define DUSTCOMMON_HELPER_DLL_EXPORT __declspec(dllexport)
  #define DUSTCOMMON_HELPER_DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define DUSTCOMMON_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
    #define DUSTCOMMON_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
    #define DUSTCOMMON_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define DUSTCOMMON_HELPER_DLL_IMPORT
    #define DUSTCOMMON_HELPER_DLL_EXPORT
    #define DUSTCOMMON_HELPER_DLL_LOCAL
  #endif
#endif

//------------------------------------------------------------------------------
//--------macros used to export symbols to shared library interface-------------
//------------------------------------------------------------------------------

#ifdef DUSTCOMMON_DLL // defined if the project is compiled as a DLL
  #ifdef DUSTCOMMON_DLL_EXPORTS // defined if we are building the dll (as opposed to using it)
    #define DUSTCOMMON_API DUSTCOMMON_HELPER_DLL_EXPORT
  #else
    #define DUSTCOMMON_API DUSTCOMMON_HELPER_DLL_IMPORT
  #endif // DUSTCOMMON_DLL_EXPORTS
  #define DUSTCOMMON_LOCAL DUSTCOMMON_HELPER_DLL_LOCAL
#else // DUSTCOMMON_DLL is not defined: this means SYS is a static lib.
  #define DUSTCOMMON_API
  #define DUSTCOMMON_LOCAL
#endif // DUSTCOMMON_DLL