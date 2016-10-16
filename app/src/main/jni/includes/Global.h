#ifndef UTUBEOPENCV_GLOBAL_H
#define UTUBEOPENCV_GLOBAL_H


#include <android/log.h>

#define  LOG_TAG    "testjni"
#define  VERBOSE true
#define  ALOG(...)  if(VERBOSE)__android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

#endif //UTUBEOPENCV_GLOBAL_H
