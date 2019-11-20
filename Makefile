# COMPILER AND LIBRARY OPTIONS #################################################
# Change as appropriate


CC = gcc
CFLAGS = `pkg-config --cflags guile-2.2` -Wall -shared -fPIC
LFLAGS = -lhdf5
EXTENSIONDIR = `pkg-config --variable=extensiondir guile-2.2`
SITEDIR = `pkg-config --variable=sitedir guile-2.2`


# INSTALL ######################################################################
#


libh5q.so: read_hdf5.c
	$(CC) $(CFLAGS) -o libh5q.so read_hdf5.c $(LFLAGS)

.PHONY: install
install: libh5q.so
	cp libh5q.so $(EXTENSIONDIR)/; cp h5q.scm $(SITEDIR)/


# CLEAN ########################################################################
#


.PHONY: clean
clean:
	rm -f libh5q.so
