/*
 * StatisticsView.js - Change log functions
 * Author - robert@cosmicrealms.com
 */

var StatisticsView = new DialogBox("statisticsView", { "percentageWidth"  : P.getBrowser()==="ie7" || P.getBrowser()==="ie" ? 0.95 : 0.95,
											     	   "percentageHeight" : P.getBrowser()==="ie7" || P.getBrowser()==="ie" ? 0.85 : 0.85,
											     	   "overflowable"     : true});
							
/**
 * This is called when the statistics view is first initialized
 */													   
StatisticsView.onInit = function()
{
	StatisticsView.tabView = new YAHOO.widget.TabView('statisticsViewTabs');
	StatisticsView.tabView.addListener("activeTabChange", StatisticsView.tabChanged);

	var statisticsViewGameDropdown = document.getElementById("statisticsViewGameDropdown");
	E.add(statisticsViewGameDropdown, "change", StatisticsView.gameDropdownChanged);
};

/**
 * Whenever the statistics view is shown, this is called after it's visible
 */
StatisticsView.afterShow = function()
{
	var statisticsViewTabsContentContainer = document.getElementById("statisticsViewTabsContentContainer");
	var statisticsViewTabsContentContainerChildren = D.getElementsByTagName(statisticsViewTabsContentContainer, "DIV");
	var statisticsViewContents = document.getElementById("statisticsViewContents");
	var statisticsViewTabsContainer = document.getElementById("statisticsViewTabsContainer");
	var statisticsViewGameDropdownContainer = document.getElementById("statisticsViewGameDropdownContainer");

	for(var i=0;i<statisticsViewTabsContentContainerChildren.length;i++)
	{
		var tabContainer = statisticsViewTabsContentContainerChildren[i];

		var containerHeight = (D.height(statisticsViewContents) - D.height(statisticsViewTabsContainer) - D.height(statisticsViewGameDropdownContainer));
		containerHeight -= (P.getBrowser()=="ie" ? 40 : 20);
		tabContainer.style.height = containerHeight + "px";
		
		var fieldsets = tabContainer.getElementsByTagName("fieldset");
		for(var m=0;m<fieldsets.length;m++)
		{
			var fieldset = fieldsets[m];
			StatisticsView.clearFieldset(fieldset);
		}	
	}
		
	if(P.getBrowser()==="gecko" || P.getBrowser()==="opera")
	{
		var statisticsViewGameDropdownLabel = document.getElementById("statisticsViewGameDropdownLabel");
		statisticsViewGameDropdownLabel.style.position = "relative";
		statisticsViewGameDropdownLabel.style.top = "-4px";
	}
	
	StatisticsView.tabsSeen = [];
	
	StatisticsView.tabChanged();
};

/**
 * Called when the game dropdown should be set to something
 * @param {Object} gameID
 */
StatisticsView.setGame = function(gameID)
{
	var statisticsViewGameDropdown = document.getElementById("statisticsViewGameDropdown");
	
	for(var i=0;i<statisticsViewGameDropdown.options.length;i++)
	{
		if(statisticsViewGameDropdown.options[i].value===gameID)
		{
			statisticsViewGameDropdown.options[i].selected = true;
			statisticsViewGameDropdown.selectedIndex = i;
		}
		else
		{
			statisticsViewGameDropdown.options[i].selected = false;
		}
	}
	
	if(StatisticsView.isShowing())
		StatisticsView.gameDropdownChanged();
};

/**
 * This is called when the game dropdown is changed
 * @param {Object} e
 */
StatisticsView.gameDropdownChanged = function(e)
{
	StatisticsView.tabsSeen = [];
	
	StatisticsView.tabChanged();
};

/**
 * This is called after someone switches a tab (also called explicitly when the dialog is shown to handle the pre-selected tab)
 * @param {Object} e
 */
StatisticsView.tabChanged = function(e)
{
	var activeIndex = StatisticsView.tabView.get("activeIndex");

	if(A.find(StatisticsView.tabsSeen, activeIndex)!==-1)
		return;
			
	StatisticsView.tabsSeen.push(activeIndex);
	
	var statisticsViewTabsContentContainer = document.getElementById("statisticsViewTabsContentContainer");
	var statisticsViewTabsContentContainerChildren = D.getElementsByTagName(statisticsViewTabsContentContainer, "DIV");
	
	var activeTabContainer =  statisticsViewTabsContentContainerChildren[activeIndex];
	
	var statisticsViewContents = document.getElementById("statisticsViewContents");
	var statisticsViewTabsContainer = document.getElementById("statisticsViewTabsContainer");
	var statisticsViewGameDropdownContainer = document.getElementById("statisticsViewGameDropdownContainer");
	var statisticsViewGameDropdown = document.getElementById("statisticsViewGameDropdown");
	var selectedGameID = statisticsViewGameDropdown.options[statisticsViewGameDropdown.selectedIndex].value;
	
	var containerHeight = (D.height(statisticsViewContents) - D.height(statisticsViewTabsContainer) - D.height(statisticsViewGameDropdownContainer));
	containerHeight -= (P.getBrowser()=="ie" ? 40 : 20);

	activeTabContainer.style.height = containerHeight + "px";
		
	var fieldsets = activeTabContainer.getElementsByTagName("fieldset");
	
	for(var i=0;i<fieldsets.length;i++)
	{
		var fieldset = fieldsets[i];
	
		StatisticsView.clearFieldset(fieldset);
		
		var spinnerContainer = document.createElement("SPAN");
		
		var spinner = document.createElement("IMG");
		spinner.src = "images/spinner.gif";
		spinner.style.position = "relative";
		spinner.style.top = "6px";
		spinner.style.paddingRight = "3px";
		
		spinnerContainer.appendChild(spinner);
		spinnerContainer.appendChild(document.createTextNode("Loading..."));
		
		fieldset.appendChild(spinnerContainer);
		
		if(activeIndex===0)
		{
			YAHOO.util.Connect.asyncRequest("POST", "getStats",
											{ success  : function(data) { StatisticsView.fieldsetDataLoadFinish(data); },
											  failure  : function(data) { StatisticsView.fieldsetDataLoadFinish(data); },
											  argument : { "fieldsetIndex" :  i, "fieldsetClassname" : fieldset.className } },
											O.serialize({ scope   : activeIndex===0 ? "local" : "global",
														  dataSet : fieldset.className,
														  gameID  : selectedGameID }));
		}
		else
		{
			var targetURL = "hourly/";
			if(selectedGameID.length>0)
				targetURL += selectedGameID + "_";
				
			targetURL += fieldset.className;
			
			YAHOO.util.Connect.asyncRequest("POST", targetURL,
											{ success  : function(data) { StatisticsView.fieldsetDataLoadFinish(data); },
											  failure  : function(data) { StatisticsView.fieldsetDataLoadFinish(data); },
											  argument : { "fieldsetIndex" :  i, "fieldsetClassname" : fieldset.className } });
		}
	}
};

/**
 * When data comes back (or FAILS) for a given fieldset, this function takes care of creating the charts/tables or showing an error
 * @param {Object} data Data object from the server
 */
StatisticsView.fieldsetDataLoadFinish = function(data)
{
	var statisticsViewTabsContentContainer = document.getElementById("statisticsViewTabsContentContainer");
	var statisticsViewTabsContentContainerChildren = D.getElementsByTagName(statisticsViewTabsContentContainer, "DIV");
	var activeIndex = StatisticsView.tabView.get("activeIndex");
	var activeTabContainer =  statisticsViewTabsContentContainerChildren[activeIndex];
	var fieldsets = activeTabContainer.getElementsByTagName("fieldset");
	var fieldset = fieldsets[data.argument["fieldsetIndex"]];
	if(fieldset.className!=data.argument["fieldsetClassname"])
		return;
	
	StatisticsView.clearFieldset(fieldset);
	
	// Calc date width
	var statisticsViewSizeCheck = document.getElementById("statisticsViewSizeCheck");
	D.clear(statisticsViewSizeCheck);
	statisticsViewSizeCheck.appendChild(document.createTextNode("2008-12-27"));
	var dateWidth = D.width(statisticsViewSizeCheck);

	// Calc 100 million number width
	var statisticsViewSizeCheck = document.getElementById("statisticsViewSizeCheck");
	D.clear(statisticsViewSizeCheck);
	statisticsViewSizeCheck.appendChild(document.createTextNode("777,777,777"));
	var millionWidth = D.width(statisticsViewSizeCheck);
	D.clear(statisticsViewSizeCheck);
	
	var dataObject = null;
		
	if(data.hasOwnProperty("responseText") && data["responseText"].charAt(0)!='[')
	{
		if(fieldset.className=="statisticsViewInfo" && activeIndex===0)
		{
			dataObject = [];
			dataObject.push({ label : "Hands Dealt", value : I.formatWithCommas(Stats.get("totalDealt")) });
			dataObject.push({ label : "Hands Played", value : I.formatWithCommas(Stats.get("totalPlayed")) });
			dataObject.push({ label : "Hands Won", value : I.formatWithCommas(Stats.get("totalWon"))});
			dataObject.push({ label : "Total Moves Made", value : I.formatWithCommas(Stats.get("totalMoves")) });
			dataObject.push({ label : "Total Playtime", value : I.formatWithCommas(Stats.get("totalTime")) });
		}
		else
		{
			fieldset.appendChild(document.createTextNode("Data Not Available"));
			return;			
		}
	}
	
	if(dataObject===null)
		dataObject = eval("(" + data["responseText"] + ")");

	var hexColor = MochiKit.Color.Color.fromHexString;		
	
	var chartObject = null;
	if(fieldset.className=="statisticsViewPopularPie" || fieldset.className=="statisticsViewTimeline")
	{
		if(P.getBrowser()=="konqueror")
		{
			var canvasCheck = document.createElement("canvas");
			if(typeof canvasCheck.getContext=="undefined")
			{
				fieldset.appendChild(document.createTextNode("Sorry, but your version of Konqueror doesn't support the cool charts I want to show here."));
				fieldset.appendChild(document.createElement("BR"));
				fieldset.appendChild(document.createTextNode("You can update your Konqueror to the latest version or use a different browser."));
				fieldset.appendChild(document.createElement("BR"));
				fieldset.appendChild(document.createTextNode("The following browsers work great: Firefox, Safari, Opera, IE"));
				return;
			}
		}
		
		var chartContainer = document.createElement("DIV");
		
		var targetHeight = D.height(fieldset);
		targetHeight -= 30;
		
		var targetWidth = D.width(fieldset);
		targetWidth -= (P.getBrowser()=="ie" ? 40 : 10);
		
		var widthToHeightRatio = D.width(fieldset) / D.height(fieldset);
		
		chartObject = document.createElement("CANVAS");
		chartObject.height = targetHeight;
		chartObject.width = targetWidth;
		chartObject.id = fieldset.className + "" + activeIndex;
		chartContainer.appendChild(chartObject);
		
		var widthToHeightRatio = D.width(fieldset) / D.height(fieldset);
	
		fieldset.appendChild(chartContainer);		
	}
	
	if(fieldset.className=="statisticsViewPopularPie")
	{		
		var pieColors = [ hexColor("#FF5555"),
				          hexColor("#55FF55"),
				          hexColor("#5555FF"),
				          hexColor("#55FFFF"),
				          hexColor("#FF55FF"),
						  hexColor("#FFFF55"),
						  hexColor("#FFAA55"),
						  hexColor("#AAAAAA"),
				          hexColor("#000000") ];	
						  	
		var pieDataSet = [];
		var pieLabelTicks = [];

		for(var i=0;i<dataObject.length;i++)
		{
			var objectPacket = dataObject[i];
			
			if(typeof objectPacket=="undefined" || objectPacket===null)
				continue;
			
			var labelObject = {};
			labelObject.label = gGames.hasOwnProperty(objectPacket["label"]) ? gGames[objectPacket["label"]]._gameName : objectPacket["label"];
			labelObject.v = i;
			pieLabelTicks.push(labelObject);
			
			var pair = [];
			pair.push(i);
			pair.push(objectPacket["value"]);
			pieDataSet.push(pair);
		}
		
		var pieLayout = new PlotKit.Layout("pie", { "xTicks" : pieLabelTicks });
		pieLayout.addDataset("pieDataSet", pieDataSet);
		pieLayout.evaluate();
		
		var pieCanvas = MochiKit.DOM.getElement(chartObject.id);
		
		var piePlotterOptions = { "colorScheme"       : pieColors,
								  "drawBackground"	  : false,
								  "axisLabelColor"    : hexColor("#000000"),
								  "pieRadius"		  : 0.45,
								  "axisLabelWidth"    : Math.floor(70*widthToHeightRatio),
								  "axisLabelFont"     : "Arial",
       							  "axisLabelFontSize" : 11 };
		var piePlotter = new PlotKit.SweetCanvasRenderer(pieCanvas, pieLayout, piePlotterOptions);
		piePlotter.render();
	}
	else if(fieldset.className=="statisticsViewTimeline")
	{
		var timelineDataSet = [];
		var timelineXTicks = [];
		
		for(var i=0;i<dataObject.length;i++)
		{
			var objectPacket = dataObject[i];
			
			var xLabelObject = {};
			xLabelObject.label = objectPacket["label"];
			xLabelObject.v = i;
			timelineXTicks.push(xLabelObject);
						
			var pair = [];
			pair.push(i);
			pair.push(objectPacket["value"]);
			timelineDataSet.push(pair);
		}
		
		var timelineLayout = new PlotKit.Layout("line", { "yTickPrecision" : 0,
														  "yNumberOfTicks" : 6,
														  "xTicks" : timelineXTicks });
		timelineLayout.addDataset("timelineDataSet", timelineDataSet);
		timelineLayout.evaluate();
		
		var timelineCanvas = MochiKit.DOM.getElement(chartObject.id);
		
		var timelinePlotterOptions = { "colorScheme"       : [ hexColor("#FF5555"), hexColor("#000000") ],
									   "drawBackground"	   : false,
									   "padding"		   : { left : millionWidth*1.05, right : 30, top : 5, bottom : 10 },
								  	   "axisLabelColor"    : hexColor("#000000"),
								  	   "axisLabelWidth"    : dateWidth*1.05,
								       "axisLabelFont"     : "Arial",
       							       "axisLabelFontSize" : 11 };
		var timelinePlotter = new PlotKit.SweetCanvasRenderer(timelineCanvas, timelineLayout, timelinePlotterOptions);
		timelinePlotter.render();
	}
	else if(fieldset.className=="statisticsViewInfo")
	{
		var infoTable = document.createElement("TABLE");
		infoTable.className = "statisticsViewInfoTable";
		
		var infoTBody = document.createElement("TBODY");
		
		var totalDealt = null;
		var totalPlayed = null;
		var totalWon = null;
		var totalPlaytime = null;
		
		for(var i=0;i<dataObject.length;i++)
		{
			var objectPacket = dataObject[i];		
			if(objectPacket["label"]=="Hands Dealt")
				totalDealt = objectPacket["value"];
			else if(objectPacket["label"]=="Hands Played")
				totalPlayed = objectPacket["value"];
			else if(objectPacket["label"]=="Hands Won")
				totalWon = objectPacket["value"];
			else if(objectPacket["label"]=="Total Playtime")
				totalPlaytime = objectPacket["value"];
		}

		var percentagePlayed = null;
		if(totalPlayed!==null && totalDealt!==null && totalPlayed>0 && totalDealt>0)
			percentagePlayed = (totalPlayed / totalDealt) * 100;
		
		var percentageWon = null;
		if(totalPlayed!==null && totalWon!==null && totalWon>0 && totalPlayed>0)
			percentageWon = (totalWon / totalPlayed) * 100;
			
		if(totalPlaytime!=null && totalPlayed!=null)
		{
			var avgTimePacket = {};
			avgTimePacket["label"] = "Average Length";
			avgTimePacket["value"] = totalPlaytime/totalPlayed;
			
			dataObject.splice(dataObject.length-2, 0, avgTimePacket);
		}
				
		for(var i=0;i<dataObject.length;i++)
		{
			var objectPacket = dataObject[i];			
			
			var infoTR = document.createElement("TR");
			
			var labelTD = document.createElement("TD");
			labelTD.className = "statisticsViewInfoLabel";
			labelTD.appendChild(document.createTextNode(objectPacket["label"] + ":"));

			var valueTD = document.createElement("TD");
			valueTD.className = "statisticsViewInfoValue";
			
			var value = objectPacket["value"];
			if(objectPacket["label"]=="Total Playtime" || objectPacket["label"]=="Average Length")
				value = I.secondsAsHumanReadable(value);
			else
				value = I.formatWithCommas(value);
				
			if(objectPacket["label"]=="Hands Played" && percentagePlayed!==null)
				value += " (" + percentagePlayed.toFixed(2) + "%)";
			else if(objectPacket["label"]=="Hands Won" && percentageWon!==null)
				value += " (" + percentageWon.toFixed(2) + "%)";
				
			valueTD.appendChild(document.createTextNode(value));
			
			infoTR.appendChild(labelTD);
			infoTR.appendChild(valueTD);
			
			infoTBody.appendChild(infoTR);
		}
		
		infoTable.appendChild(infoTBody);
		fieldset.appendChild(infoTable);
	}
	else if(fieldset.className=="statisticsViewLeaderboardShortestTime" ||
			fieldset.className=="statisticsViewLeaderboardFewestMoves" ||
			fieldset.className=="statisticsViewLeaderboardTotalNumberWon" ||
			fieldset.className=="statisticsViewLeaderboardHighestPercentageWon")
	{
		var leaderTableContainer = document.createElement("DIV");
		leaderTableContainer.className = "statisticsViewLeaderboardTableContainer";
		
		var leaderTable = document.createElement("TABLE");
		leaderTable.className = "statisticsViewLeaderboardTable";
		
		var leaderTBody = document.createElement("TBODY");
		
		if(dataObject.length>0)
		{
			var firstObjectPacket = dataObject[0];		
			
			var headerTR = document.createElement("TR");
			
			var numberTH = document.createElement("TH");
			numberTH.appendChild(document.createTextNode(""));
			
			var labelTH = document.createElement("TH");
			labelTH.appendChild(document.createTextNode("User"));
			labelTH.className = "statisticsViewLeaderboardTableHeaderUser";
			
			if(firstObjectPacket.hasOwnProperty("gameid"))
			{
				var gameTH = document.createElement("TH");
				gameTH.appendChild(document.createTextNode("Game"));
			}
			
			var valueTH = document.createElement("TH");
			valueTH.appendChild(document.createTextNode((fieldset.className=="statisticsViewLeaderboardShortestTime" ? "Time" :
			 											 (fieldset.className=="statisticsViewLeaderboardFewestMoves" ? "Moves" :
														 (fieldset.className=="statisticsViewLeaderboardTotalNumberWon" ? "Times" : "Percentage")))));
			
			if(firstObjectPacket.hasOwnProperty("when"))
			{
				var whenTH = document.createElement("TH");
				whenTH.appendChild(document.createTextNode("When"));
			}
			
			if(firstObjectPacket.hasOwnProperty("timesWon"))
			{
				var timesWonTH = document.createElement("TH");
				timesWonTH.appendChild(document.createTextNode("Times Won"));
			}
			
			headerTR.appendChild(numberTH);
			headerTR.appendChild(labelTH);
			if(firstObjectPacket.hasOwnProperty("gameid"))
				headerTR.appendChild(gameTH);
			headerTR.appendChild(valueTH);
			if(firstObjectPacket.hasOwnProperty("when"))
				headerTR.appendChild(whenTH);
			if(firstObjectPacket.hasOwnProperty("timesWon"))
				headerTR.appendChild(timesWonTH);
			
			leaderTBody.appendChild(headerTR);
	
			for(var i=0;i<dataObject.length;i++)
			{
				var objectPacket = dataObject[i];			
				
				var leaderTR = document.createElement("TR");
				
				var numberTD = document.createElement("TD");
				numberTD.appendChild(document.createTextNode((i+1 + ".")));
				numberTD.className = "statisticsViewLeaderboardTableNumber";
				
				var labelTD = document.createElement("TD");
				labelTD.appendChild(document.createTextNode(objectPacket["label"]));
				labelTD.className = "statisticsViewLeaderboardTableUser";
				
				if(objectPacket.hasOwnProperty("gameid"))
				{
					var gameTD = document.createElement("TD");
					gameTD.appendChild(document.createTextNode(gGames[objectPacket["gameid"]]._gameName));
				}
	
				var valueTD = document.createElement("TD");
				var valueText = (fieldset.className=="statisticsViewLeaderboardShortestTime" ? I.secondsAsClock(objectPacket["value"]) :
								(fieldset.className=="statisticsViewLeaderboardHighestPercentageWon" ? (objectPacket["value"]) : I.formatWithCommas(objectPacket["value"])));
				valueTD.appendChild(document.createTextNode(valueText));
				valueTD.className = "statisticsViewLeaderboardTableValue";
				
				if(objectPacket.hasOwnProperty("when"))
				{
					var whenTD = document.createElement("TD");
					whenTD.appendChild(document.createTextNode(objectPacket["when"]));
				}

				if(objectPacket.hasOwnProperty("timesWon"))
				{
					var timesWonTD = document.createElement("TD");
					timesWonTD.appendChild(document.createTextNode(I.formatWithCommas(objectPacket["timesWon"])));
					timesWonTD.className = "statisticsViewLeaderboardTableValue";
				}
				
				leaderTR.appendChild(numberTD);
				leaderTR.appendChild(labelTD);
				if(objectPacket.hasOwnProperty("gameid"))
					leaderTR.appendChild(gameTD);
				leaderTR.appendChild(valueTD);
				if(objectPacket.hasOwnProperty("when"))
					leaderTR.appendChild(whenTD);
				if(objectPacket.hasOwnProperty("timesWon"))
					leaderTR.appendChild(timesWonTD);
					
				leaderTBody.appendChild(leaderTR);
			}
			
			if(fieldset.className=="statisticsViewLeaderboardHighestPercentageWon")
			{
				var disclosureTR = document.createElement("TR");
				var disclosureTD = document.createElement("TD");
				disclosureTD.colSpan = "5";
				disclosureTD.className = "statisticsViewLeaderboardHighestPercentageWonDisclosure";
				disclosureTD.appendChild(document.createTextNode("At least 50 wins required to qualify."));
				disclosureTR.appendChild(disclosureTD);
				leaderTBody.appendChild(disclosureTR);
			}
		}
		
		leaderTable.appendChild(leaderTBody);
		leaderTableContainer.appendChild(leaderTable);

		var legends = fieldset.getElementsByTagName("LEGEND");
		leaderTableContainer.style.height = "" + (D.height(fieldset)-D.height(legends[0])-5) + "px";
		leaderTableContainer.style.width = "" + (D.width(fieldset)-10) + "px";

		fieldset.appendChild(leaderTableContainer);
	}
};

/**
 * This will clear a fieldset, every child node except the first one which we assume is a legend and we want to keep
 * @param {Object} fieldset
 */
StatisticsView.clearFieldset = function(fieldset)
{
	for(var i=fieldset.childNodes.length-1;i>=0;i--)
	{
		if(fieldset.childNodes[i].nodeName!="LEGEND")
			fieldset.removeChild(fieldset.childNodes[i]);
	}
};
