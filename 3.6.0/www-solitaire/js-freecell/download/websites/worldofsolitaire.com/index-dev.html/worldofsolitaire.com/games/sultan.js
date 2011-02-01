/* sultan.js - robert@cosmicrealms.com */

gGames["Sultan"] = new sultan("Sultan");

function sultan(gameID)
{
	this._gameID = gameID;
	this._gameName = (gameID=="Sultan" ? "Sultan" : "sultan");
	this._foundationsFilled = [ false, false, false, false, false, false, false, false, false ];
	this._reservesFilled = 0;
	
	this._board =
	{
		"width"  : 6,
		"height" : 4,
		"spots"  :
		[
			{
				"class"           : "stock",
			 	"x"               : [ 0.0 ],
				"y"		          : [ 0.0 ],
				"stackMax"        : [  87 ],
				"stackTypes"           : [ { "direction" : "", "type" : "deck" } ],
				"clickable"       : { "faceUpOnly" : false },
				"base"	          : { "clickable" : true,
									  "fillOnClick" :
									  { "spot"                : "waste",
									    "reverseUpdateSource" : true,
										"maxTimes"			  : 2,
				 					    "moveOptions"         : { "turnFaceDown"      : true,
									  				              "skipTurnAnimation" : true,
																  "skipMoveAnimation" : true } } },
				"moveOnClick"     : { "spot"                : "waste",
								      "count"               : 1,
									  "moveOptions"         : { "turnFaceUp"        : true,
									  							"skipTurnAnimation" : false },
								      "reverseUpdateTarget" : true }
				
			},								
			{
				"class"		      : "waste",
				"x"			      : [ 0.0 ],
				"y"			      : [ 1.0 ],
				"stackMax"        : [  87 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"draggable"       : { },
				"doubleclickable" : { },
				"base"			  : { },
				"reverseUpdate"   : true
			},
			{
				"class"		      : "reserve",
				"x"				  : [ 1.0, 1.0, 1.0, 1.0, 5.0, 5.0, 5.0, 5.0 ],
				"y"				  : [ 0.0, 1.0, 2.0, 3.0, 0.0, 1.0, 2.0, 3.0 ],
				"stackMax"		  : [   1,   1,   1,   1,   1,   1,   1,   1 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"base"	          : { },
				"draggable"       : { },
				"doubleclickable" : { },
				"monitorChanges"  : true
			},
			{
				"class"		      : "foundation",
				"x"			      : [ 2.0, 3.0, 4.0, 2.0, 3.0, 4.0, 2.0, 3.0, 4.0 ],
				"y"			      : [ 0.5, 0.5, 0.5, 1.5, 1.5, 1.5, 2.5, 2.5, 2.5 ],
				"stackMax"        : [  13,  13,  13,  13,   1,  13,  13,  13,  13 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"droppable"       : { }
			}
		]
	};
		
	/**
	 * Will create our stock
	 */
	this.stockCreate = function()
	{
		this._foundationsFilled = [ false, false, false, false, false, false, false, false, false ];
		this._reservesFilled = 0;
		
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
		options["moveSpeed"] = SOLITAIRE.CARD_DEAL_SPEED;
		options["turnFaceUp"] = false;

		if(card.number==13 || (card.number===1 && card.suit=="h" && this._foundationsFilled[1]===false))
		{
			options["turnFaceUp"] = true;
			options["skipTurnAnimation"] = true;

			var foundationLoc = -1;			
			if(card.suit=="s" && this._foundationsFilled[0]===false)
				foundationLoc = 0;
			else if(card.suit=="h" && card.number===1 && this._foundationsFilled[1]===false)
				foundationLoc = 1;
			else if(card.suit=="s" && this._foundationsFilled[0]===true)
				foundationLoc = 2;
			else if(card.suit=="d" && this._foundationsFilled[3]===false)
				foundationLoc = 3;
			else if(card.suit=="h" && card.number==13 && this._foundationsFilled[4]===false)
				foundationLoc = 4;
			else if(card.suit=="d" && this._foundationsFilled[3]===true)
				foundationLoc = 5;
			else if(card.suit=="c" && this._foundationsFilled[6]===false)
				foundationLoc = 6;
			else if(card.suit=="h" && card.number==13 && this._foundationsFilled[4]===true)
				foundationLoc = 7;
			else if(card.suit=="c" && this._foundationsFilled[6]===true)
				foundationLoc = 8;
							
			this._foundationsFilled[foundationLoc] = true;
			game.moveCard(card, "foundation", foundationLoc, options);
			game.updateSpot(game.getSpot("foundation", foundationLoc));
		}
		else if(this._reservesFilled<8)
		{
			game.moveCard(card, "reserve", this._reservesFilled, options);
			game.updateSpot(game.getSpot("reserve", this._reservesFilled));
			
			this._reservesFilled++;
		}
		else
		{
			options["moveSpeed"] = SOLITAIRE.CARD_MOVE_SPEED;
			options["skipMoveAnimation"] = true;
			
			game.moveCard(card, "stock", 0, options);
			
			if(stockIndex==103)
			{
				game.updateSpot(game.getSpot("stock"));
				if(Options.get("optionAnimateDeal")===true)
					setTimeout(function() { CARD.runAfterAnimations(function() { self.stockDistributeFinished(game); }); }, 100);
				else
					self.stockDistributeFinished(game);
			}
		}
	};
	
	/**
	 * This is called after the stock has been fully distributed
	 * @param {Object} game
	 */
	this.stockDistributeFinished = function(game)
	{
		var reserves = game.getSpots("reserve");
		for(var i=0;i<reserves.length;i++)
		{
			var reserve = reserves[i];
			game.turnCardFaceUp(reserve.cards[0], SOLITAIRE.CARD_DEAL_TURN_OVER_DELAY*(i), (i==(reserves.length-1)) ? { "afterFaceUp" : function() { game.stockDistributionFinished(); } } : {});
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
				return spot.subIndex!=4 && cardDragged.suit==cardDroppedOn.suit &&
				       ((cardDroppedOn.number==13 && cardDragged.number===1) ||
					   (cardDragged.number===(cardDroppedOn.number+1)));
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
			var foundation = foundations[i];
			
			if(i==4)
				continue;
				
			if(i===1)
			{
				if(foundation.cards.length<12)
					return false;
				else
					continue;
			}
				
			if(foundation.cards.length<13)
				return false;
		}
		
		return true;
	};
	
	/**
     * This is called whenever a spot's cards have changed
     * @param {Object} game
     * @param {Object} spot
     */
    this.spotChanged = function(game, spot)
    {
		if(spot["class"]=="reserve")
		{
			if(spot.cards.length!==0)
				return;
				
			var options = {};
				
			var targetSpot = game.getSpot("waste");
			if(targetSpot.cards.length===0)
			{
				targetSpot = game.getSpot("stock");
				options["turnFaceUp"] = true;
			}
			
			if(targetSpot.cards.length===0)
				return;
			
			options["undoAppendToLastMove"] = true;
			
			game.moveCard(targetSpot.cards[0], spot["class"], spot["subIndex"], options);
			
			game.updateSpot(spot);
			game.updateSpot(targetSpot);
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
		
		/*if(type=="won")
		{
			var stock = game.getSpot("stock");
			if(stock.cards.length>0)
				return false;
			
			var waste = game.getSpot("waste");
			if(waste.cards.length>1)
				return false;
			
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
		}*/
		
		var reserves = game.getSpots("reserve");
		for(var i=0;i<reserves.length;i++)
		{
			var reserve = reserves[i];
			if(reserve.cards.length<1)
				continue;
				
			var card = reserve.cards[0];
			
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
			var targetCard = foundation.cards[0];
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
