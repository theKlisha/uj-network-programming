Q		:= @
CC		:= g++
LD		:= g++

CXXFLAGS	+= -Wall
CXXFLAGS	+= -O2
CXXFLAGS	+= -std=c++17
CXXFLAGS	+= -pedantic

all: server.x

server.x: main.o protocol.o net.o
	@printf "  LD\t$^\n"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.cpp %.hpp
	@printf "  CC\t$<\n"
	$(Q)$(CC) $(CXXFLAGS) -c $< -o $@ 

clean:
	@printf "  CLEAN\n"
	$(Q)$(RM) *.o *.x *.a *.so

.PHONY: all clean
