#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "session.h"

typedef struct {
    char *command;
    char response[1024];
} CommandResult;

void protocol_process(const char *command, Session *session, char *response, int response_size);

#endif
