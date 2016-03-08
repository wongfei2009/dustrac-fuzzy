//------------------------------------------------------------------------------
//-----auxiliary macros used to export symbols to shared library interface------
//------------------------------------------------------------------------------

#if defined _WIN32
  #define DUST_HELPER_DLL_IMPORT __declspec(dllimport)
  #define DUST_HELPER_DLL_EXPORT __declspec(dllexport)
  #define DUST_HELPER_DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define DUST_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
    #define DUST_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
    #define DUST_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define DUST_HELPER_DLL_IMPORT
    #define DUST_HELPER_DLL_EXPORT
    #define DUST_HELPER_DLL_LOCAL
  #endif
#endif

//------------------------------------------------------------------------------
//--------macros used to export symbols to shared library interface-------------
//------------------------------------------------------------------------------

#ifdef DUST_DLL // defined if the project is compiled as a DLL
  #ifdef DUST_DLL_EXPORTS // defined if we are building the dll (as opposed to using it)
    #define DUST_API DUST_HELPER_DLL_EXPORT
  #else
    #define DUST_API DUST_HELPER_DLL_IMPORT
  #endif // DUST_DLL_EXPORTS
  #define DUST_LOCAL DUST_HELPER_DLL_LOCAL
#else // DUST_DLL is not defined: this means SYS is a static lib.
  #define DUST_API
  #define DUST_LOCAL
#endif // DUST_DLL
