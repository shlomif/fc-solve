/*
 * Menu.js - Menu functions
 * Author - robert@cosmicrealms.com
 */

var Menu = null;

(function() {
	var _menuBar = null;
	var _menuBarItemCurrentlyIsOver = null;
	var _menuCurrentlyIsOver = null;
	var _menuIsDisabled = false;
	var _menuOnShowHandler = null;
	var _menuRequiresClick = false;
	var _menuMouseIsDown = false;
	var _menuOpenCount = 0;
	
	Menu = 
	{
		init : function(menuBarID, onShowFunction)
		{
			_menuBar = document.getElementById(menuBarID);
			D.clearEmpty(_menuBar);
			
			E.add(document, "mouseup",  function(e) { Menu.mouseUpHandler(); }, null, Menu);
			
			var menuBarItems = _menuBar.getElementsByTagName("SPAN");
			for(var i=0;i<menuBarItems.length;i++)
			{
				if(menuBarItems[i].className!="menuBarItem")
					continue;
				var menuID = "menu" + S.replaceAll(menuBarItems[i].childNodes[0].data, " ", "");
				
				E.add(menuBarItems[i], "mouseover",  function(e, menuBarItemNumber) { _menuBarItemCurrentlyIsOver = menuBarItemNumber; Menu.mouseOverHandler(menuBarItemNumber); }, i, Menu);
				E.add(menuBarItems[i], "mouseout", function(e, menuBarItemNumber) { var selfMenuBarItemNumber = menuBarItemNumber; _menuBarItemCurrentlyIsOver = null; setTimeout(function() { Menu.mouseOutHandler(selfMenuBarItemNumber); }, 100); }, i, Menu);
				E.add(menuBarItems[i], "mousedown",  function(e, menuBarItemNumber) { _menuBarItemCurrentlyIsOver = menuBarItemNumber; Menu.mouseDownHandler(menuBarItemNumber, e); }, i, Menu);
				
				if(P.getBrowser()=="ie" || P.getBrowser()=="ie7")
				{
					E.add(menuBarItems[i], "selectstart", function(e) { return false; });
				}

				var menu = document.getElementById(menuID);
				E.add(menu, "mouseover",  function(e, menuBarItemNumber) { _menuCurrentlyIsOver = menuBarItemNumber; Menu.mouseOverHandler(menuBarItemNumber); }, i, Menu);
				E.add(menu, "mouseout", function(e, menuBarItemNumber) { var selfMenuBarItemNumber = menuBarItemNumber; _menuCurrentlyIsOver = null; setTimeout(function() { Menu.mouseOutHandler(selfMenuBarItemNumber); }, 100); }, i, Menu);
				
				var menuItems = menu.getElementsByTagName("TD");
				E.add(menuItems, "mouseover", function() { this.style.backgroundColor = "#333333"; });
				E.add(menuItems, "mouseout", function() { this.style.backgroundColor = "#000000"; });
			}
			
			if(typeof onShowFunction!=="undefined" && onShowFunction!==null)
				_menuOnShowHandler = onShowFunction;
		},
		
		disable : function(skipShading)
		{
			_menuIsDisabled = true;
			Menu.deselect();
			
			if(typeof skipShading=="undefined" || skipShading!==true)
				YAHOO.util.Dom.setStyle(_menuBar, "opacity", 0.10);
		},
		
		enable : function()
		{
			_menuIsDisabled = false;
			YAHOO.util.Dom.setStyle(_menuBar, "opacity", 0.90);
		},
		
		/**
		 * Called when the mouse is pushed down on the menu bar item itself
		 */
		mouseDownHandler : function(menuBarItemNumber, e)
		{
			if(_menuIsDisabled || !_menuRequiresClick)
				return;
				
			YAHOO.util.Event.stopEvent(e);
			
			_menuMouseIsDown = true;
			
			this.mouseOverHandler(menuBarItemNumber);
		},
		
		/**
		 * Called when the mouse is let go of
		 */
		mouseUpHandler : function()
		{
			_menuMouseIsDown = false;
		},
		
		/**
		 * Called when the mouse is moved over the menu bar item or the menu itself
		 */
		mouseOverHandler : function(menuBarItemNumber)
		{
			if(_menuIsDisabled)
				return;
			
			if(_menuBarItemCurrentlyIsOver===menuBarItemNumber && (!_menuRequiresClick || _menuMouseIsDown || _menuOpenCount>0))
			{
				var menuBarItems = _menuBar.getElementsByTagName("SPAN");
				var menuBarItem = menuBarItems[menuBarItemNumber];
				var menuID = "menu" + S.replaceAll(menuBarItem.childNodes[0].data, " ", "");

				menuBarItem.style.backgroundColor = "#333333";
				
				var menu = document.getElementById(menuID);
	
				var menuBarItemLoc = D.xy(menuBarItem);
				var menuBarItemHeight = D.height(menuBarItem);
				
				menu.style.top = "" + ((menuBarItemLoc[1] + menuBarItemHeight) - ((P.getBrowser()=="ie" || P.getBrowser()=="ie7") ? 2 : 0)) + "px";
				menu.style.left = "" + (menuBarItemLoc[0] - (menuBarItemNumber>0 ? ((P.getBrowser()=="ie" || P.getBrowser()=="ie7") ? 3 : 1) : ((P.getBrowser()=="ie7") ? 2 : 0))) + "px";
				menu.style.width = "auto";
				menu.style.height = "auto";
				
				var availableWidth = D.width(document.body);
				availableWidth -= (menuBarItemLoc[0] - (menuBarItemNumber>0 ? 1 : 0));
				
				var availableHeight = D.height(document.body);
				availableHeight -= menuBarItemHeight;
				
				menu.style.display = "block";
				
				var menuHeight = D.height(menu);
				if(menuHeight>availableHeight)
					menu.style.height = "" + (availableHeight-10) + "px";

				var menuWidth = D.width(menu);
				if(menuWidth>availableWidth)
					menu.style.width = "" + (availableWidth-10) + "px";
					
				if(menuHeight>availableHeight)
					menu.scrollTop = 0;
					
				if(menuWidth>availableWidth)
					menu.scrollLeft = 0;
					
				if(P.getBrowser()=="opera")
				{
					if(menuHeight<=availableHeight)
					{
						menu.style.overflowY = "hidden";
					}
					else
					{
						menu.style.overflowY = "auto";
					}	
					
					if(menuWidth<=availableHeight)
					{
						menu.style.overflowX = "hidden";
					}
					else
					{
						menu.style.overflowX = "auto";
					}
				}
									
				_menuOpenCount++;
									
				if(_menuOnShowHandler!==null)
					_menuOnShowHandler(menuBarItemNumber);
			}
		},
		
		/**
		 * Called when the mouse is moved over the menu bar item or the menu itself
		 */
		mouseOutHandler : function(menuBarItemNumber)
		{
			if((_menuBarItemCurrentlyIsOver===null || _menuBarItemCurrentlyIsOver!==menuBarItemNumber) &&
			   (_menuCurrentlyIsOver===null || _menuCurrentlyIsOver!==menuBarItemNumber))
			{
				var menuBarItems = _menuBar.getElementsByTagName("SPAN");
				var menuBarItem = menuBarItems[menuBarItemNumber];
				var menuID = "menu" + S.replaceAll(menuBarItem.childNodes[0].data, " ", "");
				
				menuBarItem.style.backgroundColor = "#000000";
				
				var menu = document.getElementById(menuID);
				menu.style.display = "none";
				
				_menuOpenCount--;
				if(_menuOpenCount<0)
					_menuOpenCount = 0;
			}
		},
		
		/**
		 * Deselects the menu
		 */
		deselect : function()
		{
			var menuBarItemNumber = (_menuBarItemCurrentlyIsOver!==null ? _menuBarItemCurrentlyIsOver : _menuCurrentlyIsOver);
			_menuBarItemCurrentlyIsOver = null;
			_menuCurrentlyIsOver = null;
			
			if(menuBarItemNumber!==null)
				Menu.mouseOutHandler(menuBarItemNumber);
		},
		
		/**
		 * Sets whether or not the menu requires clicking to activate
		 */
		setRequiresClick : function(requiresClick)
		{
			_menuRequiresClick = requiresClick;
		}
	};
})();


