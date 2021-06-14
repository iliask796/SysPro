# the compiler: 
	CC = g++
 
# compiler flags:
	#  -g     - this flag adds debugging information to the executable file
	# -pthread - for threading consumers
  	CFLAGS  = -g -pthread

# The build targets 
	TARGET1 = travelMonitorClient
	TARGET2 = monitorServer

all: $(TARGET1) $(TARGET2)

$(TARGET1): mainClient.o CitizenRecords.o HashUtil.o BloomFilter.o InputHandler.o TravelRecords.o
	 $(CC) $(CFLAGS) -o travelMonitorClient mainClient.o CitizenRecords.o HashUtil.o BloomFilter.o InputHandler.o TravelRecords.o

mainClient.o: mainClient.cpp CitizenRecords.h BloomFilter.h TravelRecords.h
	$(CC) $(CFLAGS) -c mainClient.cpp

$(TARGET2): Server.o CitizenRecords.o HashUtil.o BloomFilter.o SkipList.o InputHandler.o TravelRecords.o
	$(CC) $(CFLAGS) -o monitorServer Server.o CitizenRecords.o HashUtil.o BloomFilter.o SkipList.o InputHandler.o TravelRecords.o

Server.o: Server.cpp BloomFilter.h SkipList.h
	$(CC) $(CFLAGS) -c Server.cpp

CitizenRecords.o: CitizenRecords.h

HashUtil.o: HashUtil.h

BloomFilter.o: BloomFilter.h

InputHandler.o: InputHandler.h

TravelRecords.o: TravelRecords.h

SkipList.o: SkipList.h

clean:
	$(RM) $(TARGET1) 
	$(RM) $(TARGET2) 
	$(RM) mainClient.o
	$(RM) Server.o
	$(RM) CitizenRecords.o
	$(RM) HashUtil.o
	$(RM) BloomFilter.o
	$(RM) InputHandler.o
	$(RM) TravelRecords.o
	$(RM) SkipList.o
