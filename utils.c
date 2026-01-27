#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void utils_get_public_ip(char *buffer, int size) {
    if (buffer == NULL || size <= 0) {
        return;
    }

    FILE *fp = popen("curl -s ifconfig.me", "r");
    if (fp == NULL) {
        snprintf(buffer, size, "0.0.0.0");
        return;
    }

    if (fgets(buffer, size, fp) != NULL) {
        int len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    } else {
        snprintf(buffer, size, "0.0.0.0");
    }

    pclose(fp);
}

void utils_get_time_string(char *time_str, int size) {
    if (time_str == NULL || size <= 0) {
        return;
    }

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    snprintf(time_str, size, "%04d%02d%02d%02d%02d%02d000",
             tm_info->tm_year + 1900,
             tm_info->tm_mon + 1,
             tm_info->tm_mday,
             tm_info->tm_hour,
             tm_info->tm_min,
             tm_info->tm_sec);
}

int utils_calculate_checksum(const char *str) {
    if (str == NULL) {
        return 0;
    }

    int checksum = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] >= '0' && str[i] <= '9') {
            checksum += (str[i] - '0');
        }
    }

    return checksum;
}
