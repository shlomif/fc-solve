import { fc_solve__chart_bind } from "./fcs-chart--base";
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
        {
            grid: {
                clickable: true,
                hoverable: true,
            },
            series: {
                lines: {
                    show: true,
                },
                points: {
                    show: true,
                },
            },
        },
    );

    $("<div id='tooltip'></div>")
        .css({
            "background-color": "#fee",
            border: "1px solid #fdd",
            display: "none",
            opacity: 0.8,
            padding: "2px",
            position: "absolute",
        })
        .appendTo("body");

    fc_solve__chart_bind(chart_selector, plot);
    // $("#footer").prepend("Flot " + $.plot.version + " &ndash; ");
}
