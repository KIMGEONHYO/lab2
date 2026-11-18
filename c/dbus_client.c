#include <dbus/dbus.h>
#include <stdio.h>

#define SERVICE_NAME "org.example.Lab2Alarm"
#define OBJECT_PATH  "/org/example/Lab2Alarm"
#define INTERFACE    "org.example.Lab2Alarm"

int main(int argc, char *argv[])
{
    DBusError error;
    DBusConnection *connection;
    DBusMessage *message;
    DBusMessage *reply;
    DBusMessageIter arguments;
    const char *alarm_text;
    const char *reply_text = NULL;

    if (argc != 2) {
        fprintf(stderr, "사용법: %s \"알람 메시지\"\n", argv[0]);
        return 1;
    }

    alarm_text = argv[1];
    dbus_error_init(&error);

    connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
    if (dbus_error_is_set(&error)) {
        fprintf(stderr, "D-Bus 연결 실패: %s\n", error.message);
        dbus_error_free(&error);
        return 1;
    }

    message = dbus_message_new_method_call(
        SERVICE_NAME, OBJECT_PATH, INTERFACE, "SendAlarm");

    if (message == NULL) {
        fprintf(stderr, "요청 메시지를 만들지 못했습니다.\n");
        dbus_connection_unref(connection);
        return 1;
    }

    dbus_message_iter_init_append(message, &arguments);
    if (!dbus_message_iter_append_basic(
            &arguments, DBUS_TYPE_STRING, &alarm_text)) {
        fprintf(stderr, "알람 문장을 추가하지 못했습니다.\n");
        dbus_message_unref(message);
        dbus_connection_unref(connection);
        return 1;
    }

    reply = dbus_connection_send_with_reply_and_block(
        connection, message, 5000, &error);

    dbus_message_unref(message);

    if (dbus_error_is_set(&error)) {
        fprintf(stderr, "서버 응답 실패: %s\n", error.message);
        dbus_error_free(&error);
        dbus_connection_unref(connection);
        return 1;
    }

    if (reply == NULL ||
        !dbus_message_get_args(reply, &error,
                               DBUS_TYPE_STRING, &reply_text,
                               DBUS_TYPE_INVALID)) {
        fprintf(stderr, "서버의 응답을 읽지 못했습니다.\n");
        if (dbus_error_is_set(&error))
            dbus_error_free(&error);
        if (reply != NULL)
            dbus_message_unref(reply);
        dbus_connection_unref(connection);
        return 1;
    }

    printf("서버 응답: %s\n", reply_text);

    dbus_message_unref(reply);
    dbus_connection_unref(connection);
    return 0;
}
