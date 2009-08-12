#include "compat.h"

#ifdef APACHE1_3
void ap_set_content_type(request_rec *r, const char *ct)
{
    if (!ct) {
        r->content_type = NULL;
    }
    else if (!r->content_type || strcmp(r->content_type, ct)) {
        r->content_type = ct;
    }
}
#endif

API_EXPORT_NONSTD(void) ap_log_perror(const char *file, int line, int level,
                             int error, apr_pool_t *p, const char *fmt, ...)
{
    char errstr[MAX_STRING_LEN];
    va_list args;

    va_start(args, fmt);
    ap_vsnprintf(errstr, sizeof(errstr), fmt, args);
    va_end(args);
    ap_log_error(file, line, level, NULL, errstr, NULL);
}

void compat_log_error(const char *file, int line, int level, int error,
                  const server_rec *s, const char *fmt, ...)
{
    char errstr[MAX_STRING_LEN];
    va_list args;

    va_start(args, fmt);
    ap_vsnprintf(errstr, sizeof(errstr), fmt, args);
    va_end(args);
    ap_log_error(file, line, level, NULL, errstr, NULL);
}

void compat_log_rerror(const char *file, int line, int level, int error,
                   const request_rec *s, const char *fmt, ...)
{
    va_list args;
    char errstr[MAX_STRING_LEN];

    va_start(args, fmt);
    ap_vsnprintf(errstr, sizeof(errstr), fmt, args);
    va_end(args);
    ap_log_error(file, line, level, NULL, errstr, NULL);
}

