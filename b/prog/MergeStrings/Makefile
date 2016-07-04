# MergeStrings utility makefile

MAKE_DIR	= build/make
OBJ_DIR		= $(MAKE_DIR)/obj
OBJS		= $(OBJ_DIR)/main.o $(OBJ_DIR)/strings_parser.o $(OBJ_DIR)/strings_scanner.o
CXXFLAGS	= -I.
CPPFLAGS	= -O2
LDFLAGS		= -L/usr/lib/gcc/darwin/default

.PHONY		: clean

$(MAKE_DIR)/MergeStrings	: $(OBJS)
	gcc $(OBJS) -o $@ -framework CoreFoundation -ll -ly -lstdc++
	strip $@

$(OBJ_DIR)/main.o	: main.cpp main.h
	mkdir -p $(OBJ_DIR)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o$@

$(OBJ_DIR)/strings_parser.o	: $(OBJ_DIR)/strings_parser.cpp main.h
	mkdir -p $(OBJ_DIR)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o$@

$(OBJ_DIR)/strings_scanner.o	: $(OBJ_DIR)/strings_scanner.cpp $(OBJ_DIR)/strings_parser.h main.h
	mkdir -p $(OBJ_DIR)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o$@

$(OBJ_DIR)/strings_parser.h \
$(OBJ_DIR)/strings_parser.cpp	: strings_parser.ypp
	mkdir -p $(OBJ_DIR)
	yacc -o $(OBJ_DIR)/strings_parser.cpp -d $<

$(OBJ_DIR)/strings_scanner.cpp	: strings_scanner.lpp
	lex -o$@ $<

clean	:
	rm -f $(MAKE_DIR)/MergeStrings
	rm -rf $(OBJ_DIR)
