# HodoDAQ makefile

#SETTINGS
GXX = g++
LOCAL_INC = -I./include -I$(HODODAQ_INCDIR) -I$(HODODAQ_HOME)/crol_api/include -I$(TEKVXI11_DIR)/include -I$(TEKVXI11_DIR)/vxi11_1.10
LOCAL_LIB = -L$(HODODAQ_LIBDIR) -lHodo 
LOCAL_LIB += -L$(TEKVXI11_DIR)/vxi11_1.10 -lvxi11_1.10
LOCAL_LIB += -L$(TEKVXI11_DIR) -ltekvxi 
LOCAL_LIB += -L$(TEKVXI11_DIR)/vxi11_1.10 -lvxi11_1.10
CXXFLAGS = -g -fPIC -DLINUX -DROOTENABLED
LDFLAGS = -fPIC -lstdc++
ENABLEROOT = 1
ENABLEUSB = 0
EXES = run_hodo_daq

# HODO OBJS (why not in library?)
HODO_TMP = $(HODODAQ_HOME)/tmp
HODO_OBJS = $(HODO_TMP)/crystal_overlord.o $(HODO_TMP)/okFrontPanelDLL.o 
HODO_OBJS += $(HODO_TMP)/stac_commands.o $(HODO_TMP)/svi_adc.o $(HODO_TMP)/voltbus_svi.o $(HODO_TMP)/xem_wrapper.o 
HODO_OBJS += $(HODO_TMP)/HVController.o $(HODO_TMP)/mvc_voltbus.o $(HODO_TMP)/Overlord.o 
HODO_OBJS += $(HODO_TMP)/stac_response.o $(HODO_TMP)/svi_dac.o $(HODO_TMP)/xem_carrier.o

# BUILD VARIABLES (No hard-coded paths here)
CCSRC = $(wildcard src/*.cxx)
COBJS = $(addprefix .obj/, $(notdir $(CCSRC:.cxx=.o)))
CXXFLAGS += $(LOCAL_INC) -I$(VXIDIR)
EXESRC = $(addprefix exesrc/, $(addsuffix $(EXES),.cxx))
EXEOBJ = $(addprefix .obj/,$(notdir $(EXESRC:.cxx=.o)))
EXEBIN = $(addprefix bin/,$(EXES))
LOCAL_LIB += -Wl,--no-as-needed -ldl
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

all: lib $(EXEBIN)

lib: libhodotek.so

libhodotek.so: $(COBJS)
	echo "Making library"
	$(GXX) -shared $(LDFLAGS) -o $@ $^ $(LOCAL_LIB)
.obj/%.o: src/%.cxx
	mkdir -p .obj
	$(GXX) -c $(CXXFLAGS) -o $@ $^
.obj/%.o: exesrc/%.cxx
	mkdir -p .obj
	$(GXX) -c $(CXXFLAGS) -o $@ $^
bin/%: .obj/%.o libhodotek.so
	mkdir -p bin
	$(GXX) $(LDFLAGS) -o $@ $^ $(LOCAL_LIB) $(STATIC_LIB)

.DEFAULT: $(EXEBIN)

clean:
	rm -f .obj/*.o bin/* *.so



