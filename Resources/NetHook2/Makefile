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
SOURCES = NetHook2/version.cpp NetHook2/binaryreader.cpp NetHook2/crypto.cpp NetHook2/csimpledetour.cpp NetHook2/csimplescan.cpp NetHook2/injector.cpp NetHook2/logger.cpp NetHook2/net.cpp NetHook2/nethook.cpp NetHook2/sedebug.cpp NetHook2/sigscan.cpp NetHook2/string.cpp NetHook2/zip.cpp NetHook2/steammessages_base.pb.cc
OBJDIR = obj
OBJS = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))
UNAME_S := $(shell uname -s)

CXXFLAGS = -fPIC -shared -m32 -Inative-dependencies/output/include -Lnative-dependencies/output/lib/
CXXFLAGS += -Wall -Wformat -Wl,--no-undefined
LIBS = -lz -lprotobuf -ldl

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

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
