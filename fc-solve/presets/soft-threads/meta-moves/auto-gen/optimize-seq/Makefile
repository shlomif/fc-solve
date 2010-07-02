all: find_opt.exe

find_opt.exe: Find-Optimized-Sequence/Find-Optimized-Sequence/find_opt.cs
	gmcs -out:$@ $<

test: all
	runprove t/*.t

results: all
	mono ./find_opt.exe find_optimal_quotas 300 
