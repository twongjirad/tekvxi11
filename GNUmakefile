#SETTINGS
GXX = g++
VXIDIR=/home/twongjirad/working/vxi11_1.10
VXILIB=$(VXIDIR)/libvxi11_1.10.a
LOCAL_INC = -I./include
CXXFLAGS = -g -fPIC
LDFLAGS = -lstdc++
EXES = test_tkvscope

# BUILD VARIABLES (No hard-coded paths here)
CCSRC = $(wildcard src/*.cc)
COBJS = $(addprefix .obj/, $(notdir $(CCSRC:.cc=.o)))
CXXFLAGS += $(LOCAL_INC) -I$(VXIDIR)
EXESRC = $(addprefix exesrc/, $(addsuffix $(EXES),.cc))
EXEOBJ = $(addprefix .obj/,$(notdir $(EXESRC:.cc=.o)))
EXEBIN = $(addprefix bin/,$(EXES))

.PHONY: lib clean all

all: $(EXEBIN)

lib: libtekvxi.so

libtekvxi.so: $(COBJS)
	echo "Making library"
	$(GXX) -shared $(LDFLAGS) -o $@ $^
.obj/%.o: src/%.cc
	mkdir -p .obj
	$(GXX) -c $(CXXFLAGS) -o $@ $^
.obj/%.o: exesrc/%.cc
	mkdir -p .obj
	$(GXX) -c $(CXXFLAGS) -o $@ $^
bin/%: .obj/%.o libtekvxi.so
	mkdir -p bin
	$(GXX) $(CXXFLAGS) -o $@ $^ $(VXILIB)

.DEFAULT: $(EXEBIN)

clean:
	rm -f .obj/*.o bin/*

