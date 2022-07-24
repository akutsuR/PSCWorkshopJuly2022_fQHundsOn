CXX=g++
LD=g++

CXXFLAGS += -Wall -std=c++11 -g $(shell root-config --cflags) -I$(WCSIMDIR)/include
LDFLAGS += $(shell root-config --ldflags) $(shell root-config --libs) -lTreePlayer -L$(WCSIMDIR) -l WCSimRoot

.PHONY: clean Execs

%o::%cc
	$(CXX) $(CXXFLAGS) -c $? -o $@


clean:
	$(RM) *.o
