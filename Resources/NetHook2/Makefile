#
# Linux Makefile
#
# You will need X11 and GL (http://www.glfw.org):
# Linux:
#   apt-get install libx11-dev libgl-dev
#

#CXX = g++
#CXX = clang++

EXE = NetHook2.so
TMP_MINIDETOUR_SOURCES = deps/capstone/cs.c deps/capstone/MCInst.c deps/capstone/MCInstrDesc.c deps/capstone/MCRegisterInfo.c deps/capstone/SStream.c deps/capstone/utils.c deps/capstone/arch/ARM/ARMDisassembler.c deps/capstone/arch/ARM/ARMInstPrinter.c deps/capstone/arch/ARM/ARMMapping.c deps/capstone/arch/ARM/ARMModule.c deps/capstone/arch/AArch64/AArch64BaseInfo.c deps/capstone/arch/AArch64/AArch64Disassembler.c deps/capstone/arch/AArch64/AArch64InstPrinter.c deps/capstone/arch/AArch64/AArch64Mapping.c deps/capstone/arch/AArch64/AArch64Module.c deps/capstone/arch/X86/X86Disassembler.c deps/capstone/arch/X86/X86DisassemblerDecoder.c deps/capstone/arch/X86/X86IntelInstPrinter.c deps/capstone/arch/X86/X86Mapping.c deps/capstone/arch/X86/X86Module.c deps/capstone/arch/X86/X86ATTInstPrinter.c src/mini_detour.cpp
MINIDETOUR_SOURCES = $(addprefix NetHook2/mini_detour/, $(TMP_MINIDETOUR_SOURCES))
NETHOOK2_SOURCES = NetHook2/os.cpp NetHook2/version.cpp NetHook2/binaryreader.cpp NetHook2/crypto.cpp NetHook2/csimpledetour.cpp NetHook2/csimplescan.cpp NetHook2/injector.cpp NetHook2/logger.cpp NetHook2/net.cpp NetHook2/nethook.cpp NetHook2/sedebug.cpp NetHook2/sigscan.cpp NetHook2/string.cpp NetHook2/zip.cpp NetHook2/steammessages_base.pb.cc
SOURCES = $(NETHOOK2_SOURCES)  $(MINIDETOUR_SOURCES)
OBJDIR = obj
OBJS = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))
UNAME_S := $(shell uname -s)

CXXFLAGS = -fPIC -shared -m32 -INetHook2/mini_detour/include -INetHook2/mini_detour/deps/capstone/include/ -Inative-dependencies/output/include -Lnative-dependencies/output/lib/
CXXFLAGS += -g -Wall -Wformat -Wl,--no-undefined -DCAPSTONE_USE_SYS_DYN_MEM -DCAPSTONE_HAS_X86 -DCAPSTONE_HAS_ARM64 -DCAPSTONE_HAS_ARM
LIBS = -lz -lprotobuf -ldl

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

$(OBJDIR)/%.o:NetHook2/mini_detour/deps/capstone/%.c
	$(CC) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/%.o:NetHook2/mini_detour/deps/capstone/arch/**/%.c
	$(CC) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/%.o:NetHook2/mini_detour/src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/%.o:NetHook2/%.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/%.o:NetHook2/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: create_obj_dir build_version $(EXE)
	@echo Build complete

create_obj_dir:
	@sh -c "[ ! -e obj ] && mkdir obj; exit 0"

build_version:
	@bash GenerateVersionInfo.sh

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(EXE) $(OBJS)
