"use strict";

if (typeof define !== 'function') {
    var define = require('amdefine')(module);
}

define(["web-fc-solve", "libfreecell-solver.min", 'dist/fc_solve_find_index_s2ints'], function (w, Module, s2i) {

class FC_Solve_Bookmarking {
    constructor(args) {
        var that = this;

        that.bookmark_controls = args.bookmark_controls;
        that.show = args.show;

        return;
    }
    _get_loc() {
        return window.location;
    }
    _get_base_url() {
        var that = this;

        var loc = that._get_loc();
        return loc.protocol + '//' + loc.host + loc.pathname;
    }
    _each_control(cb) {
        var that = this;

        that.bookmark_controls.forEach(cb);
    }
    on_bookmarking() {
        var that = this;

        var get_v = function(myid) {
            var ctl = $('#' + myid);
            return ctl.is(':checkbox') ?  (ctl.is(':checked') ? '1' : '0') : ctl.val();
        };

        var control_values = {};

        that._each_control(function (myid) {
            control_values[myid] = get_v(myid);
        });

        var bookmark_string = that._get_base_url() + '?' + $.querystring(control_values);

        $("#fcs_bm_results_input").val(bookmark_string);

        var a_elem = $("#fcs_bm_results_a");
        // Too long to be effective.
        // a_elem.text(bookmark_string);
        a_elem.attr('href', bookmark_string);

        $("#fcs_bookmark_wrapper").removeClass("disabled");

        return;
    }
    restore_bookmark() {
        var that = this;

        var qs = that._get_loc().search;

        if (! qs.length) {
            return;
        }

        // Remove trailing 1.
        var params = $.querystring(qs.substr(1));

        that._each_control(function (myid) {
            var ctl = $('#' + myid);
            if (ctl.is(':checkbox')) {
                ctl.prop('checked', ((params[myid] == "1") ? true : false));
            }
            else {
                ctl.val(params[myid]);
            }
        });

        that.show.forEach(function (rec) {
            var id = rec.id;
            var deps = rec.deps;

            var should_toggle = false;
            deps.forEach(function (d) {
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

    return { FC_Solve_Bookmarking: FC_Solve_Bookmarking, };
});
