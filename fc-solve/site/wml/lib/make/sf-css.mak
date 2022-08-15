GEN_STYLE_CSS_FILES := \
jqui-override.css print.css solitairey-cards.css style.css web-fc-solve.css

SRC_CSS_TARGETS := $(addprefix $(POST_DEST)/,$(GEN_STYLE_CSS_FILES))

css_targets: $(SRC_CSS_TARGETS)

SASS_STYLE := compressed
# SASS_STYLE := expanded
SASS_DEBUG_FLAGS :=
SASS_CMD = pysassc $(SASS_DEBUG_FLAGS) -I lib/repos/Solitairey/ --style $(SASS_STYLE)

FORT_SASS_DEPS := lib/sass/fortunes.scss
COMMON_SASS_DEPS := lib/repos/Solitairey/solitairey-cards--common.scss lib/sass/common-style.scss lib/sass/defs.scss lib/sass/faq-common.scss

$(SRC_CSS_TARGETS): $(POST_DEST)/%.css: lib/sass/%.scss $(COMMON_SASS_DEPS)
	$(SASS_CMD) $< $@

$(POST_DEST)/style.css $(POST_DEST)/print.css: $(COMMON_SASS_DEPS)
