sudo_renice bash -c "export FCS_PATH=\"$b\"; export FCS_SRC_PATH=\"$c_src\"; time make -j4 -f par.mak"
