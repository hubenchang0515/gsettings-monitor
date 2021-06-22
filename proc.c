#include <stdio.h>
#include <unistd.h>
#include <dbus/dbus.h>
#include "proc.h"

/********************************************************************
 * @brief 根据进程号获取进程可执行文件的路径
 * @param[in] pid 进程号
 * @param[out] path 返回可执行文件的路径，传入的内存大小必须为 PATH_MAX+1
 * @return 是否成功
 * ******************************************************************/
bool proc_get_exec_path(uint32_t pid, char* path)
{
    char execLink[PATH_MAX+1];
    snprintf(execLink, PATH_MAX+1, "/proc/%u/exe", pid);
    ssize_t len = readlink(execLink, path, PATH_MAX);
    if(len < 0)
    {
        return false;
    }
    path[len] = 0;
    return true;
}

/********************************************************************
 * @brief 根据进程在DBus上的名称获取进程号
 * @param[in] name 进程在DBus上的名称
 * @return 进程号，0表示失败
 * ******************************************************************/
uint32_t proc_get_pid_by_dbus(const char* name)
{
    uint32_t pid = 0;
    DBusError err;
    dbus_error_init(&err);
    DBusConnection* conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if(conn == NULL || dbus_error_is_set(&err))
    {
        fprintf(stderr, "dbus_bus_get %s:%s\n", err.name, err.message);
        goto failed0;
    }

    DBusMessage* request = dbus_message_new_method_call("org.freedesktop.DBus",
                                                    "/org/freedesktop/DBus", 
                                                    "org.freedesktop.DBus",
                                                    "GetConnectionUnixProcessID");
    if(request == NULL)
    {
        fprintf(stderr, "dbus_message_new_method_call failed\n");
        goto failed1;
    }

    if(!dbus_message_append_args(request, DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID))
    {
        fprintf(stderr, "dbus_message_append_args failed\n");
        goto failed2;
    }

    DBusMessage* response = dbus_connection_send_with_reply_and_block(conn, request, 1000, &err);
    if(response == NULL || dbus_error_is_set(&err))
    {
        fprintf(stderr, "%s:%s\n", err.name, err.message);
        goto failed3;
    }

    if(dbus_message_get_type(response) != DBUS_MESSAGE_TYPE_METHOD_RETURN)
    {
        goto failed3;
    }

    DBusMessageIter iter;
    dbus_message_iter_init(response, &iter);
    dbus_message_iter_get_basic(&iter, &pid);

failed3:
    dbus_message_unref(response);
failed2:
    dbus_message_unref(request);
failed1:
    dbus_connection_unref(conn);
failed0:
    return pid;
}

/********************************************************************
 * @brief 根据进程在DBus上的名称获取进程的可执行文件路径
 * @param[in] name 进程在DBus上的名称
 * @param[out] path 返回可执行文件的路径，传入的内存大小必须为 PATH_MAX+1
 * @return 是否成功
 * ******************************************************************/
bool proc_get_exec_path_by_dbus(const char* name, char* path)
{
    return proc_get_exec_path(proc_get_pid_by_dbus(name), path);
}