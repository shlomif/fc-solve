sudo_renice bash -c "export FCS_PATH=\"$b\"; export FCS_SRC_PATH=\"$c_src\"; PATH=\"\$HOME/apps/perl/bleadperl/bin:\$PATH\"; time make -j4 -f par.mak"
