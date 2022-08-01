//
// Created by elmer on 7/31/2022.
//

#ifndef HHAVDEMO_HHVELOGUTILS_H
#define HHAVDEMO_HHVELOGUTILS_H

#include "android/log.h"

#define DEFAULT_TAG "HHVEDemo"

#define HHVELOGD(fmt, ...) __android_log_print(ANDROID_LOG_DEBUG, DEFAULT_TAG, fmt, __VA_ARGS__)
#define HHVELOGI(fmt, ...) __android_log_print(ANDROID_LOG_INFO, DEFAULT_TAG, fmt, __VA_ARGS__)
#define HHVELOGW(fmt, ...) __android_log_print(ANDROID_LOG_WARN, DEFAULT_TAG, fmt, __VA_ARGS__)
#define HHVELOGE(fmt, ...) __android_log_print(ANDROID_LOG_ERROR, DEFAULT_TAG, fmt, __VA_ARGS__)

//#define HHVELOGD(TAG, fmt, ...) __android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, __VA_ARGS__)
//#define HHVELOGI(TAG, fmt, ...) __android_log_print(ANDROID_LOG_INFO, TAG, fmt, __VA_ARGS__)
//#define HHVELOGW(TAG, fmt, ...) __android_log_print(ANDROID_LOG_WARN, TAG, fmt, __VA_ARGS__)
//#define HHVELOGE(TAG, fmt, ...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, __VA_ARGS__)


static void testLog() {
}

#endif //HHAVDEMO_HHVELOGUTILS_H
