obj=main.o user_manager.o communication.o user_interface.o file_manager.o
CC=gcc
CFLAGS=-Wall -O0 

execfile=ipmsg

$(execfile):$(obj)
	$(CC) -o $@ $^ $(CFLAGS) -lpthread

%O:%C %h
	$(CC) -c -o $@ $^ $(CFLAGS)

clean:
	@rm $(obj) -rf $(execfile) -rf
