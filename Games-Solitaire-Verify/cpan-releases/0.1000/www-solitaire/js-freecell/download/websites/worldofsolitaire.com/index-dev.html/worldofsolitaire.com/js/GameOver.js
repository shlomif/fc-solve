/*
 * GameOver.js - Change log functions
 * Author - robert@cosmicrealms.com
 */

var GameOver = new DialogBox("gameOver", { close : false, draggable : false });

/**
 * Called when the dialog first is initialized
 */
GameOver.onInit = function()
{
	var gameOverChooseGameButton = document.getElementById("gameOverChooseGameButton");
	E.add(gameOverChooseGameButton, "click", GameOver.chooseGameClickHandler, null, GameOver);	
	
	var gameOverDealAgainButton = document.getElementById("gameOverDealAgainButton");
	E.add(gameOverDealAgainButton, "click", GameOver.dealAgainClickHandler, null, GameOver);	
	
	var gameOverViewStatisticsButton = document.getElementById("gameOverViewStatisticsButton");
	E.add(gameOverViewStatisticsButton, "click", GameOver.showStatisticsClickHandler, null, GameOver);	
};

/**
 * Called to show the statistics window
 */
GameOver.showStatisticsClickHandler = function()
{
	StatisticsView.setGame(SOLITAIRE._lastGameID);
	StatisticsView.show();
};

/**
 * Called right before the game over dialog is shown
 */
GameOver.beforeShow = function()
{
	var gameOverYouWinImage = document.getElementById("gameOverYouWinImage");
	
	if(P.getBrowser()=="ie")
		gameOverYouWinImage.src = S.replaceAll(gameOverYouWinImage.src, ".png", ".gif");

	if(D.width(document.body)<889)
	{
		gameOverYouWinImage.width = Math.floor(889/2);
		gameOverYouWinImage.height = Math.floor(206/2);
	}
	else
	{
		gameOverYouWinImage.width = 889;
		gameOverYouWinImage.height = 206;
	}
};

/**
 * Called when user hits the deal again button
 */
GameOver.dealAgainClickHandler = function()
{
	GameOver.hide();
	setTimeout(function() { SOLITAIRE.handleDealAgain(); }, 0);
};

/**
 * Called when user hits the choose game button
 */
GameOver.chooseGameClickHandler = function()
{
	GameOver.hide();
	setTimeout(function() { GameChooser.show(); }, 0);
};

