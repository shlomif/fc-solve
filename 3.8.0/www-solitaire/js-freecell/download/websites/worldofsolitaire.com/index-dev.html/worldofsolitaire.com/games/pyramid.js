/* klondike.js - robert@cosmicrealms.com */

gGames["Pyramid"] = new pyramid("Pyramid");
gGames["PyramidEasy"] = new pyramid("PyramidEasy");
gGames["PyramidRelaxed"] = new pyramid("PyramidRelaxed");
gGames["PyramidRelaxedEasy"] = new pyramid("PyramidRelaxedEasy");

function pyramid(gameID)
{
	this._gameID = gameID;
	this._gameName = (gameID=="Pyramid" ? "Pyramid" :
	  			     (gameID=="PyramidEasy" ? "Pyramid Easy" :
	  			     (gameID=="PyramidRelaxed" ? "Pyramid Relaxed" :
	  			     (gameID=="PyramidRelaxedEasy" ? "Pyramid Relaxed Easy" : "pyramid"))));
	
	this._board =
	{
		"width"  : 8,
		"height" : 4,
		"onValidDrop" : { "moveToSpot"        : "foundation",
						  "moveCardDroppedOn" : true,
						  "moveOptions"       : { "moveSpeedFunction" : function() { return SOLITAIRE.CARD_DEAL_SPEED; } } },
		"spots"  :
		[
			{
				"class"           : "stock",
			 	"x"               : [ 0.0 ],
				"y"		          : [ 0.0 ],
				"stackMax"        : [  24 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"clickable"       : { "faceUpOnly" : false },
				"draggable"       : { },
				"droppable"       : { },
				"autoFlip"        : { },
				"turnUpOnClick"   : true,
				"moveOnClick"     : { "spot"    : "waste",
									  "askGame" : true }
			},
			{
				"class"		      : "waste",
				"x"			      : [ 1.0 ],
				"y"			      : [ 0.0 ],
				"stackMax"        : [  24 ],
				"stackTypes"      :
				[
					{ "direction" :  "", "type" : "deck" }
				],
				"draggable"       : { },
				"droppable"       : { }
			},
			{
				"class"		      : "foundation",
				"x"			      : [ 7.0 ],
				"y"			      : [ 0.0 ],
				"stackMax"        : [  52 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ]
			},
			{
				"class"           : "tableau",
				"x"			      : [             4.0,
				                                3.5, 4.5,
				 						      3.0, 4.0, 5.0,
								            2.5, 3.5, 4.5, 5.5,
								          2.0, 3.0, 4.0, 5.0, 6.0,
				                        1.5, 2.5, 3.5, 4.5, 5.5, 6.5,
				                      1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0 ],
				"y"			      : [             0.0,
				                                0.5, 0.5,
				 						      1.0, 1.0, 1.0,
								            1.5, 1.5, 1.5, 1.5,
								          2.0, 2.0, 2.0, 2.0, 2.0,
				                        2.5, 2.5, 2.5, 2.5, 2.5, 2.5,
				                      3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0 ],
				"stackMax"	      : A.pushMany(Array(0), 1, 28),
				"startingZIndex"  : A.pushSequence(Array(0), 1, 28),
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"clickable"       : { "askGame" : true },
				"draggable"       : { "askGame" : true },
				"droppable"       : { "askGame" : true }
			}
		]
	};
	
	if(this._gameID.indexOf("Easy")!=-1)
	{
		this._board.spots[0]["base"] = { "clickable"  : true,
										 "fillOnClick" : 
										 { "spot"                : "waste",
										   "reverseUpdateSource" : true,
										   "maxTimes"			 : 2,
										   "moveOptions"         : { "turnFaceDown"      : true,
																	 "skipTurnAnimation" : true,
																	 "moveSpeedFunction" : function() { return SOLITAIRE.CARD_MOVE_SPEED; } } } };										
	}
	
	/**
	 * Rename this to stockCreate to deal a winnable set of cards
	 */
	this.stockCreateWin = function()
	{
		var stock =
		[
			new Card("13s"),
			new Card("12s"), new Card("1s"),
			new Card("13d"), new Card("13h"), new Card("10s"),
			new Card("3s"),  new Card("8s"), new Card("9s"),  new Card("4s"),
			new Card("5s"),  new Card("7s"), new Card("6s"),  new Card("12h"), new Card("1h"),
			new Card("11h"), new Card("2h"), new Card("10h"), new Card("3h"),  new Card("9h"),  new Card("4h"),
			new Card("8h"),  new Card("5h"), new Card("7h"),  new Card("6h"),  new Card("11s"), new Card("12d"), new Card("1d"),
			new Card("11d"), new Card("2d"), new Card("10d"), new Card("3d"),  new Card("9d"),  new Card("4d"),  new Card("8d"), new Card("5d"), new Card("7d"),
			
			new Card("6d"), new Card("13c"), new Card("12c"), new Card("1c"), new Card("11c"), new Card("2c"), new Card("10c"), new Card("3c"), new Card("9c"), new Card("4c"), new Card("8c"), new Card("5c"), new Card("7c"), new Card("6c"), new Card("2s")
		];
		
		return stock;
	};
		
	/**
	 * This is called when the stock of cards is first distributed, once for each card in the stock
	 * @param {Object} card Which card is to be distributed
	 * @param {Object} stockIndex Where in the stock this card is
	 */
	this.stockDistribute = function(card, stockIndex)
	{
		var self = this;
		var game = card.game;

		var options = {};
		options["turnFaceUp"] = false;
		options["moveSpeed"] = SOLITAIRE.CARD_DEAL_SPEED;
				
		if(stockIndex<=27)
		{
			game.moveCard(card, "tableau", stockIndex, options);
			
			if(stockIndex===0)
			{
				game.updateSpot(game.getSpot("tableau", stockIndex));
			}
			else if(stockIndex==2)
			{
				for(var i=1;i<=2;i++) { game.updateSpot(game.getSpot("tableau", i)); }
			}
			else if(stockIndex==5)
			{
				for(var i=3;i<=5;i++) { game.updateSpot(game.getSpot("tableau", i)); }
			}
			else if(stockIndex==9)
			{
				for(var i=6;i<=9;i++) { game.updateSpot(game.getSpot("tableau", i)); }
			}
			else if(stockIndex==14)
			{
				for(var i=10;i<=14;i++) { game.updateSpot(game.getSpot("tableau", i)); }
			}
			else if(stockIndex==20)
			{
				for(var i=15;i<=20;i++) { game.updateSpot(game.getSpot("tableau", i)); }
			}
			else if(stockIndex==27)
			{
				for(var i=21;i<=27;i++) { game.updateSpot(game.getSpot("tableau", i)); }
			}
		}
		else
		{
			if(stockIndex==51)
				options["afterMove"] = function() { self.stockDistributeFinished(game); };
				
			game.moveCard(card, "stock", 0, options);
			if(stockIndex==51)
				game.updateSpot(game.getSpot("stock"));
		}
	};
	
	/**
	 * This is called after the stock has been fully distributed
	 * @param {Object} game
	 */
	this.stockDistributeFinished = function(game)
	{
		var tableaus = game.getSpots("tableau");
		for(var i=0;i<tableaus.length;i++)
		{
			if(game.getSpot("tableau", i).cards.length>0)
				game.turnCardFaceUp(game.getSpot("tableau", i).cards[0], SOLITAIRE.CARD_DEAL_TURN_OVER_DELAY*i, (i==(tableaus.length-1) ? { "afterFaceUp" : function() { game.stockDistributionFinished(); } } : null));
		}
	};
		
	/**
	 * This is called by game whenever a card that is clickable is clicked
	 * @param {Object} card Which card was clicked
	 */
	this.cardClicked = function(card)
	{
		if(card.number==13)
		{
			SOLITAIRE.movesIncrement();
			
			var game = card.game;
			var spot = card.spot;
			
			var updateSpots = this.getUpdateSpotList(card);
			
			var options = {};
			options["moveSpeed"] = SOLITAIRE.CARD_DEAL_SPEED;
			
			game.moveCard(card, "foundation", 0, options);
			updateSpots.push(game.getSpot("foundation"));
			updateSpots.push(spot);

			for(var i=0;i<updateSpots.length;i++)
			{
				game.updateSpot(updateSpots[i]);
			}
		}
	};
	
	/**
	 * Called to check to see if a card can be moved on click or not
	 * @param {Object} card
	 */
	this.canMoveOnClick = function(card)
	{
		return (card.number!=13);
	};
	
	/**
	 * Called to check to see if a given card drop is valid or not
	 * @param {Object} cardDragged
	 * @param {Object} cardDroppedOn
	 */
	this.isCardDropValid = function(cardDragged, cardDroppedOn)
	{
		if(cardDragged==cardDroppedOn)
			return false;
			
		if((cardDragged.number+cardDroppedOn.number)==13)
			return true;
		
		return false;
	};
	
	/**
	 * This is optional. If present it should returns spots that need updated after a valid drop
	 * @param {Object} cardDragged
	 * @param {Object} cardDroppedOn
	 */
	this.getPostDropUpdateSpotList = function(cardDragged, cardDroppedOn)
	{
		var spotsToUpdate = [];
		
		A.append(spotsToUpdate, this.getUpdateSpotList(cardDragged));
		A.append(spotsToUpdate, this.getUpdateSpotList(cardDroppedOn));
		
		return spotsToUpdate;
	};
	
	/**
	 * Internal work function to determine what spots to update based on card activity
	 * @param {Object} card
	 */
	this.getUpdateSpotList = function(card)
	{
		var spotsToUpdate = [];
		var spot = card.spot;
		var game = card.game;
		
		if(spot["class"]=="tableau")
		{
			var checkOffset = spot["spotYLoc"]*2;
			
			if(checkOffset>0)
			{
				var leftSpotCheck = game.getSpot("tableau", spot["subIndex"]-(checkOffset+1));
				var rightSpotCheck = game.getSpot("tableau", spot["subIndex"]-checkOffset);
				
				if(leftSpotCheck!==null && (spot["spotYLoc"]-leftSpotCheck["spotYLoc"])==0.5)
					spotsToUpdate.push(leftSpotCheck);
					
				if(rightSpotCheck!==null && (spot["spotYLoc"]-rightSpotCheck["spotYLoc"])==0.5)
					spotsToUpdate.push(rightSpotCheck);
			}
		}
		
		return spotsToUpdate;		
	};
	
	/**
	 * Called to determine if the game has been won or not
	 * @param {Object} game
	 */
	this.isGameWon = function(game)
	{
		var foundation = game.getSpot("foundation");

		if(foundation.cards.length==52)
			return true;
			
		if(this._gameID.indexOf("Relaxed")!=-1)
		{
			var tableaus = game.getSpots("tableau");
			for(var i=0;i<tableaus.length;i++)
			{
				if(tableaus[i].cards.length>0)
					return false;
			}
			
			return true;
		}
		
		return false;
	};
	
	/**
	 * Function to determine if a card is live or not
	 * @param {Object} card
	 */
	this.isCardLive = function(card, type)
	{
		var spot = card.spot;
		var game = card.game;
		
		if(card._stackIndex>0)
			return false;
		
		if(card.isFaceUp() && (spot["class"]=="stock" || spot["class"]=="waste"))
			return true;
		
		if(spot["class"]!=="tableau")
			return false;
		
		if(spot["spotYLocInt"]==3.0)
			return true;
			
		var checkOffset = 1;
		if(spot["spotYLoc"]==0.5)
			checkOffset = 2;
		else if(spot["spotYLoc"]==1.0)
			checkOffset = 3;
		else if(spot["spotYLoc"]==1.5)
			checkOffset = 4;
		else if(spot["spotYLoc"]==2.0)
			checkOffset = 5;
		else if(spot["spotYLoc"]==2.5)
			checkOffset = 6;
		
		var coveringLeftSpot = game.getSpot("tableau", spot["subIndex"]+checkOffset);
		var coveringRightSpot = game.getSpot("tableau", spot["subIndex"]+checkOffset+1);
		if(coveringLeftSpot.cards.length>0 || coveringRightSpot.cards.length>0)
			return false;
		
		return true;
	};
	
	/**
	 * Will automatically play cards
	 * @param {Object} game
	 * @param {Object} type
	 */
	this.autoPlay = function(game, type, pretendOnly)
	{
		if(typeof pretendOnly=="undefined" || pretendOnly===null)
			pretendOnly = false;
		
		if(type=="obvious" || type=="all")
		{
			var stock = game.getSpot("stock");
			
			var checkSpots = game.getSpots("tableau");
			checkSpots.push(stock);
			checkSpots.push(game.getSpot("waste"));
	
			for(var i=0;i<checkSpots.length;i++)
			{
				var spot = checkSpots[i];
				if(spot.cards.length<1)
					continue;
				
				if(this.isCardLive(spot.cards[0]) && this.tryPlayCard(spot.cards[0], type=="obvious", pretendOnly))
					return true;
			}
		}
		
		return false;
	};
	
	/**
	 * Will automatically try and play the given card
	 * @param {Object} card The card to try and play
	 * @param {Object} obviousOnly Only play obvious moves
	 * @param {Object} pretendOnly Just pretend, only interested in true or false result
	 */
	this.tryPlayCard = function(card, obviousOnly, pretendOnly)
	{
		var game = card.game;
		
		if(typeof pretendOnly=="undefined" || pretendOnly===null)
			pretendOnly = false;
			
		if(typeof obviousOnly=="undefined" || obviousOnly===null)
			obviousOnly = false;
		
		// Obvious First - Play if a king
		if(card.isFaceUp() && this.isCardLive(card, "clickable") && card.number==13)
		{
			if(pretendOnly!==true)
				game.doCardClick(card);
			return true;
		}

		if(obviousOnly===true)
			return false;

		// Anything now
		
		return false;
	};
}
