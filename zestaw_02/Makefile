Q		:= @
CC		:= gcc
LD		:= gcc

CCFLAGS		+= -Wall
CCFLAGS		+= -O2
CCFLAGS		+= -std=gnu99
CCFLAGS		+= -pedantic

all: tcp_server.x tcp_client.x udp_server.x udp_client.x

%.x: %.o
	@printf "  LD\t$^\n"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	@printf "  CC\t$<\n"
	$(Q)$(CC) $(CCFLAGS) -c $< -o $@ 

clean:
	@printf "  CLEAN\n"
	$(Q)$(RM) *.o *.x *.a *.so

.PHONY: all clean
