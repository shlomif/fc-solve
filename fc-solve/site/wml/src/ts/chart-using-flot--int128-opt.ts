import {
    PLOT_PARAMS,
    fc_solve__chart__add_tooltip,
    fc_solve__chart_bind,
    fc_solve__get_fields,
} from "./fcs-chart--base";
import "./jquery.flot";

$(() => {
    const series = [[], []];
    for (const fields of fc_solve__get_fields("#data")) {
        const x: number = fields[0];
        const y1: number = fields[1];
        const y2: number = fields[2];
        series[0].push([x, y1]);
        series[1].push([x, y2]);
    }
    const plot = $.plot(
        $("#placeholder"),
        [
            { data: series[0], label: "old-time(iters)" },
            { data: series[1], label: "new-time(iters)" },
        ],
        PLOT_PARAMS,
    );

    fc_solve__chart__add_tooltip();
    fc_solve__chart_bind("#placeholder", plot);
    // $("#footer").prepend("Flot " + $.plot.version + " &ndash; ");
});
