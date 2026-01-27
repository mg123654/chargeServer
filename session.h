#ifndef SESSION_H
#define SESSION_H

#include <time.h>

typedef struct {
    int logged_in;
    time_t last_activity;
} Session;

Session* session_create(void);
void session_destroy(Session *session);
int session_is_logged_in(Session *session);
void session_login(Session *session);
void session_logout(Session *session);
void session_update_activity(Session *session);

#endif
