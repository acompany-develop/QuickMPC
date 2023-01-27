
ROOT_DIR := $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
SOURCES := $(shell find $(ROOT_DIR)/tests $(ROOT_DIR)/quickmpc $(ROOT_DIR)/demo -type f -name \*.py -print)
PYTHON_FORMAT = autopep8

.PHONY: fmt
fmt:
	autopep8 -i $(SOURCES)

