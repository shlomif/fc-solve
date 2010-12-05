/* flowergarden.js - robert@cosmicrealms.com */

gGames["FlowerGarden"] = new flowergarden("FlowerGarden");

function flowergarden(gameID)
{
	this._gameID = gameID;
	this._gameName = (gameID=="FlowerGarden" ? "Flower Garden" : "flower garden");
	
	this._board =
	{
		"width"  : 6,
		"height" : 3,
		"spots"  :
		[
			{
				"class"		      : "foundation",
				"x"			      : [ 1.0, 2.0, 3.0, 4.0 ],
				"y"			      : [ 0.0, 0.0, 0.0, 0.0 ],
				"stackMax"        : [  13,  13,  13,  13 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"base"	          : { "droppable" : true },
				"draggable"       : { },
				"droppable"       : { }
			},
			{
				"class"           : "tableau",
				"x"			      : [ 0.0, 1.0, 2.0, 3.0, 4.0, 5.0 ],
				"y"			      : [ 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 ],
				"stackMax"        : [  24,  24,  24,  24,  24,  24 ],
				"actualStackMax"  : [  52,  52,  52,  52,  52,  52 ],
				"stackTypes"      : [ { "direction" : "S", "type" : "peek" } ],
				"draggable"       : { "topCardOnly" : true },
				"droppable"       : { },
				"doubleclickable" : { },
				"base"            : { "droppable" : true }
			},
			{
				"class"			  : "reserve",
				"x"				  : [ 0.0 ],
				"y"				  : [ 1.93 ],
				"stackMax"        : [  16 ],
				"stackTypes"      : [ { "direction" : "E", "type" : "peek", "peekModifier" : 2 } ],
				"draggable"       : { "topCardOnly" : false, "alone" : true },
				"doubleclickable" : { "topCardOnly" : false },
				"sizeExempt"      : true,
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
		options["turnFaceUp"] = false;
		options["moveSpeed"] = SOLITAIRE.CARD_DEAL_SPEED;
				
		if(stockIndex<=5)
		{
			game.moveCard(card, "tableau", 0, options);
			if(stockIndex==5)
				game.updateSpot(game.getSpot("tableau", 0));
		}
		else if(stockIndex<=11)
		{
			game.moveCard(card, "tableau", 1, options);
			if(stockIndex==11)
				game.updateSpot(game.getSpot("tableau", 1));
		}		
		else if(stockIndex<=17)
		{
			game.moveCard(card, "tableau", 2, options);
			if(stockIndex==17)
				game.updateSpot(game.getSpot("tableau", 2));
		}		
		else if(stockIndex<=23)
		{
			game.moveCard(card, "tableau", 3, options);
			if(stockIndex==23)
				game.updateSpot(game.getSpot("tableau", 3));
		}		
		else if(stockIndex<=29)
		{
			game.moveCard(card, "tableau", 4, options);
			if(stockIndex==29)
				game.updateSpot(game.getSpot("tableau", 4));
		}		
		else if(stockIndex<=35)
		{
			game.moveCard(card, "tableau", 5, options);
			if(stockIndex==35)
				game.updateSpot(game.getSpot("tableau", 5));
		}		
		else
		{
			options["turnFaceUp"] = true;
			options["skipTurnAnimation"] = true;
			if(stockIndex==51)
				options["afterMove"] = function() { self.stockDistributeFinished(game); };
				
			game.moveCard(card, "reserve", 0, options);
			if(stockIndex==51)
				game.updateSpot(game.getSpot("reserve"));
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
			var tableau = tableaus[i];
			for(var m=0;m<tableau.cards.length;m++)
			{
				game.turnCardFaceUp(tableau.cards[m], SOLITAIRE.CARD_DEAL_TURN_OVER_DELAY*(i*3), (i==(tableaus.length-1) && m==(tableau.cards.length-1)) ? { "afterFaceUp" : function() { game.stockDistributionFinished(); } } : {});
			}
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
			// Check to see if any cards in the tableau above are on top of lower ones below??
			var tableaus = game.getSpots("tableau");
			for(var i=0;i<tableaus.length;i++)
			{
				var tableau = tableaus[i];
				var lastCardNum = 0;
				for(var m=0;m<tableau.cards.length;m++)
				{
					if(tableau.cards[m].number<lastCardNum)
						return false;
					lastCardNum = tableau.cards[m].number;
				}
			}
		}
		
		var tableaus = game.getSpots("tableau");
		for(var i=0;i<tableaus.length;i++)
		{
			var tableau = tableaus[i];
			if(tableau.cards.length<1)
				continue;
				
			var card = tableau.cards[0];
			if(this.tryPlayCard(card, obviousOnly, pretendOnly))
				return true;
		}
		
		var reserve = game.getSpot("reserve");
		for(var i=0;i<reserve.cards.length;i++)
		{
			var card = reserve.cards[i];
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
