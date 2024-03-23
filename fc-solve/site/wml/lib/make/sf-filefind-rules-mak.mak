SRC_SRC_DIR := src

SRC_DEST := $(D)

SRC_DOCS_DEST := $(patsubst %,$(SRC_DEST)/%,$(SRC_DOCS))

SRC_DIRS_DEST := $(patsubst %,$(SRC_DEST)/%,$(SRC_DIRS))

SRC_IMAGES_DEST := $(patsubst %,$(SRC_DEST)/%,$(SRC_IMAGES))

SRC_TARGETS := $(SRC_DEST) $(SRC_DIRS_DEST) $(SRC_IMAGES_DEST) $(SRC_DOCS_DEST)

$(SRC_DIRS_DEST) : $(SRC_DEST)/% :
	mkdir -p $@
	touch $@

$(SRC_IMAGES_DEST) : $(SRC_DEST)/% : $(SRC_SRC_DIR)/%
	$(call LATEMP_COPY)

$(SRC_DEST):
	mkdir -p $@
	touch $@

latemp_targets: $(SRC_TARGETS)
