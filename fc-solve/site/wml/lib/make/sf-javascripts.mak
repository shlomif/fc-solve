TYPESCRIPT_DEST_FILES = $(patsubst %.js,$(OUT_PREF)/%.js,$(TYPESCRIPT_basenames))
TYPESCRIPT_DEST_FILES__NODE = $(patsubst %.js,lib/for-node/js/%.js,$(TYPESCRIPT_basenames))
TYPESCRIPT_COMMON_DEFS_FILES = src/ts/jq_qs.d.ts

run_tsc = tsc --project lib/typescript/$1/tsconfig.json

tsc_cmdline:
	$(call run_tsc,cmdline)

tsc_www:
	$(call run_tsc,www)
