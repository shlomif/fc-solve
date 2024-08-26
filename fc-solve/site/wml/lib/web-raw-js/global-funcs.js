window._set_up_tabs = () => {
    const controls = $(".phoenix");
    if (window.location.search.length) {
        controls.phoenix("remove");
    } else {
        controls.phoenix();
    }
    $("#fc_solve_tabs").tabs({
        activate: () => {
            window.my_web_fc_solve_ui.set_up_handlers(module_wrapper);

            return;
        },
        beforeActivate: (event, ui) => {
            const mynew = ui.newPanel,
                old = ui.oldPanel;

            const common_old = old.find(".fcs_common");
            const old_text = common_old.find("textarea").val(),
                common_html = common_old.html();

            common_old.html("");
            const common_new = mynew.find(".fcs_common");
            common_new.html(common_html);
            common_new.find("textarea").val(old_text);

            return;
        },
    });
    return;
};
