#libss/MathのMakefile
# ubuntu ver; 64bit にて検証

ROOT_DIR = ../..
OBJS_DIR  = ./obj
SRC_DIR  = $(ROOT_DIR)/src
INCLUDE = -I$(ROOT_DIR)/include
OUTPUT_LIBS_DIR = 

TEST_DIR = $(ROOT_DIR)/test
TEST_OBJS_DIR=./test_obj

TARGET = lib/libssmath.a

#shellコマンドの定義
COPY   := cp
RM     := rm -f
MKDIR  := mkdir -p

CPP      = g++
CPPFLAGS =
AR       = ar 

SRCS      = $(wildcard $(SRC_DIR)/*.cpp)
SRCS_FILE = $(notdir $(SRCS))
OBJS      = $(addprefix $(OBJS_DIR)/, $(SRCS_FILE:.cpp=.o))

TEST_SRCS      = $(wildcard $(TEST_DIR)/*.cpp)
TEST_SRCS_FILE = $(notdir $(TEST_SRCS))
TEST_OBJS      = $(addprefix $(TEST_OBJS_DIR)/, $(TEST_SRCS_FILE:.cpp=.o))


TEST_LIBS = -lm -lgtest  -lpthread -lgtest_main
all : debug $(TARGET) test


%.a : $(OBJS)
	@echo ""
	@echo "Archive [$@]"
	@echo "SRCS_FILE $(SRCS_FILE)"
	@echo "SRCS $(SRCS)"
	@$(MKDIR) $(dir $@)
	$(RM) $@
	$(AR) cq $@ $(OBJS) 


clean : 
	@echo ""
	@echo "make clean"
	$(RM) $(TARGET) $(OBJS) $(TEST_OBJS)

test : $(TEST_OBJS)
	@echo ""
	@echo "link objects file for test: [$(TEST_OBJS)]"
	@$(MKDIR) ./test_result
	$(CPP) $(TEST_OBJS) $(TEST_LIBS) -o $@ 
	./$@ --gtest_output=xml:test_result/test_linux.xml 

debug:
	@echo ""
	@echo "SRCS_FILE: [$(SRCS_FILE)]"
	@echo "SRCS: [$(SRCS)]"
	@echo "OBJS: [$(OBJS)]"
	@echo ""
	@echo "TEST_SRCS: [$(TEST_SRCS)]"
	@echo "TEST_SRCS_FILE: [$(TEST_SRCS_FILE)]"
	@echo "TEST_OBJS: [$(TEST_OBJS)]"

$(OBJS_DIR)/%.o:$(SRC_DIR)/%.cpp
	@echo ""
	@echo "Compiling [$<]"
	@$(MKDIR) $(dir $@)
	$(CPP) $(CPPFLAGS) $(INCLUDE)  -c $< -o $@ 

$(TEST_OBJS_DIR)/%.o: $(TEST_DIR)/%.cpp
	@echo ""
	@echo "Compiling [$<]"
	@$(MKDIR) $(dir $@)
	$(CPP) $(CPPFLAGS) $(INCLUDE) -c $< -o $@
