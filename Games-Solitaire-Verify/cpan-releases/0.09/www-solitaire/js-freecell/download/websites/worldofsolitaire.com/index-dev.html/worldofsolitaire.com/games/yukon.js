/* yukon.js - robert@cosmicrealms.com */

gGames["Yukon"] = new yukon("Yukon");
gGames["RussianSolitaire"] = new yukon("RussianSolitaire");

function yukon(gameID)
{
	this._gameID = gameID;
	this._gameName = (gameID=="RussianSolitaire" ? "Russian Solitaire" :
	  			     (gameID=="Yukon" ? "Yukon" : "yukon"));
	
	this._board =
	{
		"width"  : 9,
		"height" : 4,
		"spots"  :
		[
			{
				"class"		      : "foundation",
				"x"			      : [ 8.0, 8.0, 8.0, 8.0 ],
				"y"			      : [ 0.0, 1.0, 2.0, 3.0 ],
				"stackMax"        : [  13,  13,  13,  13 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"base"			  : { "droppable" : true },
				"draggable"       : { },
				"droppable"       : { }
			},
			{
				"class"           : "tableau",
				"x"			      : [ 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0 ],
				"y"			      : [ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 ],
				"stackMax"        : [  52,  52,  52,  52,  52,  52,  52 ],
				"sizeExempt"      : true,
				"stackTypes"      : [ { "direction" : "S", "type" : "peek" } ],
				"clickable"       : { "faceUpOnly" : false, "faceDownOnly" : true },
				"draggable"       : { "topCardOnly" : false },
				"droppable"       : { },
				"doubleclickable" : { },
				"autoFlip"        : { },
				"turnUpOnClick"   : true,
				"base"			  : { "droppable" : true }
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
				
		if(stockIndex<=0)
		{
			game.moveCard(card, "tableau", 0, options);
			game.updateSpot(game.getSpot("tableau", 0));
		}
		else if(stockIndex<=6)
		{
			game.moveCard(card, "tableau", 1, options);
			if(stockIndex==6)
				game.updateSpot(game.getSpot("tableau", 1));
		}		
		else if(stockIndex<=13)
		{
			game.moveCard(card, "tableau", 2, options);
			if(stockIndex==13)
				game.updateSpot(game.getSpot("tableau", 2));
		}		
		else if(stockIndex<=21)
		{
			game.moveCard(card, "tableau", 3, options);
			if(stockIndex==21)
				game.updateSpot(game.getSpot("tableau", 3));
		}		
		else if(stockIndex<=30)
		{
			game.moveCard(card, "tableau", 4, options);
			if(stockIndex==30)
				game.updateSpot(game.getSpot("tableau", 4));
		}		
		else if(stockIndex<=40)
		{
			game.moveCard(card, "tableau", 5, options);
			if(stockIndex==40)
				game.updateSpot(game.getSpot("tableau", 5));
		}		
		else
		{
			if(stockIndex==51)
				options["afterMove"] = function() { self.stockDistributeFinished(game); };

			game.moveCard(card, "tableau", 6, options);

			if(stockIndex==51)
				game.updateSpot(game.getSpot("tableau", 6));
		}
	};
	
	/**
	 * This is called after the stock has been fully distributed
	 * @param {Object} game
	 */
	this.stockDistributeFinished = function(game)
	{
		game.turnCardFaceUp(game.getSpot("tableau", 0).cards[0], 0);
		
		var tableaus = game.getSpots("tableau");
		for(var i=1;i<tableaus.length;i++)
		{
			for(var m=0;m<5;m++)
			{
				game.turnCardFaceUp(tableaus[i].cards[m], SOLITAIRE.CARD_DEAL_TURN_OVER_DELAY*(((i-1)*3)), (i==(tableaus.length-1) && m==4) ? { "afterFaceUp" : function() { game.stockDistributionFinished(); } } : {});
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
				return cardDragged.stackIndex===0 &&
				       ((cardDroppedOn.isBase && cardDragged.number===1) ||
					    (cardDragged.number===(cardDroppedOn.number+1) && cardDragged.suit==cardDroppedOn.suit));
			case("tableau"):
				if(this._gameID=="RussianSolitaire")
					return (cardDroppedOn.isBase && cardDragged.number===13) ||
						   (cardDragged.number===(cardDroppedOn.number-1) && cardDragged.suit==cardDroppedOn.suit);
				else
					return (cardDroppedOn.isBase && cardDragged.number===13) ||
						   (cardDragged.number===(cardDroppedOn.number-1) &&
							((cardDragged.isBlack && cardDroppedOn.isRed) || (cardDragged.isRed && cardDroppedOn.isBlack)));
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
				for(var m=0;m<tableau.cards.length;m++)
				{
					if(!tableau.cards[m].isFaceUp())
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
