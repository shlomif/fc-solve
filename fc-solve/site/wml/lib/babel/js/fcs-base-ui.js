"use strict";

define(["web-fc-solve"], function(w) {
const entityMap = {
    "&": "&amp;",
    "<": "&lt;",
    ">": "&gt;",
    '"': '&quot;',
    "'": '&#39;',
    "/": '&#x2F;',
};

function escapeHtml(string) {
    return String(string).replace(/[&<>"'\/]/g, (s) => entityMap[s]);
}

// Thanks to Stefan Petrea ( http://garage-coding.com/ ) for inspiring this
// feature.
function populate_input_with_numbered_deal() {
    let input_s = $('#deal_number').val();
    if (! input_s.match(/^[1-9][0-9]*$/)) {
        alert("Wrong input - please enter a positive integer.");
        return;
    }

    let previous_deal_idx = parseInt(input_s);

    $("#stdin").val(
        "# MS Freecell Deal #" + previous_deal_idx +
        "\n#\n" +
        w.deal_ms_fc_board(previous_deal_idx)
    );

    return;
}

class FC_Solve_Bookmarking {
    constructor(args) {
        let that = this;

        that.bookmark_controls = args.bookmark_controls;
        that.show = args.show;

        return;
    }
    _get_loc() {
        return window.location;
    }
    _get_base_url() {
        let that = this;

        const loc = that._get_loc();
        return loc.protocol + '//' + loc.host + loc.pathname;
    }
    _each_control(cb) {
        let that = this;

        that.bookmark_controls.forEach(cb);

        return;
    }
    on_bookmarking() {
        let that = this;

        function get_v(myid) {
            let ctl = $('#' + myid);
            return ctl.is(':checkbox') ?
                (ctl.is(':checked') ? '1' : '0') : ctl.val();
        }

        let control_values = {};

        that._each_control(function(myid) {
            control_values[myid] = get_v(myid);
        });

        const bookmark_string = that._get_base_url() + '?' +
            $.querystring(control_values);

        $("#fcs_bm_results_input").val(bookmark_string);

        let a_elem = $("#fcs_bm_results_a");
        // Too long to be effective.
        // a_elem.text(bookmark_string);
        a_elem.attr('href', bookmark_string);

        $("#fcs_bookmark_wrapper").removeClass("disabled");

        return;
    }
    restore_bookmark() {
        let that = this;

        const qs = that._get_loc().search;

        if (! qs.length) {
            return;
        }

        // Remove trailing 1.
        const params = $.querystring(qs.substr(1));

        that._each_control(function(myid) {
            let ctl = $('#' + myid);
            if (ctl.is(':checkbox')) {
                ctl.prop('checked', ((params[myid] == "1") ? true : false));
            } else {
                ctl.val(params[myid]);
            }
        });

        that.show.forEach(function(rec) {
            const id = rec.id;
            const deps = rec.deps;

            let should_toggle = false;
            deps.forEach(function(d) {
                if ($("#" + d).val().length > 0) {
                    should_toggle = true;
                }
            });

            if (should_toggle) {
                if ($("#" + id).hasClass("disabled")) {
                    rec.callback();
                }
            }
        });

        return;
    }
}

    return {
        FC_Solve_Bookmarking: FC_Solve_Bookmarking,
        escapeHtml: escapeHtml,
        populate_input_with_numbered_deal: populate_input_with_numbered_deal,
    };
});
