import { fc_solve__chart_bind } from "./fcs-chart--base";
import "./jquery.flot";

$(() => {
    const series = [[], []];
    const lines = $("#data")
        .text()
        .split("\n");
    lines.shift();
    lines.forEach((l) => {
        const fields = l.split("\t");
        const x: number = parseFloat(fields[0]);
        const y1: number = parseFloat(fields[1]);
        const y2: number = parseFloat(fields[2]);
        series[0].push([x, y1]);
        series[1].push([x, y2]);
    });
    const plot = $.plot(
        $("#placeholder"),
        [
            { data: series[0], label: "old-time(iters)" },
            { data: series[1], label: "new-time(iters)" },
        ],
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

    fc_solve__chart_bind("#placeholder", plot);
    // $("#footer").prepend("Flot " + $.plot.version + " &ndash; ");
});
