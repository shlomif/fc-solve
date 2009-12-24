all: find_opt.exe

find_opt.exe: find_opt.cs
	gmcs find_opt.cs

test: all
	runprove t/*.t

