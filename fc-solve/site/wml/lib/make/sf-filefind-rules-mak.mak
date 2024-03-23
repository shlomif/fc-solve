COMMON_SRC_DIR = common

SRC_SRC_DIR := src

SRC_DEST := $(D)

SRC_WML_FLAGS := $(WML_FLAGS) -DLATEMP_SERVER=src

SRC_TTML_FLAGS := $(TTML_FLAGS) -DLATEMP_SERVER=src

SRC_DOCS_DEST := $(patsubst %,$(SRC_DEST)/%,$(SRC_DOCS))

SRC_DIRS_DEST := $(patsubst %,$(SRC_DEST)/%,$(SRC_DIRS))

SRC_IMAGES_DEST := $(patsubst %,$(SRC_DEST)/%,$(SRC_IMAGES))

SRC_TTMLS_DEST := $(patsubst %,$(SRC_DEST)/%,$(SRC_TTMLS))

SRC_COMMON_IMAGES_DEST := $(patsubst %,$(SRC_DEST)/%,$(COMMON_IMAGES))

SRC_COMMON_DIRS_DEST := $(patsubst %,$(SRC_DEST)/%,$(COMMON_DIRS))

SRC_COMMON_TTMLS_DEST := $(patsubst %,$(SRC_DEST)/%,$(COMMON_TTMLS))

SRC_COMMON_DOCS_DEST := $(patsubst %,$(SRC_DEST)/%,$(COMMON_DOCS))

SRC_TARGETS := $(SRC_DEST) $(SRC_DIRS_DEST) $(SRC_COMMON_DIRS_DEST) $(SRC_COMMON_IMAGES_DEST) $(SRC_COMMON_DOCS_DEST) $(SRC_COMMON_TTMLS_DEST) $(SRC_IMAGES_DEST) $(SRC_DOCS_DEST) $(SRC_TTMLS_DEST)

$(SRC_DOCS_DEST) : $(SRC_DEST)/% : $(SRC_SRC_DIR)/%.wml $(DOCS_COMMON_DEPS)
	$(call SRC_INCLUDE_WML_RENDER)

$(SRC_TTMLS_DEST) : $(SRC_DEST)/% : $(SRC_SRC_DIR)/%.ttml $(TTMLS_COMMON_DEPS)
	ttml -o $@ $(SRC_TTML_FLAGS) -DLATEMP_FILENAME=$(patsubst $(SRC_DEST)/%,%,$(patsubst %.ttml,%,$@)) $<

$(SRC_DIRS_DEST) : $(SRC_DEST)/% :
	mkdir -p $@
	touch $@

$(SRC_IMAGES_DEST) : $(SRC_DEST)/% : $(SRC_SRC_DIR)/%
	$(call LATEMP_COPY)

$(SRC_COMMON_IMAGES_DEST) : $(SRC_DEST)/% : $(COMMON_SRC_DIR)/%
	$(call LATEMP_COPY)

$(SRC_COMMON_TTMLS_DEST) : $(SRC_DEST)/% : $(COMMON_SRC_DIR)/%.ttml $(TTMLS_COMMON_DEPS)
	ttml -o $@ $(SRC_TTML_FLAGS) -DLATEMP_FILENAME=$(patsubst $(SRC_DEST)/%,%,$(patsubst %.ttml,%,$@)) $<

$(SRC_COMMON_DOCS_DEST) : $(SRC_DEST)/% : $(COMMON_SRC_DIR)/%.wml $(DOCS_COMMON_DEPS)
	WML_LATEMP_PATH="$$(perl -MFile::Spec -e 'print File::Spec->rel2abs(shift)' '$@')" ; ( cd $(COMMON_SRC_DIR) && wml -o "$${WML_LATEMP_PATH}" $(SRC_WML_FLAGS) -DLATEMP_FILENAME=$(patsubst $(SRC_DEST)/%,%,$(patsubst %.wml,%,$@)) $(patsubst $(COMMON_SRC_DIR)/%,%,$<) )

$(SRC_COMMON_DIRS_DEST)  : $(SRC_DEST)/% :
	mkdir -p $@
	touch $@

$(SRC_DEST):
	mkdir -p $@
	touch $@
latemp_targets: $(SRC_TARGETS)

