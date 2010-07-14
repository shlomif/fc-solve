/* fan.js - robert@cosmicrealms.com */

gGames["LaBelleLucie"] = new fan("LaBelleLucie");
gGames["Trefoil"] = new fan("Trefoil");
gGames["TrefoilAndADraw"] = new fan("TrefoilAndADraw");
gGames["ThreeShufflesAndADraw"] = new fan("ThreeShufflesAndADraw");

function fan(gameID)
{
	this._gameID = gameID;
	this._gameName = (gameID=="LaBelleLucie" ? "La Belle Lucie" : 
					 (gameID=="Trefoil" ? "Trefoil" : 
					 (gameID=="TrefoilAndADraw" ? "Trefoil and a Draw" : 
	                 (gameID=="ThreeShufflesAndADraw" ? "Three Shuffles and a Draw" : "fan"))));
	
	this._board =
	{
		"width"  : 5,
		"height" : 5,
		"spots"  :
		[
			{
				"class"           : "stock",
			 	"x"               : [ 0.0 ],
				"y"		          : [ 0.0 ],
				"stackMax"        : [ 0 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"clickable"       : { },
				"base"            : { "clickable" : true, "clickLimit" : 2 }
			},
			{
				"class"		      : "foundation",
				"x"			      : [ 1.0, 2.0, 3.0, 4.0 ],
				"y"			      : [ 0.0, 0.0, 0.0, 0.0 ],
				"stackMax"        : [  13,  13,  13,  13 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"base"	          : { "droppable" : true },
				"draggable"       : { },
				"droppable"       : { }
			},
			{
				"class"           : "tableau",
				"x"			      : [ 0.0, 1.0, 2.0, 3.0, 4.0, 0.0, 1.0, 2.0, 3.0, 4.0, 0.0, 1.0, 2.0, 3.0, 4.0, 0.0, 1.0, 2.0 ],
				"y"			      : [ 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0, 2.0, 2.0, 2.0, 3.0, 3.0, 3.0, 3.0, 3.0, 4.0, 4.0, 4.0 ],
				"stackMax"        : [   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6 ],
				"actualStackMax"  : [  15,  15,  15,  15,  15,  15,  15,  15,  15,  15,  15,  15,  15,  15,  15,  15,  15,  15 ],
				"stackTypes"      : [ { "direction" : "E", "type" : "peek" } ],
				"draggable"       : { "topCardOnly" : true },
				"droppable"       : { },
				"doubleclickable" : { }
			}
		]
	};
	
	if(gameID=="ThreeShufflesAndADraw" || gameID=="TrefoilAndADraw")
	{
		this._board["spots"][2]["clickable"] = { "topCardOnly" : false, "askGame" : true };
	}
	
	this._aceDealCount = 0;
	this._drawCount = 0;

	/**
	 * This is called when the stock of cards is first distributed, once for each card in the stock
	 * @param {Object} card Which card is to be distributed
	 * @param {Object} stockIndex Where in the stock this card is
	 */
	this.stockDistribute = function(card, stockIndex)
	{
		var game = card.game;
		
		if(stockIndex===0)
		{
			this._aceDealCount = 0;
			this._drawCount = 0;
		}

		var options = {};
		options["moveSpeed"] = SOLITAIRE.CARD_DEAL_SPEED;
		
		if(stockIndex==51)
			options["afterMove"] = function() { game.stockDistributionFinished(); };
		
		game.turnCardFaceUp(card, -1);
		
		if((this._gameID=="Trefoil" || this._gameID=="TrefoilAndADraw") && card.number===1)
		{
			game.moveCard(card, "foundation", this._aceDealCount, options);
			game.updateSpot(game.getSpot("foundation", this._aceDealCount));
			
			this._aceDealCount++;
		}
		else
		{		
			var targetSpot = Math.floor(((stockIndex-this._aceDealCount)/3));
	
			game.moveCard(card, "tableau", targetSpot, options);
			
			if(((stockIndex-this._aceDealCount)+1)%3===0 || stockIndex==51)
				game.updateSpot(game.getSpot("tableau", targetSpot));
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
	 * Function to determine if a card is live or not
	 * @param {Object} card
	 */
	this.isCardLive = function(card, type)
	{
		var spot = card.spot;
		var game = card.game;
		
		if((this._gameID=="ThreeShufflesAndADraw" || this._gameID=="TrefoilAndADraw") && type=="clickable" && spot["class"]=="tableau")
		{
			if(card._stackIndex===0 || this._drawCount>0)
				return false;
				
			var stock = game.getSpot("stock");
			if(!stock.hasOwnProperty("timesBaseClicked") || stock["timesBaseClicked"]<2)
				return false;
				
			return true;
		}
		
		return false;
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

			var tableaus = game.getSpots("tableau");
			for(var i=0;i<tableaus.length;i++)
			{
				A.append(cardsToDistribute, tableaus[i].cards);
			}
			
			A.randomize(cardsToDistribute);
			
			var options = {};
			options["moveSpeed"] = SOLITAIRE.CARD_MOVE_SPEED/2;
			
			var index = 0;
			while(cardsToDistribute.length>0)
			{
				var cardToDistribute = cardsToDistribute.pop();
				
				var targetSpot = Math.floor((index/3));
				
				game.moveCard(cardToDistribute, "tableau", targetSpot, options);
				
				index++;
			}
			
			tableaus = game.getSpots("tableau");
			for(var i=0;i<tableaus.length;i++)
			{
				game.updateSpot(tableaus[i]);
			}
		}
		else if((this._gameID=="ThreeShufflesAndADraw" || this._gameID=="TrefoilAndADraw") && spot["class"]=="tableau")
		{
			game.swapCards(card, spot.cards[0]);
			
			this._drawCount++;
			
			var tableaus = game.getSpots("tableau");
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
		var spot = cardDroppedOn.spot;
		var spotClass = spot["class"];
		
		switch(spotClass)		
		{
			case("foundation"):
				return cardDragged.stackIndex===0 &&
				       ((cardDroppedOn.isBase && cardDragged.number===1) ||
					    (cardDragged.number===(cardDroppedOn.number+1) && cardDragged.suit==cardDroppedOn.suit));
			case("tableau"):
				return (cardDragged.number===(cardDroppedOn.number-1) && cardDragged.suit==cardDroppedOn.suit);
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
			// Check to see if any cards in the tableau above are on top of lower ones below??
			var tableaus = game.getSpots("tableau");
			for(var i=0;i<tableaus.length;i++)
			{
				var tableau = tableaus[i];
				var lastCardNum = 0;
				for(var m=0;m<tableau.cards.length;m++)
				{
					if(tableau.cards[m].number<lastCardNum)
						return false;
					lastCardNum = tableau.cards[m].number;
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
