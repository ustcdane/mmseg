# Makefile for mmseg
# Daniel Wang
# 2016-01-09

CXX		:= g++
LD		:= g++
AR		:= ar crus
RM		:= rm -rf

SRCROOT	:= ../src
SRCDIRS	:= $(SRCROOT)/util \
		   $(SRCROOT)
INCS	:= $(addprefix -I, $(SRCDIRS))
VPATH   := $(SRCDIRS) 
SRCS	:= $(foreach DIR, $(VPATH), $(notdir $(wildcard $(DIR)/*.cpp))) \
		   $(wildcard *.cpp)

CPPFLAGS:= $(INCS) -Wall

CXXFLAGS:= -std=c++11 -g -O0 -fPIC -shared
LDFLAGS	:= $(CXXFLAGS)
LIBS	:=
BIN		:= libmmseg.so
OBJS    := $(SRCS:.cpp=.o)
DEPS	:= $(OBJS:.o=.d)

all : $(BIN) #cp

clean :
	$(RM) $(BIN) $(OBJS) $(DEPS)

$(BIN) : $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

include $(DEPS)

%.d : %.cpp
	$(CXX) -std=c++11 -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o \1.d : ,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$
#cp :
#	cp libmmseg.so /usr/local/lib
