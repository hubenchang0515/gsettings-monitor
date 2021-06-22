#ifndef MONITOR_H
#define MONITOR_H

#include <stdint.h>
#include <stddef.h>
#include <dbus/dbus.h>

/***********************************************************************************
 * @brief 申请成为监视器
 * @param[in] filter 过滤规则
 * @param[in] len filter的长度
 * @param[out] err DBus错误消息
 * @return DBus私有连接
 * *********************************************************************************/
DBusConnection* become_monitor(char* filter[], size_t len, DBusError* err);

#endif // DBUS_MONITOR_H