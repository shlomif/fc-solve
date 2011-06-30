/*
 * GameChooser.js - Game Chooser functions
 * Author - robert@cosmicrealms.com
 */

var GameChooser = new DialogBox("gameChooser", { "percentageWidth"        : 0.80,
											     "percentageHeight"       : 0.80,
											     "overflowable"           : true});

/**
 * Called right after the dialog is init'ed
 */
GameChooser.onInit = function()
{	
    var gameChooserListRecentlyPlayed = document.getElementById("gameChooserListRecentlyPlayed");
	D.clear(gameChooserListRecentlyPlayed);
	
	GameChooser.tree = T.listToTree("gameChooserList", "gameChooserTree");
	
	var gameChooserList = document.getElementById("gameChooserList");
	gameChooserList.parentNode.removeChild(gameChooserList);
	
	GameChooser.tree.subscribe("labelClick", GameChooser.gameClicked);
	GameChooser.tree.subscribe("expandComplete", function() { T.boldFolders(GameChooser.tree.getRoot()); });
	GameChooser.selectedNode = null;
	
	GameChooser.hasShownOnce = false;
	
	E.add("gameChooserCancelButton", "click", GameChooser.hide, null, GameChooser);
	
	var gameChooserRightTop = document.getElementById("gameChooserRightTop");
	var previewImages = gameChooserRightTop.getElementsByTagName("IMG");
	for(var i=0;i<previewImages.length;i++)
	{
		previewImages[i].style.display = "none";
	}
	
	var gameChooserRightMiddle = document.getElementById("gameChooserRightMiddle");
	var gameChooserRightMiddleChildren = D.getElementsByTagName(gameChooserRightMiddle, "DIV");
	for(var i=0;i<gameChooserRightMiddleChildren.length;i++)
	{
		gameChooserRightMiddleChildren[i].style.display = "none";
	}
};

/**
 * Will resize the game choose area as needed
 */
GameChooser.resize = function()
{
	if(this.panelHeight===null || this.panelWidth===null)
		return;
	
	var gameChooserHeader = document.getElementById("gameChooserHeader");
	var finalHeight = this.panelHeight-(D.height(gameChooserHeader))-10;

	var gameChooserLeft = document.getElementById("gameChooserLeft");
	gameChooserLeft.style.height = finalHeight + "px";
	
	var gameChooserRight = document.getElementById("gameChooserRight");
	gameChooserRight.style.height = finalHeight + "px";
	
	var bottomHeight = D.height(document.getElementById("gameChooserOKButton")) + 10;
	
	var gameChooserRightBottom = document.getElementById("gameChooserRightBottom");
	gameChooserRightBottom.style.height = bottomHeight + "px";
	
	var gameChooserRightTop = document.getElementById("gameChooserRightTop");
	var gameChooserRightTopHeight = D.height(gameChooserRightTop);
	
	var gameChooserRightMiddle = document.getElementById("gameChooserRightMiddle");
	var middleHeight = ((finalHeight-bottomHeight)-gameChooserRightTopHeight);
	middleHeight = Math.max(middleHeight, 30);
	gameChooserRightMiddle.style.height = "" + middleHeight + "px";
};

/**
 * Called before the dialog is shown
 */
GameChooser.beforeShow = function()
{
	var nodeToSelect = GameChooser.selectedNode;
	
	// Update our recently played games list
	var recentlyPlayedGamesNode = T.getNodeWithLabelMatching(GameChooser.tree.getRoot(), "Games - Recently Played");
	
	if(nodeToSelect!=null && nodeToSelect.parent==recentlyPlayedGamesNode)
		nodeToSelect = null;
		
	GameChooser.tree.removeChildren(recentlyPlayedGamesNode);
	for(var i=SOLITAIRE._recentlyPlayedGames.length-1;i>=0;i--)
	{
		var gameID = SOLITAIRE._recentlyPlayedGames[i];
		var gameName = gGames[gameID]._gameName;
		
		var gameNode = new YAHOO.widget.TextNode(gameName, recentlyPlayedGamesNode, false);
		gameNode.data = gameID;
	}
	GameChooser.tree.draw();
	//T.boldFolders(GameChooser.tree.getRoot());
	
	if(nodeToSelect===null)
	{
		var folderNode = T.getNodeWithLabelMatching(GameChooser.tree.getRoot(), "Games - Recently Played");
		nodeToSelect = T.getNodeWithDataMatching(folderNode, Options.get("optionGame"));
	}
	
	var alphabeticalFolder = T.getNodeWithLabelMatching(GameChooser.tree.getRoot(), "Games - Alphabetical");
	alphabeticalFolder.expand();
	
	recentlyPlayedGamesNode.expand();

	T.openToNode(nodeToSelect);
	GameChooser.gameClicked(nodeToSelect);
};

/**
 * Called right after the dialog is shown
 */
GameChooser.afterShow = function()
{
	GameChooser.resize();
	
	if(!GameChooser.hasShownOnce && P.getBrowser()=="ie7")
	{
		GameChooser.hasShownOnce = true;
		GameChooser.gameClicked(GameChooser.selectedNode);
	}
};

/**
 * Called when a game is clicked
 * @param {Object} node
 */
GameChooser.gameClicked = function(node)
{
	if(node.data===null)
		return;
		
	if(GameChooser.selectedNode!==null)
	{
		YAHOO.util.Dom.removeClass(GameChooser.selectedNode.getEl(), "treeSelectedRow");
		if(P.getBrowser()=="safari")
			YAHOO.util.Dom.addClass(GameChooser.selectedNode.getEl(), "treeNonSelectedRow");
	}

	GameChooser.selectedNode = node;
	YAHOO.util.Dom.addClass(GameChooser.selectedNode.getEl(), "treeSelectedRow");
	
	var gameChooserRight = document.getElementById("gameChooserRight");
	gameChooserRight.style.visibility = "hidden";
	
	var treeNodes = T.getAllNodes(GameChooser.tree.getRoot());
	
	for(var i=0;i<treeNodes.length;i++)
	{
		var treeNode = treeNodes[i];
		if(treeNode.data===null)
			continue;
		
		var gameChooserRightTopGame = document.getElementById("gameChooserRightTop" + treeNode.data);
		gameChooserRightTopGame.style.display = "none";
		
		var gameChooserRightMiddleGame = document.getElementById("gameChooserRightMiddle" + treeNode.data);
		gameChooserRightMiddleGame.style.display = "none";
	}

	var gameChooserRightTopGame = document.getElementById("gameChooserRightTop" + node.data);
	gameChooserRightTopGame.style.display = "inline";
	
	var gameChooserRightMiddleGame = document.getElementById("gameChooserRightMiddle" + node.data);
	gameChooserRightMiddleGame.style.display = "block";
	
	GameChooser.resize();

	gameChooserRight.style.visibility = "";
};

/**
 * Returns the currently selected game
 */
GameChooser.getSelectedGame = function()
{
	return GameChooser.selectedNode.data;
};
