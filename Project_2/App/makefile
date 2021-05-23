# the compiler: 
	CC = g++
 
# compiler flags:
	#  -g     - this flag adds debugging information to the executable file
  	CFLAGS  = -g
 
# The build targets 
	TARGET1 = travelMonitor
	TARGET2 = Monitor

all: $(TARGET1) $(TARGET2)

$(TARGET1): main.o CitizenRecords.o HashUtil.o BloomFilter.o InputHandler.o TravelRecords.o
	 $(CC) $(CFLAGS) -o travelMonitor main.o CitizenRecords.o HashUtil.o BloomFilter.o InputHandler.o TravelRecords.o

main.o: main.cpp CitizenRecords.h BloomFilter.h TravelRecords.h
	$(CC) $(CFLAGS) -c main.cpp

$(TARGET2): Monitor.o CitizenRecords.o HashUtil.o BloomFilter.o SkipList.o InputHandler.o TravelRecords.o
	$(CC) $(CFLAGS) -o Monitor Monitor.o CitizenRecords.o HashUtil.o BloomFilter.o SkipList.o InputHandler.o TravelRecords.o

Monitor.o: Monitor.cpp BloomFilter.h SkipList.h
	$(CC) $(CFLAGS) -c Monitor.cpp

CitizenRecords.o: CitizenRecords.h

HashUtil.o: HashUtil.h

BloomFilter.o: BloomFilter.h

InputHandler.o: InputHandler.h

TravelRecords.o: TravelRecords.h

SkipList.o: SkipList.h

clean:
	$(RM) $(TARGET1) 
	$(RM) $(TARGET2) 
