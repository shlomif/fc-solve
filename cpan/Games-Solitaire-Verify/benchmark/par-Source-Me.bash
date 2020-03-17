( rm -f Results/*.res ) 2> /dev/null
if ! test -e build.ninja
then
    perl gen-par-mak.pl
fi
if perl FC_Solve/PreSanityCheck.pm
then
    # sudo_renice bash -c "export FCS_PATH=\"$b\"; export FCS_SRC_PATH=\"$c_src\"; time make -j4 -f par.mak"
    sudo_renice bash -c "export FCS_PATH=\"$b\"; export FCS_SRC_PATH=\"$c_src\"; export LD_LIBRARY_PATH=\"$LD_LIBRARY_PATH\" ; time ninja -j4"
fi
