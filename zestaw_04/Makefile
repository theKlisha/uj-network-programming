Q		:= @
CC		:= g++
LD		:= g++

CCFLAGS		+= -Wall
CCFLAGS		+= -O2
CCFLAGS		+= -std=c++17
CCFLAGS		+= -pedantic

all: udp_server.x

%.x: %.o
	@printf "  LD\t$^\n"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.cpp
	@printf "  CC\t$<\n"
	$(Q)$(CC) $(CCFLAGS) -c $< -o $@ 

clean:
	@printf "  CLEAN\n"
	$(Q)$(RM) *.o *.x *.a *.so

.PHONY: all clean
