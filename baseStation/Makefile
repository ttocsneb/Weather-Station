CXX=ccache g++
TIME=/usr/bin/time --format="Build Duration: %E" 

# directories

# .h directory
IDIR = src
# .cpp directory
SRCDIR = src
# .o directory
ODIR=bin
LIB_INC=-I/usr/local/include
LIB=/usr/local/lib
LIBS=-lrf24 -lwiringPi -lmariadbclient -lmysqlcppconn -lsystemd

PROG_NAME=baseStation


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
INCLUDES = -I$(IDIR) $(LIB_INC) -L$(LIB) $(LIBS)


# Dependency files
_DEPS = main.h radio.h eprom.h weather.h commands.h sql.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# object files
_OBJ = main.o radio.o eprom.o weather.o commands.o sql.o
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
	ccache -C
