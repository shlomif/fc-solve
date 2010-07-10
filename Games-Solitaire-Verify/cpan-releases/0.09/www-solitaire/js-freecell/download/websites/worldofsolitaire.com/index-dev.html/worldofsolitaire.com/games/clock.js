/* clock.js - robert@cosmicrealms.com */

gGames["Clock"] = new clock("Clock");

function clock(gameID)
{
	this._gameID = gameID;
	this._gameName = (gameID=="Clock" ? "Clock" : "clock");
	
	this._lastCard = null;
					 
	this._board =
	{
		"width"  : 7,
		"height" : 5,
		"spots"  :
		[
			{
				"class"		      : "foundation",
				"x"			      : [ 3.0, 4.0, 5.0, 5.6, 5.0, 4.0, 3.0, 2.0, 1.0, 0.4, 1.0, 2.0, 3.0 ],
				"y"			      : [ 0.0, 0.4, 0.8, 2.0, 3.2, 3.6, 4.0, 3.6, 3.2, 2.0, 0.8, 0.4, 1.8 ],
				"stackMax"        : [   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4 ],
				"startingZIndex"  : [   1,  11,  21,  31,  41,  51,  61,  71,  81,  91, 101, 111, 121 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"base"	          : { "droppable" : true },
				"droppable"       : { }
			},
			{
				"class"		      : "tableau",
				"x"			      : [ 3.0, 4.0, 5.0, 5.4, 5.0, 4.0, 3.0, 2.0, 1.0, 0.6, 1.0, 2.0, 3.0 ],
				"y"			      : [ 0.2, 0.6, 1.0, 2.0, 3.0, 3.4, 3.8, 3.4, 3.0, 2.0, 1.0, 0.6, 2.0 ],
				"stackMax"        : [   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4 ],
				"startingZIndex"  : [   6,  16,  26,  36,  46,  56,  66,  76,  86,  95, 106, 116, 126 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"clickable"       : { "faceUpOnly" : false, "faceDownOnly" : true, "askGame" : true },
				"draggable"       : { "faceUpOnly" : true, "faceDownOnly" : false },
				"doubleclickable" : { },
				"turnUpOnClick"   : true,
				"autoFlip"        : { "askGame" : true }
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
		
		this._lastCard = null;

		var options = {};
		options["turnFaceUp"] = false;
		options["moveSpeed"] = SOLITAIRE.CARD_DEAL_SPEED;
		
		game.moveCard(card, "tableau", stockIndex%13, options);
		if(stockIndex==51)
		{
			var tableaus = game.getSpots("tableau");
			for(var i=0;i<tableaus.length;i++)
			{
				game.updateSpot(tableaus[i]);
			}
			
			if(Options.get("optionAnimateDeal")===true)
				setTimeout(function() { CARD.runAfterAnimations(function() { game.stockDistributionFinished(game); }); }, 100);
			else
				game.stockDistributionFinished(game);
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
				if((cardDragged.number>=1 && cardDragged.number<=11 && spot.subIndex===cardDragged.number) ||
				   (cardDragged.number===12 && spot.subIndex===0) ||
				   (cardDragged.number===13 && spot.subIndex===12))
				{
					this._lastCard = cardDragged;
					return true;
				}
		}
		
		return false;
	};
		
	/**
	 * This is optional. If present it should returns spots that need updated after a valid drop
	 * @param {Object} cardDragged
	 * @param {Object} cardDroppedOn
	 */
	this.getPostDropUpdateSpotList = function(cardDragged, cardDroppedOn)
	{
		var game = cardDragged.game;
		
		var spotsToUpdate = [];
		
		spotsToUpdate.push(game.getSpot("foundation", cardDroppedOn.spot.subIndex))
		spotsToUpdate.push(game.getSpot("tableau", cardDroppedOn.spot.subIndex))
		
		return spotsToUpdate;
	};

	/**
	 * A card as been double clicked, figure out what to do with it
	 * @param {Object} card
	 */
	this.cardDoubleClicked = function(card)
	{
		var game = card.game;
		
		this.tryPlayCard(card);
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
			if(foundations[i].cards.length<4)	
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
		var matchingFoundation = game.getSpot("foundation", spot.subIndex);
		
		if(type=="clickable" || type=="autoFlip")
		{
			if(this._lastCard===null && spot.subIndex===12)
				return true;
				
			if(matchingFoundation.cards.length>0 && this._lastCard===matchingFoundation.cards[0])
				return true;
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
		var targetSpotNumber = (card.number>=1 && card.number<=11) ? card.number : (card.number===12 ? 0 : (card.number===13 ? 12 : null));
		if(targetSpotNumber===null)
			return false;
			
		var targetSpot = game.getSpot("foundation", targetSpotNumber);
		var targetCard = (targetSpot.cards.length<1) ? targetSpot.baseCard : targetSpot.cards[0];
		if(this.isCardDropValid(card, targetCard))
		{
			if(pretendOnly!==true)
				game.doCardDrop(card, targetCard);
			return true;
		}
		
		if(obviousOnly===true)
			return false;
		
		// Anything now
		
		return false;
	};
}

