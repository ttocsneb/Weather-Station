CXX=g++

# directories

# .h directory
IDIR = src
# .cpp directory
SRCDIR = src
# .o directory
ODIR=bin
LIB_INC=/usr/local/include
LIB=/usr/local/lib

PROG_NAME=baseStation

all: $(PROG_NAME)

# Compile Flags
CFLAGS=-Wall
INCLUDES = -I$(IDIR) -I$(LIB_INC) -L$(LIB) -lrf24 -lwiringPi


# Dependency files
_DEPS = main.h radio.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# object files
_OBJ = main.o radio.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


# generate .o  files
$(ODIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CXX) $(CFLAGS) $(INCLUDES) -c -o $@ $< 

# compile the main program
$(PROG_NAME): $(OBJ)
	$(CXX) $(CFLAGS) $(INCLUDES) -o $@ $^ 


# Clear the binaries
.PHONY: clean
clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
