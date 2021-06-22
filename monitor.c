#include <stdio.h>
#include <time.h>
#include <dbus/dbus.h>
#include "monitor.h"

/***********************************************************************************
 * @brief 申请成为监视器
 * @param[in] filter 过滤规则
 * @param[in] len filter的长度
 * @param[out] err DBus错误消息
 * @return DBus私有连接
 * *********************************************************************************/
DBusConnection* become_monitor(char* filter[], size_t len, DBusError* err)
{
    //  建立DBus连接
    DBusConnection* conn = dbus_bus_get_private(DBUS_BUS_SESSION, err);
    if(conn == NULL || dbus_error_is_set(err))
    {
        return NULL;
    }

    dbus_connection_set_route_peer_messages(conn, TRUE);

    DBusMessage* request = dbus_message_new_method_call("org.freedesktop.DBus",
                                                    "/org/freedesktop/DBus", 
                                                    "org.freedesktop.DBus.Monitoring",
                                                    "BecomeMonitor");

    dbus_uint32_t arg2 = 0;
    dbus_message_append_args(request, 
                            DBUS_TYPE_ARRAY, DBUS_TYPE_STRING, &filter, len,
                            DBUS_TYPE_UINT32, &arg2,
                            DBUS_TYPE_INVALID);
    DBusMessage* response = dbus_connection_send_with_reply_and_block(conn, request, -1, err);
    if(dbus_error_is_set(err))
    {
        goto failed0;
    }

    if(dbus_message_get_type(response) == DBUS_MESSAGE_TYPE_ERROR)
    {
        dbus_message_get_args(response, err, DBUS_TYPE_INVALID);
        goto failed1;
    }

    dbus_message_unref(response);
    dbus_message_unref(request);
    return conn;

failed1:
    dbus_message_unref(response);
failed0:
    dbus_message_unref(request);
    dbus_connection_close(conn);
    dbus_connection_unref(conn);
    return NULL;
}

