/* cruel.js - robert@cosmicrealms.com */

gGames["Cruel"] = new cruel("Cruel");
gGames["CruelTraditionalRedeal"] = new cruel("CruelTraditionalRedeal");

function cruel(gameID)
{
	this._gameID = gameID;
	this._gameName = (gameID=="CruelTraditionalRedeal" ? "Cruel (Traditional Redeal)" :
	                 (gameID=="Cruel" ? "Cruel" : "cruel"));
	
	this._board =
	{
		"width"  : 6,
		"height" : 3,
		"spots"  :
		[
			{
				"class"           : "stock",
			 	"x"               : [ 0.0 ],
				"y"		          : [ 0.0 ],
				"stackMax"        : [  48 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"clickable"       : { },
				"base"            : { "clickable" : true }
			},
			{
				"class"		      : "foundation",
				"x"			      : [ 2.0, 3.0, 4.0, 5.0 ],
				"y"			      : [ 0.0, 0.0, 0.0, 0.0 ],
				"stackMax"        : [  13,  13,  13,  13 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"base"	          : { "droppable" : true },
				"draggable"       : { "askGame" : true },
				"droppable"       : { }
			},
			{
				"class"           : "tableau",
				"x"			      : [ 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 0.0, 1.0, 2.0, 3.0, 4.0, 5.0 ],
				"y"			      : [ 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0 ],
				"stackMax"        : [  17,  17,  17,  17,  17,  17,  17,  17,  17,  17,  17,  17 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"draggable"       : { },
				"droppable"       : { },
				"doubleclickable" : { },
				"reverseUpdate"   : true
			}
		]
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
		options["turnFaceUp"] = true;
		options["skipTurnAnimation"] = true;
		options["moveSpeed"] = SOLITAIRE.CARD_MOVE_SPEED;
		
		if(stockIndex==51)
			options["afterMove"] = function() { self.stockDistributeFinished(game); };
			
		game.moveCard(card, "tableau", Math.floor(stockIndex/5), options);

		if(stockIndex==51)
		{
			var tableaus = game.getSpots("tableau");
			for(var i=0;i<tableaus.length;i++)
			{
				game.updateSpot(tableaus[i]);
			}
		}
	};
	
	/**
	 * This is called after the stock has been fully distributed
	 * @param {Object} game
	 */
	this.stockDistributeFinished = function(game)
	{
		var foundationNumber = 0;
		
		var options = {};
		options["moveSpeed"] = SOLITAIRE.CARD_DEAL_SPEED;
		
		var tableaus = game.getSpots("tableau");
		for(var i=0;i<tableaus.length;i++)
		{
			var tableau = tableaus[i];
			for(var m=0;m<tableau.cards.length;m++)
			{
				var card = tableau.cards[m];
				if(card.number===1)
				{
					game.moveCard(card, "foundation", foundationNumber, options);
					foundationNumber++;
					m=0;
				}
			}
		}
		
		this.reorderCards(game);
		
		if(Options.get("optionAnimateDeal")===true)
			setTimeout(function() { CARD.runAfterAnimations(function() { game.stockDistributionFinished(game); }); }, 100);
		else
			game.stockDistributionFinished(game);
	};
	
	/**
	 * Function to determine if a card is live or not
	 * @param {Object} card
	 */
	this.isCardLive = function(card, type)
	{
		if(card.number===1)
			return false;
			
		return true;
	}	
	
	/**
	 * This is called by game whenever a card that is clickable is clicked
	 * @param {Object} card Which card was clicked
	 */
	this.cardClicked = function(card)
	{
		var game = card.game;
		var spot = card.spot;
		
		if(spot["class"]=="stock")
		{
			SOLITAIRE.movesIncrement();
			
			this.reorderCards(game);
		}
	};	
	
	/**
	 * Internal function to reorder the cards
	 * @param {Object} game
	 */
	this.reorderCards = function(game)
	{
		var options = {};
		options["moveSpeed"] = SOLITAIRE.CARD_MOVE_SPEED;
		options["undoAppendToLastMove"] = false;
		
		var hasMovedOne = false;
		
		var tableaus = game.getSpots("tableau");
		for(var i=0;i<tableaus.length;i++)
		{
			var tableau = tableaus[i];
			
			if(tableau.cards.length<4)
			{
				var numCardsToAcquire = 4-tableau.cards.length;
				
				// Move cards from other stacks until we hit four
				// Traditional is to fill on the bottom taking from the top of the next stack
				for(var z=i+1;numCardsToAcquire>0 && z<tableaus.length;z++)
				{
					var acquireSource = tableaus[z];
					if(acquireSource.cards.length<1)
						continue;
					
					while(acquireSource.cards.length && numCardsToAcquire>0)
					{
						if(hasMovedOne)
							options["undoAppendToLastMove"] = true;
						
						hasMovedOne = true;
						
						if(this._gameID=="CruelTraditionalRedeal")
							game.moveCard(acquireSource.cards[0], "tableau", i, options, CONSTANTS.BOTTOM_OF_STACK);
						else
							game.moveCard(acquireSource.cards[acquireSource.cards.length-1], "tableau", i, options);
						
						numCardsToAcquire--;
					}
				}
			}
			else if(tableau.cards.length>4)
			{
				// Move cards out of this stack to the next stack
				// Traditional is to move cards on the bottom to the top of the next stack
				while(tableau.cards.length>4)
				{
					if(hasMovedOne)
						options["undoAppendToLastMove"] = true;
					
					hasMovedOne = true;
					if(this._gameID=="CruelTraditionalRedeal")
						game.moveCard(tableau.cards[tableau.cards.length-1], "tableau", i+1, options);
					else
						game.moveCard(tableau.cards[0], "tableau", i+1, options, CONSTANTS.BOTTOM_OF_STACK);
				}
			}
		}
		
		var tableaus = game.getSpots("tableau");
		for(var i=0;i<tableaus.length;i++)
		{
			game.updateSpot(tableaus[i]);
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
				return ((cardDroppedOn.isBase && cardDragged.number===1) ||
					    (cardDragged.number===(cardDroppedOn.number+1) && cardDragged.suit==cardDroppedOn.suit));
			case("tableau"):
				return (cardDragged.number===(cardDroppedOn.number-1) && cardDragged.suit==cardDroppedOn.suit);
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
			for (var i = 0; i < tableaus.length; i++)
			{
				var tableau = tableaus[i];
				if(tableau.cards.length>1)
					return false;
			}
		}
		
		var tableaus = game.getSpots("tableau");
		for(var i=0;i<tableaus.length;i++)
		{
			var tableau = tableaus[i];
			if(tableau.cards.length<1)
				continue;
				
			var card = tableau.cards[0];
			if(!card.isFaceUp())
				continue;
			
			if(this.tryPlayCard(card, obviousOnly, pretendOnly))
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
