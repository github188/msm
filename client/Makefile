CFLAGS += -Wall -O2
LDFLAGS += -ljsoncpp -lpthread -lzmq
STRIP = strip
ifdef debug
	STRIP =
	CFLAGS = -Wall -g -DDEBUG
endif
CPPFLAGS = $(CFLAGS)
CC = g++

SRC = $(wildcard *.cpp)
SRC += $(wildcard xml/*.cpp)
OBJECTS = $(SRC:%.cpp=%.o)
DEP = $(SRC:%.cpp=%.d)
TARGETS = client

all : $(TARGETS)
$(TARGETS) : $(OBJECTS)
	$(CC)  $(CPPFLAGS) -o $@ $^  $(LDFLAGS)
ifneq ($(STRIP),)
	$(STRIP) $@
endif

%.d : %.cpp
	@set -e; rm -rf $@; \
	$(CC) -MM  $< > $@.$$$$; \
	_basename=$(SHELL basename $*);\
	sed 's,\(${_basename}\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $;$@;	\
	rm -rf $@.$$$$
# sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $;$@; \



-include $(DEP)

.PHONY: clean
clean:
	@echo "cleanning..."
	@rm -rf $(OBJECTS) $(TARGETS) $(DEP) *.d.*
