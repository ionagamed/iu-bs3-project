BASIC_FLAGS=--highlight-style tango --syntax-definition extensions/shell-command.xml

ifndef NO_TOC
	TOC_FLAGS=--toc
endif

FLAGS=$(BASIC_FLAGS) $(SHELL_COMMAND_FLAGS) $(TOC_FLAGS)

LATEX_FLAGS=--number-sections -t latex
BUILD_LATEX=pandoc $(FLAGS) $(LATEX_FLAGS) -o $(BUILD_DIR)/$@.pdf $(SRC_DIR)/$@.md
OPEN_LATEX=open $(BUILD_DIR)/$@.pdf

ifdef WATCH
	WATCH_LATEX=$(OPEN_LATEX); nodemon -x "$(BUILD_LATEX)" -e md
endif

SRC_DIR=src
BUILD_DIR=build

default: report

src_dir:
	mkdir -p $(SRC_DIR)

build_dir:
	mkdir -p $(BUILD_DIR)

dirs: src_dir build_dir

%: $(SRC_DIR)/%.md dirs
	$(BUILD_LATEX)
	$(WATCH_LATEX)
