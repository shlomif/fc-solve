/* canfield.js - robert@cosmicrealms.com */

gGames["Canfield"] = new canfield("Canfield");

function canfield(gameID)
{
	this._gameID = gameID;
	this._gameName = (gameID=="Canfield" ? "Canfield" : "canfield");
	this._foundationStartNum = 0;
	
	this._board =
	{
		"width"  : 7,
		"height" : 2,
		"spots"  :
		[
			{
				"class"           : "stock",
			 	"x"               : [ 0.0 ],
				"y"		          : [ 0.0 ],
				"stackMax"        : [  34 ],
				"stackTypes"           : [ { "direction" : "", "type" : "deck" } ],
				"clickable"       : { "faceUpOnly" : false },
				"base"	          : { "clickable" : true,
									  "fillOnClick" :
									  { "spot"                : "waste",
									    "reverseUpdateSource" : true,
				 					    "moveOptions"         : { "turnFaceDown"      : true,
									  				              "skipTurnAnimation" : true,
														          "moveSpeedFunction" : function() { return SOLITAIRE.CARD_MOVE_SPEED; } } } },
				"moveOnClick"     : { "spot"                : "waste",
								      "count"               : 3,
									  "moveOptions"         : { "turnFaceUp" : true, "skipTurnAnimation" : true },
								      "reverseUpdateTarget" : true }
				
			},
			{
				"class"		      : "waste",
				"x"			      : [ 1.0 ],
				"y"			      : [ 0.0 ],
				"sizeExempt"      : true,
				"stackMax"        : [  34 ],
				"stackTypes"      :
				[
					{ "direction" : "E", "type" : "peek", "peekModifier" : 2, "index" : [ 0, 1, 2 ] },
					{ "direction" :  "", "type" : "deck" }
				],
				"draggable"       : { },
				"doubleclickable" : { },
				"reverseUpdate"   : true
			},
			{
				"class"		      : "reserve",
				"x"				  : [ 1.0 ],
				"y"				  : [ 1.0 ],
				"stackMax"		  : [  13 ],
				"stackTypes"      : [ { "direction" : "S", "type" : "peek", "peekValue" : 0.50 } ],
				"draggable"       : { },
				"autoFlip"        : { },
				"turnUpOnClick"   : true
			},
			{
				"class"		      : "foundation",
				"x"			      : [ 3.0, 4.0, 5.0, 6.0 ],
				"y"			      : [ 0.0, 0.0, 0.0, 0.0 ],
				"stackMax"        : [  13,  13,  13,  13 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"base"	          : { "droppable" : true },
				"draggable"       : { },
				"droppable"       : { }
			},
			{
				"class"           : "tableau",
				"x"			      : [  3.0, 4.0, 5.0, 6.0 ],
				"y"			      : [  1.0, 1.0, 1.0, 1.0 ],
				"stackMax"        : [   22,  22,  22,  22 ],
				"stackTypes"      : [ { "direction" : "S", "type" : "peek" } ],
				"draggable"       : { "topCardOnly" : false, "askGame" : true },
				"droppable"       : { },
				"doubleclickable" : { },
				"base"	          : { "droppable" : true },
				"monitorChanges"  : true
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
			this._foundationStartNum = card.number;
			game.moveCard(card, "foundation", 0, options);
			game.updateSpot(game.getSpot("foundation", 0));
		}
		else if(stockIndex<=13)
		{
			game.moveCard(card, "reserve", 0, options);
			if(stockIndex==13)
				game.updateSpot(game.getSpot("reserve", 0));
		}		
		else if(stockIndex<=17)
		{
			game.moveCard(card, "tableau", stockIndex-14, options);
			game.updateSpot(game.getSpot("tableau", stockIndex-14));
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
		game.turnCardFaceUp(game.getSpot("foundation", 0).cards[0], 0);
		game.turnCardFaceUp(game.getSpot("tableau", 0).cards[0], SOLITAIRE.CARD_DEAL_TURN_OVER_DELAY*1);
		game.turnCardFaceUp(game.getSpot("tableau", 1).cards[0], SOLITAIRE.CARD_DEAL_TURN_OVER_DELAY*2);
		game.turnCardFaceUp(game.getSpot("tableau", 2).cards[0], SOLITAIRE.CARD_DEAL_TURN_OVER_DELAY*3);
		game.turnCardFaceUp(game.getSpot("tableau", 3).cards[0], SOLITAIRE.CARD_DEAL_TURN_OVER_DELAY*4);
		game.turnCardFaceUp(game.getSpot("reserve", 0).cards[0], SOLITAIRE.CARD_DEAL_TURN_OVER_DELAY*5, { "afterFaceUp" : function() { game.stockDistributionFinished(); } });
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
				return cardDroppedOn.isBase ||
					   ((cardDragged.number===(cardDroppedOn.number-1) || (cardDragged.number==13 && cardDroppedOn.number===1)) &&
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
	 * Internal function to determine if a card is live or not
	 * @param {Object} card
	 */
	this.isCardLive = function(card, type)
	{
		var spot = card.spot;
		var game = card.game;
		
		if(type=="draggable" && spot["class"]=="tableau")
		{
			if(card["stackIndex"]===0 || card["stackIndex"]===(spot.cards.length-1))
				return true;
		}
		
		return false;
	};		
	
	/**
     * This is called whenever a spot's cards have changed
     * @param {Object} game
     * @param {Object} spot
     */
    this.spotChanged = function(game, spot)
    {
		if(spot["class"]=="tableau")
		{
			if(spot.cards.length>0)
				return;
			
			var reserve = game.getSpot("reserve");
			if(reserve.cards.length===0)
				return;
			
			var options = {};
			options["undoAppendToLastMove"] = true;
			game.moveCard(reserve.cards[0], spot["class"], spot["subIndex"], options);
			
			game.updateSpot(spot);
			game.updateSpot(reserve);
		}
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
			
			var reserve = game.getSpot("reserve");
			if(reserve.cards.length>0)
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
