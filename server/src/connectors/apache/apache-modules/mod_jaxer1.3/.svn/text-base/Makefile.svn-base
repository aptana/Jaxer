
APXS = /home/rbb/apinst/bin/apxs
APR_CONFIG=/home/rbb/aprinst/bin/apr-1-config
APU_CONFIG=/home/rbb/apuinst/bin/apu-1-config
SRCS = mod_jaxer.c mod_jaxer_connection.c mod_jaxer_message.c \
       mod_jaxer_conf.c mod_jaxer_filter.c mod_jaxer_proc.c compat.c
APR_INCLUDES=`${APR_CONFIG} --includes`
APU_INCLUDES=`${APU_CONFIG} --includes`
APR_CFLAGS=`${APR_CONFIG} --cppflags --cflags`
APU_CFLAGS=`${APU_CONFIG} --cppflags --cflags`
APR_LIBS=`${APR_CONFIG} --link-ld --libs`
APU_LIBS=`${APU_CONFIG} --link-ld --libs`

all: mod_jaxer.so

clean:
	$(RM) -f *.so *.o

install: mod_jaxer.so
	$(APXS) -i mod_jaxer.so 

mod_jaxer.so: $(SRCS)
	$(APXS) ${APR_INCLUDES} ${APU_INCLUDES} ${APR_LIBS} ${APU_LIBS} -Wc,'${APR_CFLAGS} -DAPACHE1_3' -c $(SRCS)

