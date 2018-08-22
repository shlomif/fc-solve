$(() => {
    const series = [[], []];
    const lines = $("#data").text().split("\n");
    lines.shift();
    lines.forEach(
        (l) => {
            const fields = l.split("\t");
            const x: number = parseFloat(fields[0]);
            const y1: number = parseFloat(fields[1]);
            const y2: number = parseFloat(fields[2]);
            series[0].push([x, y1]);
            series[1].push([x, y2]);
        },
    );
    const plot = $.plot($("#placeholder"), [
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

    $("<div id='tooltip'></div>").css({
        "background-color": "#fee",
        'border': "1px solid #fdd",
        'display': "none",
        'opacity': 0.80,
        'padding': "2px",
        'position': "absolute",
    }).appendTo("body");

    $("#placeholder").bind("plothover", (event, pos, item) => {

        {
            const str = "(" + pos.x.toFixed(2) + ", " + pos.y.toFixed(2) + ")";
            $("#hoverdata").text(str);
        }

        {
            if (item) {
                const x = (item.datapoint[0] / 1000000);
                const y = item.datapoint[1].toFixed(2);

                $("#tooltip").html(item.series.label + " of " + x.toFixed(1) + "M = " + y + "sec")
                    .css({top: item.pageY + 5, left: item.pageX + 5})
                    .fadeIn(200);
            } else {
                $("#tooltip").hide();
            }
        }
        return false;
    });

    $("#placeholder").bind("plotclick", (event, pos, item) => {
        if (item) {
            $("#clickdata").text(" - click point " + item.dataIndex + " in " + item.series.label);
            plot.highlight(item.series, item.datapoint);
        }
    });
    // $("#footer").prepend("Flot " + $.plot.version + " &ndash; ");
});
