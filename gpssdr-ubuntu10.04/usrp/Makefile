CC	= g++
ASM = g++
LINK= g++

CINCPATHFLAGS = -I.

LDFLAGS	= -lpthread -lusrp

CFLAGS = -O2 -D_FORTIFY_SOURCE=0 $(CINCPATHFLAGS)

HEADERS =	db_dbs_rx.h 
OBJS =		db_dbs_rx.o gps-usrp.o
EXE =		gps-usrp

# All Target
all: $(EXE)

$(EXE): $(OBJS)
	 $(LINK) $(LDFLAGS) -o $@ $(OBJS)
	
%.o:%.cpp
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
	@rm -rvf *.dis
	@rm -rvf $(OBJS)
	@rm -rvf $(EXE)
