# Based on https://github.com/shlomif/fc-solve/blob/1c2293f823a304ac658867b7486cccd6881a8b06/fc-solve/site/wml/lib/make/main.mak

JQUIDESTDIR := $(D)/js/jquery-ui
JQUISRCDIR := lib/jquery/jquery-ui-trimmed
JQUI_webpack_dir := lib/jquery/jquery-ui-webpack
JQUI_webpack_dest_dir := $(JQUI_webpack_dir)/dist
JQUI_webpack_bn = tot.js
JQUI_webpackintermediate_js := $(JQUI_webpack_dest_dir)/$(JQUI_webpack_bn)
JQUI_webpack_dest := $(JQUIDESTDIR)/$(JQUI_webpack_bn)

$(JQUI_webpack_dest): $(JQUI_webpackintermediate_js)
	$(MULTI_YUI) -o $@ $<

$(JQUI_webpackintermediate_js):
	webpack --config $(JQUI_webpack_dir)/webpack-config.js --mode production

JQUERYUI_JS_SRCS = \
	$(JQUISRCDIR)/keycode.js \
	$(JQUISRCDIR)/safe-active-element.js \
	$(JQUISRCDIR)/tabs.js \
	$(JQUISRCDIR)/unique-id.js \
	$(JQUISRCDIR)/version.js \
	$(JQUISRCDIR)/widget.js

JQUERYUI_JS_DESTS = $(patsubst $(JQUISRCDIR)/%.js,$(JQUIDESTDIR)/%.js,$(JQUERYUI_JS_SRCS))

$(JQUERYUI_JS_DESTS): $(JQUIDESTDIR)/%.js: $(JQUISRCDIR)/%.js
	$(MULTI_YUI) -o $@ $<

jquery_ui_sync:
	rsync -r node_modules/jquery-ui/ui/widgets/ lib/jquery/jquery-ui-trimmed/
	rsync -r node_modules/jquery-ui/ui/ lib/jquery/jquery-ui-trimmed/
	git clean -dxf lib/jquery/jquery-ui-trimmed/
