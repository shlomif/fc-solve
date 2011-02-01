/* klondike.js - robert@cosmicrealms.com */

gGames["Montana"] = new montana("Montana");

function montana(gameID)
{
	this._gameID = gameID;
	this._gameName = (gameID=="Montana" ? "Montana" : "montana");
	
	this._board =
	{
		"width"  : 13,
		"height" : 5,
		
		"spots"  :
		[
			{
				"class"           : "stock",
			 	"x"               : [ 6.0 ],
				"y"		          : [ 4.0 ],
				"stackMax"        : [ 0 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"clickable"       : { },
				"base"            : { "clickable" : true, "clickLimit" : 2 }
			},
			{
				"class"           : "tableau",
				"x"			      : A.append(A.append(A.append(
											 A.pushSequence(Array(0), 0.0, 12.0),
											 A.pushSequence(Array(0), 0.0, 12.0)),
											 A.pushSequence(Array(0), 0.0, 12.0)),
											 A.pushSequence(Array(0), 0.0, 12.0)),
				"y"			      : A.append(A.append(A.append(
											 A.pushMany(Array(0), 0.0, 13),
											 A.pushMany(Array(0), 1.0, 13)),
											 A.pushMany(Array(0), 2.0, 13)),
											 A.pushMany(Array(0), 3.0, 13)),
				"stackMax"	      : A.pushMany(Array(0), 2, 52),
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"draggable"		  : { },
				"droppable"       : { },
				"base"            : { "droppable" : true, "clear" : true }
			}
		]
	};
	
	/**
	 * Rename this to stockCreate to deal a winnable set of cards
	 */
	this.stockCreateWin = function()
	{
		var suits = ["d", "s", "h", "c"];
		var stock = [];
		for(var s=0;s<4;s++)
		{
			for(var i=1;i<=13;i++)
			{
				stock.push(new Card("" + i + suits[s]));
			}
		}
		
		return stock;
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
		game.turnCardFaceUp(card, -1);
		
		if(stockIndex==51)
			options["afterMove"] = function() { self.stockDistributeFinished(game); };
		
		game.moveCard(card, "tableau", stockIndex, options);
		game.updateSpot(game.getSpot("tableau", stockIndex));
	};
	
	/**
	 * This is called after the stock has been fully distributed
	 * @param {Object} game
	 */
	this.stockDistributeFinished = function(game)
	{
		// Remove our aces
		var options = {};
		options["moveSpeed"] = SOLITAIRE.CARD_DEAL_SPEED;
		
		var tableaus = game.getSpots("tableau");
		var acesMoved = 0;
		for(var i=0;i<tableaus.length;i++)
		{
			var card = tableaus[i].cards[0];
			if(card.number===1)
			{
				acesMoved++;
				if(acesMoved==4)
					options["afterMove"] = function() { game.stockDistributionFinished(); };
					
				game.moveCard(card, "offscreen", 0, options);
			}
		}

		game.updateSpot(game.getSpot("offscreen"));		
	};
		
	/**
	 * This is called by game whenever a card that is clickable is clicked
	 * @param {Object} card Which card was clicked
	 */
	this.cardClicked = function(card)
	{
		var game = card.game;
		var spot = card.spot;
		
		if(spot["class"]=="stock")
		{
			var cardsToDistribute = [];
			var runningCards = [];

			var tableaus = game.getSpots("tableau");
			for(var i=0;i<tableaus.length;i++)
			{
				var tableau = tableaus[i];
				if(tableau.cards.length===0)
				{
					runningCards = [];
					continue;
				}
				
				var currentCard = tableau.cards[0];
				if((i%13)===0)
				{
					runningCards = [];
					if(currentCard.number===2)
					{
						runningCards.push(currentCard);
						continue;
					}
				}
				
				if(runningCards.length<1)
				{
					cardsToDistribute.push(currentCard);
					continue;
				}
								
				if(currentCard.suit!=runningCards[(runningCards.length-1)].suit ||
				   (runningCards[(runningCards.length-1)].number+1)!=currentCard.number)
				{
					runningCards = [];
					cardsToDistribute.push(currentCard);
				}
				else
				{
					runningCards.push(currentCard);
				}
			}
			
			A.randomize(cardsToDistribute);
			
			var options = {};
			options["moveSpeed"] = SOLITAIRE.CARD_MOVE_SPEED/2;
			
			var hasMovedOne = false;
			runningCards = [];
			for(var i=0;i<tableaus.length;i++)
			{
				var tableau = tableaus[i];
				
				var currentCard = tableau.cards.length>0 ? tableau.cards[0] : null;
				if((i%13)===0)
				{
					runningCards = [];
					if(currentCard!==null && currentCard.number===2)
						runningCards.push(currentCard);
						
					continue;
				}
				
				if(currentCard!==null && runningCards.length>0 &&
				   currentCard.suit==runningCards[(runningCards.length-1)].suit &&
				   (runningCards[(runningCards.length-1)].number+1)==currentCard.number)
				{
					runningCards.push(currentCard);
					continue;
				}
				
				if(runningCards.length>0)
				{
					runningCards = [];
					continue;
				}
				
				if(hasMovedOne)
				{
					options["undoAppendToLastMove"] = true;
					options["undoCallWhenUndone"] = null;
				}
				else
				{
					options["undoAppendToLastMove"] = false;
					options["undoCallWhenUndone"] = function() { spot["timesBaseClicked"]--; game.renderRightBaseImage(spot); };
				}
				
				hasMovedOne = true;
				
				var distributeCard = cardsToDistribute.pop();
				game.moveCard(distributeCard, "tableau", i, options);
			}
			
			for(var i=0;i<tableaus.length;i++)
			{
				game.updateSpot(tableaus[i]);
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
		var game = cardDragged.game;
		
		if(cardDragged==cardDroppedOn || !cardDroppedOn.isBase)
			return false;
		
		var subIndex = cardDroppedOn.spot["subIndex"];
		if((subIndex%13)===0)
		{
			if(cardDragged.number===2)
				return true;
			
			return false;
		}
		
		var leftSpot = game.getSpot("tableau", (subIndex-1));
		if(leftSpot.cards.length===0)
			return false;
			
		var leftCard = leftSpot.cards[0];
			
		if(leftCard.suit==cardDragged.suit && leftCard.number+1==cardDragged.number)
			return true;
		
		return false;
	};
	
	/**
	 * Called to determine if the game has been won or not
	 * @param {Object} game
	 */
	this.isGameWon = function(game)
	{
		var tableaus = game.getSpots("tableau");
		var lastCard = null;
		for(var i=0;i<tableaus.length;i++)
		{
			var tableau = tableaus[i];
			
			if(tableau.cards.length===0)
			{
				if(((i+1)%13)!==0)
					return false;

				lastCard = null;
				continue;
			}
			
			if(((i%13)+2)!=tableau.cards[0].number)
				return false;

			if(i===0)
			{
				lastCard = tableau.cards[0];				
				continue;
			}

			if(lastCard!==null && lastCard.suit!=tableau.cards[0].suit)
				return false;
				
			lastCard = tableau.cards[0];
		}
		
		return true;
	};
}
