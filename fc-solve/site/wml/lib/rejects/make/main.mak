	# FIND_INDEX__PYJS__TARGETS = $(FIND_INDEX__PYJS__TGT) $(FIND_INDEX__PYJS__DEST) $(FIND_INDEX__PYJS__DEST2) $(FIND_INDEX__PYJS__NODE)
FIND_INDEX__PYJS__TGT_DIR = lib/transcrypt_module
FIND_INDEX__PYJS__NODE_DIR = lib/for-node/js
FIND_INDEX__PYJS__SRC_BN = fc_solve_find_index_s2ints.py
FIND_INDEX__PYJS__pivot =  dist/fc_solve_find_index_s2ints.js
FIND_INDEX__PYJS__NODE = $(FIND_INDEX__PYJS__NODE_DIR)/$(FIND_INDEX__PYJS__pivot)
FIND_INDEX__PYJS__TGT = $(FIND_INDEX__PYJS__TGT_DIR)/$(FIND_INDEX__PYJS__pivot)
FIND_INDEX__PYJS__DEST_DIR = $(DEST_JS_DIR)
FIND_INDEX__PYJS__DEST = $(FIND_INDEX__PYJS__DEST_DIR)/$(FIND_INDEX__PYJS__pivot)
FIND_INDEX__PYJS__DEST2_DIR = $(D)/js-fc-solve/automated-tests
FIND_INDEX__PYJS__DEST2 = $(FIND_INDEX__PYJS__DEST2_DIR)/$(FIND_INDEX__PYJS__pivot)
PIV = $(FIND_INDEX__PYJS__TGT_DIR)/dist
FIND_INDEX__PYJS__PY = $(FIND_INDEX__PYJS__TGT_DIR)/src/$(FIND_INDEX__PYJS__SRC_BN)

$(FIND_INDEX__PYJS__NODE): $(FIND_INDEX__PYJS__TGT)
	cp -f $< $@
$(FIND_INDEX__PYJS__TGT): $(BASE_FC_SOLVE_SOURCE_DIR)/board_gen/$(FIND_INDEX__PYJS__SRC_BN)
	cat $< lib/js-epilogue.py > $(FIND_INDEX__PYJS__PY)
	cd $(FIND_INDEX__PYJS__TGT_DIR) && python3 build.py
	perl -i -lpe 's:(MAX_SHIFTREDUCE_LOOPS\s*=\s*1000)(;):$${1}00$${2}:g' $(PIV)/*.js
	perl -i -lne 'print unless /String.prototype.strip = / .. /^\s*\}/' $(PIV)/*.js
	$(STRIP_TRAIL_SPACE) $$(find $(FIND_INDEX__PYJS__TGT_DIR) -regextype egrep -regex '.*\.(js|html)')
	touch $@

$(FIND_INDEX__PYJS__DEST) $(FIND_INDEX__PYJS__DEST2): %: $(FIND_INDEX__PYJS__TGT)
	mkdir -p $$(dirname "$@")
	$(MULTI_YUI) -o $@ $<

real_all: $(FIND_INDEX__PYJS__TARGETS)
