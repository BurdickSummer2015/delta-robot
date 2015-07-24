#ifdef _MSC_VER
#ifndef SPACENAV_PYTHON_EXPORTS
#define SPACENAV_PYTHON_API __declspec(dllexport)
#else
#define SPACENAV_PYTHON_API __declspec(dllimport)
#endif
#else
#define SPACENAV_PYTHON_API
#endif

#ifndef __SPACENAV_PYTHON_H
#define __SPACENAV_PYTHON_H

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
	// used by C++ source code
#endif

SPACENAV_PYTHON_API void getMousePosition(float *mylist);
SPACENAV_PYTHON_API void readEvent(double inputTime);
SPACENAV_PYTHON_API int init();
SPACENAV_PYTHON_API void close();

#ifdef __cplusplus
}
#endif

#endif