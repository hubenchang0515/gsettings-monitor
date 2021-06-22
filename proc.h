#ifndef PROC_H
#define PROC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <linux/limits.h>

/********************************************************************
 * @brief 根据进程号获取进程可执行文件的路径
 * @param[in] pid 进程号
 * @param[out] path 返回可执行文件的路径，传入的内存大小必须为 PATH_MAX+1
 * @return 是否成功
 * ******************************************************************/
bool proc_get_exec_path(uint32_t pid, char* path);

/********************************************************************
 * @brief 根据进程在DBus上的名称获取进程号
 * @param[in] name 进程在DBus上的名称
 * @return 进程号
 * ******************************************************************/
uint32_t proc_get_pid_by_dbus(const char* name);

/********************************************************************
 * @brief 根据进程在DBus上的名称获取进程的可执行文件路径
 * @param[in] name 进程在DBus上的名称
 * @param[out] path 返回可执行文件的路径，传入的内存大小必须为 PATH_MAX+1
 * @return 是否成功
 * ******************************************************************/
bool proc_get_exec_path_by_dbus(const char* name, char* path);

#endif // PROC_H