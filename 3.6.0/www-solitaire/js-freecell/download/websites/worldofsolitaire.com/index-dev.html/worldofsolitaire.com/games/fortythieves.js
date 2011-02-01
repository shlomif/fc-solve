/* fortyThieves.js - robert@cosmicrealms.com */

gGames["FortyThieves"] = new fortyThieves("FortyThieves");

function fortyThieves(gameID)
{
	this._gameID = gameID;
	this._gameName = (gameID=="FortyThieves" ? "Forty Thieves" : "fortythieves");
	
	this._board =
	{
		"width"  : 10,
		"height" : 2,
		"spots"  :
		[
			{
				"class"           : "stock",
			 	"x"               : [ 0.0 ],
				"y"		          : [ 0.0 ],
				"stackMax"        : [  64 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"clickable"       : { "faceUpOnly" : false },
				"moveOnClick"     : { "spot"                : "waste",
								      "count"               : 1,
									  "moveOptions"         : { "turnFaceUp" : true },
								      "reverseUpdateTarget" : true }
			},
			{
				"class"		      : "waste",
				"x"			      : [ 1.0 ],
				"y"			      : [ 0.0 ],
				"sizeExempt"      : true,
				"stackMax"        : [  64 ],
				"stackTypes"      : [ { "direction" :  "", "type" : "deck" } ],
				"draggable"       : { },
				"doubleclickable" : { },
				"reverseUpdate"   : true
			},
			{
				"class"		      : "foundation",
				"x"			      : [ 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0 ],
				"y"			      : [ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 ],
				"stackMax"        : [  13,  13,  13,  13,  13,  13,  13,  13 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"base"	          : { "droppable" : true },
				"draggable"       : { },
				"droppable"       : { }
			},
			{
				"class"           : "tableau",
				"x"			      : [ 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0 ],
				"y"			      : [ 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 ],
				"stackMax"        : [  17,  17,  17,  17,  17,  17,  17,  17,  17,  17 ],
				"stackTypes"      : [ { "direction" : "S", "type" : "peek" } ],
				"draggable"       : { },
				"droppable"       : { },
				"doubleclickable" : { },
				"base"	          : { "droppable" : true }
			}
		]
	};
	
	/**
	 * Will create our stock
	 */
	this.stockCreate = function()
	{
		return Stock.createStandard(2);
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

		if(stockIndex<=39)
		{
			game.moveCard(card, "tableau", parseInt(Math.floor((stockIndex/4)), 10), options);
			if(((stockIndex+1)%4)===0)
				game.updateSpot(game.getSpot("tableau",Math.floor((stockIndex/4))));
		}
		else
		{
			options["moveSpeed"] = SOLITAIRE.CARD_MOVE_SPEED;
			
			if(stockIndex==103)
				options["afterMove"] = function() { self.stockDistributeFinished(game); };
				
			game.moveCard(card, "stock", 0, options);
			if(stockIndex==103)
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
			var tableau = tableaus[i];
			for(var m=0;m<tableau.cards.length;m++)
			{
				game.turnCardFaceUp(tableau.cards[m], SOLITAIRE.CARD_DEAL_TURN_OVER_DELAY*(i*2), (i==(tableaus.length-1) && m==(tableau.cards.length-1)) ? { "afterFaceUp" : function() { game.stockDistributionFinished(); } } : {});
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
					   (cardDragged.number===(cardDroppedOn.number-1) && cardDragged.suit==cardDroppedOn.suit);
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
			var stock = game.getSpot("stock");
			if(stock.cards.length>0)
				return false;
			
			var waste = game.getSpot("waste");
			if(waste.cards.length>1)
				return false;
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
		
		var waste = game.getSpot("waste");
		if(waste.cards.length>0 && this.tryPlayCard(waste.cards[0], obviousOnly, pretendOnly))
			return true;
			
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
