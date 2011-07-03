/* royalparade.js - robert@cosmicrealms.com */

gGames["RoyalParade"] = new royalParade("RoyalParade");

function royalParade(gameID)
{
	this._gameID = gameID;
	this._gameName = (gameID=="RoyalParade" ? "Royal Parade" : "royal parade");
	
	this._board =
	{
		"width"  : 9,
		"height" : 4,
		
		"spots"  :
		[
			{
				"class"           : "stock",
			 	"x"               : [ 0.0 ],
				"y"		          : [ 1.0 ],
				"stackMax"        : [  72 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"clickable"       : { "faceUpOnly" : false },
				"moveOnClick"     : { "spot"                : "reserve",
								      "count"               : 1,
									  "reverseUpdateTarget" : true,
									  "moveOptions"         : { "turnFaceUp"        : true,
																"moveSpeedFunction" : function() { return SOLITAIRE.CARD_DEAL_SPEED; } } }				
			},
			{
				"class"           : "waste",
			 	"x"               : [ 0.0 ],
				"y"		          : [ 2.0 ],
				"stackMax"        : [   8 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"droppable"       : { },
				"base"            : { "droppable" : true }
			},
			{
				"class"           : "foundation",
				"x"			      : A.append(A.append(
											 A.pushSequence(Array(0), 1.0, 8.0),
											 A.pushSequence(Array(0), 1.0, 8.0)),
											 A.pushSequence(Array(0), 1.0, 8.0)),
				"y"			      : A.append(A.append(
											 A.pushMany(Array(0), 0.0, 8),
											 A.pushMany(Array(0), 1.0, 8)),
											 A.pushMany(Array(0), 2.0, 8)),
				"stackMax"	      : A.pushMany(Array(0), 1, 24),
				"actualStackMax"  : A.pushMany(Array(0), 4, 24),
				"stackTypes"      :
				[
				    { "direction" : "S", "type" : "peek", "peekValue" : 0.55, "extraSpacingPercentage" : 4.00 }
				],
				"draggable"		  : { },
				"droppable"       : { },
				"doubleclickable" : { },
				"base"            : { "droppable" : true, "clear" : true }
			},
			{
				"class"           : "reserve",
				"x"			      : A.pushSequence(Array(0), 1.0, 8.0),
				"y"			      : A.pushMany(Array(0), 3.0, 8),
				"stackMax"	      : A.pushMany(Array(0), 4, 8),
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"draggable"		  : { },
				"doubleclickable" : { }
			}
		]
	};
	
	this.firstSeen = {};
	
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

		if(stockIndex===0)
			this.firstSeen = {};
			
		var options = {};
		options["moveSpeed"] = SOLITAIRE.CARD_DEAL_SPEED;
		
		if(card.number>=2 && card.number<=4 && !this.firstSeen.hasOwnProperty(card.number))
			this.firstSeen[card.number] = card;
		
		if(stockIndex<32)
			game.turnCardFaceUp(card, -1);
		
		if(stockIndex==103)
			options["afterMove"] = function() { self.stockDistributeFinished(game); };
		
		if(stockIndex<24)
		{
			game.moveCard(card, "foundation", stockIndex, options);
			game.updateSpot(game.getSpot("foundation", stockIndex));
		}
		else if(stockIndex<32)
		{
			game.moveCard(card, "reserve", (stockIndex-24), options);
			game.updateSpot(game.getSpot("reserve", (stockIndex-24)));
		}
		else
			game.moveCard(card, "stock", 0, options);
			
		if(stockIndex==103)
			game.updateSpot(game.getSpot("stock"));
	};

	/**
	 * This is called after the stock has been fully distributed
	 * @param {Object} game
	 */
	this.stockDistributeFinished = function(game)
	{
		for(var i=2;i<=4;i++)
		{
			var targetFoundation = game.getSpot("foundation", ((i-2)*8));
			
			if(this.firstSeen[i].spot["class"]=="stock")
				game.turnCardFaceDown(targetFoundation.cards[0], -1);
			
			game.turnCardFaceUp(this.firstSeen[i], -1);
			
			game.swapCards(this.firstSeen[i], targetFoundation.cards[0], false, true);
		}
	
		game.stockDistributionFinished();
	};
	
	/**
	 * Function to determine if a card is live or not
	 * @param {Object} card
	 */
	this.isCardLive = function(card, type)
	{
		var spot = card.spot;
		var game = card.game;
		
		return true;
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
		var game = cardDragged.game;
		
		if(cardDragged==cardDroppedOn)
			return false;

		var spotClass = cardDroppedOn.spot["class"];
		
		if(spotClass=="waste" && cardDragged.number===1)
		{
			if(cardDragged.spot["class"]=="foundation")
				SOLITAIRE.scoreIncrement(100);
				
			return true;
		}
		else if(spotClass=="waste")
		{
			return false;
		}

		if(spotClass!="foundation")
			return false;
					
		if(cardDroppedOn.isBase)
		{
			if((Math.floor(cardDroppedOn.spot["subIndex"]/8)+2)===cardDragged.number)
				return true;
				
			return false;
		}
		
		if(cardDragged.spot.cards.length===1 && cardDroppedOn.spot.cards.length===1 &&
		   (Math.floor(cardDroppedOn.spot["subIndex"]/8)+2)===cardDragged.number &&
		   (Math.floor(cardDragged.spot["subIndex"]/8)+2)===cardDroppedOn.number)
		{
			game.swapCards(cardDragged, cardDroppedOn, true, true);
			return false;
		}
		
		if(cardDroppedOn.suit!=cardDragged.suit)
			return false;
		
		if((cardDroppedOn.number+3)!=cardDragged.number)
			return false;
			
		if(cardDroppedOn.spot.cards.length===1 && (Math.floor(cardDroppedOn.spot["subIndex"]/8)+2)!=cardDroppedOn.number)
			return false;
			
		return true;
	};
	
	/**
	 * Called to determine if the game has been won or not
	 * @param {Object} game
	 */
	this.isGameWon = function(game)
	{
		var foundations = game.getSpots("foundation");
		var lastCard = null;
		for(var i=0;i<foundations.length;i++)
		{
			var foundation = foundations[i];
			
			if(foundation.cards.length<4)
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
			// Should do checks and only return false if not yet won
			return false;
		}
		
		var foundations = game.getSpots("foundation");
		for(var i=0;i<foundations.length;i++)
		{
			var foundation = foundations[i];
			if(foundation.cards.length<1)
				continue;
				
			var card = foundation.cards[0];
			if(this.tryPlayCard(card, obviousOnly, pretendOnly))
				return true;
		}

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
		
		// Obvious First - The Aces
		if(card.number===1)
		{
			var waste = game.getSpot("waste");
			if(pretendOnly!==true)
				game.doCardDrop(card, (waste.cards.length<1) ? waste.baseCard : waste.cards[0]);
			return true;
		}
		
		if(obviousOnly===true)
			return false;
		
		// Anything now
		
		return false;
	};
}
