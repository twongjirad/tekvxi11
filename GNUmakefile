#SETTINGS
GXX = g++
VXIDIR=./vxi11_1.10
VXILIB=$(VXIDIR)/libvxi11_1.10.a
LOCAL_INC = -I./include
LOCAL_LIB = 
CXXFLAGS = -g -fPIC
LDFLAGS = -lstdc++
EXES = test_tkvscope run_synced_acq tkv_viewer ping_scope arduino_toggle check_sync
ENABLEROOT = 1
ENABLEUSB = 0

# BUILD VARIABLES (No hard-coded paths here)
CCSRC = $(wildcard src/*.cc)
COBJS = $(addprefix .obj/, $(notdir $(CCSRC:.cc=.o)))
CXXFLAGS += $(LOCAL_INC) -I$(VXIDIR)
EXESRC = $(addprefix exesrc/, $(addsuffix $(EXES),.cc))
EXEOBJ = $(addprefix .obj/,$(notdir $(EXESRC:.cc=.o)))
EXEBIN = $(addprefix bin/,$(EXES))
LOCAL_LIB += 
STATIC_LIB += $(VXILIB) 

# ROOT SETTINGS
ifeq ($(ENABLEROOT),1)
CXXFLAGS += -DROOTENABLED `root-config --cflags`
LOCAL_LIB += `root-config --libs --glibs`
#LDFLAGS += `root-config --ldflags`
endif

# LIBUSB SETTINGS
ifeq ($(ENABLEUSB),1)
EXES += list_usb usbcmd
CXXFLAGS += -DUSBENABLED
LOCAL_INC += `pkg-config --cflags libusb-1.0`
LOCAL_LIB += `pkg-config --libs libusb-1.0`
endif




.PHONY: lib clean all

all: $(EXEBIN)

lib: libtekvxi.so

libtekvxi.so: $(COBJS)
	echo "Making library"
	$(GXX) -shared $(LDFLAGS) -o $@ $^ $(LOCAL_LIB)
.obj/%.o: src/%.cc
	mkdir -p .obj
	$(GXX) -c $(CXXFLAGS) -o $@ $^
.obj/%.o: exesrc/%.cc
	mkdir -p .obj
	$(GXX) -c $(CXXFLAGS) -o $@ $^
bin/%: .obj/%.o libtekvxi.so
	mkdir -p bin
	$(GXX) $(LDFLAGS) $^ $(LOCAL_LIB) $(STATIC_LIB) -o $@

.DEFAULT: $(EXEBIN)

clean:
	rm -f .obj/*.o bin/* *.so

