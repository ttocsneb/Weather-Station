CXX=ccache g++
TIME=/usr/bin/time --format="Build Duration: %E" 

# directories

# .h directory
IDIR = includes
# .cpp directory
SRCDIR = src
# .o directory
ODIR=bin
LIB_INC=-I/usr/local/include -I/usr/include/python3.5
LIB=-L/usr/local/lib -L/usr/lib
LIBS=-lrf24 -lwiringPi
LINKS=

PROG_NAME=basestation.so


all:
	$(TIME) make all-act

update:
	$(TIME) make update-act


all-act: $(PROG_NAME)

update-act: get $(PROG_NAME)

get:
	./getcoppermind.sh


# Compile Flags
CFLAGS=-Wall
INCLUDES = -I$(IDIR) $(LIB_INC) $(LIB) $(LIBS)


# Dependency files
_DEPS = main.h radio.h commands.h basestationmodule.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# object files
_OBJ = main.o radio.o commands.o basestationmodule.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


# generate .o  files
$(ODIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CXX) $(CFLAGS) $(INCLUDES) -fpic -c -o $@ $< 

# compile the main program
$(PROG_NAME): $(OBJ)
	$(CXX) -shared $(CFLAGS) $(INCLUDES) -o $@ $^ $(LINKS)


# Clear the binaries
.PHONY: clean
clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
	ccache -C
