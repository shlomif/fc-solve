perl -Ilib -MWWW::LinkChecker::Internal::App -e 'WWW::LinkChecker::Internal::App->run()' -- check --base='http://localhost/shlomif/fc-solve-temp/' \
    --before-insert-skip='\.(?:js|epub|zip|diff|pl|xsl|xslt|rtf|txt|raw\.html)\z' \
    --before-insert-skip='/michael_mann/doxygen/html/' \
    --before-insert-skip='/downloads/(?:dbm-fc-solver|fc-solve)/[^/]+(?:\.gz|\.xz|\.bz2)\z' \
    ;
