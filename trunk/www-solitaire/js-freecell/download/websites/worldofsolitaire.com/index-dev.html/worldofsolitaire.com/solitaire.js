/* Solitaire - robert@cosmicrealms.com */
var addthis_pub="sembiance";
var addthis_disable_flash = true;

var gGames = {};

/**
 * Global constants
 */
var CONSTANTS = 
{
	STATE_LOADING : 0,
	STATE_DEALING : 1,
	STATE_PLAYING : 3,
	STATE_WON : 4,
	STATE_LOST : 5,
	
	AUTOPLAY_MIN_INTERVAL : 600,
	AUTOPLAY_MAX_INTERVAL : 1000,
	
	ANIMATE_SPEED_FASTEST : 0.05,
	ANIMATE_SPEED_SLOWEST : 5.0,
	
	CARD_SPACING_X : 20,
	CARD_SPACING_Y : 20,
	
	CARD_WIN_MOVE_SPEED : 0.1,
	CARD_WIN_EXPLODE_SPEED_PER_CARD : 0.05,
	CARD_AUTOPLAY_SPEED : 0.5,
	CARD_DEAL_SPEED : 0.3,
	CARD_MOVE_SPEED : 0.1,
	CARD_DEAL_TURN_OVER_DELAY : 90,
	CARD_TURN_OVER_SPEED : 0.04,
	CARD_TURN_OVER_DELAY : 60,
	
	BOTTOM_OF_STACK : 987654321,
	
	IS_OFFLINE : false
};

function Solitaire()
{
	this.state = CONSTANTS.STATE_LOADING;
	this.lastState = CONSTANTS.STATE_LOADING;
	this.tinyScreen = false;
	this.wii = false;
	
	this.CARD_AUTOPLAY_SPEED = CONSTANTS.CARD_AUTOPLAY_SPEED;
	this.CARD_DEAL_SPEED = CONSTANTS.CARD_DEAL_SPEED;
	this.CARD_MOVE_SPEED = CONSTANTS.CARD_MOVE_SPEED;
	this.CARD_DEAL_TURN_OVER_DELAY = CONSTANTS.CARD_DEAL_TURN_OVER_DELAY;
	this.CARD_TURN_OVER_SPEED = CONSTANTS.CARD_TURN_OVER_SPEED;
	this.CARD_TURN_OVER_DELAY = CONSTANTS.CARD_TURN_OVER_DELAY;
	this.CARD_SPACING_X = CONSTANTS.CARD_SPACING_X;
	this.CARD_SPACING_Y = CONSTANTS.CARD_SPACING_Y;
	
	this._lastGameID = "";
	this._currentGame = null;
	this._lastResizeEvent = null;
	this._lastResizeWidth = 0;
	this._lastResizeHeight = 0;
	this._winAnimationCards = [];
	this._winExplodeStack = null;
	this._winGameNumber = -1;
	this._playspace = null;
	this._startDate = null;
	this._movesCounter = 0;
	this._undoCounter = 0;
	this._score = 0;
	this._animateSpeedSlider = null;
	this._recentlyPlayedGames = [];
	this.rightAdVisible = true;
	
	this.About = null;
	this.ChangeLog = null;
	this.StatisticsView = null;
	this.DeckView = null;
	
	/**
	 * First function called, Solitaire begins here!
	 */
	this.init = function()
	{
		var hrefQueryObject = {};
		var href = window.location.href;
		if(href.indexOf("?")!=-1)
		{
			var hrefQuery = href.substring(href.indexOf("?")+1);
			if(hrefQuery.length>0)
			{
				hrefQueryObject = O.deserialize(hrefQuery);
				if(hrefQueryObject.hasOwnProperty("tinyScreen") && hrefQueryObject.tinyScreen===true)
					this.tinyScreen = true;
			}
		}
		
		// Wii
		if(P.getBrowser()=="wii")
		{
			this.wii = true;
			
			document.body.style.fontSize = "60%";
			
			var deckPreviewImages = D.getElementsByClassName(document.getElementById("menuDecks"), "img", "deckPreviewImage");
			for(var i=0;i<deckPreviewImages.length;i++)
			{
				var deckPreviewImage = deckPreviewImages[i];
				deckPreviewImage.width = Math.floor(deckPreviewImage.width/2);
				deckPreviewImage.height = Math.floor(deckPreviewImage.height/2);
			}
			
			var backgroundPreviewImages = D.getElementsByClassName(document.getElementById("menuBackground"), "img", "backgroundPreviewImage");
			for(var i=0;i<backgroundPreviewImages.length;i++)
			{
				var backgroundPreviewImage = backgroundPreviewImages[i];
				backgroundPreviewImage.width = Math.floor(backgroundPreviewImage.width/2);
				backgroundPreviewImage.height = Math.floor(backgroundPreviewImage.height/2);
			}

			var gamePreviewImages = D.getElementsByClassName(document.getElementById("gameChooserRightTop"), "img", "gamePreviewImage");
			for(var i=0;i<gamePreviewImages.length;i++)
			{
				var gamePreviewImage = gamePreviewImages[i];
				gamePreviewImage.width = Math.floor(gamePreviewImage.width/2);
				gamePreviewImage.height = Math.floor(gamePreviewImage.height/2);
			}
		}
		
		// Google Gadget, other tiny screens
		if(this.tinyScreen===true)
		{
			document.body.style.fontSize = "60%";
			
			this.CARD_SPACING_X = 5;
			this.CARD_SPACING_Y = 5;
			
			var footerBy = document.getElementById("footerBy");
			footerBy.style.display = "none";
			
			var menuBar = document.getElementById("menuBar");
			menuBar.style.display = "none";
			
			var usernameInfo = document.getElementById("usernameInfo");
			usernameInfo.style.display = "none";
			
			var mouseOverInfoCardLabel = document.getElementById("mouseOverInfoCardLabel");
			mouseOverInfoCardLabel.style.paddingRight = "0";

			var mouseOverInfoStackCountLabel = document.getElementById("mouseOverInfoStackCountLabel");
			mouseOverInfoStackCountLabel.style.paddingRight = "0";
			
			var footerPlayLargerVersion = document.getElementById("footerPlayLargerVersion");
			footerPlayLargerVersion.style.display = "block";
			
			var footerPlayLargerVersionLink = document.getElementById("footerPlayLargerVersionLink");
			footerPlayLargerVersionLink.target = "_blank";
			
			var helpIcon = document.getElementById("helpIcon");
			helpIcon.width = "16px";
			helpIcon.height = "16px";
			helpIcon.style.width = "16px";
			helpIcon.style.height = "16px";
			
			//var infiniteProgressBarImage = document.getElementById("infiniteProgressBarImage");
			//infiniteProgressBarImage.width = "60";
			//infiniteProgressBarImage.style.width = "60px";
			//infiniteProgressBarImage.src = "images/infiniteBarReduced.gif";
			
			//<img src="images/infiniteBar.gif" alt="Infinite Progress Bar" id="infiniteProgressBarImage" width="220" height="19" />
		}

		YAHOO.util.DDM.mode = YAHOO.util.DDM.INTERSECT;
		
		if(CONSTANTS.IS_OFFLINE)
		{
			var menuSolitaireViewStatistics = document.getElementById("menuSolitaireViewStatistics");
			menuSolitaireViewStatistics.parentNode.parentNode.removeChild(menuSolitaireViewStatistics.parentNode);

			var usernameInfo = document.getElementById("usernameInfo");
			usernameInfo.style.display = "none";
			
			var gameOverViewStatisticsButton = document.getElementById("gameOverViewStatisticsButton");
			gameOverViewStatisticsButton.parentNode.removeChild(gameOverViewStatisticsButton);
			
			var footerPlayOnlineVersion = document.getElementById("footerPlayOnlineVersion");
			footerPlayOnlineVersion.style.display = "block";
			
			var footerPlayOnlineVersionLink = document.getElementById("footerPlayOnlineVersionLink");
			footerPlayOnlineVersionLink.target = "_blank";
			
			var menuDeckView = document.getElementById("menuDeckView");
			D.clear(menuDeckView);
		}
		
		this._playspace = document.getElementById("playspace");
		
		if(P.getBrowser()=="ie" || P.getBrowser()=="ie7")
		{
			E.add(this._playspace, "selectstart", function(e) { return false; });
			E.add("footerBy", "selectstart", function(e) { return false; });
			E.add("importantMessage", "selectstart", function(e) { return false; });
			E.add("mouseOverInfo", "selectstart", function(e) { return false; });
			E.add("usernameInfo", "selectstart", function(e) { return false; });
			E.add("menuBar", "selectstart", function(e) { return false; });
		}
		
		E.add(window, "unload", function() { SOLITAIRE.handleShutdown(); });
		
		// Create our dialogs
		this.About = new DialogBox("about");
		
		this.ChangeLog = new DialogBox("changeLog", { "percentageWidth"  : 0.70,
											          "percentageHeight" : 0.50,
											          "overflowable"     : true});
		
		Options.init();
		
		if(!CONSTANTS.IS_OFFLINE)
			User.init();

		// If we are userID #1 (me) then enable creating decks
		// To those of you trying to pretend to be user #1 in order to access this feature...
		// It shouldn't work. I have protections server side too.
		// I really do want to allow everyone access to create decks, but it would fill up the
		// server space pretty quick and I am just one person who makes no money from this site
		// and cannot afford more space
		var userID = User.get("userID", null);
		if(userID===1)
		{
			var createDeckLI = document.createElement("LI");
			var createDeckAnchor = document.createElement("A");
			createDeckAnchor.href = "#deckViewTabCreate";
			
			var createDeckSpan = document.createElement("EM");
			createDeckSpan.appendChild(document.createTextNode("Create a Deck"));
			
			createDeckAnchor.appendChild(createDeckSpan)
			createDeckLI.appendChild(createDeckAnchor);
			
			var deckViewTabsContainer = document.getElementById("deckViewTabsContainer");
			deckViewTabsContainer.appendChild(createDeckLI);
			
			var deckViewTabCreateDeck = document.getElementById("deckViewTabCreateDeck");
			deckViewTabCreateDeck.style.display = "block";
		}		
		
		// Set up the 'hide' option for the ad
		var rightAdHideLinkTooltipOptions = {
				  context   		: "rightAdHideLink",
				   showdelay 		: 100,
				   hidedelay 		: 200,
				   autodismissdelay : 15000,
				   zIndex			: 901,
				   text      	    : "Click this to hide these ads.<br>" +
				   					  "The space will then be available for playing resulting in larger cards." };
		var rightAdHideLinkTooltip = new YAHOO.widget.Tooltip("_usernameInfoTooltip", rightAdHideLinkTooltipOptions);
		E.add("rightAdHideLink", "click", SOLITAIRE.rightAdHideHandler);
		
		User.getUserOptions(function() { SOLITAIRE.initPostUserOptions(hrefQueryObject); });
	};
	
	this.rightAdHideHandler = function(e)
	{
		var rightAdHideLinkContainer = document.getElementById("rightAdHideLinkContainer");
		rightAdHideLinkContainer.style.display = "none";
		var rightAdContainer = document.getElementById("rightAdContainer");
		rightAdContainer.style.display = "none";
		SOLITAIRE.rightAdVisible = false;
		SOLITAIRE.handleWindowResizeFollowup(true);
	};
	
	/**
	 * This is called once we've gotten our user options
	 */
	this.initPostUserOptions = function(hrefQueryObject)
	{
		// Lets init our Options and Stats
		Stats.init();

		var statTotalDealt = Stats.get("totalDealt", 0);
		var statTotalPlayed = Stats.get("totalPlayed", 0);
		var statTotalWon = Stats.get("totalWon", 0);
		var statTotalTime = Stats.get("totalTime", 0);
		var statTotalMoves = Stats.get("totalMoves", 0);
		
		Stats.set("totalDealt", statTotalDealt);
		Stats.set("totalPlayed", statTotalPlayed);
		Stats.set("totalWon", statTotalWon);
		Stats.set("totalTime", statTotalTime);
		Stats.set("totalMoves", statTotalMoves);
		
		var optionCardSize = Options.get("optionCardSize", "best");
		var optionAnimateCards = Options.get("optionAnimateCards", true);
		var optionAnimateDeal = Options.get("optionAnimateDeal", true);
		var optionAnimateSpeed = Options.get("optionAnimateSpeed", 1.0);
		var optionAutoPlay = Options.get("optionAutoPlay", "off");
		var optionAutoFlip = Options.get("optionAutoFlip", true);
		var optionShowTimer = Options.get("optionShowTimer", true);
		var optionShowScore = Options.get("optionShowScore", true);
		var optionShowMoves = Options.get("optionShowMoves", true);
		var optionMenuRequiresClick = Options.get("optionMenuRequiresClick", false);
		var optionDeck = Options.get("optionDeck", "paris");
		var optionBackground = Options.get("optionBackground", "textures/woodpanel.jpg");
		var optionGame = Options.get("optionGame", "KlondikeTurn3");
		
		
		var href = window.location.href;
		var hrefDocument = href.substring(S.lastIndexOf(href, "/"));
		if(hrefDocument.indexOf("?")!=-1)
			hrefDocument = hrefDocument.substring(0, hrefDocument.indexOf("?"));
		
		if(gGames.hasOwnProperty(hrefDocument))
			optionGame = hrefDocument;
		if(hrefQueryObject.hasOwnProperty("optionGame") && gGames.hasOwnProperty(hrefQueryObject["optionGame"]))
			optionGame = hrefQueryObject["optionGame"];
		if(hrefQueryObject.hasOwnProperty("optionDeck") && DECKINFO.hasOwnProperty(hrefQueryObject["optionDeck"]))
			optionDeck = hrefQueryObject["optionDeck"];
		
		Options.set("optionCardSize", optionCardSize);
		Options.set("optionAnimateCards", optionAnimateCards);
		Options.set("optionAnimateDeal", optionAnimateDeal);
		Options.set("optionAnimateSpeed", optionAnimateSpeed);
		Options.set("optionAutoPlay", optionAutoPlay);
		Options.set("optionAutoFlip", optionAutoFlip);
		Options.set("optionShowTimer", optionShowTimer);
		Options.set("optionShowScore", optionShowScore);
		Options.set("optionShowMoves", optionShowMoves);
		Options.set("optionMenuRequiresClick", optionMenuRequiresClick);
		Options.set("optionDeck", optionDeck);
		Options.set("optionBackground", optionBackground);
		Options.set("optionGame", optionGame);
						
		E.add("changeLogLink", "click", SOLITAIRE.ChangeLog.toggle, null, SOLITAIRE.ChangeLog);
		//E.add("changeLogLink", "click", SOLITAIRE.generateGeckoUniqueURLs, null, SOLITAIRE.ChangeLog);
		
		E.add(window, "resize", this.handleWindowResize, null, this);

		// Init our menu bar
		if(!this.tinyScreen)
			Menu.init("menuBar", function(menuItemNumber) { SOLITAIRE.handleMenuShow(menuItemNumber); });
		
		E.add("gameChooserOKButton", "click", this.handleGameChange, null, this);
		E.add("menuSolitaireDealAgain", "mouseup", this.handleDealAgain, null, this);
		E.add("menuSolitaireChooseGame", "mouseup", GameChooser.show, null, GameChooser);
		E.add("menuSolitaireViewStatistics", "mouseup", StatisticsView.toggle, null, StatisticsView);
		E.add("menuSolitaireUndoMove", "mouseup", this.handleUndoMove, null, this);
		E.add("menuSolitaireAbout", "mouseup", SOLITAIRE.About.toggle, null, SOLITAIRE.About);
		
		E.add("deckViewChooseOKButton", "click", this.handleDeckViewChooseOKButton, null, this);
		
		var deckRadios = document.getElementsByName("optionDeck");
		for(var i=0;i<deckRadios.length;i++)
		{
			E.add(deckRadios[i].parentNode, "click", this.handleDeckChange, deckRadios[i], this);
		}
		Radio.checkMatchingValue("optionDeck", Options.get("optionDeck"));
		
		if(!CONSTANTS.IS_OFFLINE)
			E.add("menuDeckView", "mouseup", DeckView.toggle, null, DeckView);
		
		var backgroundRadios = document.getElementsByName("optionBackground");
		for(var i=0;i<backgroundRadios.length;i++)
		{
			E.add(backgroundRadios[i].parentNode, "click", this.handleBackgroundChange, backgroundRadios[i], this);
		}
		var backgroundURLInput = document.getElementById("backgroundURLInput");
		var checkResult = Radio.checkMatchingValue("optionBackground", Options.get("optionBackground"));
		if(checkResult===null)
		{
			backgroundURLInput.value = Options.get("optionBackground");
			checkResult = Radio.checkMatchingValue("optionBackground", "url");
		}
		E.add("backgroundURLInput", "keyup", this.handleBackgroundChange, document.getElementById("backgroundurl"), this);
		this.handleBackgroundChange(null, checkResult, true);
		
		var optionCardSizeRadios = document.getElementsByName("optionCardSize");
		for(i=0;i<optionCardSizeRadios.length;i++)
		{
			E.add(optionCardSizeRadios[i].parentNode, "click", this.handleCardSizeChange, optionCardSizeRadios[i], this);
		}
		Radio.checkMatchingValue("optionCardSize", Options.get("optionCardSize"));

		var optionAnimateCardsCheckbox = document.getElementById("optionAnimateCardsCheckbox");
		if(Options.get("optionAnimateCards"))
			Checkbox.check(optionAnimateCardsCheckbox);
		E.add(optionAnimateCardsCheckbox.parentNode, "click", this.handleAnimateCardsChange, optionAnimateCardsCheckbox, this);

		var optionAnimateDealCheckbox = document.getElementById("optionAnimateDealCheckbox");
		if(Options.get("optionAnimateDeal"))
			Checkbox.check(optionAnimateDealCheckbox);
		E.add(optionAnimateDealCheckbox.parentNode, "click", this.handleAnimateDealChange, optionAnimateDealCheckbox, this);

		var optionAutoPlayRadios = document.getElementsByName("optionAutoPlay");
		for(i=0;i<optionAutoPlayRadios.length;i++)
		{
			E.add(optionAutoPlayRadios[i].parentNode, "click", this.handleAutoPlayChange, optionAutoPlayRadios[i], this);
		}
		Radio.checkMatchingValue("optionAutoPlay", Options.get("optionAutoPlay"));

		var optionAutoFlipCheckbox = document.getElementById("optionAutoFlipCheckbox");
		if(Options.get("optionAutoFlip"))
			Checkbox.check(optionAutoFlipCheckbox);
		E.add(optionAutoFlipCheckbox.parentNode, "click", this.handleAutoFlipChange, optionAutoFlipCheckbox, this);

		var timerContainer = document.getElementById("timerContainer");
		var optionShowTimerCheckbox = document.getElementById("optionShowTimerCheckbox");
		if(Options.get("optionShowTimer"))
		{
			timerContainer.style.visibility = "";
			Checkbox.check(optionShowTimerCheckbox);
		}
		else
		{
			timerContainer.style.visibility = "hidden";
		}
		E.add(optionShowTimerCheckbox.parentNode, "click", this.handleShowTimerChange, optionShowTimerCheckbox, this);

		var scoreContainer = document.getElementById("scoreContainer");
		var optionShowScoreCheckbox = document.getElementById("optionShowScoreCheckbox");
		if(Options.get("optionShowScore"))
		{
			scoreContainer.style.visibility = "";
			Checkbox.check(optionShowScoreCheckbox);
		}
		else
		{
			scoreContainer.style.visibility = "hidden";
		}
		E.add(optionShowScoreCheckbox.parentNode, "click", this.handleShowScoreChange, optionShowScoreCheckbox, this);

		var movesContainer = document.getElementById("movesContainer");
		var optionShowMovesCheckbox = document.getElementById("optionShowMovesCheckbox");
		if(Options.get("optionShowMoves"))
		{
			movesContainer.style.visibility = "";
			Checkbox.check(optionShowMovesCheckbox);
		}
		else
		{
			movesContainer.style.visibility = "hidden";
		}
		E.add(optionShowMovesCheckbox.parentNode, "click", this.handleShowMovesChange, optionShowMovesCheckbox, this);
		
		var optionMenuRequiresClickCheckbox = document.getElementById("optionMenuRequiresClickCheckbox");
		if(Options.get("optionMenuRequiresClick"))
		{
			Menu.setRequiresClick(true);
			Checkbox.check(optionMenuRequiresClickCheckbox);
		}
		else
		{
			Menu.setRequiresClick(false);
		}
		E.add(optionMenuRequiresClickCheckbox.parentNode, "click", this.handleMenuRequiresClickChange, optionMenuRequiresClickCheckbox, this);

		var optionAnimateSpeedThumbContainer = document.getElementById("optionAnimateSpeedThumbContainer");
		var targetThumbLocation = 200-Math.floor((optionAnimateSpeed*(200/CONSTANTS.ANIMATE_SPEED_SLOWEST)));
		optionAnimateSpeedThumbContainer.style.left = targetThumbLocation + "px";
			
		this._animateSpeedSlider = YAHOO.widget.Slider.getHorizSlider("optionAnimateSpeedContainer", "optionAnimateSpeedThumbContainer", targetThumbLocation, 200-targetThumbLocation);
		this._animateSpeedSlider.subscribe("slideEnd", function(offsetFromStart) { SOLITAIRE.handleAnimateSpeedSlider(offsetFromStart); }); 
		this.updateAnimateSpeedValue(true);
		
		var optionHasSavedOnce = Options.get("optionHasSavedOnce", false);
		if(!optionHasSavedOnce)
		{
			if(User.saveUserOptions())
				Options.set("optionHasSavedOnce", true);
		}
		
		Help.init();
		
		Menu.disable();		
		Status.show("Loading", -1);
		
		this.startGame();
		
		// Add our addthis script
		var addThisContainer = document.getElementById("addThisContainer");
		if(this.tinyScreen)
		{
			D.clear(addThisContainer);
		}
		else
		{
			var addThisScript = document.createElement("SCRIPT");
			addThisScript.type = "text/javascript";
			addThisScript.src = "http://s7.addthis.com/js/200/addthis_widget.js";
			addThisContainer.appendChild(addThisScript);
		}
			
		gaSSDSLoad("UA-186902-3");
	};
	
	/**
	 * This function is called after deck loading is finished, time to begin!
	 */
	this.startGame = function()
	{
		GameOver.hide();
		Menu.disable();
		
		var targetDeck = new Deck(Options.get("optionDeck"));
		targetDeck.loadDeck();
		
		Stats.increment("totalDealt");
		Stats.notifyServer("dealt");

		this.sizePlayspace();
		Help.resize();
		
		var optionGame = Options.get("optionGame");
		if(!gGames.hasOwnProperty(optionGame))
			optionGame = "KlondikeTurn3";

		if(this._recentlyPlayedGames.length==0 || this._recentlyPlayedGames[this._recentlyPlayedGames.length-1]!=optionGame)
			this._recentlyPlayedGames.push(optionGame);
			
		while(this._recentlyPlayedGames.length>5)
			this._recentlyPlayedGames.splice(0, 1);

		this._currentGame = new Game(optionGame, targetDeck);
		this._lastGameID = this._currentGame._game._gameID;

		Status.hide("startGame");

		SOLITAIRE.state = CONSTANTS.STATE_DEALING;		
		this._currentGame.init(this._playspace, function() { SOLITAIRE.gameStarted(); });
	};
	
	/**
	 * Called when dealing of cards is finished, everything is ready to begin playing
	 */
	this.gameStarted = function()
	{
		var rightAdHideLinkContainer = document.getElementById("rightAdHideLinkContainer");
		rightAdHideLinkContainer.style.visibility = "visible";
		
		var timerValue = document.getElementById("timerValue");
		D.clear(timerValue);
		timerValue.appendChild(document.createTextNode("0:00"));

		SOLITAIRE.state = CONSTANTS.STATE_PLAYING;
		Menu.enable();
		this._movesCounter = 0;
		this._undoCounter = 0;
		this._startDate = null;
		this.undoIncrement(0);
		this.movesIncrement(0);

		this._score = 0;
		this.scoreIncrement(0);
	};
	
	/**
	 * Ticks the timer up
	 */
	this.timerTick = function()
	{
		if(SOLITAIRE.state!==CONSTANTS.STATE_PLAYING)
			return;
		
		var rightNow = new Date();
		var secondsElapsed = 0;
		if(this._startDate!==null)
			secondsElapsed = parseInt(((rightNow.getTime()-this._startDate.getTime())/1000), 10);
	
		var timerValue = document.getElementById("timerValue");
		D.clear(timerValue);
		timerValue.appendChild(document.createTextNode(I.secondsAsClock(secondsElapsed)));
		
		setTimeout(function() { SOLITAIRE.timerTick(); }, 1000);
	};
	
	/**
	 * Increments the moves counter by howMuch
	 */
	this.movesIncrement = function(howMuch)
	{
		if(typeof howMuch=="undefined" || howMuch===null)
			howMuch = 1;
		
		this._movesCounter+=howMuch;

		// We are now officially playing! :)
		if(this._movesCounter===1)
		{
			Stats.increment("totalPlayed");
			Stats.notifyServer("played");
			
			this._startDate = new Date();
			SOLITAIRE.timerTick();
		}
		
		var movesValue = document.getElementById("movesValue");
		D.clear(movesValue);
		movesValue.appendChild(document.createTextNode(I.formatWithCommas(this._movesCounter)));
		
		var movesLabel = document.getElementById("movesLabel");
		D.clear(movesLabel);
		if(this._movesCounter===1)
			movesLabel.appendChild(document.createTextNode("move"));
		else
			movesLabel.appendChild(document.createTextNode("moves"));
	};
	
	/**
	 * Increments the score counter by howMuch
	 */
	this.scoreIncrement = function(howMuch)
	{
		if(typeof howMuch=="undefined" || howMuch===null)
			howMuch = 100;
			
		this._score+=howMuch;
		
		var scoreValue = document.getElementById("scoreValue");
		D.clear(scoreValue);
		scoreValue.appendChild(document.createTextNode(I.formatWithCommas(this._score)));
	};
		
	/**
	 * Increments the undo counter by howMuch
	 */
	this.undoIncrement = function(howMuch)
	{
		if(typeof howMuch=="undefined" || howMuch===null)
			howMuch = 1;
		
		this._undoCounter+=howMuch;		
	};

	/**
	 * This is called when the user leaves the page
	 */
	this.handleShutdown = function()
	{
		if(SOLITAIRE.state==CONSTANTS.STATE_PLAYING && this._movesCounter>0)
		{
			var rightNow = new Date();
			if(this._startDate!==null)
				Stats.increment("totalTime", parseInt(((rightNow.getTime()-this._startDate.getTime())/1000), 10));
			Stats.increment("totalMoves", this._movesCounter);
		}
	};
	
	/**
	 * This will attempt to undo a move
	 * @param {Object} e
	 * @param {Object} ignored
	 */
	this.handleUndoMove = function(e, ignored)
	{
		if(SOLITAIRE.state<CONSTANTS.STATE_PLAYING)
			return;
		
		if(this._currentGame.undoMove())
		{
			this.undoIncrement();
		}
	};
		
	/**
	 * This will start the current game over again
	 * @param {Object} e
	 * @param {Object} ignored
	 * @param {Object} skipAbandon If this is true, we won't handle it as an abandon
	 */
	this.handleDealAgain = function(e, ignored, skipAbandon)
	{
		var self = this;
		
		if(SOLITAIRE.state<CONSTANTS.STATE_PLAYING)
			return;

		if(SOLITAIRE.state==CONSTANTS.STATE_PLAYING && (typeof skipAbandon==="undefined" || skipAbandon!==true))
			this.handleGameAbandon();

		if(SOLITAIRE.state==CONSTANTS.STATE_PLAYING && this._movesCounter>0)
		{
			var rightNow = new Date();
			if(this._startDate!==null)
				Stats.increment("totalTime", parseInt(((rightNow.getTime()-this._startDate.getTime())/1000), 10));
			Stats.increment("totalMoves", this._movesCounter);
		}
		
		Menu.disable();
		Status.show("Loading", -1, "startGame");
		
		if(this._currentGame!==null)
			this._currentGame.clear();
		this._currentGame = null;
		
		setTimeout(function() { self.startGame(); }, 0);
	};
	
	/**
	 * This is called when the current game is abandoned
	 */	
	this.handleGameAbandon = function()
	{
		var postOptions = {};
		
		if(this._startDate!==null)
		{
			var rightNow = new Date();
			
			postOptions["undos"] = this._undoCounter;
			postOptions["moves"] = this._movesCounter;
			postOptions["score"] = this._score;
			postOptions["time"] = parseInt(((rightNow.getTime()-this._startDate.getTime())/1000), 10);
		}
		
		Stats.notifyServer("abandoned", postOptions);
	};
		
	/**
	 * This is called when the user changes games
	 * @param {Object} e
	 * @param {Object} ignored
	 */
	this.handleGameChange = function(e, ignored)
	{
		this.handleGameAbandon();
		
		Options.set("optionGame", GameChooser.getSelectedGame());
		User.saveUserOptions();
		
		this.updateGameChange();
	};
	
	/**
	 * Actually updates our UI with the changes of the option
	 */
	this.updateGameChange = function(skipIfSameGame)
	{
		var self = this;
		
		var optionGame = Options.get("optionGame");
		
		if(typeof skipIfSameGame!="undefined" && skipIfSameGame===true && this._lastGameID==optionGame)
			return;
			
		GameChooser.hide();
		Help.hide();
		Menu.disable();
		
		setTimeout(function() { self.handleDealAgain(null, null, true); }, 0);		
	};
	
	/**
	 * This is called when the user selects a deck in the decks view
	 * @param {Object} e
	 * @param {Object} ignored
	 */
	this.handleDeckViewChooseOKButton = function(e, ignored)
	{
		Options.set("optionDeck", DeckView.chooseGetSelectedDeck());
		User.saveUserOptions();
		
		DeckView.hide();
		
		this.updateDeckChange();
	};
	
	/**
	 * This is called when the user changes decks
	 * @param {Object} e
	 * @param {Object} radio
	 */
	this.handleDeckChange = function(e, radio)
	{
		if(Options.get("optionDeck")==radio.value)
			return;
		
		Options.set("optionDeck", radio.value);
		User.saveUserOptions();
		
		this.updateDeckChange();
	};
	
	/**
	 * Actually updates our UI with the changes of the option
	 */
	this.updateDeckChange = function()
	{
		var optionDeck = Options.get("optionDeck");
		Radio.checkMatchingValue("optionDeck", optionDeck);
		
		this._currentGame._deck = new Deck(optionDeck);
		this._currentGame._deck.loadDeck();
		
		if(this._currentGame!==null)
		{
			Menu.disable();
			this._currentGame.updateBoard(function() { Menu.enable(); });
		}		
	};
	
	/**
	 * This is called when the user changes backgrounds
	 * @param {Object} e
	 * @param {Object} radio
	 */
	this.handleBackgroundChange = function(e, radio, skipUserUpdate)
	{
		//if(Options.get("optionBackground")==radio.value)
		//	return;
		
		var textureURL = radio.value;
		if(textureURL==="url")
		{
			var backgroundURLInput = document.getElementById("backgroundURLInput");
			textureURL = backgroundURLInput.value;
		}
		
		Options.set("optionBackground", textureURL);
		
		if(typeof skipUserUpdate=="undefined" || skipUserUpdate!==true)
			User.saveUserOptions();
		
		this.updateBackgroundChange();
	};
	
	/**
	 * Actually updates our UI with the changes of the option
	 */
	this.updateBackgroundChange = function()
	{
		var optionBackground = Options.get("optionBackground");
		if(Radio.checkMatchingValue("optionBackground", optionBackground)===null)
			Radio.checkMatchingValue("optionBackground", "url");
		
		document.body.style.backgroundImage = "url(" + optionBackground + ")";
	};
	
	/**
	 * This is called when the user selects a new card size
	 * @param {Object} e
	 * @param {Object} radio
	 */
	this.handleCardSizeChange = function(e, radio)
	{
		if(Options.get("optionCardSize")===radio.value)
			return;
				
		Options.set("optionCardSize", radio.value);
		User.saveUserOptions();
			
		this.updateCardSizeChange();
	};
	
	/**
	 * Actually updates our UI with the changes of the option
	 */
	this.updateCardSizeChange = function()
	{
		Radio.checkMatchingValue("optionCardSize", Options.get("optionCardSize"));

		if(this._currentGame!==null)
		{
			Menu.disable();
			this._currentGame.updateBoard(function() { Menu.enable(); });
		}
	};	
	
	/**
	 * This is called when the animate cards checkbox option changes
	 * @param {Object} e
	 * @param {Object} checkbox
	 */
	this.handleAnimateCardsChange = function(e, checkbox)
	{
		E.stop(e);
		
		if(Options.get("optionAnimateCards")===true)
			Options.set("optionAnimateCards", false);
		else
			Options.set("optionAnimateCards", true);
		
		User.saveUserOptions();
		this.updateAnimateCardsChange();
	};
	
	/**
	 * Actually updates our UI with the changes of the option
	 */
	this.updateAnimateCardsChange = function()
	{
		var optionAnimateCardsCheckbox = document.getElementById("optionAnimateCardsCheckbox");
		if(Options.get("optionAnimateCards")===true)
			Checkbox.check(optionAnimateCardsCheckbox);
		else
			Checkbox.uncheck(optionAnimateCardsCheckbox);
	};	
	
	/**
	 * This is called when the auto flip checkbox option changes
	 * @param {Object} e
	 * @param {Object} checkbox
	 */
	this.handleAnimateDealChange = function(e, checkbox)
	{
		E.stop(e);
		
		if(Options.get("optionAnimateDeal")===true)
			Options.set("optionAnimateDeal", false);
		else
			Options.set("optionAnimateDeal", true);
			
		User.saveUserOptions();		
		this.updateAnimateDealChange();
	};	
	
	/**
	 * Actually updates our UI with the changes of the option
	 */
	this.updateAnimateDealChange = function()
	{
		var optionAnimateDealCheckbox = document.getElementById("optionAnimateDealCheckbox");
		if(Options.get("optionAnimateDeal")===true)
			Checkbox.check(optionAnimateDealCheckbox);
		else
			Checkbox.uncheck(optionAnimateDealCheckbox);
	};		
		
	/**
	 * This is called when the user selects a different auto play option
	 * @param {Object} e
	 * @param {Object} radio
	 */
	this.handleAutoPlayChange = function(e, radio)
	{
		if(Options.get("optionAutoPlay")===radio.value)
			return;
		
		Options.set("optionAutoPlay", radio.value);
		User.saveUserOptions();
		
		if(this._currentGame!==null)
			this._currentGame.autoPlayGo();
		
		this.updateAutoPlayChange();
	};	
	
	/**
	 * Actually updates our UI with the changes of the option
	 */
	this.updateAutoPlayChange = function()
	{
		Radio.checkMatchingValue("optionAutoPlay", Options.get("optionAutoPlay"));
	};	
		
	/**
	 * This is called when the auto flip checkbox option changes
	 * @param {Object} e
	 * @param {Object} checkbox
	 */
	this.handleAutoFlipChange = function(e, checkbox)
	{
		E.stop(e);
		
		if(Options.get("optionAutoFlip")===true)
			Options.set("optionAutoFlip", false);
		else
			Options.set("optionAutoFlip", true);
		
		User.saveUserOptions();
		this.updateAutoFlipChange();
	};
	
	/**
	 * Actually updates our UI with the changes of the option
	 */
	this.updateAutoFlipChange = function()
	{
		var optionAutoFlipCheckbox = document.getElementById("optionAutoFlipCheckbox");
		if(Options.get("optionAutoFlip")===true)
			Checkbox.check(optionAutoFlipCheckbox);
		else
			Checkbox.uncheck(optionAutoFlipCheckbox);
	};	
	
	/**
	 * This is called when the show timer checkbox option changes
	 * @param {Object} e
	 * @param {Object} checkbox
	 */
	this.handleShowTimerChange = function(e, checkbox)
	{
		E.stop(e);
		
		if(Options.get("optionShowTimer")===true)
			Options.set("optionShowTimer", false);
		else
			Options.set("optionShowTimer", true);
		
		User.saveUserOptions();
		this.updateShowTimerChange();
	};
	
	/**
	 * Actually updates our UI with the changes of the option
	 */
	this.updateShowTimerChange = function()
	{
		var optionShowTimerCheckbox = document.getElementById("optionShowTimerCheckbox");
		var timerContainer = document.getElementById("timerContainer");
		if(Options.get("optionShowTimer")===true)
		{
			timerContainer.style.visibility = "";
			Checkbox.check(optionShowTimerCheckbox);
		}
		else
		{
			timerContainer.style.visibility = "hidden";
			Checkbox.uncheck(optionShowTimerCheckbox);
		}
	};
		
	/**
	 * This is called when the show score checkbox option changes
	 * @param {Object} e
	 * @param {Object} checkbox
	 */
	this.handleShowScoreChange = function(e, checkbox)
	{
		E.stop(e);
		
		if(Options.get("optionShowScore")===true)
			Options.set("optionShowScore", false);
		else
			Options.set("optionShowScore", true);
		
		User.saveUserOptions();
		this.updateShowScoreChange();
	};
	
	/**
	 * Actually updates our UI with the changes of the option
	 */
	this.updateShowScoreChange = function()
	{
		var optionShowScoreCheckbox = document.getElementById("optionShowScoreCheckbox");
		var scoreContainer = document.getElementById("scoreContainer");
		if(Options.get("optionShowScore")===true)
		{
			scoreContainer.style.visibility = "";
			Checkbox.check(optionShowScoreCheckbox);
		}
		else
		{
			scoreContainer.style.visibility = "hidden";
			Checkbox.uncheck(optionShowScoreCheckbox);
		}
	};
	
	/**
	 * This is called when the show moves checkbox option changes
	 * @param {Object} e
	 * @param {Object} checkbox
	 */
	this.handleShowMovesChange = function(e, checkbox)
	{
		E.stop(e);
		
		if(Options.get("optionShowMoves")===true)
			Options.set("optionShowMoves", false);
		else
			Options.set("optionShowMoves", true);
		
		User.saveUserOptions();
		this.updateShowMovesChange();
	};
		
	/**
	 * Actually updates our UI with the changes of the option
	 */
	this.updateShowMovesChange = function()
	{
		var optionShowMovesCheckbox = document.getElementById("optionShowMovesCheckbox");
		var movesContainer = document.getElementById("movesContainer");
		if(Options.get("optionShowMoves")===true)
		{
			movesContainer.style.visibility = "";
			Checkbox.check(optionShowMovesCheckbox);
		}
		else
		{
			movesContainer.style.visibility = "hidden";
			Checkbox.uncheck(optionShowMovesCheckbox);
		}
	};
	
	/**
	 * This is called when the menu requires click checkbox option changes
	 * @param {Object} e
	 * @param {Object} checkbox
	 */
	this.handleMenuRequiresClickChange = function(e, checkbox)
	{
		E.stop(e);
		
		if(Options.get("optionMenuRequiresClick")===true)
			Options.set("optionMenuRequiresClick", false);
		else
			Options.set("optionMenuRequiresClick", true);
		
		User.saveUserOptions();
		this.updateMenuRequiresClickChange();
	};

	/**
	 * Actually updates our UI with the changes of the option
	 */
	this.updateMenuRequiresClickChange = function()
	{
		var optionMenuRequiresClickCheckbox = document.getElementById("optionMenuRequiresClickCheckbox");
		if(Options.get("optionMenuRequiresClick")===true)
		{
			Menu.setRequiresClick(true);
			Checkbox.check(optionMenuRequiresClickCheckbox);
		}
		else
		{
			Menu.setRequiresClick(false);
			Checkbox.uncheck(optionMenuRequiresClickCheckbox);
		}
		
	};
	
	/**
	 * Handles animation speed changes
	 */
	this.handleAnimateSpeedSlider = function(brokenParameter)
	{
		var optionAnimateSpeedContainer = document.getElementById("optionAnimateSpeedContainer");
		var optionAnimateSpeedThumbContainer = document.getElementById("optionAnimateSpeedThumbContainer");
		if(optionAnimateSpeedContainer===null || optionAnimateSpeedThumbContainer===null)
			return;
			
		var containerXY = D.xy(optionAnimateSpeedContainer);
		var thumbXY = D.xy(optionAnimateSpeedThumbContainer);
		if(containerXY===false || containerXY[0]===0 || thumbXY[0]===0)
			return;
			
		var offsetFromStart = thumbXY[0] - containerXY[0];
		if(isNaN(offsetFromStart))
			return;
			
		var optionAnimateSpeed = ((200-offsetFromStart) / (200/CONSTANTS.ANIMATE_SPEED_SLOWEST));
		optionAnimateSpeed = Math.min(optionAnimateSpeed, CONSTANTS.ANIMATE_SPEED_SLOWEST);
		optionAnimateSpeed = Math.max(optionAnimateSpeed, CONSTANTS.ANIMATE_SPEED_FASTEST);

		Options.set("optionAnimateSpeed", optionAnimateSpeed);
		User.saveUserOptions();
		
		this.updateAnimateSpeedValue(true);
	};
	
	/**
	 * Actually updates our speed values based on the current animateSpeed ratio
	 */
	this.updateAnimateSpeedValue = function(skipSliderSet)
	{
		var optionAnimateSpeed = Options.get("optionAnimateSpeed");

		if(typeof skipSliderSet=="undefined" || skipSliderSet!==true)
		{
			//var optionAnimateSpeedThumbContainer = document.getElementById("optionAnimateSpeedThumbContainer");
			var targetThumbLocation = 200-Math.floor((optionAnimateSpeed*(200/CONSTANTS.ANIMATE_SPEED_SLOWEST)));
			//optionAnimateSpeedThumbContainer.style.left = targetThumbLocation + "px";
			this._animateSpeedSlider.setValue(targetThumbLocation, true);
		}
		
		this.CARD_AUTOPLAY_SPEED = CONSTANTS.CARD_AUTOPLAY_SPEED*optionAnimateSpeed;
		this.CARD_DEAL_SPEED = CONSTANTS.CARD_DEAL_SPEED*optionAnimateSpeed;
		this.CARD_MOVE_SPEED = CONSTANTS.CARD_MOVE_SPEED*optionAnimateSpeed;
		this.CARD_DEAL_TURN_OVER_DELAY = CONSTANTS.CARD_DEAL_TURN_OVER_DELAY*optionAnimateSpeed;
		this.CARD_TURN_OVER_SPEED = CONSTANTS.CARD_TURN_OVER_SPEED*optionAnimateSpeed;
		this.CARD_TURN_OVER_DELAY = CONSTANTS.CARD_TURN_OVER_DELAY*optionAnimateSpeed;
	};
		
	/**
	 * This is called when new options have been set by a third party and we need to now use them
	 */
	this.refreshUI = function()
	{
		this.updateAnimateSpeedValue();
		this.updateShowMovesChange();
		this.updateShowScoreChange();
		this.updateShowTimerChange();
		this.updateAutoFlipChange();
		this.updateAutoPlayChange();
		this.updateAnimateDealChange();
		this.updateAnimateCardsChange();
		this.updateCardSizeChange();
		this.updateBackgroundChange();
		this.updateDeckChange();
	};
	
	/**
	 * This is called whenever the window is resized
	 */	
	this.handleWindowResize = function()
	{
		var self = this;
		
		var curWidth = D.width(document.body);
		var curHeight = D.height(document.body);
		
		var widthDifference = curWidth-this._lastResizeWidth;
		var heightDifference = curHeight-this._lastResizeHeight;
		
		if(widthDifference<25 && widthDifference>-25 && heightDifference<25 && heightDifference>-25)
			return;

		this._lastResizeWidth = curWidth;
		this._lastResizeHeight = curHeight;
		
		this._lastResizeEvent = new Date();
		
		if(SOLITAIRE.state==CONSTANTS.STATE_PLAYING)
		{
			Menu.disable();
			Status.show("Resizing Cards", -1, "windowResize");
		}
		
		setTimeout(function() { self.handleWindowResizeFollowup(); }, 500);
	};
	
	/**
	 * Followup to the window resize, prevents lots of resizing taking place too quickly
	 */
	this.handleWindowResizeFollowup = function(pForceResize)
	{
		var self = this;
		
		var forceResize = false;
		if(typeof pForceResize!=="undefined")
			forceResize = pForceResize;
		
		if(!forceResize && this._lastResizeEvent===null)
			return;

		var rightNow = new Date();
		if(!forceResize && ((rightNow.getTime()-this._lastResizeEvent.getTime())<500 || SOLITAIRE.state!=CONSTANTS.STATE_PLAYING))
		{
			setTimeout(function() { self.handleWindowResizeFollowup(); }, 500);
			return;
		}

		Menu.disable();
		Status.show("Resizing Cards", -1, "windowResize");
		
		this._lastResizeEvent = null;
		this.sizePlayspace();
		Help.resize();
		DeckView.resize();
				
		if(this._currentGame!==null)
			this._currentGame.updateBoard(function() { Status.hide("windowResize"); Menu.enable(); });			
	};

	/**
	 * Sizes our playspace
	 */
	this.sizePlayspace = function()
	{
		// See if our ads are too big if so we need to relocate 
		var footerBy = document.getElementById("footerBy");
		var rightAdContainer = document.getElementById("rightAdContainer");
		var rightAdContainerXY = D.xy(rightAdContainer);
		if((rightAdContainerXY[1]+D.height(rightAdContainer)+D.height(footerBy))>D.height(document.body))
			footerBy.style.marginRight = "130px";
		else
			footerBy.style.marginRight = "0";
		
		// Size our actual playspace
		this._playspace.style.marginTop = this.tinyScreen ? "5px" : "20px";
		this._playspace.style.left = this.tinyScreen ? "5px" : "10px";
		this._playspace.style.width = D.width(document.body)-(this.tinyScreen ? 15 : (this.rightAdVisible ? 144 : 24)) + "px";
		this._playspace.style.height = (D.height(document.body)-D.height(document.getElementById("menuBar")))-(this.tinyScreen ? 15 : 34) + "px";
	};
	
	/**
	 * Is called when a menu is shown
	 * @param {Object} menuItemNumber
	 */
	this.handleMenuShow = function(menuItemNumber)	
	{
		if(menuItemNumber===2)
		{
			if(this._animateSpeedSlider!==null)
				this._animateSpeedSlider.verifyOffset(true);
		}
	};
	
	/**
	 * Function to handle losing
	 */
	this.lostGameHandler = function()
	{
		SOLITAIRE.state = CONSTANTS.STATE_LOST;
		
		GameOver.show(false);
	};
		
	/**
	 * Function to handle winning, kicks off the reward
	 */
	this.wonGameHandler = function()
	{
		var self = this;
				
		var rightNow = new Date();
		
		Stats.increment("totalWon");
		Stats.notifyServer("won", { undos : this._undoCounter,
									moves : this._movesCounter,
									score : this._score,
									time  : parseInt(((rightNow.getTime()-this._startDate.getTime())/1000), 10)});
		
		Stats.increment("totalTime", parseInt(((rightNow.getTime()-this._startDate.getTime())/1000), 10));
		Stats.increment("totalMoves", this._movesCounter);
		
		this._winGameNumber = Stats.get("totalDealt");
			
		for(var i=0;i<this._currentGame._spotsExpanded.length;i++)
		{
			var spot = this._currentGame._spotsExpanded[i];
			if(spot.hasOwnProperty("base"))
				spot["baseCard"].getDomCard().style.visibility = "hidden";
		}		
		
		this._winAnimationCards = [];
			
		for(i=0;i<this._currentGame._spotsExpanded.length;i++)
		{
			spot = this._currentGame._spotsExpanded[i];
			if(spot.cards.length>0)
				this._winAnimationCards.push(spot.cards);
		}

		setTimeout(function() { self.winAnimateNextStack(); }, 1500);		
	};
	
	/**
	 * Moves the next stack to where it is safe to explode
	 */
	this.winAnimateNextStack = function()
	{
		var self = this;
		
		// All done animating?
		if(this._winAnimationCards.length===0)
		{
			if(this._currentGame!==null && this._winGameNumber==Stats.get("totalDealt"))
			{
				this._currentGame.clear();
				this._currentGame = null;
				
				GameOver.show(true);
			}
			return;
		}
		
		this._winExplodeStack = this._winAnimationCards.pop();
		if(this._winExplodeStack===null || this._winExplodeStack.length<1)
		{
			setTimeout(function() { self.winAnimateNextStack(); }, 100);
			return;	
		}
		
		var domCard = this._winExplodeStack[0].getDomCard();
		var cardHeight = D.height(domCard);
		var cardWidth = D.width(domCard);
					
		var randomLocation = [I.random(cardWidth, D.width(document.body)-(cardWidth*3)),
						      I.random(cardHeight, D.height(document.body)-(cardHeight*3))];
		
		for(var i=0;i<this._winExplodeStack.length;i++)
		{
			var card = this._winExplodeStack[i];
			card.show();
			domCard = card.getDomCard();

			var moveOptions = {    left : { to : randomLocation[0] },
			                        top : { to : randomLocation[1] }};
			var moveAnimation = new YAHOO.util.Anim(domCard, moveOptions, CONSTANTS.CARD_WIN_MOVE_SPEED, YAHOO.util.Easing.easeOut);
			
			if((this._winExplodeStack.length-1)==i)
				moveAnimation.onComplete.subscribe(function() { self.winExplodeStack(); });
				
			moveAnimation.animate();
		}
	};
	
	/**
	 * Explodes the next stack
	 */
	this.winExplodeStack = function()
	{
		var self = this;
		
		for(var i=0;i<this._winExplodeStack.length;i++)
		{
			var card = this._winExplodeStack[i];
			var domCard = card.getDomCard();
			
			var cardXY = D.xy(domCard);
			var cardHeight = D.height(domCard);
			var cardWidth = D.width(domCard);
			var degree = 2*Math.PI*(i/this._winExplodeStack.length);
			var xSpeed = Math.sin(degree);
			var ySpeed = Math.cos(degree);
			var destXLoc = cardXY[0] + (xSpeed*(cardWidth*2));
			var destYLoc = cardXY[1] + (ySpeed*(cardHeight*2));

			var moveOptions = {    left : { to : destXLoc },
			                        top : { to : destYLoc },
								opacity : { to :        0 } };
			var moveAnimation = new YAHOO.util.Anim(domCard, moveOptions, CONSTANTS.CARD_WIN_EXPLODE_SPEED_PER_CARD*this._winExplodeStack.length, YAHOO.util.Easing.easeOut);
			
			if((this._winExplodeStack.length-1)==i)
				moveAnimation.onComplete.subscribe(function() { self.winAnimateNextStack(); });
				
			moveAnimation.animate();
		}
	};	
	
	/**
	 * Development only. Generates unique URLs for all the decks
	 * 1. Uncommont uses of gAllDeckURLS in Deck.js
	 * 2. Change
	 * 
	 */
	this.generateGeckoUniqueURLs = function()
	{
		for(var deckKey in DECKINFO)
		{
			if(!DECKINFO.hasOwnProperty(deckKey))
				continue;
			
			var targetDeck = new Deck(deckKey);
			targetDeck.loadDeck();
		}
		
		var targetTextArea = document.createElement("TEXTAREA");
		targetTextArea.style.width = "1200px";
		targetTextArea.style.height = "900px";
		targetTextArea.style.position = "absolute";
		targetTextArea.style.zIndex = "500";
		document.body.appendChild(targetTextArea);
		
		var data = "{";
		
		for(var cardURLKey in gAllDeckURLS)
		{
			if(!gAllDeckURLS.hasOwnProperty(cardURLKey))
				continue;
			
			data += ", \"" + cardURLKey + "\" : \"" + gAllDeckURLS[cardURLKey] + "\"\n";
		}
		
		targetTextArea.value = data;
	};
}
