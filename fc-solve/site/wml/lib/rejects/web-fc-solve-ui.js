/*
 * web-fc-solve-ui.js
 * Copyright (C) 2018 Shlomi Fish <shlomif@cpan.org>
 *
 * Distributed under terms of the MIT license.
 */
function toggle_advanced() {
    let ctl = $("#fcs_advanced");
    ctl.toggleClass("disabled");

    const set_text = function(my_text) {
        $("#fcs_advanced_toggle").text(my_text);
    };

    set_text(
        ctl.hasClass("disabled") ? "Advanced ▼" : "Advanced ▲"
    );

    return;
}

        show: [{
            id: 'fcs_advanced',
            deps: ['string_params'],
            callback: toggle_advanced,
        },
        ],

    $("#fcs_advanced_toggle").click(toggle_advanced);
