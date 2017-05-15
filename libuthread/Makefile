TARGET  := libuthread.a
OBJ 	:= queue.o sem.o tps.o
RMS 	:= queue.o sem.o tps.o *.a *.d


CC 	:= gcc
AR 	:= ar
CFLAGS  := -g -c
AFLAGS  := rcs
CLARGS  := -f $(RMS) 

ifneq ($(V),1)
Q = @
endif

all: $(OBJ) $(TARGET) 

deps := $(patsubst %.o,%.d,$(OBJ))
-include $(deps)
DEPFLAGS = -MMD -MF $(@:.o=.d)

libuthread.a: $(OBJ) thread.o
	@echo "CC $@"
	$(AR) $(AFLAGS) $(TARGET) $(OBJ) thread.o	
%.o: %.c 
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ $< $(DEPFLAGS)	
clean:
	rm $(CLARGS)

