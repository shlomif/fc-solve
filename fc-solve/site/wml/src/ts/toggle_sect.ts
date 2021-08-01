"use strict";

const toggle_sect_key: string = "fc_solve_section_navigation_menu_shown";
const toggle_main_key: string = "fc_solve_main_navigation_menu_shown";

function build_sect_nav_menu_toggler(): void {
    build_toggler({
        default_state: false,
        hide_text: "Hide",
        ls_key: toggle_sect_key,
        show_text: "Show",
        toggled_class: "novis",
        toggled_selector: "#sect_menu_wrapper",
        toggler_selector: "#toggle_sect_menu",
    });
}

function build_main_nav_menu_toggler(): void {
    build_toggler({
        default_state: false,
        hide_text: "Hide NavBar ⬈",
        ls_key: toggle_main_key,
        show_text: "Show NavBar ⬋",
        toggled_class: "novis",
        toggled_selector: "#navbar , #navbar_wrapper, #main",
        toggler_selector: "#show_navbar",
    });
}

$(document).ready(function (): void {
    build_sect_nav_menu_toggler();
    build_main_nav_menu_toggler();
});
