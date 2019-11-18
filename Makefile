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
	c++ -std=c++11 -o $(TARGET) $(CFLAGS) $(OFILES)

clean:
	rm $(OFILES) $(TARGET)

%.o : %.cpp
	c++ -std=c++11 -o $@ -c $(CFLAGS) $<
