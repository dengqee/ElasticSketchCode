GCC = g++
CFLAGS = -g -std=c++14 
SSEFLAGS = -msse2 -mssse3 -msse4.1 -msse4.2 -mavx -march=native
FILES = cm-throughput.out ss-throughput.out es-throughput.out c-throughput.out cu-throughput.out

all: $(FILES)

cm-throughput.out: cmsketch-throughput.cpp
	$(GCC) $(CFLAGS) -o cm-throughput.out cmsketch-throughput.cpp

ss-throughput.out: spacesaving-throughput.cpp
	$(GCC) $(CFLAGS) -o ss-throughput.out spacesaving-throughput.cpp

es-throughput.out: elastic-throughput.cpp
	$(GCC) $(CFLAGS) $(SSEFLAGS) -o es-throughput.out elastic-throughput.cpp

c-throughput.out: countsketch-throughput.out
	$(GCC) $(CFLAGS) -o c-throughput.out countsketch-throughput.cpp

cu-throughput.out: cu-throughput.out
	$(GCC) $(CFLAGS) -o cu-throughput.out cu-throughput.cpp

clean:
	rm $(all) -f *~ *.o *.out


