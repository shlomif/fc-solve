import {
    PLOT_PARAMS,
    fc_solve__chart__add_tooltip,
    fc_solve__chart_bind,
    fc_solve__get_fields,
} from "./fcs-chart--base";
import "./jquery.flot";

export function chart_data(data_selector: string, chart_selector: string) {
    const plot = $.plot(
        $(chart_selector),
        [
            {
                data: fc_solve__get_fields(data_selector).map((fields) => [
                    fields[0],
                    fields[1],
                ]),
                label: "queue-items(iters)",
            },
        ],
        PLOT_PARAMS,
    );

    fc_solve__chart__add_tooltip();
    fc_solve__chart_bind(chart_selector, plot);
    // $("#footer").prepend("Flot " + $.plot.version + " &ndash; ");
}
