#include "protocol.h"
#include "utils.h"
#include "session.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void handle_login(Session *session, char *response, int response_size) {
    session_login(session);
    snprintf(response, response_size, "Logon\r\n");
    printf("[LOGIN] User authenticated\n");
}

static void handle_ping(Session *session, char *response, int response_size) {
    if (!session_is_logged_in(session)) {
        snprintf(response, response_size, "unknown\r\n");
        return;
    }

    snprintf(response, response_size, "pong\r\n");
    session_update_activity(session);
    printf("[PING] Heartbeat received\n");
}

static void handle_gettime(Session *session, char *response, int response_size) {
    if (!session_is_logged_in(session)) {
        snprintf(response, response_size, "unknown\r\n");
        return;
    }

    char time_str[20];
    utils_get_time_string(time_str, sizeof(time_str));

    int checksum = utils_calculate_checksum(time_str);

    snprintf(response, response_size, "time %s %d\r\n", time_str, checksum);
    session_update_activity(session);
    printf("[GETTIME] Sent time: %s (checksum: %d)\n", time_str, checksum);
}

static void handle_log_data(const char *command, Session *session) {
    if (!session_is_logged_in(session)) {
        return;
    }

    printf("[LOG DATA] %s\n", command);
    session_update_activity(session);
}

static void handle_log_over(const char *command, Session *session, char *response, int response_size) {
    if (!session_is_logged_in(session)) {
        snprintf(response, response_size, "unknown\r\n");
        return;
    }

    char orderid[64] = {0};
    sscanf(command, "log over %s", orderid);

    snprintf(response, response_size, "over %s\r\n", orderid);
    session_update_activity(session);
    printf("[LOG OVER] Order: %s - Charging session ended\n", orderid);
}

static void handle_on_command(const char *command, Session *session, char *response, int response_size) {
    if (!session_is_logged_in(session)) {
        snprintf(response, response_size, "unknown\r\n");
        return;
    }

    int sock;
    int time;
    char orderid[64] = {0};
    sscanf(command, "on %d %d %s", &sock, &time, orderid);

    printf("[ON] Socket: %d, Time: %d min, Order: %s - Charging started\n", sock, time, orderid);
    session_update_activity(session);
}

static void handle_off_command(const char *command, Session *session) {
    if (!session_is_logged_in(session)) {
        return;
    }

    int sock;
    char orderid[64] = {0};
    sscanf(command, "off %d %s", &sock, orderid);

    printf("[OFF] Socket: %d, Order: %s - Charging stopped\n", sock, orderid);
    session_update_activity(session);
}

static void handle_reboot(Session *session) {
    if (!session_is_logged_in(session)) {
        return;
    }

    printf("[REBOOT] Device reboot command received\n");
    session_update_activity(session);
}

static void handle_cat_data(Session *session) {
    if (!session_is_logged_in(session)) {
        return;
    }

    printf("[CAT_DATA] Data readback command received\n");
    session_update_activity(session);
}

static void handle_update(const char *command, Session *session, char *response, int response_size) {
    if (!session_is_logged_in(session)) {
        snprintf(response, response_size, "unknown\r\n");
        return;
    }

    char version[64] = {0};
    sscanf(command, "update %s", version);

    printf("[UPDATE] OTA update command for version: %s\n", version);
    session_update_activity(session);
}

void protocol_process(const char *command, Session *session, char *response, int response_size) {
    if (command == NULL || session == NULL || response == NULL) {
        return;
    }

    response[0] = '\0';

    if (strncmp(command, "LOGIN", 5) == 0) {
        handle_login(session, response, response_size);
    }
    else if (strncmp(command, "PING", 4) == 0) {
        handle_ping(session, response, response_size);
    }
    else if (strncmp(command, "GETTIME", 7) == 0) {
        handle_gettime(session, response, response_size);
    }
    else if (strncmp(command, "log data", 8) == 0) {
        handle_log_data(command, session);
    }
    else if (strncmp(command, "log over", 8) == 0) {
        handle_log_over(command, session, response, response_size);
    }
    else if (strncmp(command, "on ", 3) == 0) {
        handle_on_command(command, session, response, response_size);
    }
    else if (strncmp(command, "off ", 4) == 0) {
        handle_off_command(command, session);
    }
    else if (strncmp(command, "reboot", 6) == 0) {
        handle_reboot(session);
    }
    else if (strncmp(command, "cat_data", 8) == 0) {
        handle_cat_data(session);
    }
    else if (strncmp(command, "update", 6) == 0) {
        handle_update(command, session, response, response_size);
    }
    else {
        if (session_is_logged_in(session)) {
            printf("[UNKNOWN] Unknown command: %s\n", command);
        } else {
            snprintf(response, response_size, "unknown\r\n");
        }
    }
}
