CC     ?= clang
CFLAGS ?= -D NDEBUG -D NVERIFY -std=c++20 -O2

#================================================================================================================================

.PHONY: all
all:;


SRCS := $(patsubst $(ROOT_PREFIX)%.cpp, %.cpp, $(shell find $(PREFIX) -name "*.cpp"))
OBJS := $(patsubst %.cpp, $(BUILD_DIR)%.o, $(SRCS))
DEPS := $(patsubst %.o, %.d, $(OBJS))

$(DEPS): $(BUILD_DIR)%.d: $(ROOT_PREFIX)%.cpp
	mkdir -p $(dir $@)
	$(CC) $< $(CFLAGS) $(INCLUDES) -MM -MT '$(BUILD_DIR)$*.o $(BUILD_DIR)$*.d' -MF $@
include $(DEPS)

$(OBJS): $(BUILD_DIR)%.o: $(ROOT_PREFIX)%.cpp
	$(CC) $< $(CFLAGS) $(INCLUDES) -c -o $@
all: $(OBJS)
