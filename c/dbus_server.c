#include <dbus/dbus.h>
#include <stdio.h>

#define SERVICE_NAME "org.example.Lab2Alarm"
#define OBJECT_PATH  "/org/example/Lab2Alarm"
#define INTERFACE    "org.example.Lab2Alarm"

static int alarm_received = 0;

static DBusHandlerResult alarm_filter(DBusConnection *connection,
                                      DBusMessage *message,
                                      void *user_data)
{
    (void)user_data;

    if (!dbus_message_is_method_call(message, INTERFACE, "SendAlarm"))
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

    DBusError error;
    const char *alarm_text = NULL;

    dbus_error_init(&error);

    if (!dbus_message_get_args(message, &error,
                               DBUS_TYPE_STRING, &alarm_text,
                               DBUS_TYPE_INVALID)) {
        fprintf(stderr, "메시지 읽기 실패: %s\n", error.message);
        dbus_error_free(&error);
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    printf("알람 메시지를 받았습니다: %s\n", alarm_text);
    fflush(stdout);

    DBusMessage *reply = dbus_message_new_method_return(message);
    if (reply == NULL) {
        fprintf(stderr, "응답 메시지를 만들지 못했습니다.\n");
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    const char *reply_text = "알람 메시지 전달 완료";
    dbus_message_append_args(reply,
                             DBUS_TYPE_STRING, &reply_text,
                             DBUS_TYPE_INVALID);
    dbus_connection_send(connection, reply, NULL);
    dbus_connection_flush(connection);
    dbus_message_unref(reply);

    alarm_received = 1;
    return DBUS_HANDLER_RESULT_HANDLED;
}

int main(void)
{
    DBusError error;
    DBusConnection *connection;

    dbus_error_init(&error);
    connection = dbus_bus_get(DBUS_BUS_SESSION, &error);

    if (dbus_error_is_set(&error)) {
        fprintf(stderr, "D-Bus 연결 실패: %s\n", error.message);
        dbus_error_free(&error);
        return 1;
    }

    int result = dbus_bus_request_name(
        connection, SERVICE_NAME, DBUS_NAME_FLAG_DO_NOT_QUEUE, &error);

    if (dbus_error_is_set(&error)) {
        fprintf(stderr, "서비스 이름 요청 실패: %s\n", error.message);
        dbus_error_free(&error);
        dbus_connection_unref(connection);
        return 1;
    }

    if (result != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        fprintf(stderr, "서비스 이름을 사용할 수 없습니다.\n");
        dbus_connection_unref(connection);
        return 1;
    }

    if (!dbus_connection_add_filter(connection, alarm_filter, NULL, NULL)) {
        fprintf(stderr, "메시지 필터 등록 실패\n");
        dbus_connection_unref(connection);
        return 1;
    }

    printf("알람 서버가 실행되었습니다. 클라이언트 메시지를 기다립니다.\n");
    fflush(stdout);

    while (!alarm_received &&
           dbus_connection_read_write_dispatch(connection, -1)) {
        /* 필터에서 알람 메시지를 처리함 */
    }

    dbus_connection_unref(connection);
    return 0;
}
