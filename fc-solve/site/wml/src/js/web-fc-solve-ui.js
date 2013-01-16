"use strict";
var entityMap = {
    "&": "&amp;",
    "<": "&lt;",
    ">": "&gt;",
    '"': '&quot;',
    "'": '&#39;',
    "/": '&#x2F;'
};

function escapeHtml(string) {
    return String(string).replace(/[&<>"'\/]/g, function (s) {
        return entityMap[s];
    });
}

function _webui_set_output(buffer) {
    $("#output").val(buffer);

    return;
}

function clear_output() {
    return _webui_set_output('');
}

function _webui_set_status_callback(myclass, mylabel)
{
    var ctl = $("#fc_solve_status");
    ctl.removeClass();
    ctl.addClass(myclass);
    ctl.html(escapeHtml(mylabel));

    return;
}

function fc_solve_do_solve() {
    var cmd_line_preset = $("#preset").val();
    var board_string = $("#stdin").val().replace(/#[^\r\n]*\r?\n?/g, '');

    var instance = new FC_Solve({
        cmd_line_preset: cmd_line_preset,
        set_status_callback: _webui_set_status_callback,
        set_output: _webui_set_output,
    });

    return instance.do_solve(board_string);
}

// Thanks to Stefan Petrea ( http://garage-coding.com/ ) for inspiring this
// feature.
var previous_deal_idx = 1;

function populate_input_with_numbered_deal() {

    /*
    var new_idx = prompt("Enter MS Freecell deal number:");

    // Prompt returned null (user cancelled).
    if (! new_idx) {
        return;
    }

    previous_deal_idx = parseInt(new_idx);
    */

    previous_deal_idx = parseInt($('#deal_number').val());

    $("#stdin").val(
        "# MS Freecell Deal #" + previous_deal_idx +
        "\n#\n" +
        deal_ms_fc_board(previous_deal_idx)
    );

    return;
}

function _get_loc() {
    return window.location;
}

function _get_base_url() {
    var loc = _get_loc();
    return loc.protocol + '//' + loc.host + loc.pathname;
}

function on_bookmarking() {
    var get_v = function(myid) { return $('#' + myid).val() };

    var bookmark_string = _get_base_url() + '?' +
        $.querystring({stdin: get_v('stdin'), preset: get_v('preset'), deal_number: get_v('deal_number')});

    $("#fcs_bm_results_input").val(bookmark_string);

    var a_elem = $("#fcs_bm_results_a");
    // Too long to be effective.
    // a_elem.text(bookmark_string);
    a_elem.attr('href', bookmark_string);

    $("#fcs_bookmark_wrapper").removeClass("disabled");

    return;
}

function restore_bookmark() {
    var qs = _get_loc().search;

    if (! qs.length) {
        return;
    }

    // Remove trailing 1.
    var params = $.querystring(qs.substr(1));

    ['stdin', 'preset', 'deal_number'].forEach(function (myid) {
        $('#' + myid).val(params[myid]);
    });

    return;
}
