#include <stdio.h>
#include "monitor.h"
#include "gsettings_blob.h"
#include "proc.h"

static DBusHandlerResult gsettings_changed(DBusConnection* conn, DBusMessage* msg, void* userdata);

int main()
{
    // 使用行缓冲，方便通过管道处理
    setvbuf(stdout, NULL, _IOLBF, 128);
    
    DBusError err;
    dbus_error_init(&err);

    // 成为监视器
    char* filter[] = {"interface=ca.desrt.dconf.Writer", "member=Change"};
    DBusConnection* conn = become_monitor(filter, 2, &err);
    if(conn == NULL || dbus_error_is_set(&err))
    {
        fprintf(stderr, "%s:%s\n", err.name, err.message);
        return EXIT_FAILURE;
    }

    // 设置监视回调函数
    dbus_connection_add_filter(conn, gsettings_changed, NULL, NULL);
    
    // 主循环
    while(dbus_connection_read_write_dispatch(conn, -1));
    dbus_connection_close(conn);
    dbus_connection_unref(conn);
    return EXIT_SUCCESS;
}

/***********************************************************************************
 * @brief gsettings被改写时响应的回掉函数
 * @param[in] conn DBus连接
 * @param[in] msg DBus消息
 * @param[in] userdata 用户数据
 * *********************************************************************************/
static DBusHandlerResult gsettings_changed(DBusConnection* conn, DBusMessage* msg, void* userdata)
{
    (void)(conn);
    (void)(userdata);
    if(!dbus_message_is_method_call(msg, "ca.desrt.dconf.Writer", "Change"))
    {
        // fprintf(stderr, "not method call of ca.desrt.dconf.Writer.Change\n");
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    DBusMessageIter iter;
    if(!dbus_message_iter_init(msg, &iter))
    {
        fprintf(stderr, "failed to init dbus message iter\n");
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    int len = dbus_message_iter_get_element_count(&iter);
    if(len <= 0)
    {
        fprintf(stderr, "empty iter\n");
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    uint8_t* data = malloc(len);
    if(data == NULL)
    {
        fprintf(stderr, "bad alloc\n");
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    DBusMessageIter subiter;
    dbus_message_iter_recurse(&iter, &subiter);
    ptrdiff_t index = 0;
    do
    {
        if(dbus_message_iter_get_arg_type(&subiter) != DBUS_TYPE_BYTE)
        {
            fprintf(stderr, "type error\n");
        }
        dbus_message_iter_get_basic(&subiter, data + index);
        index++;
    }while(dbus_message_iter_next(&subiter));
    GVariant* arg = gsettings_blob_parse(data, len);
    if(arg == NULL)
    {
        free(data);
        fprintf(stderr, "failed to parse gsettings blob\n");
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    
    char exe[PATH_MAX+1];
    uint32_t pid = proc_get_pid_by_dbus(dbus_message_get_sender(msg));
    proc_get_exec_path(pid, exe);

    GVariant* dictEntry = g_variant_get_child_value(arg, 0);
    GVariant* key = g_variant_get_child_value(dictEntry, 0);
    GVariant* value = g_variant_get_child_value(dictEntry, 1);
    char* keyString = g_variant_print(key, false);
    char* valueString = g_variant_print(value, false);

    printf("\n");
    printf("PID: %u\n", pid);
    printf("EXE: %s\n", exe);
    printf("PATH: %s\n", keyString);
    printf("VALUE: %s\n", valueString);
    
    free(keyString);
    free(valueString);
    g_variant_unref(key);
    g_variant_unref(value);
    g_variant_unref(dictEntry);
    g_variant_unref(arg);
    free(data);

    return DBUS_HANDLER_RESULT_HANDLED;
}