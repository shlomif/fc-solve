function chart_data(data_selector: string, chart_selector: string) {
	var series = [];
	var lines = $(data_selector).text().split("\n");
	lines.shift();
	lines.forEach(
		function (l) {
			var fields = l.split("\t");
			series[0].push([parseFloat(fields[0]),parseFloat(fields[1])]);
		}
	);
    var plot = $.plot($(chart_selector), [
            { data: series, label: "queue-items(iters)", },
        ],
        {
            series: {
                lines: {
                    show: true
                },
                points: {
                    show: true
                }
            },
            grid: {
                hoverable: true,
                clickable: true
            },
        }
    );

		$("<div id='tooltip'></div>").css({
			position: "absolute",
			display: "none",
			border: "1px solid #fdd",
			padding: "2px",
			"background-color": "#fee",
			opacity: 0.80
		}).appendTo("body");

		$(chart_selector).bind("plothover", function (event, pos, item) {

            {
				var str = "(" + pos.x.toFixed(2) + ", " + pos.y.toFixed(2) + ")";
				$("#hoverdata").text(str);
            }

            {
				if (item) {
					var x = (item.datapoint[0] / 1000000),
						y = item.datapoint[1].toFixed(2);

					$("#tooltip").html(item.series.label + " of " + x.toFixed(1) + "M = " + y + "sec")
						.css({top: item.pageY+5, left: item.pageX+5})
						.fadeIn(200);
				} else {
					$("#tooltip").hide();
				}
			}
            return false;
		});

		$(chart_selector).bind("plotclick", function (event, pos, item) {
			if (item) {
				$("#clickdata").text(" - click point " + item.dataIndex + " in " + item.series.label);
                plot.highlight(item.series, item.datapoint);
			}
		});
	// $("#footer").prepend("Flot " + $.plot.version + " &ndash; ");
}
