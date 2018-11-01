import {
    PLOT_PARAMS,
    fc_solve__chart__add_tooltip,
    fc_solve__chart_bind,
} from "./fcs-chart--base";
import "./jquery.flot";

export function chart_data(data_selector: string, chart_selector: string) {
    const series = [];
    const lines = $(data_selector)
        .text()
        .split("\n");
    lines.shift();
    lines.forEach((l) => {
        const fields = l.split("\t");
        series.push([parseFloat(fields[0]), parseFloat(fields[1])]);
    });
    const plot = $.plot(
        $(chart_selector),
        [{ data: series, label: "queue-items(iters)" }],
        PLOT_PARAMS,
    );

    fc_solve__chart__add_tooltip();
    fc_solve__chart_bind(chart_selector, plot);
    // $("#footer").prepend("Flot " + $.plot.version + " &ndash; ");
}
