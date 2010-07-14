/* klondike.js - robert@cosmicrealms.com */

gGames["KlondikeTurn1"] = new klondike("KlondikeTurn1");
gGames["KlondikeTurn3"] = new klondike("KlondikeTurn3");
gGames["DoubleKlondikeTurn1"] = new klondike("DoubleKlondikeTurn1");
gGames["DoubleKlondikeTurn3"] = new klondike("DoubleKlondikeTurn3");
gGames["TripleKlondikeTurn1"] = new klondike("TripleKlondikeTurn1");
gGames["TripleKlondikeTurn3"] = new klondike("TripleKlondikeTurn3");
gGames["Easthaven"] = new klondike("Easthaven");
gGames["EasthavenKingsOnly"] = new klondike("EasthavenKingsOnly");
gGames["DoubleEasthaven"] = new klondike("DoubleEasthaven");
gGames["DoubleEasthavenKingsOnly"] = new klondike("DoubleEasthavenKingsOnly");
gGames["TripleEasthaven"] = new klondike("TripleEasthaven");
gGames["TripleEasthavenKingsOnly"] = new klondike("TripleEasthavenKingsOnly");



// Tips
// In the stackTypes, those with "index" must go first
//
// clickable status is false by default
// If clickable object exists then default is to only allow clicking the top card and only if it's face up
// Clickable has "faceUpOnly" and "topCardOnly" booleans
//
// draggable is false by default
// If draggable object exists then the default is to only allow dragging the top card and only if it's face up
// Draggable has "faceUpOnly" and "topCardOnly" booleans
//
// droppable is false by default
// If droppable object exists then the default is to only allow dropping on the top card and only if it's face up
// In addition if droppable exists, the default is to allow dropping on the base if base is set for that spot
// droppable has a "faceUpOnly" and "" booleans
//
// autoFlip is false by default
// if autoFlip exists then the default is to automatically flip the top card and only if it's face down and only if the option to autoflip is on
// No options exist for autoFlip right now
//
// doubleclickable is false by default
// If doubleclickable exists then the default is to allow doubleclicks on the top card only and only if it's face up
// doubleclickable has "faceUpOnly" and "topCardOnly" booleans

function klondike(gameID)
{
	this._gameID = gameID;
	this._gameName = (gameID=="KlondikeTurn1" ? "Klondike (Turn One)" : 
	                 (gameID=="KlondikeTurn3" ? "Klondike (Turn Three)" :
					 (gameID=="DoubleKlondikeTurn1" ? "Double Klondike (Turn One)" : 
	                 (gameID=="DoubleKlondikeTurn3" ? "Double Klondike (Turn Three)" :
					 (gameID=="TripleKlondikeTurn1" ? "Triple Klondike (Turn One)" : 
	                 (gameID=="TripleKlondikeTurn3" ? "Triple Klondike (Turn Three)" :
					 (gameID=="Easthaven" ? "Easthaven" : 
	                 (gameID=="EasthavenKingsOnly" ? "Easthaven (Kings Only)" :
					 (gameID=="DoubleEasthaven" ? "Double Easthaven" : 
	                 (gameID=="DoubleEasthavenKingsOnly" ? "Double Easthaven (Kings Only)" :
					 (gameID=="TripleEasthaven" ? "Triple Easthaven" : 
	                 (gameID=="TripleEasthavenKingsOnly" ? "Triple Easthaven (Kings Only)" : "klondike"))))))))))));
	
	if(gameID.indexOf("Easthaven")!=-1)
	{
		this._board =
		{
			"width"  : (gameID.indexOf("Triple")===0 ? 13 : (gameID.indexOf("Double")===0 ? 9 : 7)),
			"height" : 2,
			"spots"  :
			[
				{
					"class"           : "stock",
				 	"x"               : [ 0.0 ],
					"y"		          : [ 0.0 ],
					"stackMax"        : [ (gameID.indexOf("Triple")===0 ? 120 : (gameID.indexOf("Double")===0 ? 80 : 31)) ],
					"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
					"clickable"       : { "faceUpOnly" : false },
					"moveOnClick"     : { "spot"                : "tableau",
								      	  "count"               : 1,
									  	  "reverseUpdateTarget" : true,
									  	  "moveOptions"         : { "turnFaceUp"        : true,
																	"moveSpeedFunction" : function() { return SOLITAIRE.CARD_DEAL_SPEED; } } }				
				},
				{
					"class"		      : "foundation",
					"x"			      : (gameID.indexOf("Triple")===0 ? A.pushSequence(Array(0), 1.0, 12.0) :
										(gameID.indexOf("Double")===0 ? A.pushSequence(Array(0), 1.0, 8.0) :
										   								A.pushSequence(Array(0), 3.0, 6.0))),
					"y"			      : (gameID.indexOf("Triple")===0 ? A.pushMany(Array(0), 0.0, 12) :
										(gameID.indexOf("Double")===0 ? A.pushMany(Array(0), 0.0, 8) :
										   								A.pushMany(Array(0), 0.0, 4))),
					"stackMax"		  : (gameID.indexOf("Triple")===0 ? A.pushMany(Array(0), 13, 12) :
										(gameID.indexOf("Double")===0 ? A.pushMany(Array(0), 13, 8) :
										   								A.pushMany(Array(0), 13, 4))),
					"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
					"base"	          : { "droppable" : true },
					"draggable"       : { },
					"droppable"       : { }
				},
				{
					"class"           : "tableau",
					"x"			      : (gameID.indexOf("Triple")===0 ? A.pushSequence(Array(0), 1.0, 12.0) :
										(gameID.indexOf("Double")===0 ? A.pushSequence(Array(0), 1.0, 8.0) :
										   								A.pushSequence(Array(0), 0.0, 6.0))),
					"y"			      : (gameID.indexOf("Triple")===0 ? A.pushMany(Array(0), 1.0, 12) :
										(gameID.indexOf("Double")===0 ? A.pushMany(Array(0), 1.0, 8) :
										   								A.pushMany(Array(0), 1.0, 7))),
					"stackMax"		  : (gameID.indexOf("Triple")===0 ? A.pushSequence(Array(0), 13, 36) :
										(gameID.indexOf("Double")===0 ? A.pushSequence(Array(0), 13, 36) :
										   								A.pushSequence(Array(0), 13, 36))),
					"stackTypes"      : [ { "direction" : "S", "type" : "peek" } ],
					"clickable"       : { "faceUpOnly" : false, "faceDownOnly" : true },
					"draggable"       : { "topCardOnly" : false, "askGame" : true },
					"droppable"       : { },
					"doubleclickable" : { },
					"autoFlip"        : { },
					"turnUpOnClick"   : true,
					"base"            : { "droppable" : true }
				}
			]
		};
	}
	else // Klondike
	{
		this._board =
		{
			"width"  : (gameID.indexOf("Triple")===0 ? 15 : (gameID.indexOf("Double")===0 ? 11 : 9)),
			"height" : 2,
			"spots"  :
			[
				{
					"class"           : "stock",
				 	"x"               : [ 0.0 ],
					"y"		          : [ 0.0 ],
					"stackMax"        : [ (gameID.indexOf("Triple")===0 ? 65 : (gameID.indexOf("Double")===0 ? 59 : 24)) ],
					"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
					"clickable"       : { "faceUpOnly" : false },
					"base"            : { "clickable"   : true,
										  "fillOnClick" :
										  { "spot"                : "waste",
										    "reverseUpdateSource" : true,
					 					    "moveOptions"         : { "turnFaceDown"      : true,
										  				              "skipTurnAnimation" : true,
															          "moveSpeedFunction" : function() { return SOLITAIRE.CARD_MOVE_SPEED; } } } },
					"moveOnClick"     : { "spot"                : "waste",
									      "count"               : (S.endswith(gameID, "Turn3") ? 3 : 1),
										  "moveOptions"         : { "turnFaceUp" : true, "skipTurnAnimation" : (S.endswith(gameID, "Turn3") ? true : false) },
									      "reverseUpdateTarget" : true }
				},
				{
					"class"		      : "waste",
					"x"			      : [ 1.0 ],
					"y"			      : [ 0.0 ],
					"sizeExempt"      : true,
					"stackMax"        : [ (gameID.indexOf("Triple")===0 ? 65 : (gameID.indexOf("Double")===0 ? 59 : 24)) ],
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
					"class"		      : "foundation",
					"x"			      : (gameID.indexOf("Triple")===0 ? A.pushSequence(Array(0), 3.0, 14.0) :
										(gameID.indexOf("Double")===0 ? A.pushSequence(Array(0), 3.0, 10.0) :
										   								  A.pushSequence(Array(0), 5.0, 8.0))),
					"y"			      : (gameID.indexOf("Triple")===0 ? A.pushMany(Array(0), 0.0, 12) :
										(gameID.indexOf("Double")===0 ? A.pushMany(Array(0), 0.0, 8) :
										   								  A.pushMany(Array(0), 0.0, 4))),
					"stackMax"		  : (gameID.indexOf("Triple")===0 ? A.pushMany(Array(0), 13, 12) :
										(gameID.indexOf("Double")===0 ? A.pushMany(Array(0), 13, 8) :
										   								  A.pushMany(Array(0), 13, 4))),
					"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
					"base"	          : { "droppable" : true },
					"draggable"       : { },
					"droppable"       : { }
				},
				{
					"class"           : "tableau",
					"x"			      : (gameID.indexOf("Triple")===0 ? A.pushSequence(Array(0), 2.0, 14.0) :
										  (gameID.indexOf("Double")===0 ? A.pushSequence(Array(0), 2.0, 10.0) :
										   								  A.pushSequence(Array(0), 2.0, 8.0))),
					"y"			      : (gameID.indexOf("Triple")===0 ? A.pushMany(Array(0), 1.0, 13) :
										  (gameID.indexOf("Double")===0 ? A.pushMany(Array(0), 1.0, 9) :
										   								  A.pushMany(Array(0), 1.0, 7))),
					"stackMax"		  : (gameID.indexOf("Triple")===0 ? A.pushSequence(Array(0), 13, 25) :
										  (gameID.indexOf("Double")===0 ? A.pushSequence(Array(0), 13, 21) :
										   								  A.pushSequence(Array(0), 13, 19))),
					"stackTypes"      : [ { "direction" : "S", "type" : "peek" } ],
					"clickable"       : { "faceUpOnly" : false, "faceDownOnly" : true },
					"draggable"       : { "topCardOnly" : false },
					"droppable"       : { },
					"doubleclickable" : { },
					"autoFlip"        : { },
					"turnUpOnClick"   : true,
					"base"            : { "droppable" : true }
				}
			]
		};
		
		if(S.endswith(gameID, "Turn1"))
			this._board["spots"][1]["stackTypes"].splice(0, 1);
	}
	
	this.runningCountArray = [];
	
	/**
	 * Will create our stock
	 */
	this.stockCreate = function()
	{
		return Stock.createStandard((gameID.indexOf("Triple")===0 ? 3 : (gameID.indexOf("Double")===0 ? 2 : 1)));
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
			this.runningCountArray = [];

		var options = {};
		options["turnFaceUp"] = false;
		options["moveSpeed"] = SOLITAIRE.CARD_DEAL_SPEED;
		
		if(stockIndex==((this._gameID.indexOf("Triple")===0 ? 155 : (this._gameID.indexOf("Double")===0 ? 103 : 51))))
			options["afterMove"] = function() { self.stockDistributeFinished(game); };
			
		if(this._gameID.indexOf("Easthaven")!=-1)
		{
			if((this._gameID.indexOf("Triple")===0 && stockIndex<36) ||
			   (this._gameID.indexOf("Double")===0 && stockIndex<24) ||
			   stockIndex<21)
			{
				game.moveCard(card, "tableau", Math.floor(stockIndex/3), options);
				if(((stockIndex+1)%3)===0)
					game.updateSpot(game.getSpot("tableau", Math.floor(stockIndex/3)));
			}
			else
			{
				game.moveCard(card, "stock", 0, options);
			}
		}
		else
		{
			var tableaus = game.getSpots("tableau");
			if(this.runningCountArray.length<=tableaus.length &&
			   (this.runningCountArray.length<2 ||
			    this.runningCountArray[this.runningCountArray.length-1]>this.runningCountArray[this.runningCountArray.length-2]))
			{
				this.runningCountArray.push(0);
				if(this.runningCountArray.length>1)
					game.updateSpot(game.getSpot("tableau", this.runningCountArray.length-2));
			}
			
			if(this.runningCountArray.length<=tableaus.length)
				this.runningCountArray[this.runningCountArray.length-1]++;
			
			if(this.runningCountArray.length>tableaus.length)
				game.moveCard(card, "stock", 0, options);
			else
				game.moveCard(card, "tableau", this.runningCountArray.length-1, options);
		}
		
		if(stockIndex==((this._gameID.indexOf("Triple")===0 ? 155 : (this._gameID.indexOf("Double")===0 ? 103 : 51))))
			game.updateSpot(game.getSpot("stock"));
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
			game.turnCardFaceUp(game.getSpot("tableau", i).cards[0], SOLITAIRE.CARD_DEAL_TURN_OVER_DELAY*i, i==(tableaus.length-1) ? { "afterFaceUp" : function() { game.stockDistributionFinished(); } } : null);
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
				if(this._gameID.indexOf("Easthaven")!=-1)
					return (cardDroppedOn.isBase && (this._gameID.indexOf("KingsOnly")==-1 || cardDragged.number===13)) ||
						   (cardDragged.number===(cardDroppedOn.number-1) &&
							((cardDragged.isBlack && cardDroppedOn.isRed) || (cardDragged.isRed && cardDroppedOn.isBlack)));
				else
					return (cardDroppedOn.isBase && cardDragged.number===13) ||
						   (cardDragged.number===(cardDroppedOn.number-1) &&
							((cardDragged.isBlack && cardDroppedOn.isRed) || (cardDragged.isRed && cardDroppedOn.isBlack)));
		}
		
		return false;
	};
	
	/**
	 * Internal function to determine if a card is live or not
	 * @param {Object} card
	 */
	this.isCardLive = function(card, type)
	{
		var spot = card.spot;
		var game = card.game;
		
		for(var i=card["stackIndex"];i>=0;i--)
		{
			if(i==card["stackIndex"])
				continue;
			
			if(!this.isCardDropValid(spot.cards[i], spot.cards[i+1]))
				return false;
		}
		
		return true;
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
			if(waste!=null && waste.cards.length>1)
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
		if(waste!=null && waste.cards.length>0 && this.tryPlayCard(waste.cards[0], obviousOnly, pretendOnly))
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
