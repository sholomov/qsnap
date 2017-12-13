#ifndef QSNAP_GLOBAL_H
#define QSNAP_GLOBAL_H

#ifdef _MSC_VER

  #ifdef QSNAP_LIB
  # define QSNAP_API __declspec(dllexport)
  #else
  # define QSNAP_API __declspec(dllimport)
  #endif

#else
  #define QSNAP_API
  //#pragma message("QSNAP_API macro must be defined for non Windows platform") 
#endif

#endif // QSNAP_GLOBAL_H
