#!/bin/bash
# ruby -p -i -e '$_.gsub!(/\bptr_state_with_locations_(key|val)\b/) { |s| "ptr_state_#{$1}" }' *.[ch]
ruby -pi -e 'gsub! /\bptr_state_with_locations_orig_(key|val)\b/, "ptr_orig_state_\\1"' *.[ch]
