/*
 * Help.js - Code for handling the help section on the main screen
 * Author - robert@cosmicrealms.com
 */

var Help = null;

(function() {
	var _currentGameID = null;
	var _resizerDragDrop = null;
	var _currentWidth = null;
	var _resizerX = null;
	
	Help = 
	{
		init : function()
		{
			var helpSpace = document.getElementById("helpSpace");
			YAHOO.util.Dom.setStyle(helpSpace, "opacity", 0.90);
			
			E.add("helpIcon", "click", Help.toggle, null, Help);
			
			_resizerDragDrop = new YAHOO.util.DD("helpSpaceResizer", "helpSpaceResizer");
			_resizerDragDrop.startDrag = function(x, y) { Help.resizerDragStart(x, y); };
			_resizerDragDrop.onDrag = function(e) { Help.resizerDrag(e); };
			
			if(P.getBrowser()=="ie")
			{
				var helpIcon = document.getElementById("helpIcon");
				helpIcon.src = "images/help.gif";
			}
		},
		
		/**
		 * Called when resizer drag starts
		 * @param {Object} x The X point where the mousedown event was
		 * @param {Object} y The Y point where the mousedown event was
		 */
		resizerDragStart : function(x, y)
		{
			var playspace = document.getElementById("playspace");
			var playspaceRegion = D.region(playspace);
			
			var helpSpaceResizer = document.getElementById("helpSpaceResizer");
			var helpSpaceResizerXY = D.xy(helpSpaceResizer);
			
			_resizerDragDrop.resetConstraints();
			_resizerDragDrop.setXConstraint((helpSpaceResizerXY[0]-50), (playspaceRegion.right-50)-helpSpaceResizerXY[0]);
			_resizerDragDrop.setYConstraint(0, 0);
		}, 
		
		/**
		 * This is called when the resizer is dragged
		 * @param {Object} e
		 */
		resizerDrag : function(e)
		{
			var helpSpaceResizer = document.getElementById("helpSpaceResizer");
			var helpSpaceResizerXY = D.xy(helpSpaceResizer);
			
			_currentWidth += (_resizerX-helpSpaceResizerXY[0]);
			_resizerX = helpSpaceResizerXY[0];
			
			var helpSpace = document.getElementById("helpSpace");
			helpSpace.style.width = "" + _currentWidth + "px";
		},
	
		/**
		 * Shows the help section
		 */
		show : function()
		{
			var gameID = Options.get("optionGame");
			
			if(_currentGameID!==null && _currentGameID!==gameID)
				Help.clear();
			
			if(_currentGameID!==gameID)
			{
				_currentGameID = gameID;
				Help.populate();
			}
			
			_currentGameID = gameID;
	
			Help.resize();
			
			var helpSpace = document.getElementById("helpSpace");
			helpSpace.style.visibility = "visible";
			
			var helpSpaceResizer = document.getElementById("helpSpaceResizer");
			helpSpaceResizer.style.visibility = "visible";
		},
		
		/**
		 * Hides the help section
		 */
		hide : function()
		{
			var helpSpace = document.getElementById("helpSpace");
			helpSpace.style.visibility = "hidden";
			
			var helpSpaceResizer = document.getElementById("helpSpaceResizer");
			helpSpaceResizer.style.visibility = "hidden";
		},
		
		/**
		 * Clears the help contents
		 */
		clear : function()
		{
			var helpSpace = document.getElementById("helpSpace");
			
			var helpRulesDetailsSpotHeaders = D.getElementsByClassName(helpSpace, "span", "helpRulesDetailsSpotHeader");
			for(var i=0;i<helpRulesDetailsSpotHeaders.length;i++)
			{
				var helpRulesDetailsSpotHeader = helpRulesDetailsSpotHeaders[i];
				
				E.remove(helpRulesDetailsSpotHeader, "mouseover", Help.spotHeaderMouseOver);
				E.remove(helpRulesDetailsSpotHeader, "mouseout", Help.spotHeaderMouseOut);
			}
			
			D.clear(helpSpace);
			
			_currentGameID = null;
		},
		
		/**
		 * Will populate the help contents with the current gameID instructions
		 */
		populate : function()
		{
			var instructionsID = "gameChooserRightMiddle" + _currentGameID;
			var instructions = document.getElementById(instructionsID);
			if(instructions===null)
				return;
				
			var helpSpace = document.getElementById("helpSpace");
			for(var i=0;i<instructions.childNodes.length;i++)
			{
				helpSpace.appendChild(instructions.childNodes[i].cloneNode(true));
			}

			var helpRulesDetailsSpotHeaders = D.getElementsByClassName(helpSpace, "span", "helpRulesDetailsSpotHeader");
			for(var i=0;i<helpRulesDetailsSpotHeaders.length;i++)
			{
				var helpRulesDetailsSpotHeader = helpRulesDetailsSpotHeaders[i];
				
				var helpRulesDetailsSpotHeaderMagnifyingGlass = document.createElement("IMG");
				helpRulesDetailsSpotHeaderMagnifyingGlass.src = ((P.getBrowser()=="ie") ? "images/magnifying_glass.gif" : "images/magnifying_glass.png");
				helpRulesDetailsSpotHeader.appendChild(helpRulesDetailsSpotHeaderMagnifyingGlass);
				
				E.add(helpRulesDetailsSpotHeader, "mouseover", Help.spotHeaderMouseOver, helpRulesDetailsSpotHeaders[i]);
				E.add(helpRulesDetailsSpotHeader, "mouseout", Help.spotHeaderMouseOut, helpRulesDetailsSpotHeaders[i]);
			}
		},
		
		/**
		 * Called when the mouse is moved over a spot header
		 */
		spotHeaderMouseOver : function(e, theSpan)
		{
			var game = SOLITAIRE._currentGame;
			if(game===null)
				return;

			var cardWidth = parseInt(game._deck.getCardWidth(game._deckSize, game._cardScale), 10);
			var cardHeight = parseInt(game._deck.getCardHeight(game._deckSize, game._cardScale), 10);
			
			var spotName = theSpan.childNodes[0].nodeValue.toLowerCase();
			var spots = game.getSpots(spotName);
			
			for(var i=0;i<spots.length;i++)
			{
				var spot = spots[i];
				
				var spotWidth = cardWidth;
				var spotHeight = cardHeight;
				
				if(spot.cards.length>0)
				{
					var firstCard = spot.cards[(spot.cards.length-1)].getDomCard();
					var firstCardXY = D.xy(firstCard);
					var lastCard = spot.cards[0].getDomCard();
					var lastCardXY = D.xy(lastCard);
					
					spotWidth += lastCardXY[0]-firstCardXY[0];
					spotHeight += lastCardXY[1]-firstCardXY[1];
				}

				var highlightSpan = document.createElement("SPAN");
				highlightSpan.id = spotName + i;
				highlightSpan.className = "helpSpotHighlighter"; 
				highlightSpan.style.border = "5px solid yellow";
				highlightSpan.style.position = "absolute";
				highlightSpan.style.top = "" + (spot["screenY"]-((P.getBrowser()=="ie" || P.getBrowser()=="ie7") ? 7 : 5)) + "px";
				highlightSpan.style.left = "" + (spot["screenX"]-((P.getBrowser()=="ie" || P.getBrowser()=="ie7") ? 7 : 5)) + "px";
				highlightSpan.style.width = "" + spotWidth + "px";
				highlightSpan.style.height = "" + spotHeight + "px";
				
				document.body.appendChild(highlightSpan);
			}
		},
		
		/**
		 * Called when the mouse is moved out of a spot header
		 */
		spotHeaderMouseOut : function(e, theSpan)
		{
			var highlightSpans = D.getElementsByClassName(document.body, "span", "helpSpotHighlighter");
			for(var i=highlightSpans.length-1;i>=0;i--)
			{
				document.body.removeChild(highlightSpans[i]);
			}
		},
		
		/**
		 * Will resize the help and position the help icon
		 */
		resize : function()
		{
			var playspace = document.getElementById("playspace");
			var playspaceTopMargin = parseInt(playspace.style.marginTop);
			
			var playspaceXY = D.xy(playspace);
			playspaceXY[1] -= (playspaceTopMargin + ((P.getBrowser()=="ie" || P.getBrowser()=="ie7") ? 2 : 0));
			
			var playspaceHeight = D.height(playspace);
			playspaceHeight += playspaceTopMargin;
			var playspaceWidth = D.width(playspace);
			
			var helpIcon = document.getElementById("helpIcon");
			helpIcon.style.top = "" + (playspaceXY[1]) + "px";
			if(helpIcon.style.visibility!="visible")
				helpIcon.style.visibility = "visible";
			
			var helpSpace = document.getElementById("helpSpace");
			helpSpace.style.top = "" + playspaceXY[1] + "px";
			helpSpace.style.height= "" + playspaceHeight + "px";
			
			if(_currentWidth===null)
				_currentWidth = (playspaceWidth*0.28);
				
			helpSpace.style.width = "" + _currentWidth + "px";
			
			var helpSpaceRegion = D.region(helpSpace);
			_resizerX = (helpSpaceRegion.left-4);
			
			var helpSpaceResizer = document.getElementById("helpSpaceResizer");
			helpSpaceResizer.style.top = "" + (helpSpaceRegion.top + ((P.getBrowser()=="ie" || P.getBrowser()=="ie7") ? -2 : 0)) + "px";
			helpSpaceResizer.style.height= "" + (helpSpaceRegion.bottom-helpSpaceRegion.top) + "px";
			helpSpaceResizer.style.left = "" + _resizerX + "px";
		},
		
		/**
		 * Will either hide or show the game chooser
		 */
		toggle : function()
		{
			if(!Help.isShowing())
				Help.show();
			else
				Help.hide();
		},
		
		/**
		 * Returns true if the dialog box is showing or not
		 */
		isShowing : function()
		{
			var helpSpace = document.getElementById("helpSpace");
			
			return helpSpace.style.visibility==="visible";
		}
	};
})();
