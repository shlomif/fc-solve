/* Game.js - robert@cosmicrealms.com */

function Game(gameID, deck)
{	
	this._game = gGames[gameID];
	this._stock = null;
	this._deck = deck;
	this._deckSize = 0;
	this._rowHeights = [];
	this._colWidths = [];
	this._spotsExpanded = [];
	this._cardScale = 1.00;
	this._totalBoardWidth = 65000;
	this._totalBoardHeight = 65000;
	this._boardOffsetX = 0;
	this._playspace = null;
	this._updateBoardFinishHandler = null;
	this._gameStartedHandler = null;
	this._autoPlayCounter = 0;
	this._autoPlayTimeoutID = null;
	this._autoPlayPaused = false;
	this._autoPlayTrying = false;
	this._autoPlayedCards = [];
	this._restoreOptions = null;
	this._skipCardClicks = {};
	this._undoMoves = [];
	
	/**
	 * This function is called to initialize the game
	 * @param {Object} playspace What DOM object to play the game inside of
	 */
	this.init = function(playspace, gameStartedHandler)
	{
		var self = this;
		
		this._playspace = playspace;

		if(typeof gameStartedHandler!=="undefined")
			this._gameStartedHandler = gameStartedHandler;
		else
			this._gameStartedHandler = null;
		
		// Make our cards larger or smaller to perfectly fit the board
		this.createIdealBoard();
		
		// Preload images
		this._deck.preloadDeckSize(this._deckSize, this._cardScale, function() { self.initFinished(); });
	};
	
	/**
	 * Called when init is finished
	 */
	this.initFinished = function()
	{
		var self = this;
		
		// Create our base cards
		for(var i=0;i<this._spotsExpanded.length;i++)
		{
			var spot = this._spotsExpanded[i];
			if(spot.hasOwnProperty("base"))
			{
				var baseProperties = spot["base"];
				
				if(!spot.hasOwnProperty("baseCard"))
				{
					spot["baseCard"] = new Card(baseProperties.hasOwnProperty("clear") && baseProperties["clear"]===true ? "base_clear" : (spot["class"]=="foundation" ? "base_foundation" : "base"));
					spot["baseCard"].createCard(this._deck, this._deckSize);
				}
				
				spot["baseCard"].setScale(this._cardScale);
				spot["baseCard"].moveTo(spot["screenX"], spot["screenY"], -1);
				spot["baseCard"].spot = spot;
				spot["baseCard"].game = self;

				if(baseProperties.hasOwnProperty("droppable") && baseProperties["droppable"]===true)
				{
					spot["baseCard"].enableDropping();
				}
				
				if(baseProperties.hasOwnProperty("clickable") && baseProperties["clickable"]===true)
				{
					spot["baseCard"]["id"] = "base_redeal";
					spot["baseCard"].updateCard();
					
					E.add(spot["baseCard"].getDomCard(), "click", this.cardClicked, spot["baseCard"], this);
					spot["baseCard"].hasClickHandler = true;
					spot["baseCard"].getDomCard().style.cursor = (P.getBrowser()=="ie" ? "hand" : "pointer");
				}
			}
		}
		
		// Create our stock of cards
		if(this._game.hasOwnProperty("stockCreate"))
			this._stock = this._game.stockCreate();
		else
			this._stock = Stock.createStandard();
			
		// Randomize offscreen location
		/*var offscreenCoordinates = [];
		var offscreenSide = I.random(0, 4);
		if(offscreenSide===0)
			offscreenCoordinates.push(-50, I.random(-50, D.height(this._playspace)+50));
		else if(offscreenSide===1)
			offscreenCoordinates.push(I.random(-50, D.width(this._playspace)+50), -50);
		else if(offscreenSide===2)
			offscreenCoordinates.push(I.random(-50, D.width(this._playspace)+50), D.height(this._playspace)+50);
		else if(offscreenSide===3)
			offscreenCoordinates.push(D.width(this._playspace)+50, I.random(-50, D.height(this._playspace)+50));*/

		// Place the stock into play, disable certain options during this initial distribution of cards
		this._restoreOptions = {};
		this._restoreOptions["optionAutoFlip"] = Options.get("optionAutoFlip");
		this._restoreOptions["optionAnimateCards"] = Options.get("optionAnimateCards");
		this._restoreOptions["optionAutoPlay"] = Options.get("optionAutoPlay");

		Options.set("optionAutoFlip", false, true);
		if(Options.get("optionAnimateDeal")===true && this._restoreOptions["optionAnimateCards"]!==true)
			Options.set("optionAnimateCards", true, true);
		else if(Options.get("optionAnimateDeal")!==true)
			Options.set("optionAnimateCards", false, true);
			
		Options.set("optionAutoPlay", "off", true);
		
		for(i=0;i<this._stock.length;i++)
		{
			var card = this._stock[i];
			card.createCard(this._deck, this._deckSize);
			card.setScale(this._cardScale);
			card.hasClickHandler = false;
			card.hasDoubleClickHandler = false;
			card.spot = null;
			card.game = self;
			card.currentScreenX = 0;
			card.currentScreenY = 0;
			card.setDragStartHandler(function(card) { self.cardDragStarted(card); });
			card.setDragHandler(function(card) { self.cardDragged(card); });
			card.setDragStopHandler(function(card) { self.cardDragStopped(card); });
			card.setDroppedHandler(function(cardDragged, cardDroppedOn) { return self.cardDropped(cardDragged, cardDroppedOn); });
			card.setDragSpace(this._playspace);
			card.setMouseOverHandler(function(card) { self.cardMousedOver(card); });
			card.setMouseOutHandler(function(card) { self.cardMousedOut(card); });
			this._game.stockDistribute(card, i);
		}
	};
	
	/**
	 * This is called by the game when stock distribution is finished
	 */
	this.stockDistributionFinished = function()
	{
		if(this._restoreOptions!==null)
		{
			Options.set("optionAutoFlip", this._restoreOptions["optionAutoFlip"], true);
			Options.set("optionAnimateCards", this._restoreOptions["optionAnimateCards"], true);
			Options.set("optionAutoPlay", this._restoreOptions["optionAutoPlay"], true);

			this._restoreOptions = null;
		}
				
		document.title = "World of Solitaire: " + this._game._gameName;
		
		if(this._gameStartedHandler!==null)
			this._gameStartedHandler();
		this._gameStartedHandler = null;

		for(var i=0;i<this._spotsExpanded.length;i++)
		{
			var spot = this._spotsExpanded[i];
			this.updateSpot(spot);
		}
		
		this.autoPlayGo();
	};
	
	/**
	 * The window has been resized, or the deck has been changed
	 */
	this.updateBoard = function(updateBoardFinishHandler)
	{
		var self = this;
			
		SOLITAIRE.lastState = SOLITAIRE.state;
		
		if(typeof updateBoardFinishHandler!=="undefined")
			this._updateBoardFinishHandler = updateBoardFinishHandler;
		else
			this._updateBoardFinishHandler = null;
		
		this.createIdealBoard();

		this._deck.preloadDeckSize(this._deckSize, this._cardScale, function() { self.updateBoardFinish(); });
	};
	
	/**
	 * Finish updating the board
	 */
	this.updateBoardFinish = function()
	{	
		var self = this;
		
		CARD.updateAllCards(this._deck, this._deckSize, this._cardScale);
		
		for(var i=0;i<this._spotsExpanded.length;i++)
		{
			var spot = this._spotsExpanded[i];

			if(spot.hasOwnProperty("base"))
			{
				spot["baseCard"].spot = spot;
				spot["baseCard"].game = self;
				spot["baseCard"].moveTo(spot["screenX"], spot["screenY"], -1);
			}
			
			for(var m=0;m<spot.cards.length;m++)
			{
				var card = spot.cards[m];
				card.spot = spot;
				card.game = self;
				
				card.moveOptions = { "skipMoveAnimation" : true };
			}
		
			this.updateSpot(spot);
		}
		
		SOLITAIRE.state = SOLITAIRE.lastState;
		
		if(this._updateBoardFinishHandler!==null)
			this._updateBoardFinishHandler();
		this._updateBoardFinishHandler = null;
	};
	
	/**
	 * This function is called when you need to auto move a card
	 * @param {Object} cardDragged Which card to play
	 * @param {Object} cardDroppedOn Which card to play it on
	 */
	this.doCardDrop = function(cardDragged, cardDroppedOn)
	{
		if(cardDragged.isDragging || cardDroppedOn.isDragging)
			return;
		
		if(!cardDragged.hasOwnProperty("moveOptions"))
			cardDragged.moveOptions = {};
		cardDragged.moveOptions["moveSpeed"] = SOLITAIRE.CARD_AUTOPLAY_SPEED;
		
		if(!cardDroppedOn.hasOwnProperty("moveOptions"))
			cardDroppedOn.moveOptions = {};
		cardDroppedOn.moveOptions["moveSpeed"] = SOLITAIRE.CARD_AUTOPLAY_SPEED;
		
		this.cardDropped(cardDragged, cardDroppedOn);
	};
	
	/**
	 * This function is called when you need to auto click a card
	 * @param {Object} card Which card to click on
	 */
	this.doCardClick = function(card)
	{
		if(card.isDragging)
			return;
			
		if(!card.hasOwnProperty("moveOptions"))
			card.moveOptions = {};
		card.moveOptions["moveSpeed"] = SOLITAIRE.CARD_AUTOPLAY_SPEED;
		
		this.cardClicked(null, card);
	};
	
	/**
	 * This will turn a card face up
	 * @param {Object} card The card to turn face up
	 */
	this.turnCardFaceUp = function(card, delay, turnOptions)
	{
		var turnOverDelay = null;
		if(typeof delay!="undefined")
			turnOverDelay = delay;
		else if(typeof turnOptions!="undefined" && turnOptions.hasOwnProperty("turnOverDelay"))
			turnOverDelay = turnOptions["turnOverDelay"];

		if(typeof turnOptions=="undefined")
			turnOptions = null;
		
		if(!this.canAnimate())
			card.moveOptions["skipTurnAnimation"] = true;

		card.turnFaceUp(this.canAnimate() ? turnOverDelay : -1, turnOptions);
		this.updateCardHandlers(card);
	};

	/**
	 * This will turn a card face up
	 * @param {Object} card The card to turn face up
	 */
	this.turnCardFaceDown = function(card, delay, turnOptions)
	{
		var turnOverDelay = null;
		if(typeof delay!="undefined")
			turnOverDelay = delay;
		else if(typeof turnOptions!="undefined" && turnOptions.hasOwnProperty("turnOverDelay"))
			turnOverDelay = turnOptions["turnOverDelay"];

		if(typeof turnOptions=="undefined")
			turnOptions = null;
		
		if(!this.canAnimate())
			card.moveOptions["skipTurnAnimation"] = true;

		card.turnFaceDown(this.canAnimate() ? turnOverDelay : -1, turnOptions);
		this.updateCardHandlers(card);
	};
	
	/**
	 * This function is called when you need to undo a move
	 */
	this.undoMove = function()
	{
		var self = this;
		
		if(this._undoMoves.length<1 || SOLITAIRE.state!==CONSTANTS.STATE_PLAYING)
			return false;
			
		this._autoPlayPaused = true;
			
		var moveToUndoArray = this._undoMoves.pop();
		
		var spotsToUpdate = [];
		
		for(var i=moveToUndoArray.length-1;i>=0;i--)
		{
			var moveToUndo = moveToUndoArray[i];

			if(!moveToUndo.hasOwnProperty("moveOptions"))
				moveToUndo["moveOptions"] = {};
				
			moveToUndo.moveOptions["isUndo"] = true;

			var oldSpot = moveToUndo["card"].spot;
			if(A.find(spotsToUpdate, oldSpot)===-1)
				spotsToUpdate.push(oldSpot);

			if(moveToUndo.hasOwnProperty("undoType") && moveToUndo["undoType"]=="cardFlip")
			{
				if(moveToUndo["flipDirection"]=="up")
					this.turnCardFaceUp(moveToUndo["card"]);
				else					
					this.turnCardFaceDown(moveToUndo["card"]);
			}
			else
			{
				this.moveCard(moveToUndo["card"], moveToUndo["spotClass"], moveToUndo["spotSubIndex"], moveToUndo["moveOptions"], moveToUndo["locToInsert"]);
				
				var newSpot = this.getSpot(moveToUndo["spotClass"], moveToUndo["spotSubIndex"]);
				if(A.find(spotsToUpdate, newSpot)===-1)
					spotsToUpdate.push(newSpot);

				if(this._game.hasOwnProperty("getPostDropUpdateSpotList"))
				{
					var updateSpotList = this._game.getPostDropUpdateSpotList(moveToUndo["card"], moveToUndo["card"]);
					for(var z=0;z<updateSpotList.length;z++)
					{
						if(A.find(spotsToUpdate, updateSpotList[z])===-1)
							spotsToUpdate.push(updateSpotList[z]);
					}					
				}
			}
			
			if(moveToUndo.hasOwnProperty("undoCallWhenUndone") && moveToUndo["undoCallWhenUndone"]!==null)
				moveToUndo["undoCallWhenUndone"]();
		}

		for(var i=0;i<spotsToUpdate.length;i++)		
		{
			var spotToUpdate = spotsToUpdate[i];
			this.updateSpot(spotsToUpdate[i], null, null, true);
		}
		
		if(this._game.hasOwnProperty("postDropHandler"))
			this._game.postDropHandler(self);
		
		return true;
	};
		
	/**
	 * Reverse the order of the cards for the last undo move
	 */
	this.reverseLastUndoMoveOrder = function()
	{
		if (this._undoMoves.length > 0) 
		{
			for(var i=0;i<this._undoMoves[this._undoMoves.length - 1].length;i++)
			{
				this._undoMoves[this._undoMoves.length - 1][i]["locToInsert"] = null;
			}
			
			this._undoMoves[this._undoMoves.length - 1].reverse();
		}
	};

	/**
	 * This function is called when the this._game() needs to move a card around
	 * @param {Object} card Which card to move
	 * @param {Object} spotClass Which spot class it's going to
	 * @param {Object} spotSubIndex Which subindex of that spot to go to
	 * @param {Object} options Additional options
	 */
	this.moveCard = function(card, spotClass, spotSubIndex, options, locToInsert)
	{
		var self = this;
		
		if(typeof spotSubIndex=="undefined" || spotSubIndex===null)
			spotSubIndex = 0;
			
		if(typeof options=="undefined" || options===null)
			options = {};
			
		if(typeof locToInsert=="undefined" || locToInsert===null)
			locToInsert = 0;
					
		// Only certain existing move options are allowed to stay
		var newMoveOptions = O.clone(options);
		if(card.hasOwnProperty("moveOptions"))
		{
			if(card.moveOptions.hasOwnProperty("moveSpeed"))
				newMoveOptions["moveSpeed"] = card.moveOptions["moveSpeed"];
			if(card.moveOptions.hasOwnProperty("moveSpeedFunction"))
				newMoveOptions["moveSpeedFunction"] = card.moveOptions["moveSpeedFunction"];
		}
		card.moveOptions = newMoveOptions;
		
		if(SOLITAIRE.state===CONSTANTS.STATE_PLAYING && card.spot!==null &&
		   (!card.moveOptions.hasOwnProperty("isUndo") || card.moveOptions["isUndo"]!==true))
		{
			// We are an actual move, not an undo move
			if(this._autoPlayTrying===true)
				this._autoPlayedCards.push(card);

			if(this._autoPlayTrying!==true && A.find(this._autoPlayedCards, card)!==-1)
			{
				var autoPlayCardLoc = -1;
				while((autoPlayCardLoc=A.find(this._autoPlayedCards, card))!==-1)
				{
					this._autoPlayedCards.splice(autoPlayCardLoc, 1);
				}
				this._autoPlayPaused = true;
			}
			
			var moveToUndo = {};
			moveToUndo["card"] = card;
			moveToUndo["spotClass"] = card.spot["class"];
			moveToUndo["spotSubIndex"] = card.spot["subIndex"];
			moveToUndo["locToInsert"] = A.find(card.spot.cards, card);
			//moveToUndo["locToInsert"] = null;
			moveToUndo["moveOptions"] = {};
			if(card.moveOptions.hasOwnProperty("turnFaceUp")&& card.moveOptions.turnFaceUp===true)
				moveToUndo.moveOptions["turnFaceDown"] = true;
			if(card.moveOptions.hasOwnProperty("turnFaceDown")&& card.moveOptions.turnFaceDown===true)
				moveToUndo.moveOptions["turnFaceUp"] = true;
			if(card.moveOptions.hasOwnProperty("undoCallWhenUndone"))
				moveToUndo["undoCallWhenUndone"] = card.moveOptions["undoCallWhenUndone"];
			
			if(card.moveOptions.hasOwnProperty("undoAppendToLastMove") && card.moveOptions["undoAppendToLastMove"]===true &&
			   this._undoMoves.length>0)
			{
				this._undoMoves[this._undoMoves.length-1].push(moveToUndo);
			}
			else
			{
				var moveToUndoArray = [];
				moveToUndoArray.push(moveToUndo);
				
				this._undoMoves.push(moveToUndoArray);
			}
		}
		
		/*if(card.hasOwnProperty("moveOptions"))
			O.merge(card.moveOptions, options, true);
		else
			card.moveOptions = O.clone(options);*/
		
		if(!this.canAnimate())
		{
			card.moveOptions["skipMoveAnimation"] = true;
			card.moveOptions["skipTurnAnimation"] = true;
		}
		
		if(spotClass=="foundation" && (card.spot===null || card.spot["class"]!="foundation"))
			SOLITAIRE.scoreIncrement(100);
		else if(card.spot!==null && card.spot["class"]=="foundation" && spotClass!="foundation")
			SOLITAIRE.scoreIncrement(-100);
		
		// Move to new spot
		var spot = this.getSpot(spotClass, spotSubIndex);
		
		if(locToInsert===CONSTANTS.BOTTOM_OF_STACK)
			spot.cards.push(card);
		else
			spot.cards.splice(locToInsert, 0, card);

		// Take out of old spot		
		if(card.spot!==null)
			card.spot.cards.splice(A.find(card.spot.cards, card), 1);
		
		card.spot = spot;
	};
	
	/**
	 * Swaps two cards with each other
	 * @param {Object} cardOne
	 * @param {Object} cardTwo
	 */
	this.swapCards = function(cardOne, cardTwo, supportUndo, updateSpots)
	{
		var cardOneSpot = cardOne.spot;
		var cardTwoSpot = cardTwo.spot;
		
		if(!this.canAnimate())
		{
			cardOne.moveOptions["skipMoveAnimation"] = true;
			cardOne.moveOptions["skipTurnAnimation"] = true;
			cardTwo.moveOptions["skipMoveAnimation"] = true;
			cardTwo.moveOptions["skipTurnAnimation"] = true;
		}

		if(SOLITAIRE.state===CONSTANTS.STATE_PLAYING && cardOne.spot!==null && cardTwo.spot!==null &&
		   (!cardOne.moveOptions.hasOwnProperty("isUndo") || cardOne.moveOptions["isUndo"]!==true) &&
		   (!cardTwo.moveOptions.hasOwnProperty("isUndo") || cardTwo.moveOptions["isUndo"]!==true) &&
		   (typeof supportUndo!="undefined" && supportUndo===true))
		{
			var cardOneUndo = {};
			cardOneUndo["card"] = cardOne;
			cardOneUndo["spotClass"] = cardOne.spot["class"];
			cardOneUndo["spotSubIndex"] = cardOne.spot["subIndex"];
			cardOneUndo["locToInsert"] = A.find(cardOne.spot.cards, cardOne);
			cardOneUndo["moveOptions"] = {};
			if(cardOne.moveOptions.hasOwnProperty("turnFaceUp")&& cardOne.moveOptions.turnFaceUp===true)
				cardOneUndo.moveOptions["turnFaceDown"] = true;
			if(cardOne.moveOptions.hasOwnProperty("turnFaceDown")&& cardOne.moveOptions.turnFaceDown===true)
				cardOneUndo.moveOptions["turnFaceUp"] = true;
			if(cardOne.moveOptions.hasOwnProperty("undoCallWhenUndone"))
				cardOneUndo["undoCallWhenUndone"] = cardOne.moveOptions["undoCallWhenUndone"];
				
			var cardTwoUndo = {};
			cardTwoUndo["card"] = cardTwo;
			cardTwoUndo["spotClass"] = cardTwo.spot["class"];
			cardTwoUndo["spotSubIndex"] = cardTwo.spot["subIndex"];
			cardTwoUndo["locToInsert"] = A.find(cardTwo.spot.cards, cardTwo);
			cardTwoUndo["moveOptions"] = {};
			if(cardTwo.moveOptions.hasOwnProperty("turnFaceUp")&& cardTwo.moveOptions.turnFaceUp===true)
				cardTwoUndo.moveOptions["turnFaceDown"] = true;
			if(cardTwo.moveOptions.hasOwnProperty("turnFaceDown")&& cardTwo.moveOptions.turnFaceDown===true)
				cardTwoUndo.moveOptions["turnFaceUp"] = true;
			if(cardTwo.moveOptions.hasOwnProperty("undoCallWhenUndone"))
				cardTwoUndo["undoCallWhenUndone"] = cardTwo.moveOptions["undoCallWhenUndone"];

			var moveToUndoArray = [];
			moveToUndoArray.push(cardOneUndo);
			moveToUndoArray.push(cardTwoUndo);
			
			this._undoMoves.push(moveToUndoArray);
			
			SOLITAIRE.movesIncrement();
		}
		
		/*if(cardTwo.spot["class"]=="foundation" && (cardOne.spot===null || cardOne.spot["class"]!="foundation"))
			SOLITAIRE.scoreIncrement(100);
		else if(cardOne.spot!==null && cardOne.spot["class"]=="foundation" && cardTwo.spot["class"]!="foundation")
			SOLITAIRE.scoreIncrement(-100);
		
		if(cardOne.spot["class"]=="foundation" && (cardTwo.spot===null || cardTwo.spot["class"]!="foundation"))
			SOLITAIRE.scoreIncrement(100);
		else if(cardTwo.spot!==null && cardTwo.spot["class"]=="foundation" && cardOne.spot["class"]!="foundation")
			SOLITAIRE.scoreIncrement(-100);*/
		
		var cardOneLoc = -1;
		for(var i=0;i<cardOneSpot.cards.length;i++)
		{
			if(cardOneSpot.cards[i]===cardOne)
			{
				cardOneLoc = i;
				break;
			}
		}
		
		var cardTwoLoc = -1;
		for(var i=0;i<cardTwoSpot.cards.length;i++)
		{
			if(cardTwoSpot.cards[i]===cardTwo)
			{
				cardTwoLoc = i;
				break;
			}
		}
		
		cardOneSpot.cards[cardOneLoc] = cardTwo;
		cardTwoSpot.cards[cardTwoLoc] = cardOne;
		
		cardOne.spot = cardTwoSpot;
		cardTwo.spot = cardOneSpot;
		
		if(typeof updateSpots!="undefined" && updateSpots===true)
		{
			this.updateSpot(cardOne.spot);
			this.updateSpot(cardTwo.spot);
		}
	};
	
	/**
	 * Updates all the cards in a spot
	 * @param {Object} spot The spot to update cards in
	 */
	this.updateSpot = function(spot, reverseOrder, delay, isUndoUpdate)
	{
		var self = this;
		
		if(typeof delay!="undefined" && delay!==null && delay>0)
		{
			setTimeout(function() { self.updateSpot(spot, reverseOrder); }, delay);
			return;
		}
		
		if(spot.hasOwnProperty("reverseUpdate") && spot["reverseUpdate"]===true)
			reverseOrder = true;
			
		if(typeof reverseOrder!=="undefined" && reverseOrder!==null && reverseOrder===true)
		{
			for(var i=0;i<spot.cards.length;i++)
			{
				var card = spot.cards[i];
				if(i==(spot.cards.length-1))
					card.moveOptions["postAnimation"] = function() { self.updateSpotFinal(spot); };
				this.updateCardPosition(card);
			}
		}
		else
		{
			for(var i=spot.cards.length-1;i>=0;i--)
			{
				var card = spot.cards[i];
				if(i===0)
					card.moveOptions["postAnimation"] = function() { self.updateSpotFinal(spot); };
				this.updateCardPosition(card);
			}
		}
				
		if(spot.hasOwnProperty("base") && spot["base"].hasOwnProperty("droppable") && spot["base"]["droppable"]===true)
		{
			if(spot.cards.length===0)
				spot["baseCard"].enableDropping();
			else
				spot["baseCard"].disableDropping();
		}
		
		// Auto-flip cards
		if(Options.get("optionAutoFlip")===true &&
		   spot.hasOwnProperty("autoFlip") &&
		   spot.cards.length>0 &&
		   !spot.cards[0].isFaceUp() &&
		   (!spot["autoFlip"].hasOwnProperty("askGame") || spot["autoFlip"]["askGame"]!==true || this._game.isCardLive(card, "autoFlip")))
		{
			if(SOLITAIRE.state===CONSTANTS.STATE_PLAYING && this._undoMoves.length>0)
			{
				var lastMoveToUndoArray = this._undoMoves[this._undoMoves.length-1];
				for(var i=lastMoveToUndoArray.length-1;i>=0;i--)
				{
					var lastMoveToUndo = lastMoveToUndoArray[i];
					if(lastMoveToUndo.hasOwnProperty("spotClass") &&
					   lastMoveToUndo["spotClass"]===spot["class"] &&
					   lastMoveToUndo.hasOwnProperty("spotSubIndex") &&
					   lastMoveToUndo["spotSubIndex"]===spot["subIndex"])
					{
						var moveToUndo = {};
						moveToUndo["undoType"] = "cardFlip";
						moveToUndo["card"] = spot.cards[0];
						moveToUndo["flipDirection"] = "down";
						this._undoMoves[this._undoMoves.length-1].push(moveToUndo);
					
						break;	
					}
				}
			}
			
			spot.cards[0].turnFaceUp(!this.canAnimate() ? -1 : null);
			this.updateCardHandlers(spot.cards[0]);
		}
		
		if(spot.hasOwnProperty("monitorChanges") && spot["monitorChanges"]===true && !A.equalTo(spot.cards, spot["lastUpdateCardsSnapshot"]))
			this._game.spotChanged(self, spot, typeof isUndoUpdate!="undefined" ? isUndoUpdate===true : false);

		spot["lastUpdateCardsSnapshot"] = A.clone(spot.cards);
		
		if(SOLITAIRE.state===CONSTANTS.STATE_PLAYING)
		{
			this.checkGameWon();
			//this.checkGameLost();		// May return in the future!
			this.autoPlayGo();
		}
	};
	
	/**
	 * Will play the next auto play card or start it if needed
	 */
	this.autoPlayGo = function()
	{
		var self = this;

		this._autoPlayCounter++;
		
		var optionAutoPlay = Options.get("optionAutoPlay");
		if(this._autoPlayPaused===false && optionAutoPlay!=="off" && this._autoPlayTimeoutID===null)
		{
			if(!CARD.isCurrentlyAnimating())
				this._autoPlayTimeoutID = setTimeout(function() { self.autoPlayDispatcher(); }, 0);
			else
				this._autoPlayTimeoutID = CARD.runAfterAnimations(function() { self.autoPlayDispatcher(); });
		}
	};
	
	/**
	 * Makes automatic plays if the option is turned on
	 */
	this.autoPlayDispatcher = function()
	{
		var self = this;
	
		if(this._autoPlayCounter>4)
			this._autoPlayCounter = 4;
			
		this._autoPlayCounter--;
		
		var optionAutoPlay = Options.get("optionAutoPlay");
		if(this._autoPlayPaused===false && optionAutoPlay!=="off" && this._undoMoves.length>0)
		{
			var madeMove = false;
			
			this._autoPlayTrying = true;
			
			if(!CARD.isCurrentlyAnimating() && this._game.hasOwnProperty("autoPlay"))
				madeMove = this._game.autoPlay(self, optionAutoPlay);
				
			this._autoPlayTrying = false;
			
			if(this._autoPlayCounter>0)
			{
				if(!CARD.isCurrentlyAnimating())
					this._autoPlayTimeoutID = setTimeout(function() { self.autoPlayDispatcher(); }, (madeMove ? CONSTANTS.AUTOPLAY_MIN_INTERVAL : CONSTANTS.AUTOPLAY_MAX_INTERVAL));
				else
					this._autoPlayTimeoutID = CARD.runAfterAnimations(function() { self.autoPlayDispatcher(); });
			}
			else
				this._autoPlayTimeoutID = null;
		}
		else
		{
			this._autoPlayTimeoutID = null;
		}
	};
	
	/**
	 * Updates a card location
	 * @param {Object} card The card being updated
	 */
	this.updateCardPosition = function(card)
	{
		var self = this;

		var spot = card.spot;
		
		if(!card.hasOwnProperty("moveOptions"))
			card.moveOptions = {};			
		
		if(card.moveOptions.hasOwnProperty("turnFaceUp") && card.moveOptions["turnFaceUp"]===true)
			card.moveOptions["afterFaceUp"] = function() { self.updateCardHandlers(card); };
			
		if(card.moveOptions.hasOwnProperty("turnFaceDown") && card.moveOptions["turnFaceDown"]===true)
			card.moveOptions["afterFaceDown"] = function() { self.updateCardHandlers(card); };
			
		card.stackIndex = A.find(spot.cards, card);
		card.coordinates = this.calculateSpotCoordinates(spot, card.stackIndex, ((spot.cards.length-1)-card.stackIndex));

		card.finalZIndex = Math.max(1, (card.spot.startingZIndex+(card.spot.cards.length-card.stackIndex)));

		var domCard = card.getDomCard();
		var coordinates = D.xy(domCard);
		
		if(card.isDragging)
		{
			card.moveOptions = {};
			return;
		}
		
		if(!this.canAnimate())
		{
			card.moveOptions["skipMoveAnimation"] = true;
			card.moveOptions["skipTurnAnimation"] = true;
		}
				
		if(card.coordinates[0]!=coordinates[0] || card.coordinates[1]!=coordinates[1])
		{
			card.moveOptions["preAnimation"] = function() { card.setZIndex(card.finalZIndex + 400); };
			
			var moveSpeed = SOLITAIRE.CARD_MOVE_SPEED;
			if(card.moveOptions.hasOwnProperty("moveSpeed"))
				moveSpeed = parseFloat(card.moveOptions["moveSpeed"]);
			if(card.moveOptions.hasOwnProperty("moveSpeedFunction"))
				moveSpeed = parseFloat(card.moveOptions.moveSpeedFunction());
			
			card.moveTo(card.coordinates[0],
			    		card.coordinates[1],
						(card.moveOptions.hasOwnProperty("skipMoveAnimation") && card.moveOptions["skipMoveAnimation"]===true) ? -1 : ((moveSpeed*0.08)*1000),
						card.moveOptions);
		}
		else
		{
			if(card.moveOptions.hasOwnProperty("turnFaceUp") && card.moveOptions["turnFaceUp"]===true)
			{
				card.turnFaceUp(card.moveOptions.hasOwnProperty("skipTurnAnimation") && card.moveOptions["skipTurnAnimation"]===true ? -1 : null, card.moveOptions);
				this.updateCardHandlers(card);
			}
			
			if(card.moveOptions.hasOwnProperty("turnFaceDown") && card.moveOptions["turnFaceDown"]===true)
			{
				card.turnFaceDown(card.moveOptions.hasOwnProperty("skipTurnAnimation") && card.moveOptions["skipTurnAnimation"]===true ? -1 : null, card.moveOptions);
				this.updateCardHandlers(card);
			}
			
			if(card.moveOptions.hasOwnProperty("postAnimation"))
				card.moveOptions["postAnimation"]();
		}
		
		if(!card.moveOptions.hasOwnProperty("turnFaceUp") || card.moveOptions["turnFaceUp"]!==true)
			this.updateCardHandlers(card);
			
		if(!card.moveOptions.hasOwnProperty("turnFaceDown") || card.moveOptions["turnFaceDown"]!==true)
			this.updateCardHandlers(card);
			
		card.moveOptions = {};
	};
		
	/**
	 * This will update all the card handlers in the spot of question
	 * @param {Object} spot
	 */
	this.updateSpotCardHandlers = function(spot)
	{
		for(var i=0;i<spot.cards.length;i++)
		{
			this.updateCardHandlers(spot.cards[i]);
		}
	};
	
	/**
	 * This will update all the cards in a spot with proper ZIndexes
	 * @param {Object} spot
	 */
	this.updateSpotFinal = function(spot)
	{
		for(var m=spot.cards.length-1;m>=0;m--)
		{
			var card = spot.cards[m];
			card.setZIndex(card.finalZIndex);
		}
		
		var visibleIndexes = [ ];
		var lastIndex = 0;
		for(var i=0;i<spot.stackTypes.length;i++)
		{
			var stackType = spot.stackTypes[i];
			if(stackType["type"]!="deck")
			{
				if(stackType.hasOwnProperty("index"))
				{
					visibleIndexes = A.append(visibleIndexes, stackType["index"]);
					lastIndex = stackType["index"][(stackType["index"].length-1)];
				}
				else
				{
					for(var z=(lastIndex+1);z<spot.actualStackMax;z++)
					{
						visibleIndexes.push(z);						
					}
				}
			}
		}
		
		visibleIndexes.push(lastIndex);
		visibleIndexes.push((lastIndex+1));
		visibleIndexes.push((lastIndex+2));
				
		for(var m=spot.cards.length-1;m>=0;m--)
		{
			var card = spot.cards[m];
			
			if(A.find(visibleIndexes, card.stackIndex)!==-1)
				card.show();
			else
				card.hide();
		}
	};
	
	/**
	 * This will update a card with the proper click and drag handlers
	 * @param {Object} card The card being updated
	 */
	this.updateCardHandlers = function(card)
	{
		var self = this;

		var spot = card.spot;
		
		if(SOLITAIRE.state!==CONSTANTS.STATE_PLAYING)
			return;
		
		// Handle adding or removing click handlers
		if(spot.hasOwnProperty("clickable") &&
		   ((((!spot["clickable"].hasOwnProperty("faceUpOnly") || spot["clickable"]["faceUpOnly"]===true) &&  card.isFaceUp()) ||
		     (( spot["clickable"].hasOwnProperty("faceUpOnly") && spot["clickable"]["faceUpOnly"]!==true) && !card.isFaceUp())) ||
		    (((!spot["clickable"].hasOwnProperty("faceDownOnly") || spot["clickable"]["faceDownOnly"]!==true) &&  card.isFaceUp()) ||
		     (( spot["clickable"].hasOwnProperty("faceDownOnly") && spot["clickable"]["faceDownOnly"]===true) && !card.isFaceUp()))) &&
		   (((!spot["clickable"].hasOwnProperty("topCardOnly") || spot["clickable"]["topCardOnly"]===true) && card.stackIndex===0) ||
		    (( spot["clickable"].hasOwnProperty("topCardOnly") && spot["clickable"]["topCardOnly"]!==true) && card.stackIndex>=0)) &&
		   (!spot["clickable"].hasOwnProperty("askGame") || spot["clickable"]["askGame"]!==true || this._game.isCardLive(card, "clickable")))
		{
			if(!card.hasClickHandler)
			{
				E.add(card.getDomCard(), "click", this.cardClicked, card, this);
				card.hasClickHandler = true;
			}
		}
		else
		{
			if(card.hasClickHandler)
			{
				E.remove(card.getDomCard(), "click", this.cardClicked);
				card.hasClickHandler = false;
			}
		}
		
		// Handle adding or removing double handlers
		if(spot.hasOwnProperty("doubleclickable") &&
		   ((((!spot["doubleclickable"].hasOwnProperty("faceUpOnly") || spot["doubleclickable"]["faceUpOnly"]===true) &&  card.isFaceUp()) ||
		     (( spot["doubleclickable"].hasOwnProperty("faceUpOnly") && spot["doubleclickable"]["faceUpOnly"]!==true) && !card.isFaceUp())) ||
		    (((!spot["doubleclickable"].hasOwnProperty("faceDownOnly") || spot["doubleclickable"]["faceDownOnly"]!==true) &&  card.isFaceUp()) ||
		     (( spot["doubleclickable"].hasOwnProperty("faceDownOnly") && spot["doubleclickable"]["faceDownOnly"]===true) && !card.isFaceUp()))) &&
		   (((!spot["doubleclickable"].hasOwnProperty("topCardOnly") || spot["doubleclickable"]["topCardOnly"]===true) && card.stackIndex===0) ||
		    (( spot["doubleclickable"].hasOwnProperty("topCardOnly") && spot["doubleclickable"]["topCardOnly"]!==true) && card.stackIndex>=0)) &&
		   (!spot["doubleclickable"].hasOwnProperty("askGame") || spot["doubleclickable"]["askGame"]!==true || this._game.isCardLive(card, "doubleclickable")))
		{
			if(!card.hasDoubleClickHandler)
			{
				E.add(card.getDomCard(), "dblclick", this.cardDoubleClicked, card, this);
				E.add(card.getDomCard(), "contextmenu", this.cardDoubleClicked, card, this);
				card.hasDoubleClickHandler = true;
			}
		}
		else
		{
			if(card.hasDoubleClickHandler)
			{
				E.remove(card.getDomCard(), "dblclick", this.cardDoubleClicked);
				E.remove(card.getDomCard(), "contextmenu", this.cardDoubleClicked);
				card.hasDoubleClickHandler = false;
			}
		}		
		
		// Handle adding or removing drag handlers
		if(spot.hasOwnProperty("draggable") &&
		   (((!spot["draggable"].hasOwnProperty("faceUpOnly") || spot["draggable"]["faceUpOnly"]===true) &&  card.isFaceUp()) ||
		    (( spot["draggable"].hasOwnProperty("faceUpOnly") && spot["draggable"]["faceUpOnly"]!==true) && !card.isFaceUp())) &&
		   (((!spot["draggable"].hasOwnProperty("topCardOnly") || spot["draggable"]["topCardOnly"]===true) && card.stackIndex===0) ||
		    (( spot["draggable"].hasOwnProperty("topCardOnly") && spot["draggable"]["topCardOnly"]!==true) && card.stackIndex>=0)) &&
		   (!spot["draggable"].hasOwnProperty("askGame") || spot["draggable"]["askGame"]!==true || this._game.isCardLive(card, "draggable")))
		{
			card.enableDragging();
			card.isDraggable = true;
		}
		else
		{
			card.isDraggable = false;
			card.disableDragging();
		}
		
		// Handle adding or removing drop handlers
		if(spot.hasOwnProperty("droppable") &&
		   card.stackIndex===0 &&
		   (((!spot["droppable"].hasOwnProperty("faceUpOnly") || spot["droppable"]["faceUpOnly"]===true) &&  card.isFaceUp()) ||
		    (( spot["droppable"].hasOwnProperty("faceUpOnly") && spot["droppable"]["faceUpOnly"]!==true) && !card.isFaceUp())) &&
		   (!spot["droppable"].hasOwnProperty("askGame") || spot["droppable"]["askGame"]!==true || this._game.isCardLive(card, "droppable")))
		{
			card.enableDropping();
		}
		else
		{
			card.disableDropping();
		}
		
		var domCard = card.getDomCard();
		if(card.hasClickHandler || card.isDraggable || card.hasDoubleClickHandler)
			domCard.style.cursor = (P.getBrowser()=="ie" ? "hand" : "pointer");
		else
			domCard.style.cursor = "default";
	};
	
	/**
	 * This is called when a card is moused over
	 * @param {Object} card
	 */
	this.cardMousedOver = function(card)
	{
		var mouseOverInfoStackCount = document.getElementById("mouseOverInfoStackCount");
		D.clear(mouseOverInfoStackCount);
		if(card.spot!==null)
			mouseOverInfoStackCount.appendChild(document.createTextNode(I.formatWithCommas(card.spot.cards.length)));

		var mouseOverInfoCard = document.getElementById("mouseOverInfoCard");
		D.clear(mouseOverInfoCard);
		if(card.isFaceUp())
			mouseOverInfoCard.appendChild(document.createTextNode(card.name));
	};

	/**
	 * This is called when a card is moused out of
	 * @param {Object} card
	 */
	this.cardMousedOut = function(card)
	{
		var mouseOverInfoStackCount = document.getElementById("mouseOverInfoStackCount");
		D.clear(mouseOverInfoStackCount);

		var mouseOverInfoCard = document.getElementById("mouseOverInfoCard");
		D.clear(mouseOverInfoCard);
	};
	
	/**
	 * Event handler for when a valid card is clicked
	 * @param {Object} e The event object
	 * @param {Object} card The card that was clicked
	 */
	this.cardClicked = function(e, card)
	{
		var self = this;
		var spot = card.spot;
		
		this._autoPlayPaused = false;
		
		if(card.isBase && spot["base"].hasOwnProperty("clickLimit"))
		{
			if(spot.hasOwnProperty("timesBaseClicked") && spot["timesBaseClicked"]>=spot["base"]["clickLimit"])
			{
				this.renderRightBaseImage(spot);
				return;
			}
				
			if(!spot.hasOwnProperty("timesBaseClicked"))
				spot["timesBaseClicked"] = 1;
			else
				spot["timesBaseClicked"]++;
			
			this.renderRightBaseImage(spot);
		}
			
		if(card.hasOwnProperty("skipNextClick") && card.skipNextClick===true)
		{
			card.skipNextClick = false;
			return;
		}
				
		if(!card.isBase && spot.hasOwnProperty("turnUpOnClick") && spot["turnUpOnClick"]===true && !card.isFaceUp())
		{
			this.turnCardFaceUp(card);
		}
		else if(!card.isBase && spot.hasOwnProperty("moveOnClick") &&
		        (!spot.moveOnClick.hasOwnProperty("askGame") || spot.moveOnClick["askGame"]!==true || this._game.canMoveOnClick(card)))
		{
			var count = spot.moveOnClick.hasOwnProperty("count") ? spot.moveOnClick["count"] : 1;
			
			var options = {};	
			if(spot.moveOnClick.hasOwnProperty("moveOptions"))
				options = O.clone(spot.moveOnClick["moveOptions"]);

			var targetSpots = this.getSpots(spot.moveOnClick["spot"]);
			
			if(spot.moveOnClick.hasOwnProperty("targetCannotBeEmpty") && spot.moveOnClick["targetCannotBeEmpty"]===true)
			{
				for(var i=0;i<targetSpots.length;i++)
				{
					if(targetSpots[i].cards.length<1)
					{
						// Notify user!
						return;
					}
				}
			}
			
			SOLITAIRE.movesIncrement();
			
			for(var i=0;i<targetSpots.length;i++)
			{
				var targetSpot = targetSpots[i];
				
				var targetStackIndex = card.stackIndex;
				for(var m=0;m<count;m++)
				{
					if(spot.cards.length<=targetStackIndex)
						break;
					
					if(m>0 || i>0)
						options["undoAppendToLastMove"] = true;
					else
						options["undoAppendToLastMove"] = false;
	
					options["turnOverDelay"] = SOLITAIRE.CARD_TURN_OVER_DELAY*i;
					
					this.moveCard(spot.cards[targetStackIndex], targetSpot["class"], targetSpot["subIndex"], options);
				}
			}
			
			for(var i=0;i<targetSpots.length;i++)
			{
				var targetSpot = targetSpots[i];
				
				this.updateSpot(targetSpot, spot.moveOnClick.hasOwnProperty("reverseUpdateTarget") ? spot.moveOnClick["reverseUpdateTarget"]===true : false, (SOLITAIRE.CARD_DEAL_SPEED)*i);
			}
			
			this.updateSpot(spot, spot.moveOnClick.hasOwnProperty("reverseUpdateSource") ? spot.moveOnClick["reverseUpdateSource"]===true : false, (SOLITAIRE.CARD_DEAL_SPEED)*i);
		}
		else if(card.isBase && spot["base"].hasOwnProperty("fillOnClick"))
		{
			if(!spot.hasOwnProperty("baseFilledCounter"))
				spot["baseFilledCounter"] = 0;
			
			var fillOnClickProperties = spot["base"]["fillOnClick"];
				
			if(!fillOnClickProperties.hasOwnProperty("maxTimes") || (spot["baseFilledCounter"]+1)<=fillOnClickProperties.maxTimes)
			{
				spot.baseFilledCounter++;
				this._restoreOptions = {};
				this._restoreOptions["optionAutoFlip"] = Options.get("optionAutoFlip");
				Options.set("optionAutoFlip", false, true);
				
				var options = {};	
				if(fillOnClickProperties.hasOwnProperty("moveOptions"))
					options = O.clone(fillOnClickProperties["moveOptions"]);
				
				var targetSpot = this.getSpot(fillOnClickProperties["spot"], fillOnClickProperties.hasOwnProperty("spotSubIndex") ? fillOnClickProperties["spotSubIndex"] : 0);
				for(var i=0;targetSpot.cards.length;i++)
				{
					if(i>0)
					{
						options["undoAppendToLastMove"] = true;
						options["undoCallWhenUndone"] = null;
					}
					else
					{
						options["undoAppendToLastMove"] = false;
						options["undoCallWhenUndone"] = function() { spot.baseFilledCounter--; self.renderRightBaseImage(spot); };
					}
	
					this.moveCard(targetSpot.cards[0], spot["class"], spot["subIndex"], options);
				}
				
				this.updateSpot(spot, fillOnClickProperties.hasOwnProperty("reverseUpdateSource") ? fillOnClickProperties["reverseUpdateSource"]===true : false);
				this.updateSpot(targetSpot, fillOnClickProperties.hasOwnProperty("reverseUpdateTarget") ? fillOnClickProperties["reverseUpdateTarget"]===true : false);
				
				Options.set("optionAutoFlip", this._restoreOptions["optionAutoFlip"], true);
				this._restoreOptions = null;
			}
			
			this.renderRightBaseImage(spot);			
		}
		else
		{			
			this._game.cardClicked(card);
		}
	};
	
	/**
	 * This is called to render the correct base image
	 * @param {Object} spot
	 */
	this.renderRightBaseImage = function(spot)
	{
		if(spot.hasOwnProperty("base"))
		{
			var baseProperties = spot["base"];
			
			if(baseProperties.hasOwnProperty("clickable") && baseProperties["clickable"]===true)
			{
				spot["baseCard"]["id"] = "base_redeal";
				spot["baseCard"].getDomCard().style.cursor = (P.getBrowser()=="ie" ? "hand" : "pointer");
			}
			
			if(baseProperties.hasOwnProperty("fillOnClick"))
			{
				var fillOnClickProperties = baseProperties["fillOnClick"];
				
				if(fillOnClickProperties.hasOwnProperty("maxTimes") && spot.hasOwnProperty("baseFilledCounter") &&
				   spot["baseFilledCounter"]>=fillOnClickProperties.maxTimes)
				{
					spot["baseCard"]["id"] = "base_stop";
					spot["baseCard"].getDomCard().style.cursor = "default";
				}
			}
			
			if(baseProperties.hasOwnProperty("clickable") && baseProperties["clickable"]===true &&
			   baseProperties.hasOwnProperty("clickLimit") && spot.hasOwnProperty("timesBaseClicked") &&
			   spot["timesBaseClicked"]>=baseProperties["clickLimit"])
			{
				spot["baseCard"]["id"] = "base_stop";
				spot["baseCard"].getDomCard().style.cursor = "default";
			}
			
			spot["baseCard"].updateCard();
		}
	};
	
	/**
	 * Event handler for when a valid card is double clicked
	 * @param {Object} e The event object
	 * @param {Object} card The card that was clicked
	 */
	this.cardDoubleClicked = function(e, card)
	{
		this._autoPlayPaused = false;
		
		E.stop(e);

		this._game.cardDoubleClicked(card);
	};	
	
	/**
	 * Called when a card starts being dragged
	 * @param {Object} card The card that is going to be dragged
	 */
	this.cardDragStarted = function(card)
	{
		Menu.disable(true);
		
		var domCard = card.getDomCard();
		CARD.animateStop(domCard.id);
		card.setZIndex(700);
		
		card.isDragging = true;
		
		if(card.stackIndex>0 && (!card.spot.hasOwnProperty("draggable") || !card.spot.draggable.hasOwnProperty("alone") || card.spot.draggable.alone!==true))
		{
			for(var i=card.stackIndex-1;i>=0;i--)
			{
				var anotherCard = card.spot.cards[i];

				var anotherCardDom = anotherCard.getDomCard();
				CARD.animateStop(anotherCardDom.id);
				anotherCard.setZIndex((700+(card.stackIndex-i)));

				anotherCard.isDragging = true;
			}
		}
	};
		
	/**
	 * This is called while a card is being dragged
	 * @param {Object} card The card being dragged
	 */
	this.cardDragged = function(card)
	{
		if(card.stackIndex===0)
			return;
		
		var cardCoords = D.xy(card.getDomCard());
		
		var stackEnd = 0;
		if(card.spot.hasOwnProperty("draggable") && card.spot.draggable.hasOwnProperty("alone") && card.spot.draggable.alone===true)
			stackEnd = card.stackIndex;
		
		for(var i=card.stackIndex-1;i>=stackEnd;i--)
		{
			var anotherCard = card.spot.cards[i];
			var anotherCardDom = anotherCard.getDomCard();

			anotherCardDom.style.top = "" + (anotherCard.coordinates[1]-(card.coordinates[1]-cardCoords[1])) + "px";
			anotherCardDom.style.left = "" + (anotherCard.coordinates[0]-(card.coordinates[0]-cardCoords[0])) + "px";
		}
	};
	
	/**
	 * Called when a card is let go
	 * @param {Object} card The card that was let go of
	 */
	this.cardDragStopped = function(card)
	{
		var self = this;
		var spot = card.spot;
		
		if(card.hasOwnProperty("isDragging") && card.isDragging===true)
			card.skipNextClick = true;
		
		card.isDragging = false;
		
		Menu.enable();
		
		card.coordinates = D.xy(card.getDomCard());
		
		if(card.stackIndex>0 && (!card.spot.hasOwnProperty("draggable") || !card.spot.draggable.hasOwnProperty("alone") || card.spot.draggable.alone!==true))
		{
			for(var i=card.stackIndex-1;i>=0;i--)
			{
				var anotherCard = card.spot.cards[i];
				anotherCard.isDragging = false;
				
				var anotherCardDom = anotherCard.getDomCard();
				anotherCard.coordinates = D.xy(anotherCardDom);
			}
		}

		if(card.stackIndex===0 || (card.spot.hasOwnProperty("draggable") && card.spot.draggable.hasOwnProperty("alone") && card.spot.draggable.alone===true))
			card.moveOptions["postAnimation"] = function() { self.updateSpotFinal(spot); };

		this.updateCardPosition(card);
		if(card.stackIndex>0 && (!card.spot.hasOwnProperty("draggable") || !card.spot.draggable.hasOwnProperty("alone") || card.spot.draggable.alone!==true))
		{
			for(var i=card.stackIndex-1;i>=0;i--)
			{
				var anotherCard = card.spot.cards[i];
				
				if(i===0)
					anotherCard.moveOptions["postAnimation"] = function() { self.updateSpotFinal(spot); };
					
				this.updateCardPosition(anotherCard);
			}
		}
	};	
	
	/**
	 * Event handler for when a card is dropped on another card
	 * @param {Object} e The mouseup event object
	 * @param {Object} cardDroppedOn Which card it was dropped on
	 */
	this.cardDropped = function(cardDragged, cardDroppedOn)
	{
		var self = this;
		
		this._autoPlayPaused = false;
		
		if(cardDragged.hasOwnProperty("isDragging") && cardDragged.isDragging===true)
			cardDragged.skipNextClick = true;

		cardDragged.isDragging = false;
		
		if(cardDragged.stackIndex>0 && (!cardDragged.spot.hasOwnProperty("draggable") || !cardDragged.spot.draggable.hasOwnProperty("alone") || cardDragged.spot.draggable.alone!==true))
		{
			for(var i=cardDragged.stackIndex-1;i>=0;i--)
			{
				var anotherCard = cardDragged.spot.cards[i];
				anotherCard.isDragging = false;
			}
		}
		
		Menu.enable();
		
		var affectedSpots = [];
		affectedSpots.push(cardDroppedOn.spot);
		
		var spot = cardDroppedOn.spot;
		
		// Here we check our game rules to determine what to do with the card that might be valid here
		if(this._game.hasOwnProperty("isCardDropValid") && this._game.isCardDropValid(cardDragged, cardDroppedOn))
		{
			SOLITAIRE.movesIncrement();
			
			var destinationSpotClass = spot["class"];
			var destinationSpotSubIndex = spot["subIndex"];
			
			if(this._game._board.hasOwnProperty("onValidDrop") && this._game._board.onValidDrop.hasOwnProperty("moveToSpot"))
			{
				destinationSpotClass = this._game._board.onValidDrop["moveToSpot"];
				destinationSpotSubIndex = 0;
				
				affectedSpots.push(this.getSpot(destinationSpotClass));
			}
			
			var cardsToMove = [];
			cardsToMove.push(cardDragged);
			
			if(this._game._board.hasOwnProperty("onValidDrop") && this._game._board.onValidDrop.hasOwnProperty("moveCardDroppedOn"))
				cardsToMove.push(cardDroppedOn);

			if(this._game.hasOwnProperty("getPostDropUpdateSpotList"))
			{
				var updateSpotList = this._game.getPostDropUpdateSpotList(cardDragged, cardDroppedOn);
				for(var i=0;i<updateSpotList.length;i++)
				{
					affectedSpots.push(updateSpotList[i]);
				}
			}
			
			if(A.find(affectedSpots, cardDragged.spot)===-1)
				affectedSpots.push(cardDragged.spot);
			
			if(cardDragged.stackIndex>0 && (!cardDragged.spot.hasOwnProperty("draggable") || !cardDragged.spot.draggable.hasOwnProperty("alone") || cardDragged.spot.draggable.alone!==true))
			{
				for(var i=cardDragged.stackIndex-1;i>=0;i--)
				{
					cardsToMove.push(cardDragged.spot.cards[i]);
					if(A.find(affectedSpots, cardDragged.spot.cards[i].spot)===-1)
						affectedSpots.push(cardDragged.spot.cards[i].spot);
				}
			}
			
			var moveOptions = {};
			if(this._game._board.hasOwnProperty("onValidDrop") && this._game._board.onValidDrop.hasOwnProperty("moveOptions"))
				moveOptions = O.clone(this._game._board.onValidDrop["moveOptions"]);
			
			for(i=0;i<cardsToMove.length;i++)
			{
				if(i>0)
					moveOptions["undoAppendToLastMove"] = true;
				else
					moveOptions["undoAppendToLastMove"] = false;
									
				moveOptions["skipSpotUpdate"] = (i<(cardsToMove.length-1));
				this.moveCard(cardsToMove[i], destinationSpotClass, destinationSpotSubIndex, moveOptions);
			}
			
			if(cardsToMove.length>1)
				this.reverseLastUndoMoveOrder();
			
			for(i=0;i<affectedSpots.length;i++)
			{
				this.updateSpot(affectedSpots[i]);
			}
			
			if(this._game.hasOwnProperty("postDropHandler"))
				this._game.postDropHandler(self);
			
			return true;
		}
		
		return false;
	};
	
	/**
	 * This function will check to see if we won, if so the winning activities will begin
	 */
	this.checkGameWon = function()
	{
		var self = this;
		
		if(SOLITAIRE.state!=CONSTANTS.STATE_PLAYING)
			return;
		
		if(!this._game.isGameWon(self))
			return;
			
		SOLITAIRE.state = CONSTANTS.STATE_WON;
		
		CARD.runAfterAnimations(function() { SOLITAIRE.wonGameHandler(); }, 100);
	};
	
	/**
	 * This function will check to see if we lost
	 */
	this.checkGameLost = function()
	{
		var self = this;
		
		if(!this._game.isGameLost(self))
			return;
			
		SOLITAIRE.state = CONSTANTS.STATE_LOST;
		
		CARD.runAfterAnimations(function() { SOLITAIRE.lostGameHandler(); });
	};

	/**
	 * This will return an expandedSpot based on the spotClass and sub index passed
	 * @param {Object} spot
	 * @param {Object} spotSubIndex
	 */
	this.getSpot = function(spotClass, spotSubIndex)
	{
		if(typeof spotSubIndex=="undefined" || spotSubIndex===null)
			spotSubIndex = 0;
		
		var subIndex = 0;
		for(var i=0;i<this._spotsExpanded.length;i++)
		{
			var spot = this._spotsExpanded[i];
			if(spot["class"]==spotClass && spot["subIndex"]===spotSubIndex)
				return spot;
		}
		
		return null;
	};
	
	/**
	 * This will return an array of expanded spots that match the class
	 * @param {Object} spotClass
	 */
	this.getSpots = function(spotClass)
	{
		var foundSpots = [];
		
		for(var i=0;i<this._spotsExpanded.length;i++)
		{
			var spot = this._spotsExpanded[i];
			if(spot["class"]==spotClass)
				foundSpots.push(spot);
		}
		
		return foundSpots;
	};
	
	/**
	 * This will calculate the screen coordinates where  card would go given the parameters
	 * @param {Object} spot
	 * @param {Object} stackIndex
	 * @param {Object} stackIndexTranslated
	 */
	this.calculateSpotCoordinates = function(spot, stackIndex, stackIndexTranslated)
	{		
		var cardWidth = parseInt(this._deck.getCardWidth(this._deckSize, this._cardScale), 10);
		var cardHeight = parseInt(this._deck.getCardHeight(this._deckSize, this._cardScale), 10);
		var spotStackMax = spot["stackMax"];
		/*if(spot.cards.length>spot["stackMax"])
			spotStackMax = Math.floor(spot.cards.length*1.20);*/

		var coordinates = [];
		coordinates[0] = spot["screenX"];
		coordinates[1] = spot["screenY"];
					
		var spotStackTypes = spot["stackTypes"];
		var stackOffsetX = 0;
		var stackOffsetY = 0;
		var runningCardCount = 0;
		for(var t=0;t<spotStackTypes.length;t++)
		{
			var stackType = spotStackTypes[t];
			if(stackType.hasOwnProperty("index"))
				runningCardCount += stackType["index"].length;
			
			if(stackType["type"]=="deck")
				continue;
			
			var percentageToShowX = 0.00;
			var percentageToShowY = 0.00;
			if(stackType["type"]=="peek")
			{
				percentageToShowX = (stackType.hasOwnProperty("peekValue") ? (this._deck._peekX*stackType["peekValue"]) : this._deck._peekX);
				percentageToShowY = (stackType.hasOwnProperty("peekValue") ? (this._deck._peekY*stackType["peekValue"]) : this._deck._peekY);
			}
			
			if(stackType.hasOwnProperty("peekModifier"))
			{
				percentageToShowX *= stackType["peekModifier"];
				percentageToShowY *= stackType["peekModifier"];
			}
			
			var cardOffsetX = parseInt((cardWidth*percentageToShowX), 10);
			var cardOffsetY = parseInt((cardHeight*percentageToShowY), 10);				
			
			var cardCount = stackType.hasOwnProperty("index") ? stackType["index"].length : ((spotStackMax)-runningCardCount);
			
			if(stackType.hasOwnProperty("index"))
			{
				var stackTypeIndex = stackType["index"];
				var stackTypeIndexMatchLoc = -1;
				if((stackTypeIndexMatchLoc = A.find(stackTypeIndex, stackIndex))!=-1)
				{
					if(stackType["direction"].indexOf("E")!=-1)
						coordinates[0] += (stackOffsetX+(cardOffsetX*((stackTypeIndex.length-1)-stackTypeIndexMatchLoc)));
					else if(stackType["direction"].indexOf("W")!=-1)
						coordinates[0] += (stackOffsetX+(cardOffsetX*stackTypeIndexMatchLoc));
					else if(stackType["direction"].indexOf("N")!=-1)
						coordinates[1] += (stackOffsetY+(cardOffsetY*((stackTypeIndex.length-1)-stackTypeIndexMatchLoc)));
					else if(stackType["direction"].indexOf("S")!=-1)
						coordinates[1] += (stackOffsetY+(cardOffsetY*stackTypeIndexMatchLoc));
					
					return coordinates;
				}

				if(stackType["direction"].indexOf("E")!=-1 || stackType["direction"].indexOf("W")!=-1)
					stackOffsetX += (cardOffsetX*(stackTypeIndex.length-1));
				else if(stackType["direction"].indexOf("N")!=-1 || stackType["direction"].indexOf("S")!=-1)
					stackOffsetY += (cardOffsetY*(stackTypeIndex.length-1));
			}
			else	// ATTENTION TODO - Will need to so something similar to the (stackTypeIndex.length-1)-stackTypeIndexMatchLoc) down below too??
			{
				for(var m=runningCardCount;m<=stackIndexTranslated;m++)			
				{
					if(m==stackIndexTranslated)
					{
						if(stackType["direction"].indexOf("E")!=-1)
							coordinates[0] += (stackOffsetX+(cardOffsetX*m));
						else if(stackType["direction"].indexOf("W")!=-1)
							coordinates[0] += (stackOffsetX+(cardOffsetX*m));
						else if(stackType["direction"].indexOf("N")!=-1)
							coordinates[1] += (stackOffsetY+(cardOffsetY*m));
						else if(stackType["direction"].indexOf("S")!=-1)
							coordinates[1] += (stackOffsetY+(cardOffsetY*m));
							
						return coordinates;
					}

					if(stackType["direction"].indexOf("E")!=-1 || stackType["direction"].indexOf("W")!=-1)
						stackOffsetX += cardOffsetX;
					else if(stackType["direction"].indexOf("N")!=-1 || stackType["direction"].indexOf("S")!=-1)
						stackOffsetY += cardOffsetY;
				}
			}
		}
		
		coordinates[0] = Math.floor(coordinates[0]);
		coordinates[1] = Math.floor(coordinates[1]);
		
		return coordinates;
	};
	
	/**
	 * Creates an idealy sized board, cards height and width to use all available space
	 */
	this.createIdealBoard = function()
	{
		var self = this;
		
		var availableWidth = D.width(this._playspace);
		var availableHeight = D.height(this._playspace);
		
		this._totalBoardWidth = 65000;
		this._totalBoardHeight = 65000;
		this._cardScale=1.00;
		for(;this._totalBoardWidth>availableWidth;this._cardScale-=0.05)
		{
			this.resizeBoard();
			if(this._totalBoardWidth<=availableWidth)
				break;
		}
		for(;this._totalBoardHeight>availableHeight;this._cardScale-=0.05)
		{
			this.resizeBoard();
			if(this._totalBoardHeight<=availableHeight)
				break;
		}

		for(;this._totalBoardWidth<availableWidth && this._totalBoardHeight<availableHeight;this._cardScale+=0.05)
		{
			this.resizeBoard();
			if(this._totalBoardWidth>=availableWidth || this._totalBoardHeight>=availableHeight)
			{
				this._cardScale-=0.05;
				this.resizeBoard();
				break;
			}
		}
				
		this._deckSize = this._deck.calculateSizeNumber(this._deck.getCardHeight(this._deckSize, this._cardScale));
		
		this._totalBoardWidth = 65000;
		this._totalBoardHeight = 65000;
		this._cardScale=1.00;
		if(Options.get("optionCardSize")=="largest")
		{
			for(;this._totalBoardWidth>availableWidth;this._cardScale-=0.05)
			{
				this.resizeBoard();
				if(this._totalBoardWidth<=availableWidth)
					break;
			}
			for(;this._totalBoardHeight>availableHeight;this._cardScale-=0.05)
			{
				this.resizeBoard();
				if(this._totalBoardHeight<=availableHeight)
					break;
			}
			
			for(;this._totalBoardWidth<availableWidth && this._totalBoardHeight<availableHeight;this._cardScale+=0.05)
			{
				this.resizeBoard();
				if(this._totalBoardWidth>=availableWidth || this._totalBoardHeight>=availableHeight)
				{
					this._cardScale-=0.05;
					this.resizeBoard();
					break;
				}
			}			
		}
		else
		{
			this.resizeBoard();
		}

		this.resizeBoardFinalize();
	};
	
	/**
	 * Will resize the board including updating all existing card locations, sizes, etc.
	 */
	this.resizeBoard = function()
	{
		// Get our card height and width with a default deckSize
		var cardWidth = parseInt(this._deck.getCardWidth(this._deckSize, this._cardScale), 10);
		var cardHeight = parseInt(this._deck.getCardHeight(this._deckSize, this._cardScale), 10);
		
		// How many cols and rows this board has
		var boardCols = this._game._board["width"];
		var boardRows = this._game._board["height"];
		
		// Determine the row/col X/Y pixel coordinates
		var rowHeightModifiers = [];
		this._rowHeights = [];
		for(var i=0;i<boardRows;i++)
		{
			this._rowHeights.push(cardHeight + (((boardRows-1)==i) ? 0 : SOLITAIRE.CARD_SPACING_Y));
			rowHeightModifiers.push(0);
		}
		
		var colWidthModifiers = [];
		this._colWidths = [];
		for(i=0;i<boardCols;i++)
		{
			this._colWidths.push(cardWidth + (((boardCols-1)==i) ? 0 : SOLITAIRE.CARD_SPACING_X));
			colWidthModifiers.push(0);
		}
		
		// Expand our rowLocs and colLocs as needed in order to fit the maximum possible cards stacked in various spots
		var spots = this._game._board["spots"];
		for(i=0;i<spots.length;i++)
		{
			var spot = spots[i];
			var inGameSpot = this.getSpot(spot["class"], i);

			if(spot.hasOwnProperty("sizeExempt") && spot["sizeExempt"]===true)
				continue;
				
			for(var z=0;z<spot["x"].length;z++)
			{
				var spotXLoc = spot["x"][z];
				var spotYLoc = spot["y"][z];
				
				var spotXLocInt = parseInt(spotXLoc, 10);
				var spotYLocInt = parseInt(spotYLoc, 10);
				
				var spotStackMax = spot["stackMax"][z];
				/*if(inGameSpot!=null && inGameSpot.cards.length>spot["stackMax"])
					spotStackMax = Math.floor(inGameSpot.cards.length*1.20);*/
				
				var runningCardCount = 0;
				var stackTypes = spot["stackTypes"];
				for(var t=0;t<stackTypes.length;t++)
				{
					var stackType = stackTypes[t];
					if(stackType.hasOwnProperty("index"))
						runningCardCount += stackType["index"].length;

					if(stackType["type"]=="deck")
						continue;
						
					var percentageToShowX = 0.00;
					var percentageToShowY = 0.00;
					if(stackType["type"]=="peek")
					{
						percentageToShowX = (stackType.hasOwnProperty("peekValue") ? (this._deck._peekX*stackType["peekValue"]) : this._deck._peekX);
						percentageToShowY = (stackType.hasOwnProperty("peekValue") ? (this._deck._peekY*stackType["peekValue"]) : this._deck._peekY);
					}
					
					var cardCount = stackType.hasOwnProperty("index") ? stackType["index"].length : ((spotStackMax)-runningCardCount);
					if(spotStackMax>1)
						cardCount++;
						
					var stackOffsetX = parseInt((cardCount*(cardWidth*percentageToShowX)), 10);
					if(spotStackMax>1)
						stackOffsetX += SOLITAIRE.CARD_SPACING_X*2;
						
					var stackOffsetY = parseInt((cardCount*(cardHeight*percentageToShowY)), 10);
					if(spotStackMax>1)
						stackOffsetY += SOLITAIRE.CARD_SPACING_Y*2;

					if(stackType.hasOwnProperty("extraSpacingPercentage"))
					{
						if(stackType["direction"].indexOf("N")!=-1 || stackType["direction"].indexOf("S")!=-1)
							stackOffsetY += (percentageToShowY*cardHeight)*stackType["extraSpacingPercentage"];
							
						if(stackType["direction"].indexOf("E")!=-1 || stackType["direction"].indexOf("W")!=-1)
							stackOffsetX += (percentageToShowX*cardWidth)*stackType["extraSpacingPercentage"];
					}
					
					if(stackType["direction"].indexOf("N")!=-1 || stackType["direction"].indexOf("S")!=-1)
						rowHeightModifiers[spotYLoc] = Math.max(rowHeightModifiers[spotYLoc], stackOffsetY);

					if(stackType["direction"].indexOf("E")!=-1 || stackType["direction"].indexOf("W")!=-1)
						colWidthModifiers[spotXLoc] = Math.max(colWidthModifiers[spotXLoc], stackOffsetX);
				}
			}
		}
		
		// Expand our col widths by our largest offset and Calculate our total board width and height
		this._totalBoardWidth = 0;
		for(i=0;i<boardCols;i++)
		{
			this._colWidths[i] += colWidthModifiers[i];
			this._totalBoardWidth += this._colWidths[i];
		}

		this._totalBoardHeight = 0;
		for(i=0;i<boardRows;i++)
		{
			this._rowHeights[i] += rowHeightModifiers[i];
			this._totalBoardHeight += this._rowHeights[i];
		}
	};
	
	this.resizeBoardFinalize = function()
	{
		var playspaceXY = D.xy(this._playspace);
		
		var playspaceWidth = D.width(this._playspace);
		
		this._boardOffsetX = Math.floor((playspaceWidth - this._totalBoardWidth) / 2)

		if(isNaN(this._boardOffsetX))
			this._boardOffsetX = 0;
		
		// Create our expanded spots internal array
		var spotsExpanded = [];
		
		for(i=0;i<this._game._board["spots"].length;i++)
		{
			var spot = this._game._board["spots"][i];
			for(var z=0;z<spot["x"].length;z++)
			{
				spotXLoc = spot["x"][z];
				spotYLoc = spot["y"][z];
				spotXLocInt = parseInt(spotXLoc, 10);
				spotYLocInt = parseInt(spotYLoc, 10);
				spotStackMax = spot["stackMax"][z];
				spotStackTypes = spot["stackTypes"];
							
				var oldSpot = null;
				for(var m=0;m<this._spotsExpanded.length;m++)
				{
					if(this._spotsExpanded[m]["spotXLoc"]==spotXLoc &&
					   this._spotsExpanded[m]["spotYLoc"]==spotYLoc)
					{
						oldSpot = this._spotsExpanded[m];
						break;
					}
				}
				
				var spotExpanded = {};
				spotExpanded["class"] = spot["class"];
				spotExpanded["lastUpdateCardsSnapshot"] = [];
				spotExpanded["subIndex"] = z;
				spotExpanded["spotXLoc"] = spotXLoc;
				spotExpanded["spotYLoc"] = spotYLoc;
				spotExpanded["spotXLocInt"] = spotXLocInt;
				spotExpanded["spotYLocInt"] = spotYLocInt;
				spotExpanded["stackMax"] = spotStackMax;
				spotExpanded["stackTypes"] = spotStackTypes;
				spotExpanded["screenX"] = playspaceXY[0] + this._boardOffsetX;
				spotExpanded["screenY"] = playspaceXY[1];
				spotExpanded["startingZIndex"] = spot.hasOwnProperty("startingZIndex") ? spot["startingZIndex"][z] : 1;
				spotExpanded["actualStackMax"] = spot.hasOwnProperty("actualStackMax") ? spot["actualStackMax"][z] : spotStackMax;
				spotExpanded["lastStackCount"] = 0;
					
				spotExpanded["cards"] = [];
				if(oldSpot!==null)
				{
					spotExpanded["cards"] = oldSpot["cards"];
					if(oldSpot.hasOwnProperty("baseCard"))
						spotExpanded["baseCard"] = oldSpot["baseCard"];
				}
				
				for(m=0;m<spotXLocInt;m++)
				{
					spotExpanded["screenX"] += this._colWidths[m];
				}

				if(spotXLoc>spotXLocInt)
					spotExpanded["screenX"] += (this._colWidths[spotXLocInt] * (spotXLoc-spotXLocInt));

				for(m=0;m<spotYLocInt;m++)
				{
					spotExpanded["screenY"] += this._rowHeights[m];
				}
				
				if(spotYLoc>spotYLocInt)
					spotExpanded["screenY"] += (this._rowHeights[spotYLocInt] * (spotYLoc-spotYLocInt));
				
				spotExpanded["screenX"] = Math.floor(spotExpanded["screenX"]);
				spotExpanded["screenY"] = Math.floor(spotExpanded["screenY"]);
				
				O.merge(spotExpanded, spot, true);
				
				spotsExpanded.push(spotExpanded);
			}			
		}
		
		// Offscreen Spot
		var offscreenSpot = {};
		offscreenSpot["class"] = "offscreen";
		offscreenSpot["lastUpdateCardsSnapshot"] = [];
		offscreenSpot["subIndex"] = 0;
		offscreenSpot["stackMax"] = 9999;
		offscreenSpot["screenX"] = -5000;
		offscreenSpot["screenY"] = -5000;
		offscreenSpot["stackTypes"] = [ { "type" : "deck" } ];
		offscreenSpot["cards"] = [];
		offscreenSpot["startingZIndex"] = 0;
		spotsExpanded.push(offscreenSpot);
		
		this._spotsExpanded = spotsExpanded;		
	};
	
	/**
	 * Returns true or false depending on whether animation should be allowed
	 */
	this.canAnimate = function()
	{
		if(Options.get("optionAnimateCards")!==true)
			return false;
			
		return true;
	};
	
	/**
	 * This will clear the entire game
	 */
	this.clear = function()
	{
		this.undoMoves = [];
		
		YAHOO.util.AnimMgr.stop();
		
		for(var i=0;i<this._spotsExpanded.length;i++)
		{
			var spot = this._spotsExpanded[i];

			if(spot.hasOwnProperty("base"))
				E.remove(spot["baseCard"].getDomCard(), "click", this.cardClicked);
			
			for(var m=0;m<spot.cards.length;m++)
			{
				var card = spot.cards[m];
				
				if(card.hasClickHandler)
					E.remove(card.getDomCard(), "click", this.cardClicked);
				if(card.hasDoubleClickHandler)
					E.remove(card.getDomCard(), "dblclick", this.cardDoubleClicked);
			}
		}
		
		CARD.removeAllCards();
	};
}
