/* spider.js - robert@cosmicrealms.com */

gGames["AcesUp"] = new other("AcesUp");

function other(gameID)
{
	this._gameID = gameID;
	this._gameName = (gameID=="AcesUp" ? "Aces Up" : "other");
	
	this._board =
	{
		"width"  : 7,
		"height" : 1,
		"spots"  :
		[
			{
				"class"           : "stock",
			 	"x"               : [ 0.0 ],
				"y"		          : [ 0.0 ],
				"stackMax"        : [  48 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"clickable"       : { "faceUpOnly" : false },
				"moveOnClick"     : { "spot"                : "tableau",
								      "count"               : 1,
									  "reverseUpdateTarget" : true,
									  "moveOptions"         : { "turnFaceUp"        : true,
																"moveSpeedFunction" : function() { return SOLITAIRE.CARD_DEAL_SPEED; } } }				
			},
			{
				"class"		      : "tableau",
				"x"			      : [ 1.5, 2.5, 3.5, 4.5 ],
				"y"			      : [ 0.0, 0.0, 0.0, 0.0 ],
				"stackMax"        : [  13,  13,  13,  13 ],
				"stackTypes"      : [ { "direction" : "S", "type" : "peek" } ],
				"reverseUpdate"   : true,
				"clickable"       : { },
				"draggable"       : { },
				"droppable"       : { },
				"base"            : { "droppable" : true }
			},			
			{
				"class"           : "foundation",
				"x"			      : [ 6.0 ],
				"y"			      : [ 0.0 ],
				"stackMax"        : [  48 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"base"            : { }
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
		
		if(stockIndex<4)
		{
			game.moveCard(card, "tableau", stockIndex, options);
			game.updateSpot(game.getSpot("tableau", stockIndex));
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
		game.turnCardFaceUp(game.getSpot("tableau", 0).cards[0], 0);
		game.turnCardFaceUp(game.getSpot("tableau", 1).cards[0], SOLITAIRE.CARD_DEAL_TURN_OVER_DELAY*1);
		game.turnCardFaceUp(game.getSpot("tableau", 2).cards[0], SOLITAIRE.CARD_DEAL_TURN_OVER_DELAY*2);
		game.turnCardFaceUp(game.getSpot("tableau", 3).cards[0], SOLITAIRE.CARD_DEAL_TURN_OVER_DELAY*3, { "afterFaceUp" : function() { game.stockDistributionFinished(); } });
	};
	
	/**
	 * This is called by game whenever a card that is clickable is clicked
	 * @param {Object} card Which card was clicked
	 */
	this.cardClicked = function(card)
	{
		var game = card.game;
		var spot = card.spot;
		
		if(card.spot["class"]=="tableau")
		{
			var tableaus = game.getSpots("tableau");
			for(var i=0;i<tableaus.length;i++)
			{
				var tableau = tableaus[i];
				
				if(tableau.cards.length<1 || tableau.cards[0]==card)
					continue;
				
				var tableauCard = tableau.cards[0];
				if(tableauCard.suit===card.suit && (tableauCard.number===1 || (tableauCard.number>card.number && card.number!==1)))
				{
					SOLITAIRE.movesIncrement();
					
					game.moveCard(card, "foundation");

					game.updateSpot(spot);
					game.updateSpot(game.getSpot("foundation"));
					
					return;
				}
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
				return cardDroppedOn.isBase;
		}
		
		return false;
	};
		
	/**
	 * Called to determine if the game has been won or not
	 * @param {Object} game
	 */
	this.isGameWon = function(game)
	{
		var foundation = game.getSpot("foundation");
		if(foundation.cards.length<48)
			return false;
		
		return true;
	};
}
