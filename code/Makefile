CFLAGS = -O2 -DNDEBUG
#CFLAGS = -pg -O5
#CFLAGS = -g -Wall -W
CPP = g++
OBJS = term.o sentence.o statistics.o function.o

all: extractRel
extractRel: ExtractOccurrence.o $(OBJS)
#	$(CPP) -o extractRel  $(CFLAGS) $(OBJS) main.o -lboost_iostreams -lboost_filesystem
	$(CPP) -o extractRel  $(CFLAGS) $(OBJS) ExtractOccurrence.o -lboost_iostreams -lboost_filesystem -lboost_system
#countRel: countRelation.o $(OBJS)
#	$(CPP) -o countRel $(CFLAGS) countRelation.o

clean:
	/bin/rm -r -f $(OBJS) extractRel *.o *~
	
.cpp.o:
	$(CPP) -c $(CFLAGS) $<
