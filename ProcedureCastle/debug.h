#pragma once
#include "debug_info.h"
#define ELPP_NO_DEFAULT_LOG_FILE
#include "elpp.h"
#include <exception>
#include <Windows.h>

class verbose_exception : public std::exception
{
public:
	verbose_exception(const std::exception& _e)
	{
		MessageBox(NULL, (LPCSTR)_e.what(), (LPCSTR)"Exception!", MB_ICONWARNING | MB_OK);
		if (1) throw _e;
	}
};

#define DEBUG_SHOULD_ERROR_CHECK (DEBUG_LEVEL >= 1)
#define DEBUG_SHOULD_LOG (DEBUG_LEVEL >= 2)

#define DEBUG_LOG(_X_, _Y_) LOG_IF(DEBUG_SHOULD_LOG, _X_) << _Y_

#define DEBUG_ERROR(_X_) {																						\
							if(DEBUG_SHOULD_LOG){																\
								if(DEBUG_SUPRESS_ERRORS == 0){													\
									LOG(ERROR) << _X_;															\
									throw verbose_exception(std::exception(_X_));								\
								}																				\
								else {																			\
									LOG(ERROR) << _X_;															\
								}																				\
							}																					\
							else {																				\
								if (DEBUG_SUPRESS_ERRORS == 0) throw verbose_exception(std::exception(_X_));	\
							}																					\
						}



