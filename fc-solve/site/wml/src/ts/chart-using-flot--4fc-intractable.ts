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

export function process_and_chart_data(
    data_selector: string,
    chart_selector: string,
) {
    const lines = $(data_selector)
        .text()
        .split("\n");
    let head = lines.shift();
    if (head.length === 0) {
        head = lines.shift();
    }
    let text = "Iterations\t" + head + "\n";
    const step = 1000000;
    let reached = step;
    for (const l of lines) {
        text += "" + reached + "\t" + l + "\n";
        reached += step;
    }
    $(data_selector).text(text);
    return chart_data(data_selector, chart_selector);
}
