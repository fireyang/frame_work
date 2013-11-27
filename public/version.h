
#ifndef VERSION_H_
#define VERSION_H_

#define BASE_VERSION "1.0.0"
#if defined(DEBUG) || defined(_DEBUG) || defined(_DEBUG_)
#define VERSION BASE_VERSION "_debug_" "1eb8253f0d5f4a9f86a00ef78e2fa07ada842f86" "_" __DATE__ " "  __TIME__
#else
#define VERSION BASE_VERSION "_release_" "1eb8253f0d5f4a9f86a00ef78e2fa07ada842f86" "_" __DATE__ " "  __TIME__
#endif

#endif

