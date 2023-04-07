#
# Makefile for module.
#

CROSS		?= arm-none-eabi-
NAME		:= music

#
# Load variables of flag.
#
ASFLAGS		:= -g -ggdb -Wall -O2
CFLAGS		:= -g -ggdb -Wall -O2
CXXFLAGS	:= -g -ggdb -Wall -O2
LDFLAGS		:= -T link.ld -nostdlib
ARFLAGS		:= -rcs
OCFLAGS		:= -v -O binary
ODFLAGS		:=
MCFLAGS		:= -mcpu=cortex-a8 -march=armv7-a

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=

#
# Add necessary directory for INCDIRS and SRCDIRS.
#
INCDIRS		+= include
SRCDIRS		+= source

#
# You shouldn't need to change anything below this point.
#
AS			:= $(CROSS)gcc -x assembler-with-cpp
CC			:= $(CROSS)gcc
CXX			:= $(CROSS)g++
LD			:= $(CROSS)ld
AR			:= $(CROSS)ar
OC			:= $(CROSS)objcopy
OD			:= $(CROSS)objdump
MKDIR		:= mkdir -p
CP			:= cp -af
RM			:= rm -fr
CD			:= cd
FIND		:= find
MKHEADER	:= mkv210

#
# X variables
#
X_ASFLAGS	:= $(MCFLAGS) $(ASFLAGS)
X_CFLAGS	:= $(MCFLAGS) $(CFLAGS)
X_CXXFLAGS	:= $(MCFLAGS) $(CXXFLAGS)
X_LDFLAGS	:= $(LDFLAGS)
X_OCFLAGS	:= $(OCFLAGS)
X_LIBDIRS	:= $(LIBDIRS)
X_LIBS		:= $(LIBS) -lgcc

X_OUT		:= output
X_NAME		:= $(patsubst %, $(X_OUT)/%, $(NAME))
X_INCDIRS	:= $(patsubst %, -I %, $(INCDIRS))
X_SRCDIRS	:= $(patsubst %, %, $(SRCDIRS))
X_OBJDIRS	:= $(patsubst %, .obj/%, $(X_SRCDIRS))

X_SFILES	:= $(foreach dir, $(X_SRCDIRS), $(wildcard $(dir)/*.S))
X_CFILES	:= $(foreach dir, $(X_SRCDIRS), $(wildcard $(dir)/*.c))
X_CPPFILES	:= $(foreach dir, $(X_SRCDIRS), $(wildcard $(dir)/*.cpp))

X_SDEPS		:= $(patsubst %, .obj/%, $(X_SFILES:.S=.o.d))
X_CDEPS		:= $(patsubst %, .obj/%, $(X_CFILES:.c=.o.d))
X_CPPDEPS	:= $(patsubst %, .obj/%, $(X_CPPFILES:.cpp=.o.d))
X_DEPS		:= $(X_SDEPS) $(X_CDEPS) $(X_CPPDEPS)

X_SOBJS		:= $(patsubst %, .obj/%, $(X_SFILES:.S=.o))
X_COBJS		:= $(patsubst %, .obj/%, $(X_CFILES:.c=.o))
X_CPPOBJS	:= $(patsubst %, .obj/%, $(X_CPPFILES:.cpp=.o)) 
X_OBJS		:= $(X_SOBJS) $(X_COBJS) $(X_CPPOBJS)

VPATH		:= $(X_OBJDIRS)

.PHONY:	all clean
all : $(X_NAME)

$(X_NAME) : $(X_OBJS)
	@echo [LD] Linking $@.elf
	@$(CC) $(X_LDFLAGS) $(X_LIBDIRS) -Wl,--cref,-Map=$@.map $^ -o $@.elf $(X_LIBS)
	@echo [OC] Objcopying $@.bin
	@$(OC) $(X_OCFLAGS) $@.elf $@.bin
	@echo make header information for irom booting
	@$(MKHEADER) $@.bin

$(X_SOBJS) : .obj/%.o : %.S
	@echo [AS] $<
	@$(AS) $(X_ASFLAGS) $(X_INCDIRS) -c $< -o $@
	@$(AS) $(X_ASFLAGS) -MD -MP -MF $@.d $(X_INCDIRS) -c $< -o $@

$(X_COBJS) : .obj/%.o : %.c
	@echo [CC] $<
	@$(CC) $(X_CFLAGS) $(X_INCDIRS) -c $< -o $@
	@$(CC) $(X_CFLAGS) -MD -MP -MF $@.d $(X_INCDIRS) -c $< -o $@

$(X_CPPOBJS) : .obj/%.o : %.cpp
	@echo [CXX] $<
	@$(CXX) $(X_CXXFLAGS) $(X_INCDIRS) -c $< -o $@	
	@$(CXX) $(X_CXXFLAGS) -MD -MP -MF $@.d $(X_INCDIRS) -c $< -o $@

clean:
	@$(RM) $(X_DEPS) $(X_OBJS) $(X_OBJDIRS) $(X_OUT)
	@echo Clean complete.

#
# Include the dependency files, should be place the last of makefile
#
sinclude $(shell $(MKDIR) $(X_OBJDIRS) $(X_OUT)) $(X_DEPS)
