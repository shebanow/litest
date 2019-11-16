#
# @file Makefile
# @author m.shebanow
# @date 11/09/2019
# @brief Master makefile
#

HFILES := $(wildcard *.h)
CFILES := $(wildcard *.cpp)
OFILES := $(patsubst %.cpp,%.o,$(CFILES))
CFLAGS = -g
TARGET = litest
SRCS = $(HFILES) $(CFILES)

litest: $(OFILES)
	g++ -o $(TARGET) $(CFLAGS) $(OFILES)

clean:
	rm $(OFILES) $(TARGET)

docs:
	@doxygen
	@open doxygen/html/index.html

%.o : %.cpp
	g++ -o $@ -c $(CFLAGS) $<
