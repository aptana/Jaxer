/* ====================================================================
 * Copyright (c) 1995 The Apache Group.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the Apache Group
 *    for use in the Apache HTTP server project (http://www.apache.org/)."
 *
 * 4. The names "Apache Server" and "Apache Group" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission.
 *
 * 5. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the Apache Group
 *    for use in the Apache HTTP server project (http://www.apache.org/)."
 *
 * THIS SOFTWARE IS PROVIDED BY THE APACHE GROUP ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE APACHE GROUP OR
 * IT'S CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Group and was originally based
 * on public domain software written at the National Center for
 * Supercomputing Applications, University of Illinois, Urbana-Champaign.
 * For more information on the Apache Group and the Apache HTTP server
 * project, please see <http://www.apache.org/>.
 *
 */


/*
 * Module definition information - the part between the -START and -END
 * lines below is used by Configure. This could be stored in a separate
 * instead.
 *
 * MODULE-DEFINITION-START
 * Name: mysql_auth_module
 * ConfigStart
     MYSQL_LIB="-L/usr/local/lib/mysql -lmysqlclient -lm -lz"
     if [ "X$MYSQL_LIB" != "X" ]; then
         LIBS="$LIBS $MYSQL_LIB"
         echo " + using $MYSQL_LIB for MySQL support"
     fi
 * ConfigEnd
 * MODULE-DEFINITION-END
 */

#define STRING(x) STR(x)		/* Used to build strings from compile options */
#define STR(x) #x

#include "ap_mmn.h"			/* For MODULE_MAGIC_NUMBER */
/* Use the MODULE_MAGIC_NUMBER to check if at least Apache 2.0 */
#if AP_MODULE_MAGIC_AT_LEAST(20010223,0)
  #define APACHE2
#endif

/* Compile time options for code generation */
#ifdef AES
  #define _AES 1
#else
  #define _AES 0
#endif
/* set any defaults not specified at compile time */
#ifdef HOST				/* Host to use */
  #define _HOST STRING(HOST)
#else
  #define _HOST 0			/* Will default to localhost */
#endif

/* Apache 1.x defines the port as a string, but Apache 2.x uses an integer */
#ifdef PORT				/* The port to use */
  #ifdef APACHE2
    #define _PORT PORT
  #else
    #define _PORT STRING(PORT)
  #endif
#else
  #ifdef APACHE2
    #define _PORT MYSQL_PORT		/* Use the one from MySQL */
  #else
    #define _PORT STRING(MYSQL_PORT)
  #endif
#endif

#ifdef SOCKET				/* UNIX socket */
  #define _SOCKET STRING(SOCKET)
#else
  #define _SOCKET MYSQL_UNIX_ADDR
#endif

#ifdef USER				/* Authorized user */
  #define _USER STRING(USER)
#else
  #define _USER 0			/* User must be specified in config */
#endif

#ifdef PASSWORD				/* Default password */
  #define _PASSWORD STRING(PASSWORD)
#else
  #define _PASSWORD 0			/* Password must be specified in config */
#endif

#ifdef DB				/* Default database */
  #define _DB STRING(DB)
#else
  #define _DB "test"			/* Test database */
#endif

#ifdef PWTABLE				/* Password table */
  #define _PWTABLE STRING(PWTABLE)
#else
  #define _PWTABLE "user_info" 		/* Default is user_info */
#endif

#ifdef NAMEFIELD			/* Name column in password table */
  #define _NAMEFIELD STRING(NAMEFIELD)
#else
  #define _NAMEFIELD "user_name"	/* Default is "user_name" */
#endif

#ifdef PASSWORDFIELD			/* Password column in password table */
  #define _PASSWORDFIELD STRING(PASSWORDFIELD)
#else
  #define _PASSWORDFIELD "user_password" /* Default is user_password */
#endif

#ifdef GROUPUSERNAMEFIELD
  #define _GROUPUSERNAMEFIELD STRING(GROUPUSERNAMEFIELD)
#else
  #define _GROUPUSERNAMEFIELD NULL
#endif

#ifdef ENCRYPTION			/* Encryption type */
  #define _ENCRYPTION STRING(ENCRYPTION)
#else
  #define _ENCRYPTION 0			/* Will default to "crypt" in code */
#endif

#ifdef SALTFIELD			/* If a salt column is not defined */
  #define _SALTFIELD STRING(SALTFIELD)
#else
  #define _SALTFIELD "<>"		/* Default is no salt */
#endif

#ifdef KEEPALIVE			/* Keep the connection alive */
  #define _KEEPALIVE KEEPALIVE
#else
  #define _KEEPALIVE 0			/* Do not keep it alive */
#endif

#ifdef AUTHORITATIVE			/* If we are the last word */
  #define _AUTHORITATIVE AUTHORITATIVE
#else
  #define _AUTHORITATIVE 1 		/* Yes, we are */
#endif

#ifdef NOPASSWORD			/* If password not needed */
  #define _NOPASSWORD NOPASSWORD
#else
  #define _NOPASSWORD 0			/* It is required */
#endif

#ifdef ENABLE				/* If we are to be enabled */
  #define _ENABLE ENABLE
#else
  #define _ENABLE 1			/* Assume we are */
#endif

#ifdef CHARACTERSET
  #define _CHARACTERSET STRING(CHARACTERSET)
#else
  #define _CHARACTERSET NULL		/* Default is no character set */
#endif

#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "http_log.h"
#include "http_protocol.h"

#ifdef APACHE2
  #define PCALLOC apr_pcalloc
  #define SNPRINTF apr_snprintf
  #define PSTRDUP apr_pstrdup
  #define PSTRNDUP apr_pstrndup
  #define STRCAT ap_pstrcat
  #define POOL apr_pool_t
  #include "http_request.h"   /* for ap_hook_(check_user_id | auth_checker)*/
  #include "ap_compat.h"
  #include "apr_strings.h"
  #include "apr_sha1.h"
  #include "apr_base64.h"
  #include "apr_lib.h"
  #define ISSPACE apr_isspace
  #ifdef CRYPT
    #include "crypt.h"
  #else
    #include "unistd.h"
  #endif
  #define LOG_ERROR(lvl, stat, rqst, msg)  \
	  ap_log_rerror (APLOG_MARK, lvl, stat, rqst, msg)
  #define LOG_ERROR_1(lvl, stat, rqst, msg, parm)  \
	  ap_log_rerror (APLOG_MARK, lvl, stat, rqst, msg, parm)
  #define LOG_ERROR_2(lvl, stat, rqst, msg, parm1, parm2)  \
	  ap_log_rerror (APLOG_MARK, lvl, stat, rqst, msg, parm1, parm2)
  #define LOG_ERROR_3(lvl, stat, rqst, msg, parm1, parm2, parm3)  \
	  ap_log_rerror (APLOG_MARK, lvl, stat, rqst, msg, parm1, parm2, parm3)
  #define APACHE_FUNC static apr_status_t
  #define APACHE_FUNC_RETURN(rc) return rc
  #define NOT_AUTHORIZED HTTP_UNAUTHORIZED
  #define TABLE_GET apr_table_get
#else
  #define PCALLOC ap_pcalloc
  #define SNPRINTF ap_snprintf
  #define PSTRDUP ap_pstrdup
  #define PSTRNDUP ap_pstrndup
  #define STRCAT ap_pstrcat
  #define POOL pool
  #include <stdlib.h>
  #include "ap_sha1.h"
  #include "ap_ctype.h"
  #define LOG_ERROR(lvl, stat, rqst, msg) \
	  ap_log_error(APLOG_MARK, lvl, rqst->server, msg)
  #define LOG_ERROR_1(lvl, stat, rqst, msg, parm) \
	  ap_log_error(APLOG_MARK, lvl, rqst->server, msg, parm)
  #define LOG_ERROR_2(lvl, stat, rqst, msg, parm1, parm2) \
	  ap_log_error(APLOG_MARK, lvl, rqst->server, msg, parm1, parm2)
  #define LOG_ERROR_3(lvl, stat, rqst, msg, parm1, parm2, parm3) \
	  ap_log_error(APLOG_MARK, lvl, rqst->server, msg, parm1, parm2, parm3)
  #define APACHE_FUNC static void
  #define APACHE_FUNC_RETURN(rc) return
  #define NOT_AUTHORIZED AUTH_REQUIRED
  #define TABLE_GET ap_table_get
  #define ISSPACE ap_isspace
#endif

#include "util_md5.h"
#ifndef APACHE2
/* Both Apache 1's ap_config.h and my_global.h define closesocket (to the same value) */
/* This gets rid of a warning message.  It's OK because we don't use it anyway */
  #undef closesocket
#endif
#if _AES  /* Only needed if AES encryption desired */
  #include <my_global.h>
#endif
#include <mysql.h>
#if _AES
  #include <my_aes.h>
#endif

#ifndef SCRAMBLED_PASSWORD_CHAR_LENGTH /* Ensure it is defined for older MySQL releases */
  #define SCRAMBLED_PASSWORD_CHAR_LENGTH 32 /* Big enough for the old method of scrambling */
#endif

/* salt flags */
#define NO_SALT		      0
#define SALT_OPTIONAL	      1
#define SALT_REQUIRED	      2

/* forward function declarations */
static short pw_scrambled(POOL * pool, const char * real_pw, const char * sent_pw, const char * salt);
static short pw_md5(POOL * pool, const char * real_pw, const char * sent_pw, const char * salt);
static short pw_crypted(POOL * pool, const char * real_pw, const char * sent_pw, const char * salt);
#if _AES
static short pw_aes(POOL * pool, const char * real_pw, const char * sent_pw, const char * salt);
#endif
static short pw_sha1(POOL * pool, const char * real_pw, const char * sent_pw, const char * salt);
static short pw_plain(POOL * pool, const char * real_pw, const char * sent_pw, const char * salt);
static short pw_dummy(POOL * pool, const char * real_pw, const char * sent_pw, const char * salt);

static char * format_remote_host(request_rec * r, char ** parm);
static char * format_remote_ip(request_rec * r, char ** parm);
static char * format_filename(request_rec * r, char ** parm);
static char * format_server_name(request_rec * r, char ** parm);
static char * format_server_hostname(request_rec * r, char ** parm);
static char * format_protocol(request_rec * r, char ** parm);
static char * format_method(request_rec * r, char ** parm);
static char * format_args(request_rec * r, char ** parm);
static char * format_request(request_rec * r, char ** parm);
static char * format_uri(request_rec * r, char ** parm);
static char * format_percent(request_rec * r, char ** parm);
static char * format_cookie(request_rec * r, char ** parm);


typedef struct {	      /* Encryption methods */
  char * string; 	      /* Identifing string */
  short salt_status;	      /* If a salt is required, optional or unused */
  short (*func)(POOL * pool, const char * real_pw, const char * sent_pw, const char * salt);
} encryption ;

/* Encryption methods used.  The first entry is the default entry */
static encryption encryptions[] = {{"crypt", SALT_OPTIONAL, pw_crypted},
					   {"none", NO_SALT, pw_plain},
					   {"scrambled", NO_SALT, pw_scrambled},
					   {"md5", NO_SALT, pw_md5},
#if _AES
					   {"aes", SALT_REQUIRED, pw_aes},
#endif
					   {"aptana", NO_SALT, pw_dummy},
					   {"sha1", NO_SALT, pw_sha1}};
typedef struct {		/* User formatting patterns */
  char pattern;			/* Pattern to match */
  char * (*func)(request_rec * r, char ** parm);
} format;

format formats[] = {{'h', format_remote_host},
	            {'a', format_remote_ip},
		    {'f', format_filename},
		    {'V', format_server_name},
		    {'v', format_server_hostname},
		    {'H', format_protocol},
		    {'m', format_method},
		    {'q', format_args},
		    {'r', format_request},
		    {'U', format_uri},
		    {'%', format_percent},
		    {'C', format_cookie}};
/*
 * structure to hold the configuration details for the request
 */
typedef struct  {
  char *mysqlhost;		/* host name of db server */
#ifdef APACHE2
  int  mysqlport;		/* port number of db server */
#else
  char * mysqlport;		/* port number of db server */
#endif
  char *mysqlsocket;		/* socket path of db server */
  char *mysqluser;		/* user ID to connect to db server */
  char *mysqlpasswd;		/* password to connect to db server */
  char *mysqlDB;		/* DB name */
  char *mysqlpwtable;		/* user password table */
  char *mysqlgrptable;		/* user group table */
  char *mysqlNameField;		/* field in password table with username */
  char *mysqlPasswordField;	/* field in password table with password */
  char *mysqlGroupField;	/* field in group table with group name */
  char *mysqlGroupUserNameField;/* field in group table with username */
  char *mysqlEncryptionField;   /* encryption type for passwords */
  char *mysqlSaltField;		/* salt for scrambled password */
  int  mysqlKeepAlive;		/* keep connection persistent? */
  int  mysqlAuthoritative;	/* are we authoritative? */
  int  mysqlNoPasswd;		/* do we ignore password? */
  int  mysqlEnable;		/* do we bother trying to auth at all? */
  char *mysqlUserCondition; 	/* Condition to add to the user where-clause in select query */
  char *mysqlGroupCondition; 	/* Condition to add to the group where-clause in select query */
  char *mysqlCharacterSet;	/* MySQL character set to use */
} mysql_auth_config_rec;

/*
 * Global information for the database connection.  Contains
 * the host name, userid and database name used to open the
 * connection.  If handle is not null, assume it is
 * still valid.  MySQL in recent incarnations will re-connect
 * automaticaly if the connection is closed, so we don't have
 * to worry about that here.
 */
typedef struct {
  MYSQL * handle;
  char host [255];
  char user [255];
  char db [255];
  time_t last_used;
} mysql_connection;

static mysql_connection connection = {NULL, "", "", ""};

/*
 * Global handle to db.  If not null, assume it is still valid.
 * MySQL in recent incarnations will re-connect automatically if the
 * connection is closed, so we don't worry about that here.
 */
/* static MYSQL *mysql_handle = NULL; */

static void close_connection() {
  if (connection.handle)
    mysql_close(connection.handle);
  connection.handle = NULL;		/* make sure we don't try to use it later */
  return;
}

/*
 * Callback to close mysql handle when necessary.  Also called when a
 * child httpd process is terminated.
 */
APACHE_FUNC
mod_auth_mysql_cleanup (void *notused)
{
  close_connection();
  APACHE_FUNC_RETURN(0);
}

/*
 * empty function necessary because register_cleanup requires it as one
 * of its parameters
 */
APACHE_FUNC
mod_auth_mysql_cleanup_child (void *data)
{
  /* nothing */
  APACHE_FUNC_RETURN(0);
}


#ifndef APACHE2
/*
 * handler to do cleanup on child exit
 */
static void
child_exit(server_rec *s, pool *p)
{
  mod_auth_mysql_cleanup(NULL);
}
#endif



#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/*
 * open connection to DB server if necessary.  Return TRUE if connection
 * is good, FALSE if not able to connect.  If false returned, reason
 * for failure has been logged to error_log file already.
 */
static int
open_db_handle(request_rec *r, mysql_auth_config_rec *m)
{
  static MYSQL mysql_conn;
  char query[MAX_STRING_LEN];
  short host_match = FALSE;
  short user_match = FALSE;

  if (connection.handle) {

#ifdef _DO_APTANA_DEBUG
  ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, r->pool, "in open_db_handle: already got handle");
#endif
    /* See if the host has changed */
    if (!m->mysqlhost || (strcmp(m->mysqlhost, "localhost") == 0)) {
      if (connection.host[0] == '\0')
        host_match = TRUE;
    }
    else
      if (m->mysqlhost && (strcmp(m->mysqlhost, connection.host) == 0))
	host_match = TRUE;

    /* See if the user has changed */
    if (m->mysqluser) {
      if (strcmp(m->mysqluser, connection.user) == 0)
	user_match = TRUE;
    }
    else
      if (connection.user[0] == '\0')
        user_match = TRUE;

    /* if the host, or user have changed, need to close and reopen database connection */
    if (host_match && user_match) {
      /* If the database hasn't changed, we can just return */
      if (m->mysqlDB && strcmp(m->mysqlDB, connection.db) == 0)
      {
#ifdef _DO_APTANA_DEBUG
  ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, r->pool, "in open_db_handle: same host/user/DB - reuse connection");
#endif
	return TRUE; /* already open */

      /* Otherwise we need to reselect the database */
      }else {
#ifdef _DO_APTANA_DEBUG
  ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, r->pool, "in open_db_handle: reselect db");
#endif
	if (mysql_select_db(connection.handle,m->mysqlDB) != 0) {
	  LOG_ERROR_1(APLOG_ERR, 0, r, "MySQL ERROR: %s", mysql_error(connection.handle));
	  return FALSE;
	}
	else {
	  strcpy (connection.db, m->mysqlDB);
	  return TRUE;
	}
      }
    }
    else
      close_connection();
  }
#ifdef _DO_APTANA_DEBUG
  ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, r->pool, "in open_db_handle: closed db due to user/host change");
  if (!host_match)
    ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, r->pool, "old host=%s", connection.host);
  if (!user_match)
    ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, r->pool, "old user=%s", connection.user);
#endif

  connection.handle = mysql_init(&mysql_conn);
  if (! connection.handle) {
    LOG_ERROR_1(APLOG_ERR, 0, r, "MySQL ERROR: %s", mysql_error(&mysql_conn));
  }

  if (!m->mysqlhost || strcmp(m->mysqlhost,"localhost") == 0) {
    connection.host[0] = '\0';
  } else {
    strcpy(connection.host, m->mysqlhost);
  }

#ifdef APACHE2
  connection.handle=mysql_real_connect(&mysql_conn,connection.host,m->mysqluser,
		  		  m->mysqlpasswd, NULL, m->mysqlport,
				  m->mysqlsocket, 0);
#else
  connection.handle=mysql_real_connect(&mysql_conn,connection.host,m->mysqluser,
		  		  m->mysqlpasswd, NULL, atoi(m->mysqlport),
				  m->mysqlsocket, 0);
#endif
  if (!connection.handle) {
    LOG_ERROR_1(APLOG_ERR, 0, r, "MySQL ERROR: %s", mysql_error(&mysql_conn));
    return FALSE;
  }

  if (!m->mysqlKeepAlive) {
    /* close when request done */
#ifdef APACHE2
    apr_pool_cleanup_register(r->pool, (void *)NULL, mod_auth_mysql_cleanup, mod_auth_mysql_cleanup_child);
#else
    ap_register_cleanup(r->pool, (void *)NULL, mod_auth_mysql_cleanup, mod_auth_mysql_cleanup_child);
#endif
  }

  if (m->mysqluser)
    strcpy(connection.user, m->mysqluser);
  else
    connection.user[0] = '\0';

#ifdef _DO_APTANA_DEBUG
  ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, r->pool, "in open_db_handle: user=%s host=%s", connection.user, connection.user);
#endif

  if (mysql_select_db(connection.handle,m->mysqlDB) != 0) {
    LOG_ERROR_1(APLOG_ERR, 0, r, "MySQL ERROR: %s", mysql_error(connection.handle));
    return FALSE;
  }
  else {
    strcpy (connection.db, m->mysqlDB);
  }
  if (m->mysqlCharacterSet) {	/* If a character set was specified */
    SNPRINTF(query, sizeof(query)-1, "SET CHARACTER SET %s", m->mysqlCharacterSet);
    if (mysql_query(connection.handle, query) != 0) {
      LOG_ERROR_2(APLOG_ERR, 0, r, "MySQL ERROR: %s: %s", mysql_error(connection.handle), r->uri);
      return FALSE;
    }
  }

  return TRUE;
}

static void * create_mysql_auth_dir_config (POOL *p, char *d)
{
  mysql_auth_config_rec *m = PCALLOC(p, sizeof(mysql_auth_config_rec));
  if (!m) return NULL;		/* failure to get memory is a bad thing */

  /* default values */
  m->mysqlhost = _HOST;
  m->mysqlport = _PORT;
  m->mysqlsocket = _SOCKET;
  m->mysqluser = _USER;
  m->mysqlpasswd = _PASSWORD;
  m->mysqlDB = _DB;
  m->mysqlpwtable = _PWTABLE;
  m->mysqlgrptable = 0;                             /* user group table */
  m->mysqlNameField = _NAMEFIELD;		    /* default user name field */
  m->mysqlPasswordField = _PASSWORDFIELD;	    /* default user password field */
  m->mysqlGroupUserNameField = _GROUPUSERNAMEFIELD; /* user name field in group table */
  m->mysqlEncryptionField = _ENCRYPTION;  	    /* default encryption is encrypted */
  m->mysqlSaltField = _SALTFIELD;	    	    /* default is scramble password against itself */
  m->mysqlKeepAlive = _KEEPALIVE;         	    /* do not keep persistent connection */
  m->mysqlAuthoritative = _AUTHORITATIVE; 	    /* we are authoritative source for users */
  m->mysqlNoPasswd = _NOPASSWORD;         	    /* we require password */
  m->mysqlEnable = _ENABLE;		    	    /* authorization on by default */
  m->mysqlUserCondition = 0;             	    /* No condition to add to the user
						       where-clause in select query */
  m->mysqlGroupCondition = 0;            	    /* No condition to add to the group
						       where-clause in select query */
  m->mysqlCharacterSet = _CHARACTERSET;		    /* default characterset to use */
  return (void *)m;
}

#ifdef APACHE2
static
command_rec mysql_auth_cmds[] = {
	AP_INIT_TAKE1("AuthMySQLHost", ap_set_string_slot,
	(void *) APR_XtOffsetOf(mysql_auth_config_rec, mysqlhost),
	OR_AUTHCFG, "mysql server host name"),

	AP_INIT_TAKE1("AuthMySQLPort", ap_set_int_slot,
	(void *) APR_XtOffsetOf(mysql_auth_config_rec, mysqlport),
	OR_AUTHCFG, "mysql server port number"),

	AP_INIT_TAKE1("AuthMySQLSocket", ap_set_string_slot,
	(void *) APR_XtOffsetOf(mysql_auth_config_rec, mysqlsocket),
	OR_AUTHCFG, "mysql server socket path"),

	AP_INIT_TAKE1("AuthMySQLUser", ap_set_string_slot,
	(void *) APR_XtOffsetOf(mysql_auth_config_rec, mysqluser),
	OR_AUTHCFG, "mysql server user name"),

	AP_INIT_TAKE1("AuthMySQLPassword", ap_set_string_slot,
	(void *) APR_XtOffsetOf(mysql_auth_config_rec, mysqlpasswd),
	OR_AUTHCFG, "mysql server user password"),

	AP_INIT_TAKE1("AuthMySQLDB", ap_set_string_slot,
	(void *) APR_XtOffsetOf(mysql_auth_config_rec, mysqlDB),
	OR_AUTHCFG, "mysql database name"),

	AP_INIT_TAKE1("AuthMySQLUserTable", ap_set_string_slot,
	(void *) APR_XtOffsetOf(mysql_auth_config_rec, mysqlpwtable),
	OR_AUTHCFG, "mysql user table name"),

	AP_INIT_TAKE1("AuthMySQLGroupTable", ap_set_string_slot,
	(void *) APR_XtOffsetOf(mysql_auth_config_rec, mysqlgrptable),
	OR_AUTHCFG, "mysql group table name"),

	AP_INIT_TAKE1("AuthMySQLNameField", ap_set_string_slot,
	(void *) APR_XtOffsetOf(mysql_auth_config_rec, mysqlNameField),
	OR_AUTHCFG, "mysql User ID field name within User table"),

	AP_INIT_TAKE1("AuthMySQLGroupField", ap_set_string_slot,
	(void *) APR_XtOffsetOf(mysql_auth_config_rec, mysqlGroupField),
	OR_AUTHCFG, "mysql Group field name within table"),

	AP_INIT_TAKE1("AuthMySQLGroupUserNameField", ap_set_string_slot,
	(void *) APR_XtOffsetOf(mysql_auth_config_rec, mysqlGroupUserNameField),
	OR_AUTHCFG, "mysql User ID field name within Group table"),

	AP_INIT_TAKE1("AuthMySQLPasswordField", ap_set_string_slot,
	(void *) APR_XtOffsetOf(mysql_auth_config_rec, mysqlPasswordField),
	OR_AUTHCFG, "mysql Password field name within table"),

	AP_INIT_TAKE1("AuthMySQLPwEncryption", ap_set_string_slot,
	(void *) APR_XtOffsetOf(mysql_auth_config_rec, mysqlEncryptionField),
	OR_AUTHCFG, "mysql password encryption method"),

	AP_INIT_TAKE1("AuthMySQLSaltField", ap_set_string_slot,
	(void*) APR_XtOffsetOf(mysql_auth_config_rec, mysqlSaltField),
	OR_AUTHCFG, "mysql salfe field name within table"),

#if 1 //aptana
	AP_INIT_FLAG("AuthMySQLKeepAlive", ap_set_flag_slot,
	(void *) APR_XtOffsetOf(mysql_auth_config_rec, mysqlKeepAlive),
	OR_AUTHCFG, "mysql connection kept open across requests if On"),
#endif

	AP_INIT_FLAG("AuthMySQLAuthoritative", ap_set_flag_slot,
	(void *) APR_XtOffsetOf(mysql_auth_config_rec, mysqlAuthoritative),
	OR_AUTHCFG, "mysql lookup is authoritative if On"),

	AP_INIT_FLAG("AuthMySQLNoPasswd", ap_set_flag_slot,
	(void *) APR_XtOffsetOf(mysql_auth_config_rec, mysqlNoPasswd),
	OR_AUTHCFG, "If On, only check if user exists; ignore password"),

	AP_INIT_FLAG("AuthMySQLEnable", ap_set_flag_slot,
	(void *) APR_XtOffsetOf(mysql_auth_config_rec, mysqlEnable),
	OR_AUTHCFG, "enable mysql authorization"),

	AP_INIT_TAKE1("AuthMySQLUserCondition", ap_set_string_slot,
	(void *) APR_XtOffsetOf(mysql_auth_config_rec, mysqlUserCondition),
	OR_AUTHCFG, "condition to add to user where-clause"),

	AP_INIT_TAKE1("AuthMySQLGroupCondition", ap_set_string_slot,
	(void *) APR_XtOffsetOf(mysql_auth_config_rec, mysqlGroupCondition),
	OR_AUTHCFG, "condition to add to group where-clause"),

	AP_INIT_TAKE1("AuthMySQLCharacterSet", ap_set_string_slot,
	(void *) APR_XtOffsetOf(mysql_auth_config_rec, mysqlCharacterSet),
	OR_AUTHCFG, "mysql character set to be used"),

  { NULL }
};
#else
static
command_rec mysql_auth_cmds[] = {
  { "AuthMySQLHost", ap_set_string_slot,
    (void*)XtOffsetOf(mysql_auth_config_rec, mysqlhost),
    OR_AUTHCFG, TAKE1, "mysql server host name" },

  { "AuthMySQLSocket", ap_set_string_slot,
    (void*)XtOffsetOf(mysql_auth_config_rec, mysqlsocket),
    OR_AUTHCFG, TAKE1, "mysql server socket path" },

  { "AuthMySQLPort", ap_set_string_slot,
    (void*)XtOffsetOf(mysql_auth_config_rec, mysqlport),
    OR_AUTHCFG, TAKE1, "mysql server port number" },

  { "AuthMySQLUser", ap_set_string_slot,
    (void*)XtOffsetOf(mysql_auth_config_rec, mysqluser),
    OR_AUTHCFG, TAKE1, "mysql server user name" },

  { "AuthMySQLPassword", ap_set_string_slot,
    (void*)XtOffsetOf(mysql_auth_config_rec, mysqlpasswd),
    OR_AUTHCFG, TAKE1, "mysql server user password" },

  { "AuthMySQLDB", ap_set_string_slot,
    (void*)XtOffsetOf(mysql_auth_config_rec, mysqlDB),
    OR_AUTHCFG, TAKE1, "mysql database name" },

  { "AuthMySQLUserTable", ap_set_string_slot,
    (void*)XtOffsetOf(mysql_auth_config_rec, mysqlpwtable),
    OR_AUTHCFG, TAKE1, "mysql user table name" },

  { "AuthMySQLGroupTable", ap_set_string_slot,
    (void*)XtOffsetOf(mysql_auth_config_rec, mysqlgrptable),
    OR_AUTHCFG, TAKE1, "mysql group table name" },

  { "AuthMySQLNameField", ap_set_string_slot,
    (void*)XtOffsetOf(mysql_auth_config_rec, mysqlNameField),
    OR_AUTHCFG, TAKE1, "mysql User ID field name within User table" },

  { "AuthMySQLGroupField", ap_set_string_slot,
    (void*)XtOffsetOf(mysql_auth_config_rec, mysqlGroupField),
    OR_AUTHCFG, TAKE1, "mysql Group field name within table" },

  { "AuthMySQLGroupUserNameField", ap_set_string_slot,
    (void*)XtOffsetOf(mysql_auth_config_rec, mysqlGroupUserNameField),
    OR_AUTHCFG, TAKE1, "mysql User ID field name within Group table" },

  { "AuthMySQLPasswordField", ap_set_string_slot,
    (void*)XtOffsetOf(mysql_auth_config_rec, mysqlPasswordField),
    OR_AUTHCFG, TAKE1, "mysql Password field name within table" },

  { "AuthMySQLPwEncryption", ap_set_string_slot,
    (void *)XtOffsetOf(mysql_auth_config_rec, mysqlEncryptionField),
    OR_AUTHCFG, TAKE1, "mysql password encryption method" },

  { "AuthMySQLSaltField", ap_set_string_slot,
    (void *)XtOffsetOf(mysql_auth_config_rec, mysqlSaltField),
    OR_AUTHCFG, TAKE1, "mysql salt field name within table" },

#if 1 // Aptana
  { "AuthMySQLKeepAlive", ap_set_flag_slot,
    (void*)XtOffsetOf(mysql_auth_config_rec, mysqlKeepAlive),
    OR_AUTHCFG, FLAG, "mysql connection kept open across requests if On" },
#endif

  { "AuthMySQLAuthoritative", ap_set_flag_slot,
    (void*)XtOffsetOf(mysql_auth_config_rec, mysqlAuthoritative),
    OR_AUTHCFG, FLAG, "mysql lookup is authoritative if On" },

  { "AuthMySQLNoPasswd", ap_set_flag_slot,
    (void*)XtOffsetOf(mysql_auth_config_rec, mysqlNoPasswd),
    OR_AUTHCFG, FLAG, "If On, only check if user exists; ignore password" },

  { "AuthMySQLEnable", ap_set_flag_slot,
    (void *)XtOffsetOf(mysql_auth_config_rec, mysqlEnable),
    OR_AUTHCFG, FLAG, "enable mysql authorization"},

  { "AuthMySQLUserCondition", ap_set_string_slot,
    (void*)XtOffsetOf(mysql_auth_config_rec, mysqlUserCondition),
    OR_AUTHCFG, TAKE1, "condition to add to user where-clause" },

  { "AuthMySQLGroupCondition", ap_set_string_slot,
    (void*)XtOffsetOf(mysql_auth_config_rec, mysqlGroupCondition),
    OR_AUTHCFG, TAKE1, "condition to add to group where-clause" },

  { "AuthMySQLCharacterSet", ap_set_string_slot,
    (void*)XtOffsetOf(mysql_auth_config_rec, mysqlCharacterSet),
    OR_AUTHCFG, TAKE1, "mysql character set to use" },

  { NULL }
};
#endif

module mysql_auth_module;

/*
 * Convert binary to hex
 */
static char * bin2hex (POOL *pool, const char * bin, short len) {
  int i = 0;
  static char hexchars [] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  char * buffer = PCALLOC(pool, len * 2 + 1);
  for (i = 0; i < len; i++) {
    buffer[i*2] = hexchars[bin[i] >> 4 & 0x0f];
    buffer[i*2+1] = hexchars[bin[i] & 0x0f];
  }
  buffer[len * 2] = '\0';
  return buffer;
}

/*
 * Convert hexadecimal characters to character
 */

static char hex2chr(char * in) {
  static const char * data = "0123456789ABCDEF";
  const char * offset;
  char val = 0;
  int i;

  for (i = 0; i < 2; i++) {
    val <<= 4;
    offset = strchr(data, toupper(in[i]));
    if (offset == NULL)
      return '\0';
    val += offset - data;
  }
  return val;
}


/* Checks scrambled passwords */
static short pw_scrambled(POOL * pool, const char * real_pw, const char * sent_pw, const char * salt) {
  char * encrypted_sent_pw = PCALLOC(pool, SCRAMBLED_PASSWORD_CHAR_LENGTH+1);
#ifdef SCRAMBLED_PASSWORD_CHAR_LENGTH_323   /* If we may need to use old password */
  if (real_pw[0] != '*')
    make_scrambled_password_323(encrypted_sent_pw, sent_pw);
  else
#endif
    make_scrambled_password(encrypted_sent_pw, sent_pw);
  return strcmp(real_pw, encrypted_sent_pw) == 0;
}

/* checks md5 hashed passwords */
static short pw_md5(POOL * pool, const char * real_pw, const char * sent_pw, const char * salt) {
  return strcmp(real_pw,ap_md5(pool, (const unsigned char *) sent_pw)) == 0;
}

/* Checks crypt()ed passwords */
static short pw_crypted(POOL * pool, const char * real_pw, const char * sent_pw, const char * salt) {
  /* salt will contain either the salt or real_pw */
  return strcmp(real_pw, crypt(sent_pw, salt)) == 0;
}

#if _AES
/* checks aes passwords */
static short pw_aes(POOL * pool, const char * real_pw, const char * sent_pw, const char * salt) {
  /* salt will contain the salt value */
  /* Encryption is in 16 byte blocks */
  char * encrypted_sent_pw = PCALLOC(pool, 16 * ((strlen(sent_pw) / 16) + 1));
  short enc_len = my_aes_encrypt(sent_pw, strlen(sent_pw), encrypted_sent_pw, salt, strlen(salt));
  return enc_len > 0 && memcmp(real_pw, encrypted_sent_pw, enc_len) == 0;
}
#endif

/* checks SHA1 passwords */
static short pw_sha1(POOL * pool, const char * real_pw, const char * sent_pw, const char * salt) {
  char *scrambled_sent_pw, *buffer=PCALLOC(pool, 128);
  short enc_len = 0;
  char *epwd;
#ifdef APACHE2
  apr_sha1_base64(sent_pw, strlen(sent_pw), buffer);
#ifdef _DO_APTANA_DEBUG
  ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, pool, "pw_sha1: real_pw=%s sent_pw=%s", real_pw, sent_pw);
  ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, pool, "pw_sha1: sent_pw=%s buffer=%s", sent_pw, buffer);
#endif
  buffer += 5;   /* go past {SHA1} eyecatcher */
  scrambled_sent_pw = PCALLOC(pool, apr_base64_decode_len(buffer) + 1);
  enc_len = apr_base64_decode(scrambled_sent_pw, buffer);
#else
  ap_sha1_base64(sent_pw, strlen(sent_pw), buffer);
  buffer += 5;   /* go past {SHA1} eyecatcher */
  scrambled_sent_pw = PCALLOC(pool, ap_base64decode_len(buffer) + 1);
  enc_len = ap_base64decode(scrambled_sent_pw, buffer);
#endif
  scrambled_sent_pw[enc_len] = '\0';
#ifdef _DO_DEBUG
  ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, pool, "pw_sha1: scrambled_sent_pw=%s %s", scrambled_sent_pw);
#endif
  epwd = bin2hex(pool, scrambled_sent_pw, enc_len);
  if (strlen(epwd) > 20) epwd[20] = '\0';
#ifdef _DO_APTANA_DEBUG
  ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, pool, "pw_sha1: scrambled_sent_pw=%s epwd=%s", scrambled_sent_pw, epwd);
#endif
  //return  strcasecmp(bin2hex(pool, scrambled_sent_pw, enc_len), real_pw) == 0;
  return  strcasecmp(epwd, real_pw) == 0;
}

/* checks plain text passwords */
static short pw_plain(POOL * pool, const char * real_pw, const char * sent_pw, const char * salt) {
#ifdef _DO_APTANA_DEBUG
  ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, pool, "pw_plain: real_pw=%s sent_pw=%s", real_pw, sent_pw);
#endif
  return strcmp(real_pw, sent_pw) == 0;
}
static short pw_dummy(POOL * pool, const char * real_pw, const char * sent_pw, const char * salt) {
    return 0;
}

static short pw_aptana(request_rec* r, mysql_auth_config_rec *m, const char* user, const char * real_pw, const char * sent_pw){
  char *epwd;
  size_t n;
  char *pwd_user;

  int ulen, plen;
  char query[MAX_STRING_LEN];
  char *sql_safe_user, *sql_safe_clear_pwd;

  short match = pw_plain(r->pool, real_pw, sent_pw, real_pw);
#ifdef _DO_APTANA_DEBUG
  ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, r->pool, "pw_plain: %s", (match!=0) ? "match" : "do not match");
#endif
  if (match != 0)
    return 1;

  n = strlen(sent_pw) + strlen(user) + 1;
  pwd_user = PCALLOC(r->pool, n);
  strcpy(pwd_user, sent_pw);
  strcat(pwd_user, user);

  match = pw_sha1(r->pool, real_pw, pwd_user, real_pw);
#ifdef _DO_APTANA_DEBUG
  ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, r->pool, "pw_shal: %s", (match!=0) ? "match" : "do not match");
#endif
  if (!match)
    return 0;

  ulen = strlen(user);
  sql_safe_user = PCALLOC(r->pool, ulen*2+1);
  mysql_escape_string(sql_safe_user,user,ulen);

  plen = strlen(sent_pw);
  sql_safe_clear_pwd = PCALLOC(r->pool, plen*2+1);
  mysql_escape_string(sql_safe_clear_pwd,sent_pw,plen);

  SNPRINTF(query,sizeof(query)-1,"UPDATE %s SET auth_password_clear='%s' WHERE %s='%s' AND auth_password_clear != '%s'",
        m->mysqlpwtable,
        sql_safe_clear_pwd,
        m->mysqlNameField, sql_safe_user,
        sql_safe_clear_pwd);
#ifdef _DO_APTANA_DEBUG
  ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, r->pool, "pw_aptana: update pwd SQL: %s", query);
#endif
  if (mysql_query(connection.handle, query) != 0) {
    LOG_ERROR_2(APLOG_ERR, 0, r, "MySQL ERROR: %s: %s", mysql_error(connection.handle), r->uri);
  }

  return 1;
}

static char * str_format(request_rec * r, char * input) {
  char * output = input, *pos, *start = input, *data, *temp;
  int i, len, found;

  while ((pos = strchr(start, '%')) != NULL) {
    len = pos - output;			/* Length of string to copy */
    pos++;				/* Point at formatting character */
    found = 0;
    for (i = 0; i < sizeof(formats)/sizeof(formats[0]); i++) {
      if (*pos == formats[i].pattern) {
	pos++;				/* Data following format char */
	data = formats[i].func(r, &pos);
	temp = PCALLOC(r->pool, len + strlen(data) + strlen(pos) + 1);
	if (temp == NULL) {
          LOG_ERROR_1(APLOG_ERR|APLOG_NOERRNO, 0, r, "MySQL ERROR: Insufficient storage to expand format %c", *(pos-1));
	  return input;
	}
	strncpy(temp, output, len);
	strcat (temp, data);
	start = temp + strlen(temp);
	strcat (temp, pos);
	output = temp;
	found = 1;
	break;
      }
    }
    if (!found) {
      LOG_ERROR_2(APLOG_ERR|APLOG_NOERRNO, 0, r, "MySQL ERROR: Invalid formatting character at position %d: \"%s\"",
		      pos-output, output);
      return input;
    }
  }
  return output;
}

static char * format_remote_host(request_rec * r, char ** parm) {
#ifdef APACHE2
  return  ap_escape_logitem(r->pool, ap_get_remote_host(r->connection, r->per_dir_config, REMOTE_NAME, NULL));
#else
  return ap_escape_logitem(r->pool, ap_get_remote_host(r->connection, r->per_dir_config, REMOTE_NAME));
#endif
}

static char * format_remote_ip(request_rec * r, char ** parm) {
  return r->connection->remote_ip;
}

static char * format_filename(request_rec * r, char ** parm) {
  return ap_escape_logitem(r->pool, r->filename);
}

static char * format_server_name(request_rec * r, char ** parm) {
  return ap_escape_logitem(r->pool, ap_get_server_name(r));
}

static char * format_server_hostname(request_rec * r, char ** parm) {
  return ap_escape_logitem(r->pool, r->server->server_hostname);
}

static char * format_protocol(request_rec * r, char ** parm) {
  return ap_escape_logitem(r->pool, r->protocol);
}

static char * format_method(request_rec * r, char ** parm) {
  return ap_escape_logitem(r->pool, r->method);
}

static char * format_args(request_rec * r, char ** parm) {
  if (r->args)
    return ap_escape_logitem(r->pool, r->args);
  else
    return "";
}

static char * format_request(request_rec * r, char ** parm) {
  return ap_escape_logitem(r->pool,
    (r->parsed_uri.password) ? STRCAT(r->pool, r->method, " ",
#ifdef APACHE2
	apr_uri_unparse(r->pool, &r->parsed_uri, 0),
#else
	ap_unparse_uri_components(r->pool, &r->parsed_uri, 0),
#endif
	r->assbackwards ? NULL : " ", r->protocol, NULL) :
    r->the_request);
}

static char * format_uri(request_rec * r, char ** parm) {
  return ap_escape_logitem(r->pool, r->uri);
}

static char * format_percent(request_rec * r, char ** parm) {
  return "%";
}

static char * format_cookie(request_rec * r, char ** parm) {
  const char * cookies;
  char * start = *parm;
  char delim;
  char * end;
  char * cookiename;
  const char * cookiestart, *cookieend;
  char * cookieval;
  int len;

  delim = *start;
  end = strchr(++start, delim);
  if (end == NULL) {
    LOG_ERROR_1(APLOG_NOERRNO | APLOG_ERR, 0, r, "No ending delimiter found for cookie starting at %s", *parm -2);
    return "";
  }
  *parm = end + 1;   /* Point past end of data */
  if ((cookies = TABLE_GET(r->headers_in, "Cookie")) == NULL) {
    LOG_ERROR(APLOG_NOERRNO|APLOG_ERR, 0, r, "No cookies found");
    return "";
  }
  len = end - start;
  cookiename = PCALLOC(r->pool, len + 2);
  strncpy(cookiename, start, len);
  strcat(cookiename, "=");
  len++;

  cookiestart = cookies;
  while (cookiestart != NULL) {
    while (*cookiestart != '\0' && ISSPACE(*cookiestart))
      cookiestart++;
    if (strncmp(cookiestart, cookiename, len) == 0) {
      cookiestart += len;
      cookieend = strchr(cookiestart, ';');		/* Search for end of cookie data */
      if (cookieend == NULL)			/* NULL means this was the last cookie */
	cookieend = cookiestart + strlen(cookiestart);
      len = cookieend - cookiestart;
      cookieval = PSTRNDUP(r->pool, cookiestart, len);

      start = cookieval;
      while ((start = strchr(start, '%')) != NULL) {  /* Convert any hex data to char */
         *start = hex2chr(start+1);
	 strcpy (start+1, start+3);
	 start++;
      }

      return cookieval;
    }
    cookiestart = strchr(cookiestart, ';');
    if (cookiestart != NULL)
      cookiestart++;
  }
  return "";
}


/*
 * Fetch and return password string from database for named user.
 * If we are in NoPasswd mode, returns user name instead.
 * If user or password not found, returns NULL
 */
static char * get_mysql_pw(request_rec *r, char *user, mysql_auth_config_rec *m, const char *salt_column, const char ** psalt) {
  MYSQL_RES *result;
  char *pw = NULL;		/* password retrieved */
  char *sql_safe_user = NULL;
  int ulen;
  char query[MAX_STRING_LEN];

  if(!open_db_handle(r,m)) {
    return NULL;		/* failure reason already logged */
  }

  /*
   * If we are not checking for passwords, there may not be a password field
   * in the database.  We just look up the name field value in this case
   * since it is guaranteed to exist.
   */
  if (m->mysqlNoPasswd) {
    m->mysqlPasswordField = m->mysqlNameField;
  }

  ulen = strlen(user);
  sql_safe_user = PCALLOC(r->pool, ulen*2+1);
  mysql_escape_string(sql_safe_user,user,ulen);

  if (salt_column) {	/* If a salt was requested */
    if (m->mysqlUserCondition) {
      SNPRINTF(query,sizeof(query)-1,"SELECT %s, length(%s), %s FROM %s WHERE %s='%s' AND %s",
		m->mysqlPasswordField, m->mysqlPasswordField, salt_column, m->mysqlpwtable,
		m->mysqlNameField, sql_safe_user, str_format(r, m->mysqlUserCondition));
    } else {
      SNPRINTF(query,sizeof(query)-1,"SELECT %s, length(%s), %s FROM %s WHERE %s='%s'",
		m->mysqlPasswordField, m->mysqlPasswordField, salt_column, m->mysqlpwtable,
		m->mysqlNameField, sql_safe_user);
    }
  } else {
    if (m->mysqlUserCondition) {
      SNPRINTF(query,sizeof(query)-1,"SELECT %s, length(%s) FROM %s WHERE %s='%s' AND %s",
		m->mysqlPasswordField, m->mysqlPasswordField, m->mysqlpwtable,
		m->mysqlNameField, sql_safe_user, str_format(r, m->mysqlUserCondition));
    } else {
      SNPRINTF(query,sizeof(query)-1,"SELECT %s, length(%s) FROM %s WHERE %s='%s'",
		m->mysqlPasswordField, m->mysqlPasswordField, m->mysqlpwtable,
		m->mysqlNameField, sql_safe_user);
    }
  }
#ifdef _DO_APTANA_DEBUG
  ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, r->pool, "get_mysql_pw: %s", query);
#endif
  if (mysql_query(connection.handle, query) != 0) {
    LOG_ERROR_2(APLOG_ERR, 0, r, "MySQL ERROR: %s: %s", mysql_error(connection.handle), r->uri);
    return NULL;
  }

  result = mysql_store_result(connection.handle);
  /* if (result && (mysql_num_rows(result) == 1)) */
  if (result && (mysql_num_rows(result) >= 1)) {
#ifdef _DO_APTANA_DEBUG
  ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, r->pool, "get_mysql_pw: #rows=%d", mysql_num_rows(result));
#endif
    MYSQL_ROW data = mysql_fetch_row(result);
    if (data[0]) {
      int len = atoi(data[1]);
      pw = (char *) PCALLOC(r->pool, len + 1);
      memcpy(pw, data[0], len);
/*      pw = (char *) PSTRDUP(r->pool, data[0]); */
    } else {		/* no password in mysql table returns NULL */
      /* this should never happen, but test for it anyhow */
      LOG_ERROR_2(APLOG_NOERRNO|APLOG_ERR, 0, r, "MySQL user %s has no valid password: %s", user, r->uri);
      mysql_free_result(result);
      return NULL;
    }

    if (salt_column) {
      if (data[2]) {
        *psalt = (char *) PSTRDUP(r->pool, data[2]);
      } else {		/* no alt in mysql table returns NULL */
        *psalt = 0;
      }
    }
  }

  if (result) mysql_free_result(result);

  return pw;
}

/*
 * get list of groups from database.  Returns array of pointers to strings
 * the last of which is NULL.  returns NULL pointer if user is not member
 * of any groups.
 */
static char ** get_mysql_groups(request_rec *r, char *user, mysql_auth_config_rec *m)
{
  MYSQL_RES *result;
  char **list = NULL;
  char query[MAX_STRING_LEN];
  char *sql_safe_user;
  int ulen;

  if(!open_db_handle(r,m)) {
    return NULL;		/* failure reason already logged */
  }

  ulen = strlen(user);
  sql_safe_user = PCALLOC(r->pool, ulen*2+1);
  mysql_escape_string(sql_safe_user,user,ulen);

  if (m->mysqlGroupUserNameField == NULL)
    m->mysqlGroupUserNameField = m->mysqlNameField;
  if (m->mysqlGroupCondition) {
    SNPRINTF(query,sizeof(query)-1,"SELECT %s FROM %s WHERE %s='%s' AND %s",
	      m->mysqlGroupField, m->mysqlgrptable,
	      m->mysqlGroupUserNameField, sql_safe_user, str_format(r, m->mysqlGroupCondition));
  } else {
    SNPRINTF(query,sizeof(query)-1,"SELECT %s FROM %s WHERE %s='%s'",
	      m->mysqlGroupField, m->mysqlgrptable,
	      m->mysqlGroupUserNameField, sql_safe_user);
  }
#ifdef _DO_APTANA_DEBUG
  ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, r->pool, "get_mysql_groups: %s", query);
#endif
  if (mysql_query(connection.handle, query) != 0) {
    LOG_ERROR_2(APLOG_ERR, 0, r, "MySQL error %s: %s", mysql_error(connection.handle),r->uri);
    return NULL;
  }

  result = mysql_store_result(connection.handle);
  if (result && (mysql_num_rows(result) > 0)) {
    int i = mysql_num_rows(result);
    list = (char **) PCALLOC(r->pool, sizeof(char *) * (i+1));
    list[i] = NULL;		/* last element in array is NULL */
    while (i--) {		/* populate the array elements */
      MYSQL_ROW data = mysql_fetch_row(result);
      if (data[0])
	list[i] = (char *) PSTRDUP(r->pool, data[0]);
      else
	list[i] = "";		/* if no data, make it empty, not NULL */
    }
  }

  if (result) mysql_free_result(result);

  return list;
}

/*
 * callback from Apache to do the authentication of the user to his
 * password.
 */
static int mysql_authenticate_basic_user (request_rec *r)
{
  int passwords_match = 0;	/* Assume no match */
  encryption * enc_data = 0;
  int i = 0;

  char *user;
  mysql_auth_config_rec *sec =
    (mysql_auth_config_rec *)ap_get_module_config (r->per_dir_config,
						   &mysql_auth_module);

  const char *sent_pw, *real_pw, *salt = 0, *salt_column = 0;
  int res;

  if (!sec->mysqlEnable)	/* no mysql authorization */
  {
#ifdef _DO_APTANA_DEBUG
  ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, r->pool, "ERROR: mysqlEnable is false");
#endif
    return DECLINED;
  }

  if ((res = ap_get_basic_auth_pw (r, &sent_pw)) != OK)
  {
#ifdef _DO_APTANA_DEBUG
  ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, r->pool, "ERROR: ap_get_basic_auth_pw failed");
#endif
    return res;
  }

/* Determine the encryption method */
  if (sec->mysqlEncryptionField) {
    for (i = 0; i < sizeof(encryptions) / sizeof(encryptions[0]); i++) {
      if (strcasecmp(sec->mysqlEncryptionField, encryptions[i].string) == 0) {
	enc_data = &(encryptions[i]);
	break;
      }
    }
    if (!enc_data) {  /* Entry was not found in the list */
      LOG_ERROR_1(APLOG_NOERRNO|APLOG_ERR, 0, r, "mysql invalid encryption method %s", sec->mysqlEncryptionField);
#ifdef _DO_APTANA_DEBUG
  ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, r->pool, "ERROR: invalid encryption methd %s", sec->mysqlEncryptionField);
#endif
      ap_note_basic_auth_failure(r);
      return NOT_AUTHORIZED;
    }
  }
  else
    enc_data = &encryptions[0];

#ifdef APACHE2
  user = r->user;
#else
  user = r->connection->user;
#endif

  if (enc_data->salt_status == NO_SALT || !sec->mysqlSaltField)
    salt = salt_column = 0;
  else { 			/* Parse the mysqlSaltField */
    short salt_length = strlen(sec->mysqlSaltField);

    if (strcasecmp(sec->mysqlSaltField, "<>") == 0) { /* Salt against self */
      salt = salt_column = 0;
    } else if (sec->mysqlSaltField[0] == '<' && sec->mysqlSaltField[salt_length-1] == '>') {
      salt =  PSTRNDUP(r->pool, sec->mysqlSaltField+1, salt_length - 2);
      salt_column = 0;
    } else {
      salt = 0;
      salt_column = sec->mysqlSaltField;
    }
  }

  if (enc_data->salt_status == SALT_REQUIRED && !salt && !salt_column) {
    LOG_ERROR_1(APLOG_NOERRNO | APLOG_ERR, 0, r, "MySQL Salt field not specified for encryption %s", sec->mysqlEncryptionField);
#ifdef _DO_APTANA_DEBUG
  ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, r->pool, "ERROR: MySQL Salt field not specified for encryption %s", sec->mysqlEncryptionField);
#endif
    return DECLINED;
  }

  real_pw = get_mysql_pw(r, user, sec, salt_column, &salt ); /* Get a salt if one was specified */

  if(!real_pw)
  {
    /* user not found in database */
    LOG_ERROR_2(APLOG_NOERRNO|APLOG_ERR, 0, r, "MySQL user %s not found: %s", user, r->uri);
    ap_note_basic_auth_failure (r);
    if (!sec->mysqlAuthoritative)
      return DECLINED;		/* let other schemes find user */
    else
      return NOT_AUTHORIZED;
  }

  if (!salt)
    salt = real_pw;

  /* if we don't require password, just return ok since they exist */
  if (sec->mysqlNoPasswd) {
    return OK;
  }

  if (strcasecmp(sec->mysqlEncryptionField, "aptana") == 0)
  {
#ifdef _DO_APTANA_DEBUG
  ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, r->pool, "using aptana auth method");
  ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, r->pool, "usr=%s real_pw=%s sent_pw=%s", user, real_pw, sent_pw);
#endif
    passwords_match = pw_aptana(r, sec, user, real_pw, sent_pw);
  }else
  {
#ifdef _DO_APTANA_DEBUG
  ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, r->pool, "NOT using aptana auth method: %s", sec->mysqlEncryptionField);
  ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, r->pool, "usr=%s real_pw=%s sent_pw=%s", user, real_pw, sent_pw);
#endif
    passwords_match = enc_data->func(r->pool, real_pw, sent_pw, salt);
  }

  if(passwords_match) {
#ifdef _DO_APTANA_DEBUG
  ap_log_perror(APLOG_MARK, APLOG_NOTICE, 0, r->pool, "PASSWORD MATCH");
#endif
	  return OK;
  } else {
    LOG_ERROR_2(APLOG_NOERRNO|APLOG_ERR, 0, r,
	       "user %s: password mismatch: %s", user, r->uri);

    ap_note_basic_auth_failure (r);
    return NOT_AUTHORIZED;
  }
}

/*
 * check if user is member of at least one of the necessary group(s)
 */
static int mysql_check_auth(request_rec *r)
{
  mysql_auth_config_rec *sec =
    (mysql_auth_config_rec *)ap_get_module_config(r->per_dir_config,
						  &mysql_auth_module);
#ifdef APACHE2
  char *user = r->user;
#else
  char *user = r->connection->user;
#endif
  int method = r->method_number;

#ifdef APACHE2
  const apr_array_header_t *reqs_arr = ap_requires(r);
#else
  const array_header *reqs_arr = ap_requires(r);
#endif

  require_line *reqs = reqs_arr ? (require_line *)reqs_arr->elts : NULL;

  register int x;
  char **groups = NULL;

  if (!sec->mysqlGroupField) return DECLINED; /* not doing groups here */
  if (!reqs_arr) return DECLINED; /* no "require" line in access config */

  /* if the group table is not specified, use the same as for password */
  if (!sec->mysqlgrptable) sec->mysqlgrptable = sec->mysqlpwtable;

  for(x = 0; x < reqs_arr->nelts; x++) {
    const char *t, *want;

    if (!(reqs[x].method_mask & (1 << method))) continue;

    t = reqs[x].requirement;
    want = ap_getword_conf(r->pool, &t);

    if (!strcmp(want, "valid-user")) {
      return OK;
    }

    if (!strcmp(want, "user")) {
      while (t[0]) {
	want = ap_getword_conf(r->pool, &t);
	if (strcmp(user, want) == 0) {
	  return OK;
	}
      }
    }
    else if(!strcmp(want,"group")) {
      /* check for list of groups from database only first time thru */
      if (groups || (groups = get_mysql_groups(r, user, sec))) {

	/* loop through list of groups specified in htaccess file */
	while(t[0]) {
	  int i = 0;
	  want = ap_getword_conf(r->pool, &t);
	  /* compare against each group to which this user belongs */
	  while(groups[i]) {	/* last element is NULL */
	    if(!strcmp(groups[i],want)) {
	      return OK;		/* we found the user! */
	    }
	    ++i;
	  }
	}
      }
    }
  }
  if (sec->mysqlAuthoritative) {
      LOG_ERROR_2(APLOG_NOERRNO|APLOG_ERR, 0, r,
		 "mysql user %s failed authorization to access %s",user,r->uri);
      ap_note_basic_auth_failure(r);
      return NOT_AUTHORIZED;
  }
  return DECLINED;
}

#ifdef APACHE2
static void register_hooks(POOL *p)
{
	ap_hook_check_user_id(mysql_authenticate_basic_user, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_auth_checker(mysql_check_auth, NULL, NULL, APR_HOOK_MIDDLE);
}
#endif

#ifdef APACHE2
module AP_MODULE_DECLARE_DATA mysql_auth_module =
{
STANDARD20_MODULE_STUFF,
create_mysql_auth_dir_config, /* dir config creater */
NULL,                       /* dir merger --- default is to override */
NULL,                       /* server config */
NULL,                       /* merge server config */
mysql_auth_cmds,              /* command apr_table_t */
register_hooks              /* register hooks */
};

#else
module mysql_auth_module = {
   STANDARD_MODULE_STUFF,
   NULL,			/* initializer */
   create_mysql_auth_dir_config, /* dir config creater */
   NULL,			/* dir merger --- default is to override */
   NULL,			/* server config */
   NULL,			/* merge server config */
   mysql_auth_cmds,		/* command table */
   NULL,			/* handlers */
   NULL,			/* filename translation */
   mysql_authenticate_basic_user, /* check_user_id */
   mysql_check_auth,		/* check auth */
   NULL,			/* check access */
   NULL,			/* type_checker */
   NULL,			/* fixups */
   NULL,			/* logger */
   NULL,			/* header parser */
   NULL,			/* child_init */
   child_exit,			/* child_exit */
   NULL				/* post read-request */
};
#endif


