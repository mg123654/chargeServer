#include "session.h"
#include <stdlib.h>
#include <time.h>

Session* session_create(void) {
    Session *session = (Session *)malloc(sizeof(Session));
    if (session == NULL) {
        return NULL;
    }

    session->logged_in = 0;
    session->last_activity = time(NULL);

    return session;
}

void session_destroy(Session *session) {
    if (session != NULL) {
        free(session);
    }
}

int session_is_logged_in(Session *session) {
    if (session == NULL) {
        return 0;
    }

    return session->logged_in;
}

void session_login(Session *session) {
    if (session != NULL) {
        session->logged_in = 1;
        session->last_activity = time(NULL);
    }
}

void session_logout(Session *session) {
    if (session != NULL) {
        session->logged_in = 0;
    }
}

void session_update_activity(Session *session) {
    if (session != NULL) {
        session->last_activity = time(NULL);
    }
}
