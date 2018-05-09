#ifndef EASYLOGPP_H_
#define EASYLOGPP_H_

#define ELPP_NO_DEFAULT_LOG_FILE
#define ELPP_THREAD_SAFE

#include "easylogging++.h"

#undef LOG
#define LOG(LEVEL) CLOG(LEVEL, "default", "fileLogger")

#endif //EASYLOGPP_H_
