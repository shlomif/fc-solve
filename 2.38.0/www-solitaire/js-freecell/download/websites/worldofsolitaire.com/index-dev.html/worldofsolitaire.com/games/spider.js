/* spider.js - robert@cosmicrealms.com */

gGames["Spider"] = new spider("Spider");
gGames["Spider2Suits"] = new spider("Spider2Suits");
gGames["Spider1Suit"] = new spider("Spider1Suit");
gGames["Scorpion"] = new spider("Scorpion");

function spider(gameID)
{
	this._gameID = gameID;
	this._gameName = (gameID=="Spider" ? "Spider" :
				     (gameID=="Spider2Suits" ? "Spider (2 Suits)" :
					 (gameID=="Spider1Suit" ? "Spider (1 Suit)" :
					 (gameID=="Scorpion" ? "Scorpion" : "spider"))));
	this._stockDistributeNum = 0;
	
	this._board =
	{
		"width"  : (gameID=="Scorpion" ? 7 : 10),
		"height" : 2,
		"spots"  :
		[
			{
				"class"		      : "foundation",
				"x"			      : A.pushSequence([], (gameID=="Scorpion" ? 3.0 : 2.0), (gameID=="Scorpion" ? 6.0 : 9.0)),
				"y"			      : A.pushMany([], 0.0, (gameID=="Scorpion" ? 4 : 8)),
				"stackMax"        : A.pushMany([],  13, (gameID=="Scorpion" ? 4 : 8)),
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"base"            : { }
			},			
			{
				"class"           : "tableau",
				"x"			      : A.pushSequence([], 0.0, (gameID=="Scorpion" ? 6.0 : 9.0)),
				"y"			      : A.pushMany([], 1.0, (gameID=="Scorpion" ? 7 : 10)),
				"stackMax"        : A.pushMany([],  28, (gameID=="Scorpion" ? 7 : 10)),
				"actualStackMax"  : A.pushMany([],  52, (gameID=="Scorpion" ? 7 : 10)),
				"stackTypes"      : [ { "direction" : "S", "type" : "peek" } ],
				"clickable"       : { "faceUpOnly" : false, "faceDownOnly" : true },
				"draggable"       : { "topCardOnly" : false, "askGame" : (gameID=="Scorpion" ? false : true) },
				"droppable"       : { },
				"autoFlip"        : { },
				"turnUpOnClick"   : true,
				"base"            : { "droppable" : true },
				"monitorChanges"  : true
			},
			{
				"class"           : "stock",
			 	"x"               : [ 0.0 ],
				"y"		          : [ 0.0 ],
				"stackMax"        : [  50 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"clickable"       : { "faceUpOnly" : false },
				"moveOnClick"     : { "spot"                : "tableau",
								      "count"               : 1,
									  "targetCannotBeEmpty" : (gameID=="Scorpion" ? false : true),
									  "moveOptions"         : { "turnFaceUp"        : true,
									  						    "skipTurnAnimation" : true,
																"moveSpeedFunction" : function() { return SOLITAIRE.CARD_DEAL_SPEED; } } }
			}
		]
	};
	
	/**
	 * Will create our stock
	 */
	this.stockCreate = function()
	{
		return Stock.createStandard((this._gameID=="Spider2Suits" ? 4 :
									(this._gameID=="Spider1Suit" ? 8 :
									(this._gameID=="Scorpion" ? 1 : 2))),
									(this._gameID=="Spider2Suits" ? ((A.randomize(["h", "d"])).splice(0, 1)).join("") + ((A.randomize(["c", "s"])).splice(0, 1)).join("") :
									(this._gameID=="Spider1Suit" ? ((A.randomize(["h", "s", "c", "d"])).splice(0, 1)).join("") : "")));
	};
	
	
	/**
	 * Rename this to stockCreate to deal a winnable set of cards
	 */
	this.stockCreateWin = function()
	{
		// Only coded for certain spider variations
		var stock =
		[
			new Card("5h"), new Card("2h"), new Card("13d"), new Card("13h"), new Card("3c"), new Card("2c"), new Card("1c"), new Card("12h"), new Card("11h"), new Card("10h"),
			new Card("6h"), new Card("3h"), new Card("1h"), new Card("13h"), new Card("9c"), new Card("8c"), new Card("7c"), new Card("6c"), new Card("5c"), new Card("4c"),
			new Card("7h"), new Card("4h"), new Card("13c"), new Card("10c"), new Card("4d"), new Card("3d"), new Card("2d"), new Card("1d"), new Card("12c"), new Card("11c"),
			new Card("8h"), new Card("13d"), new Card("6d"), new Card("5d"), new Card("12d"), new Card("11d"), new Card("10d"), new Card("9d"), new Card("8d"), new Card("7d"),
			new Card("9h"), new Card("3s"), new Card("2s"), new Card("1s"), new Card("9s"), new Card("8s"), new Card("7s"), new Card("6s"), new Card("5s"), new Card("4s"),
			new Card("13s"), new Card("12s"), new Card("11s"), new Card("10s"),
			
			new Card("12d"), new Card("11d"), new Card("10d"), new Card("9d"), new Card("8d"), new Card("7d"), new Card("6d"), new Card("5d"), new Card("4d"), new Card("3d"), new Card("2d"), new Card("1d"),
			new Card("12h"), new Card("11h"), new Card("10h"), new Card("9h"), new Card("8h"), new Card("7h"), new Card("6h"), new Card("5h"), new Card("4h"), new Card("3h"), new Card("2h"), new Card("1h"),
			new Card("13c"), new Card("12c"), new Card("11c"), new Card("10c"), new Card("9c"), new Card("8c"), new Card("7c"), new Card("6c"), new Card("5c"), new Card("4c"), new Card("3c"), new Card("2c"), new Card("1c"),
			new Card("13s"), new Card("12s"), new Card("11s"), new Card("10s"), new Card("9s"), new Card("8s"), new Card("7s"), new Card("6s"), new Card("5s"), new Card("4s"), new Card("3s"), new Card("2s"), new Card("1s")
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
		
		if(stockIndex<(this._gameID=="Scorpion" ? 49 : 54))
		{
			if(this._gameID=="Scorpion")
			{
				options["skipTurnAnimation"] = true;
				
				if((stockIndex%7>=4 && stockIndex%7<=6 && Math.floor(stockIndex/7)>=0 && Math.floor(stockIndex/7)<=2) ||
				   (Math.floor(stockIndex/7)>=3))
					options["turnFaceUp"] = true;
			}
			
			game.moveCard(card, "tableau", stockIndex%(this._gameID=="Scorpion" ? 7 : 10), options);
		}
		else
		{
			options["moveSpeed"] = SOLITAIRE.CARD_MOVE_SPEED;
			game.moveCard(card, "stock", 0, options);
		}

		if(stockIndex==(this._gameID=="Scorpion" ? 51 : 103))
		{
			this._stockDistributeNum = 0;
			self.stockDistributeAnimate(game);			
		}
	};
	
	/**
	 * Called to actually do the animation of the stock distribution
	 * @param {Object} game
	 */
	this.stockDistributeAnimate = function(game)
	{
		var self = this;
		
		if(this._stockDistributeNum>(this._gameID=="Scorpion" ? 6 : 9))
		{
			game.updateSpot(game.getSpot("stock"));
			if(this._gameID=="Scorpion")
			{
				if(Options.get("optionAnimateDeal")===true)
					setTimeout(function() { CARD.runAfterAnimations(function() { game.stockDistributionFinished(); }); }, 100);
				else
					game.stockDistributionFinished();
			}
			else
			{
				if(Options.get("optionAnimateDeal")===true)
					setTimeout(function() { CARD.runAfterAnimations(function() { self.stockDistributeFinished(game); }); }, 100);
				else
					self.stockDistributeFinished(game);
			}
			return;
		}

		var spot = game.getSpot("tableau", this._stockDistributeNum);
		this._stockDistributeNum++;
		
		game.updateSpot(spot);
		self.stockDistributeAnimate(game);
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
			var options = null;
				
			if(this._gameID!="Scorpion")
			{
				if(i==(tableaus.length-1))
					options = { "afterFaceUp" : function() { game.stockDistributionFinished(); } };
				
				game.turnCardFaceUp(game.getSpot("tableau", i).cards[0], SOLITAIRE.CARD_DEAL_TURN_OVER_DELAY*i, options);
			}
		}
	};
	
	/**
	 * Called to check to see if a given card drop is valid or not
	 * @param {Object} cardDragged
	 * @param {Object} cardDroppedOn
	 */
	this.isCardDropValid = function(cardDragged, cardDroppedOn)
	{
		var spot = cardDroppedOn.spot;
		var spotClass = spot["class"];
		
		switch(spotClass)		
		{
			case("tableau"):
				if(this._gameID=="Scorpion")
					return (cardDroppedOn.isBase && cardDragged.number==13) ||
					       (cardDragged.number===(cardDroppedOn.number-1) && cardDragged.suit==cardDroppedOn.suit);
				else
					return cardDroppedOn.isBase ||
						   (cardDragged.number===(cardDroppedOn.number-1));
		}
		
		return false;
	};
		
	/**
	 * Called to determine if the game has been won or not
	 * @param {Object} game
	 */
	this.isGameWon = function(game)
	{
		var foundations = game.getSpots("foundation");
		for(var i=0;i<foundations.length;i++)
		{
			if(foundations[i].cards.length<13)	
				return false;
		}
		
		return true;
	};
	

	/**
	 * Internal function to determine if a card is live or not
	 * @param {Object} card
	 */
	this.isCardLive = function(card, type)
	{
		var spot = card.spot;
		var game = card.game;
		
		for(var i=card["stackIndex"];i>=0;i--)
		{
			if(i==card["stackIndex"])
				continue;
			
			if(!this.isCardDropValid(spot.cards[i], spot.cards[i+1]) || spot.cards[i].suit!=spot.cards[i+1].suit)
				return false;
		}
		
		return true;
	};	
	
	/**
     * This is called whenever a spot's cards have changed
     * @param {Object} game
     * @param {Object} spot
     */
    this.spotChanged = function(game, spot, isUndoUpdate)
    {
		if(typeof isUndoUpdate!="undefined" && isUndoUpdate===true)
			return;
			
		if(spot["class"]=="tableau")
		{
			if(spot.cards.length<13)
				return;
			
			var run = 0;
			var lastCardNum = 0;
			var runStartLoc = 0;
			for(var m=0;m<spot.cards.length && run<13;m++)
			{
				if(!spot.cards[m].isFaceUp())
					return;
					
				if(lastCardNum===0 && spot.cards[m].number===1)
				{
					runStartLoc = m;
					lastCardNum = spot.cards[m].number;
					run++;
					continue;
				}
				else if(lastCardNum===0)
				{
					continue;
				}
				
				if(spot.cards[m].suit==spot.cards[m-1].suit && spot.cards[m].number===(lastCardNum+1))
				{
					run++;
					lastCardNum = spot.cards[m].number;
					continue;
				}
				
				run = 0;
				lastCardNum = 0;
				runStartLoc = 0;
			}
			
			if(run!=13)
				return;
			
			// We have a complete stack, move it to an open foundation
			var foundations = game.getSpots("foundation");
			for(var i=0;i<foundations.length;i++)
			{
				var foundation = foundations[i];
				if(foundation.cards.length>0)
					continue;
				
				var cardsToMove = [];
				for(var m=(runStartLoc+12);m>=runStartLoc;m--)
				{
					if(!spot.cards[m].isCurrentlyFaceUp())
						spot.cards[m].turnFaceUp(-1);
						
					cardsToMove.push(spot.cards[m]);
				}

				var options = {};
				options["moveSpeed"] = SOLITAIRE.CARD_MOVE_SPEED;
				options["undoAppendToLastMove"] = true;
						
				while(cardsToMove.length)
				{
					game.moveCard(cardsToMove.pop(), "foundation", i, options);					
				}
				
				game.updateSpot(spot);
				game.updateSpot(foundation);
				
				return;
			}
		}
	};
}
