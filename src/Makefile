SRC_TETRIS = brick_game/tetris
SRC_CLI = gui/cli
TEST_DIR = tests
BUILD_DIR = build
BIN_DIR = bin
REPORT_DIR = report
HTML_REPORT_DIR = $(REPORT_DIR)/html

PROJECT = tetris

SRCS = $(SRC_CLI)/main.c $(SRC_CLI)/frontend.c $(SRC_TETRIS)/tetris.c
OBJS = $(patsubst $(SRC_CLI)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

TEST_SRCS = $(TEST_DIR)/test_tetris.c
TEST_OBJS = $(BUILD_DIR)/test_tetris.o
TEST_BIN = $(BIN_DIR)/test_tetris

INCLUDES = -I$(SRC_CLI) -I$(SRC_TETRIS)

CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99 -D_GNU_SOURCE $(INCLUDES)
LDFLAGS = -lncurses -lrt

GCOV_FLAGS = -fprofile-arcs -ftest-coverage
GCOV_LIB = -lgcov

CHECK_LIBS = -lcheck -lsubunit -lm -lpthread

PREFIX ?= $(PWD)
BINDIR = $(PREFIX)/bin

CLANG_FORMAT_CONFIG = ../materials/linters/.clang-format

.PHONY: all install uninstall clean dvi dist test gcov_report correctformat checkformat valgrind_test

all: clean $(BIN_DIR)/$(PROJECT)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BUILD_DIR)/%.o: $(SRC_CLI)/%.c $(SRC_CLI)/frontend.h $(SRC_TETRIS)/tetris.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/tetris.o: $(SRC_TETRIS)/tetris.c $(SRC_TETRIS)/tetris.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test_tetris.o: $(TEST_DIR)/test_tetris.c $(SRC_TETRIS)/tetris.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/$(PROJECT): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

install: all
	install -d $(BINDIR)
	install -m 755 $(BIN_DIR)/$(PROJECT) $(BINDIR)/$(PROJECT)_installed

uninstall:
	rm -f $(BINDIR)/$(PROJECT)_installed

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) $(REPORT_DIR) coverage.info $(PROJECT).tar.gz valgrind_full.log highscore.txt
	find . -name '*.gcno' -delete
	find . -name '*.gcda' -delete
	find . -name '*.gcov' -delete
	find . -name '*.o' -delete
	find . -name 'test_tetris' -delete

dvi:
	@:

dist:
	tar czf $(PROJECT).tar.gz $(SRC_CLI) $(SRC_TETRIS) $(TEST_DIR) Makefile

test: clean $(BUILD_DIR) $(BIN_DIR) $(BUILD_DIR)/tetris.o $(BUILD_DIR)/test_tetris.o
	$(CC) $(CFLAGS) -o $(TEST_BIN) $(BUILD_DIR)/tetris.o $(BUILD_DIR)/test_tetris.o $(CHECK_LIBS)
	$(TEST_BIN)

gcov_report: clean
	$(MAKE) CFLAGS="$(CFLAGS) $(GCOV_FLAGS)" LDFLAGS="$(CHECK_LIBS) $(GCOV_LIB)" test
	mkdir -p $(REPORT_DIR)
	@gcov $(SRC_TETRIS)/*.c -o $(BUILD_DIR)
	@mv *.gcov $(REPORT_DIR) 2>/dev/null || true
	@echo "Coverage report generated in $(REPORT_DIR)/"
	mkdir -p $(HTML_REPORT_DIR)
	lcov --directory $(BUILD_DIR) --capture --output-file coverage.info
	genhtml coverage.info --output-directory $(HTML_REPORT_DIR)
	@echo "HTML coverage report generated in $(HTML_REPORT_DIR)/"

checkformat:
	clang-format -n -style=file:$(CLANG_FORMAT_CONFIG) */*/*.c */*/*.h

correctformat:
	clang-format -i -style=file:$(CLANG_FORMAT_CONFIG) */*/*.c */*/*.h	

valgrind_test: clean test
	valgrind -s --leak-check=full --show-leak-kinds=all ./$(TEST_BIN) 2>&1 | tee valgrind_full.log
