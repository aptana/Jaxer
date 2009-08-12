mod_auth_mysql
    This is modified based on existing code for aptana_specific auth:  sent_pw is either clear text or sha1-encrypted.  We only use the 1st 20 bytes of the encrypted.  For clear text, we also update value in table if diff.

pw_sha1 is also modified to take only the 1st 20 bytes.

apache config

AuthMySQLPwEncryption aptana

is needed to use this

