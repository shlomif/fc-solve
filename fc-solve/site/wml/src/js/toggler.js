"use strict";

function build_toggler(args) {
    var has_ls = (typeof localStorage !== "undefined" && localStorage !== null);
    var toggle_sect_key = args['ls_key'];
    var toggler_selector = args['toggler_selector'];
    var toggled_selector = args['toggled_selector'];
    var toggled_class = args['toggled_class'];
    var hide_text = args['hide_text'];
    var show_text = args['show_text'];
    var default_state = (('default_state' in args) ? args['default_state'] : true);

    var toggle_sect_menu = function() {
        var elem = $(toggler_selector);

        var was_off = elem.hasClass("off");
        var was_on = !was_off;
        var is_on_now = !was_on;

        elem.text(was_off ? hide_text : show_text);
        $(toggled_selector).toggleClass(toggled_class);
        elem.toggleClass("off");
        elem.toggleClass("on");
        if (has_ls) {
            if (is_on_now == default_state) {
                localStorage.removeItem(toggle_sect_key);
            } else {
                localStorage.setItem(toggle_sect_key, (is_on_now ? "1" : "0"));
            }
        }
    };

    var elem = $(toggler_selector);

    elem.on("click", function (event) { toggle_sect_menu(); });

    $(document).ready(function() {

        if (has_ls) {
            var in_storage = localStorage.getItem(toggle_sect_key);

            if (in_storage == undefined) {
                in_storage = default_state;
            } else {
                in_storage = (in_storage == "1" ? true : false);
            }

            var in_elem = elem.hasClass("on");

            if ((in_storage && (!in_elem)) || ((!in_storage) && in_elem)) {
                toggle_sect_menu();
            }
        }
    });

    return;
}
