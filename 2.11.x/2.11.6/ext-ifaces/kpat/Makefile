all: code.c

code.c: get_preset format_preset.pl
	./get_preset | perl format_preset.pl > $@

FCS_DIR = ../../fc-solve/source

get_preset: get_preset.o cl_chop.o
	gcc -o $@ get_preset.o cl_chop.o
	
get_preset.o: get_preset.c
	gcc -g -c -I../../fc-solve/source $<

cl_chop.o: $(FCS_DIR)/cl_chop.c
	gcc -g -c -I../../fc-solve/source $<
	
