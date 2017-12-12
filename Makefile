
# outdir := out

# SRCS := wire.c

# TEST_SRC := test.c

TASKNAME := test
OBJS := wire.o
CFLAGS := -g -O3 -I.
CC := c99

$(TASKNAME): $(OBJS)

# .PHONY: all
# all: $(outdir) $(SRCS) $(TEST_SRC)
# 	c99 $(CFLAGS) -o $(outdir)/$(TASKNAME) $(SRCS) $(TEST_SRC)

# .PHONY: clean
# clean:
# 	rm -fR $(outdir)/*

.PHONY: clean
clean:
	rm -fR $(TASKNAME) test.dSYM *.o

# $(TEST_SRC): $(SRCS)

# $(outdir):
# 	mkdir -p $(outdir)
