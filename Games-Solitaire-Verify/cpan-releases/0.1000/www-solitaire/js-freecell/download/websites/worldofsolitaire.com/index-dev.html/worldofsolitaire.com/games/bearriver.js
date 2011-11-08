/* bearriver.js - robert@cosmicrealms.com */

gGames["BearRiver"] = new bearriver("BearRiver");

function bearriver(gameID)
{
	this._gameID = gameID;
	this._gameName = (gameID=="BearRiver" ? "Bear River" : "bearriver");
	
	this._board =
	{
		"width"  : 6,
		"height" : 4,
		"spots"  :
		[
			{
				"class"		      : "foundation",
				"x"			      : [ 1.0, 2.0, 3.0, 4.0 ],
				"y"			      : [ 0.0, 0.0, 0.0, 0.0 ],
				"stackMax"        : [  13,  13,  13,  13 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"base"	          : { "droppable" : true },
				"droppable"       : { }
			},
			{
				"class"           : "tableau",
				"x"			      : [ 0.0, 1.0, 2.0, 3.0, 4.0, 0.0, 1.0, 2.0, 3.0, 4.0, 0.0, 1.0, 2.0, 3.0, 4.0 ],
				"y"			      : [ 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0, 2.0, 2.0, 2.0, 3.0, 3.0, 3.0, 3.0, 3.0 ],
				"stackMax"        : [   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3 ],
				"actualStackMax"  : [   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3 ],
				"stackTypes"      : [ { "direction" : "E", "type" : "peek" } ],
				"draggable"       : { "topCardOnly" : true },
				"droppable"       : { },
				"doubleclickable" : { }
			},
			{
				"class"           : "hole",
				"x"			      : [ 5.0, 5.0, 5.0 ],
				"y"			      : [ 1.0, 2.0, 3.0 ],
				"stackMax"        : [   3,   3,   3 ],
				"actualStackMax"  : [   3,   3,   3 ],
				"stackTypes"      : [ { "direction" : "E", "type" : "peek" } ],
				"draggable"       : { "topCardOnly" : true },
				"base"	          : { "droppable" : true },
				"droppable"       : { },
				"doubleclickable" : { }
			}
		]
	};
	
	this._foundationStartNum = 0;

	/**
	 * This is called when the stock of cards is first distributed, once for each card in the stock
	 * @param {Object} card Which card is to be distributed
	 * @param {Object} stockIndex Where in the stock this card is
	 */
	this.stockDistribute = function(card, stockIndex)
	{
		var game = card.game;
		
		var options = {};
		options["moveSpeed"] = SOLITAIRE.CARD_DEAL_SPEED;
		
		if(stockIndex==51)
			options["afterMove"] = function() { game.stockDistributionFinished(); };
		
		game.turnCardFaceUp(card, -1);
		
		if(stockIndex==0)
		{
			this._foundationStartNum = card.number;
			game.moveCard(card, "foundation", 0, options);
			game.updateSpot(game.getSpot("foundation", 0));
		}
		else if(stockIndex<46)
		{
			var targetSpot = Math.ceil(stockIndex/3)-1;
			game.moveCard(card, "tableau", targetSpot, options);
			if(stockIndex%3===0)
				game.updateSpot(game.getSpot("tableau", targetSpot));
		}
		else
		{
			var targetSpot = Math.ceil((stockIndex-45)/2)-1;
			game.moveCard(card, "hole", targetSpot, options);
			if((stockIndex-45)%2===0 || stockIndex==51)
				game.updateSpot(game.getSpot("hole", targetSpot));
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
				       ((cardDroppedOn.isBase && cardDragged.number===this._foundationStartNum) ||
					    (cardDragged.number===(cardDroppedOn.number+1) && cardDragged.suit==cardDroppedOn.suit) ||
						(cardDragged.number===1 && cardDroppedOn.number==13 && cardDragged.suit==cardDroppedOn.suit));
			case("tableau"):
				return cardDragged.stackIndex===0 && !cardDroppedOn.isBase && spot.cards.length<3 &&
				       ((cardDragged.number===(cardDroppedOn.number+1) && cardDragged.suit==cardDroppedOn.suit) ||
						(cardDragged.number===1 && cardDroppedOn.number==13 && cardDragged.suit==cardDroppedOn.suit) ||
						((cardDragged.number+1)===cardDroppedOn.number && cardDragged.suit==cardDroppedOn.suit) ||
						(cardDragged.number===13 && cardDroppedOn.number==1 && cardDragged.suit==cardDroppedOn.suit));
			case("hole"):
				return cardDragged.stackIndex===0 && spot.cards.length<3 &&
				       (cardDroppedOn.isBase ||
					    (cardDragged.number===(cardDroppedOn.number+1) && cardDragged.suit==cardDroppedOn.suit) ||
						(cardDragged.number===1 && cardDroppedOn.number==13 && cardDragged.suit==cardDroppedOn.suit) ||
						((cardDragged.number+1)===cardDroppedOn.number && cardDragged.suit==cardDroppedOn.suit) ||
						(cardDragged.number===13 && cardDroppedOn.number==1 && cardDragged.suit==cardDroppedOn.suit));
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
			// Check to see if any cards in the tableau have more than 1 card
			var tableaus = game.getSpots("tableau");
			A.append(tableaus, game.getSpots("hole"));
			for(var i=0;i<tableaus.length;i++)
			{
				if(tableaus[i].cards.length>1)
					return false;
			}
		}
		
		var tableaus = game.getSpots("tableau");
		A.append(tableaus, game.getSpots("hole"));
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
