CC:=gcc 
CFLAGS:=-fPIC
FILES:=http_ext.c http_light.c http_server.c ioutil.c socketWrap.c str_map.c
OBJECTS:=$(subst .c,.o,$(FILES))
TARGET:=libhttp_server.so
LIBNAME:=libhttp_server.so.1
TARGETLIB:=libhttp_server.so.1.0.1

%.o:%.c
	$(CC) $(CFLAGS) -c  -o $@ $^
	
$(TARGET):$(OBJECTS)
	$(CC) $(CFLAGS) -shared -Wl,-soname,$(LIBNAME) -o $(TARGETLIB) $^
	
clean:
	rm $(TARGETLIB)
	rm *.o





