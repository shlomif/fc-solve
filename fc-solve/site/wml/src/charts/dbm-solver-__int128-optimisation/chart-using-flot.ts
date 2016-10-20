/// <reference path="typings/index.d.ts" />
/// <reference path="jquery.flot.d.ts" />

$(function() {
	var series = [[],[]];
	var lines = $("#data").text().split("\n");
	lines.shift();
	lines.forEach(
		function (l) {
			var fields = l.split("\t");
			var x:number = parseFloat(fields[0]);
			var y1:number = parseFloat(fields[1]);
			var y2:number = parseFloat(fields[2]);
			series[0].push([x,y1]);
			series[1].push([x,y2]);
		}
	);
    var plot = $.plot($("#placeholder"), [
            { data: series[0], label: "old-time(iters)", },
            { data: series[1], label: "new-time(iters)", },
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

		$("#placeholder").bind("plothover", function (event, pos, item) {

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

		$("#placeholder").bind("plotclick", function (event, pos, item) {
			if (item) {
				$("#clickdata").text(" - click point " + item.dataIndex + " in " + item.series.label);
                plot.highlight(item.series, item.datapoint);
			}
		});
	// $("#footer").prepend("Flot " + $.plot.version + " &ndash; ");
});
