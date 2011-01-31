
all: compile

compile: build

BUILD_STAMP = build.stamp
DOWNLOAD_STAMP = download.stamp
TRUNK_STAMP = fcs_trunk.stamp
BOARD_IDX = 982
BOARD_FILE = $(BOARD_IDX).board
S_FILE = source/$(BOARD_FILE)

build: $(BUILD_STAMP)

$(BUILD_STAMP): $(DOWNLOAD_STAMP)
	cd source && \
	./Tatzer -l x64m --prefix=$(HOME)/opt/fc-solve --nfc=2 --kazlib --tracemem
	cd source && make
	cd source && make install
	if ! test -e $@ ; then > $@ ; fi

download: $(DOWNLOAD_STAMP)

$(DOWNLOAD_STAMP): $(TRUNK_STAMP)
	if ! test -e $@ ; then > $@ ; fi

fcs_trunk: $(TRUNK_STAMP)

$(TRUNK_STAMP):
	svn co 'http://svn.berlios.de/svnroot/repos/fc-solve/fc-solve/trunk/fc-solve/source/'
	if ! test -e $@ ; then > $@ ; fi

$(S_FILE): build
	source/board_gen/pi-make-microsoft-freecell-board -t 982 > $@

run: build $(S_FILE)
	cd source && ./fc-solve -o $(BOARD_IDX).sol --method bfs -to 01ABCDE -sp r:tf -p -t --cache-limit 4000000 $(BOARD_FILE) | tee $(BOARD_IDX).log

upload:
	rsync -a --progress -v Makefile "$${HOMEPAGE_SSH_PATH}/Files/files/code/fcs/Makefile"

.PHONY: download all compile build download fcs_trunk

