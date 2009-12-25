all: find_opt.exe

find_opt.exe: Find-Optimized-Sequence/Find-Optimized-Sequence/find_opt.cs
	gmcs -out:$@ $<

test: all
	runprove t/*.t

