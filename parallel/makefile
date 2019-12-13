CC = mpicc
CFLAGS =  
LDFLAGS =
OBJFILES = gol.o 
TARGET = part_1_parallel.out
all: $(TARGET)
$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET) *~