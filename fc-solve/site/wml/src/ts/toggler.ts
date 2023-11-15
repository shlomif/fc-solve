"use strict";

class TogglerWrapper {
    constructor(
        public readonly toggle_cb: () => void,
        public readonly apply_state_cb: () => void,
        public readonly calc_in_elem: () => boolean,
    ) {
        return;
    }
}

interface TogglerParams {
    default_state?: boolean;
    hide_text: string;
    ls_key: string;
    show_text: string;
    toggled_class: string;
    toggled_selector: string;
    toggled_type?: string;
    toggler_selector: string;
}

function build_toggler(args: TogglerParams): void {
    function _is_null(x: any): boolean {
        return typeof x === "undefined" || x === undefined || x === null;
    }

    const has_ls: boolean = !_is_null(localStorage);
    const toggled_type: string =
        "toggled_type" in args ? args["toggled_type"] : "class";
    const is_class: boolean = toggled_type == "class";
    const _key_name: string = args["ls_key"];
    const toggler_selector: string = args["toggler_selector"];
    const toggled_selector: string = args["toggled_selector"];
    const toggled_class: string = args["toggled_class"];
    const hide_text: string = args["hide_text"];
    const show_text: string = args["show_text"];
    const default_state: boolean =
        "default_state" in args ? args["default_state"] : true;

    const _calc_elem = () => {
        return $(toggler_selector);
    };

    function toggle_sect_menu() {
        const elem = _calc_elem();

        const was_off: boolean = elem.hasClass("off");
        const was_on: boolean = !was_off;
        const is_on_now: boolean = !was_on;

        elem.html(was_off ? hide_text : show_text);
        $(toggled_selector).toggleClass(toggled_class);
        elem.toggleClass("off");
        elem.toggleClass("on");
        if (has_ls) {
            if (is_on_now === default_state) {
                localStorage.removeItem(_key_name);
            } else {
                localStorage.setItem(_key_name, is_on_now ? "1" : "0");
            }
        }
    }

    const toggled_attr = "open";

    function _calc__is_on_now(elem): boolean {
        const was_off: boolean = !elem.attr(toggled_attr);
        const was_on: boolean = !was_off;
        // NOTE THE DIFFERENCE FROM toggle_sect_menu!!!
        const is_on_now: boolean = was_on;
        return is_on_now;
    }
    function apply_details_state(): void {
        const elem = _calc_elem();
        const is_on_now: boolean = _calc__is_on_now(elem);
        if (is_on_now) {
            elem.removeAttr(toggled_attr);
        } else {
            elem.attr(toggled_attr, "open");
        }
    }

    function toggle_details(): void {
        if (!has_ls) {
            return;
        }
        const elem = _calc_elem();
        const is_on_now: boolean = _calc__is_on_now(elem);

        if (is_on_now === default_state) {
            localStorage.removeItem(_key_name);
        } else {
            localStorage.setItem(_key_name, is_on_now ? "1" : "0");
        }
    }
    const elem = _calc_elem();
    const toggler_wrapper: TogglerWrapper = is_class
        ? new TogglerWrapper(toggle_sect_menu, toggle_sect_menu, function () {
              return elem.hasClass("on");
          })
        : new TogglerWrapper(toggle_details, apply_details_state, function () {
              return _calc__is_on_now(elem);
          });

    elem.on(is_class ? "click" : "toggle", function (event) {
        toggler_wrapper.toggle_cb();
    });

    $(document).ready(function () {
        if (!has_ls) {
            return;
        }
        const in_storage_s: string = localStorage.getItem(_key_name);

        const in_storage: boolean = _is_null(in_storage_s)
            ? default_state
            : in_storage_s == "1"
              ? true
              : false;

        const in_elem: boolean = toggler_wrapper.calc_in_elem();

        if ((in_storage && !in_elem) || (!in_storage && in_elem)) {
            toggler_wrapper.apply_state_cb();
        }
    });
}
