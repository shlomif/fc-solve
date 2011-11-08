
all: compile

compile: build

AVL_BASE = avl-2.0.3
AVL_ARC = $(AVL_BASE).tar.gz
BUILD_STAMP = build.stamp
DOWNLOAD_STAMP = download.stamp
TRUNK_STAMP = fcs_trunk.stamp
LIBAVL2_STAMP = libavl2.stamp
BOARD_IDX = 982
BOARD_FILE = $(BOARD_IDX).board
S_FILE = source/$(BOARD_FILE)

build: $(BUILD_STAMP)

$(BUILD_STAMP): $(DOWNLOAD_STAMP)
	cd source && \
	LIBAVL2_SOURCE_DIR="$$(pwd)/../$(AVL_BASE)" ./Tatzer -l x64m --prefix=$(HOME)/opt/fc-solve --nfc=2 --libavl2-p=prb
	cd source && make
	cd source && make install
	if ! test -e $@ ; then > $@ ; fi

download: $(DOWNLOAD_STAMP)

$(DOWNLOAD_STAMP): $(TRUNK_STAMP) $(LIBAVL2_STAMP)
	if ! test -e $@ ; then > $@ ; fi

fcs_trunk: $(TRUNK_STAMP)

$(TRUNK_STAMP):
	svn co 'http://svn.berlios.de/svnroot/repos/fc-solve/fc-solve/trunk/fc-solve/source/'
	if ! test -e $@ ; then > $@ ; fi



libavl2: $(LIBAVL2_STAMP)

$(LIBAVL2_STAMP):
	wget -c http://ftp.gnu.org/pub/gnu/avl/$(AVL_ARC)
	tar -xvf $(AVL_ARC)
	if ! test -e $@ ; then > $@ ; fi

$(S_FILE): build
	source/board_gen/pi-make-microsoft-freecell-board -t 982 > $@

run: build $(S_FILE)
	cd source && ./fc-solve -o $(BOARD_IDX).sol --method bfs -to 01ABCDE -sp r:tf -p -t --cache-limit 4000000 $(BOARD_FILE) | tee $(BOARD_IDX).log

upload:
	rsync -a --progress -v Makefile "$${HOMEPAGE_SSH_PATH}/Files/files/code/fcs/Makefile"
.PHONY: download all compile build download libavl2 fcs_trunk
