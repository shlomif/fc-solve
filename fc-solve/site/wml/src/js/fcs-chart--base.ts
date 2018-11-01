export function fc_solve__chart_bind(chart_selector: string, plot): void {
    $(chart_selector).bind("plothover", (event, pos, item) => {
        {
            const str = `(${pos.x.toFixed(2)},${pos.y.toFixed(2)})`;
            $("#hoverdata").text(str);
        }

        {
            if (item) {
                const x = item.datapoint[0] / 1000000;
                const y = item.datapoint[1].toFixed(2);

                $("#tooltip")
                    .html(`${item.series.label} of ${x.toFixed(1)} M = ${y}sec`)
                    .css({ top: item.pageY + 5, left: item.pageX + 5 })
                    .fadeIn(200);
            } else {
                $("#tooltip").hide();
            }
        }
        return false;
    });

    $(chart_selector).bind("plotclick", (event, pos, item) => {
        if (item) {
            $("#clickdata").text(
                ` - click point ${item.dataIndex} in ${item.series.label}`,
            );
            plot.highlight(item.series, item.datapoint);
        }
    });
}
