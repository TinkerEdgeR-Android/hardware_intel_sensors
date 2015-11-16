/*
* Copyright (C) 2015 Intel Corp
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/select.h>
#include <cutils/log.h>
#include <linux/input.h>

#include "SensorBase.h"

/*****************************************************************************/

SensorBase::SensorBase(const char* dev_name,
                       const char* data_name) : dev_name(dev_name),
                                                data_name(data_name),
                                                dev_fd(-1),
                                                data_fd(-1)
{
    FUNC_LOG;
    int ret;
    unsigned int clockId;
    ALOGV("%s(): dev_name=%s, data_name=%s ", __func__, dev_name, data_name);

    if (data_name) {
        data_fd = openInput(data_name);
        if (data_fd >= 0) {
            clockId = CLOCK_BOOTTIME;
            ret = ioctl(data_fd, EVIOCSCLOCKID, &clockId);
            if (ret != 0)
               ALOGE("set timestamp to CLOCK_BOOTTIME failed!");
        }
    }
}

SensorBase::~SensorBase() {
    FUNC_LOG;
    if (data_fd >= 0) {
        close(data_fd);
    }
    if (dev_fd >= 0) {
        close(dev_fd);
    }
}

int SensorBase::open_device() {
    FUNC_LOG;
    if (dev_fd<0 && dev_name) {
        dev_fd = open(dev_name, O_RDONLY);
        LOGE_IF(dev_fd<0, "Couldn't open %s (%s)", dev_name, strerror(errno));
    }
    return 0;
}

int SensorBase::close_device() {
    FUNC_LOG;
    if (dev_fd >= 0) {
        close(dev_fd);
        dev_fd = -1;
    }
    return 0;
}

int SensorBase::getFd() const {
    FUNC_LOG;
    if (!data_name) {
        return dev_fd;
    }
    return data_fd;
}

int SensorBase::setDelay(int32_t /* handle */, int64_t /* ns */) {
    FUNC_LOG;
    return 0;
}

bool SensorBase::hasPendingEvents() const {
    FUNC_LOG;
    return false;
}

int64_t SensorBase::getTimestamp() {
    FUNC_LOG;
    struct timespec t;
    t.tv_sec = t.tv_nsec = 0;
    clock_gettime(CLOCK_BOOTTIME, &t);
    return int64_t(t.tv_sec) * 1000000000LL + t.tv_nsec;
}

int SensorBase::openInput(const char* inputName) {
    FUNC_LOG;
    int fd = -1;
    const char *dirname = "/dev/input";
    char devname[PATH_MAX];
    char *filename;
    DIR *dir;
    struct dirent *de;
    dir = opendir(dirname);
    if(dir == NULL)
        return -1;
    strcpy(devname, dirname);
    filename = devname + strlen(devname);
    *filename++ = '/';
    while((de = readdir(dir))) {
        if(de->d_name[0] == '.' &&
                (de->d_name[1] == '\0' ||
                        (de->d_name[1] == '.' && de->d_name[2] == '\0')))
            continue;
        strcpy(filename, de->d_name);
        fd = open(devname, O_RDONLY);
        LOGV_IF(EXTRA_VERBOSE, "path open %s", devname);
        LOGI("path open %s", devname);
        if (fd >= 0) {
            char name[80];
            if (ioctl(fd, EVIOCGNAME(sizeof(name) - 1), &name) < 1) {
                name[0] = '\0';
            }
            LOGV_IF(EXTRA_VERBOSE, "name read %s", name);
            if (!strncmp(name, inputName, strlen(inputName))) {
                break;
            } else {
                close(fd);
                fd = -1;
            }
        }
    }
    closedir(dir);
    LOGE_IF(fd < 0, "couldn't find '%s' input device", inputName);
    return fd;
}

int SensorBase::enable(int32_t /* handle */, int /* enabled */)
{
    FUNC_LOG;
    return 0;
}

int SensorBase::isActivated(int /* handle */)
{
    FUNC_LOG;
    return 0;
}

#ifdef HAL_VERSION_GT_1_0
int SensorBase::batch(int /* handle */, int /* flags */, int64_t /* period_ns */, int64_t /* timeout */)
{
    FUNC_LOG;
    return 0;
}
#endif
