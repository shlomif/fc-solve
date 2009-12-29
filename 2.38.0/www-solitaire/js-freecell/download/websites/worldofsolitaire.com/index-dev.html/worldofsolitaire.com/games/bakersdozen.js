/* freecell.js - robert@cosmicrealms.com */

gGames["BakersDozen"] = new bakersDozen("BakersDozen");
gGames["CastlesInSpain"] = new bakersDozen("CastlesInSpain");
gGames["SpanishPatience"] = new bakersDozen("SpanishPatience");

function bakersDozen(gameID)
{
	this._gameID = gameID;
	this._gameName = (gameID=="BakersDozen" ? "Baker's Dozen" :
					 (gameID=="CastlesInSpain" ? "Castles in Spain" :
			         (gameID=="SpanishPatience" ? "Spanish Patience" : "bakersdozen")));
					 
	this._board =
	{
		"width"  : 8,
		"height" : 5,
		"spots"  :
		[
			{
				"class"		      : "foundation",
				"x"			      : [ 7.0, 7.0, 7.0, 7.0 ],
				"y"			      : [ 0.0, 1.33, 2.67, 4.0 ],
				"stackMax"        : [  13,  13,  13,  13 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"base"	          : { "droppable" : true },
				"draggable"       : { },
				"droppable"       : { }
			},
			{
				"class"           : "tableau",
				"x"			      : [ 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 0.0, 1.0, 2.0, 3.0, 4.0, 5.0 ],
				"y"			      : [ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0 ],
				"stackMax"        : [  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16 ],
				"sizeExempt"      : true,
				"stackTypes"      : [ { "direction" : "S", "type" : "peek" } ],
				"draggable"       : { },
				"droppable"       : { },
				"doubleclickable" : { }
			}
		]
	};
	
	if(gameID==="CastlesInSpain")
	{
		this._board.spots[1].clickable = { "faceUpOnly" : false, "faceDownOnly" : true };
		this._board.spots[1].autoFlip = { };
		this._board.spots[1].turnUpOnClick = true;
		this._board.spots[1].base = { "droppable" : true };
	}
	
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
		if(this._gameID!=="CastlesInSpain")
		{
			options["turnFaceUp"] = true;
			options["skipTurnAnimation"] = true;
		}
		options["moveSpeed"] = SOLITAIRE.CARD_DEAL_SPEED;
		
		if(stockIndex==51 && this._gameID==="CastlesInSpain")
			options["afterMove"] = function() { self.stockDistributeFinished(game); };
		
		game.moveCard(card, "tableau", stockIndex%13, options);
		
		if(stockIndex==51)
		{
			var tableaus = game.getSpots("tableau");
			
			if(this._gameID!=="CastlesInSpain")
			{
				for(var i=0;i<tableaus.length;i++)
				{
					var tableau = tableaus[i];
					
					var availableCardIndex = tableau.cards.length-1;
					while(availableCardIndex>=0 && tableau.cards[availableCardIndex].number==13)
					{
						availableCardIndex--;
					}
					if(availableCardIndex<0)
						continue;
					
					for(var m=(availableCardIndex-1);m>=0;m--)
					{
						var tableauCard = tableau.cards[m];
						if(tableauCard.number===13)
						{
							game.swapCards(tableauCard, tableau.cards[availableCardIndex]);
							availableCardIndex--;
						}
					}
				}
			}

			for(var i=0;i<tableaus.length;i++)
			{
				game.updateSpot(tableaus[i]);
			}
			
			if(this._gameID!=="CastlesInSpain")
			{
				if(Options.get("optionAnimateDeal")===true)
					setTimeout(function() { CARD.runAfterAnimations(function() { game.stockDistributionFinished(game); }); }, 100);
				else
					game.stockDistributionFinished(game);
			}
		}
	};
	
	/**
	 * This is called after the stock has been fully distributed
	 * @param {Object} game
	 */
	this.stockDistributeFinished = function(game)
	{
		for(var i=0;i<13;i++)
		{
			game.turnCardFaceUp(game.getSpot("tableau", i).cards[0], SOLITAIRE.CARD_DEAL_TURN_OVER_DELAY*i, i==12 ? { "afterFaceUp" : function() { game.stockDistributionFinished(); } } : {} );
		}
	};
	
	/**
	 * A card as been double clicked, figure out what to do with it
	 * @param {Object} card
	 */
	this.cardDoubleClicked = function(card)
	{
		this.tryPlayCard(card, true);
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
			case("foundation"):
				if(this._gameID==="SpanishPatience")
					return cardDragged.stackIndex===0 &&
					       ((cardDroppedOn.isBase && cardDragged.number===1) ||
						    (cardDragged.number===(cardDroppedOn.number+1)));
				else
					return cardDragged.stackIndex===0 &&
					       ((cardDroppedOn.isBase && cardDragged.number===1) ||
						    (cardDragged.number===(cardDroppedOn.number+1) && cardDragged.suit==cardDroppedOn.suit));
			case("tableau"):
				if(this._gameID==="CastlesInSpain")
					return cardDroppedOn.isBase ||
						   (cardDragged.number===(cardDroppedOn.number-1) &&
							((cardDragged.isBlack && cardDroppedOn.isRed) || (cardDragged.isRed && cardDroppedOn.isBlack)));
				else
					return (cardDragged.number===(cardDroppedOn.number-1));
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
	 * Will automatically play cards
	 * @param {Object} game
	 * @param {Object} type
	 */
	this.autoPlay = function(game, type, pretendOnly)
	{
		var obviousOnly = (type=="won" || type=="obvious");

		if(typeof pretendOnly=="undefined" || pretendOnly===null)
			pretendOnly = false;
		
		if(type=="won")
		{
			var tableaus = game.getSpots("tableau");
			for(var i=0;i<tableaus.length;i++)
			{
				var tableau = tableaus[i];
				for(var m=tableau.cards.length-2;m>=0;m--)
				{
					if(tableau.cards[m].number<=tableau.cards[m+1].number)
						return false;
				}
			}
		}
		
		var tableaus = game.getSpots("tableau");
		for(var i=0;i<tableaus.length;i++)
		{
			var tableau = tableaus[i];
			if(tableau.cards.length<1)
				continue;
			
			if(this.tryPlayCard(tableau.cards[0], obviousOnly, pretendOnly))
				return true;
		}
			
		if(type=="all")
		{
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
		
		// Obvious First - Try the foundations
		var foundations = game.getSpots("foundation");
		for(var i=0;i<foundations.length;i++)
		{
			var foundation = foundations[i];
			var targetCard = (foundation.cards.length<1) ? foundation.baseCard : foundation.cards[0];
			if(this.isCardDropValid(card, targetCard))
			{
				if(pretendOnly!==true)
					game.doCardDrop(card, targetCard);
				return true;
			}
		}
		
		if(obviousOnly===true)
			return false;
		
		// Anything now
		
		return false;
	};
}
