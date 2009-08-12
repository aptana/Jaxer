#ifndef JAX_COMPAT_H
#define JAX_COMPAT_H

#include "apr_pools.h"
#include "httpd.h"

void ap_log_perror(const char *file, int line, int level,
                   int error, apr_pool_t *p, const char *fmt, ...);

void compat_log_error(const char *file, int line, int level, int error,
                  const server_rec *s, const char *fmt, ...);
void compat_log_rerror(const char *file, int line, int level, int error,
                   const request_rec *s, const char *fmt, ...);

void ap_set_content_type(request_rec *r, const char *ct);

#endif 
