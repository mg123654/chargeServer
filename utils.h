#ifndef UTILS_H
#define UTILS_H

void utils_get_public_ip(char *buffer, int size);
void utils_get_time_string(char *time_str, int size);
int utils_calculate_checksum(const char *str);

#endif
