Ryan Bloom's mod_jaxer

Need APR,APR_UTILS, APACHE 13 to compile and install

The current issue is with apr_socket_connect.  Got a 111 (Connection refused error code).  It might be a mismatch/alignment/logic issue; some unrelated changes can sometimes make connection work.

There are a few debug msgs added.  The main logic for controlling handler/filter needs to be corrected.  Need to cleanup the connection.

The testbed is set on 10.10.1.123.

The Jaxer build is 3003.

Need to upgrade protocol after the current code is working.

