/* freecell.js - robert@cosmicrealms.com */

gGames["FreeCell"] = new freeCell("FreeCell");
gGames["BakersGame"] = new freeCell("BakersGame");
gGames["BakersGameKingsOnly"] = new freeCell("BakersGameKingsOnly");
gGames["EightOff"] = new freeCell("EightOff");
gGames["SeahavenTowers"] = new freeCell("SeahavenTowers");

function freeCell(gameID)
{
	this._gameID = gameID;
	this._gameName = (gameID=="FreeCell" ? "FreeCell" : 
	 				 (gameID=="BakersGame" ? "Baker's Game" :
					 (gameID=="BakersGameKingsOnly" ? "Baker's Game (Kings Only)" :
					 (gameID=="EightOff" ? "Eight Off" :
	                 (gameID=="SeahavenTowers" ? "Seahaven Towers" : "freecell")))));
					 
	this._board =
	{
		"width"  : (gameID=="SeahavenTowers" ? 11 : 9),
		"height" : 2,
		"spots"  :
		[
			{
				"class"		      : "cell",
				"x"			      : (gameID=="SeahavenTowers" ? [ 3.0, 4.33, 5.66, 7.0 ] :
									(gameID=="EightOff" ? A.pushSequence(Array(0), 0.0, 7.0)
														: A.pushSequence(Array(0), 0.0, 3.0))),
				"y"			      : gameID=="EightOff" ? A.pushMany(Array(0), 0.0, 8) : A.pushMany(Array(0), 0.0, 4),
				"stackMax"        : gameID=="EightOff" ? A.pushMany(Array(0),   1, 8) : A.pushMany(Array(0),   1, 4),
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"draggable"       : { },
				"droppable"       : { },
				"doubleclickable" : { },
				"base"            : { "droppable" : true }
			},
			{
				"class"		      : "foundation",
				"x"			      : (gameID=="SeahavenTowers" ? [ 0.0, 1.0, 9.0, 10.0 ] :
									(gameID=="EightOff" ? A.pushMany(Array(0), 8.0, 4)
														: A.pushSequence(Array(0), 5.0, 8.0))),
				"y"			      : gameID=="EightOff" ? [ 0.0, 1.06, 1.38, 1.71 ] : A.pushMany(Array(0), 0.0, 4),
				"stackMax"        : A.pushMany(Array(0), 13, 4),
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"draggable"       : { },
				"droppable"       : { },
				"base"            : { "droppable" : true }
			},
			{
				"class"           : "tableau",
				"x"			      : (gameID=="SeahavenTowers" ? [ 0.0, 1.12, 2.24, 3.36, 4.48, 5.52, 6.64, 7.76, 8.88, 10.0 ] :
									(gameID=="EightOff" ? A.pushSequence(Array(0), 0.0, 7.0)
														: [ 0.0, 1.16, 2.32, 3.48, 4.52, 5.68, 6.84, 8.00 ])),
				"y"			      : [ 1.0, 1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0 ],
				"stackMax"        : [  28,  28,   28,   28,   28,   28,   28,   28 ],
				"stackTypes"      : [ { "direction" : "S", "type" : "peek" } ],
				"draggable"       : { "topCardOnly" : false, "askGame" : true },
				"droppable"       : { },
				"doubleclickable" : { },
				"base"            : { "droppable" : true },
				"reverseUpdate"   : true
			}
		]
	};

	if(gameID=="SeahavenTowers")
	{
		this._board["spots"][2]["y"].push(1.0);
		this._board["spots"][2]["y"].push(1.0);
		this._board["spots"][2]["stackMax"].push(28);
		this._board["spots"][2]["stackMax"].push(28);
	}
		
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
		options["turnFaceUp"] = true;
		options["skipTurnAnimation"] = true;
		options["moveSpeed"] = SOLITAIRE.CARD_DEAL_SPEED;
		
		if(this._gameID=="EightOff" && stockIndex>=48)
		{
			game.moveCard(card, "cell", (stockIndex-48), options);
		}
		else if(this._gameID=="SeahavenTowers" && stockIndex>=50)
		{
			game.moveCard(card, "cell", (stockIndex-49), options);
		}
		else
		{
			game.moveCard(card, "tableau", stockIndex%(this._gameID=="SeahavenTowers" ? 10 : 8), options);
		}

		if(stockIndex==51)
		{
			var tableaus = game.getSpots("tableau");
			for(var i=0;i<tableaus.length;i++)
			{
				game.updateSpot(tableaus[i]);
			}
			
			if(this._gameID=="SeahavenTowers")
			{
				game.updateSpot(game.getSpot("cell", 1));
				game.updateSpot(game.getSpot("cell", 2));		
			}
			else if(this._gameID=="EightOff")
			{
				var cells = game.getSpots("cell");
				for(var i=0;i<cells.length;i++)
				{
					game.updateSpot(cells[i]);
				}
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
		var game = cardDragged.game;
		
		switch(spotClass)		
		{
			case("foundation"):
				return cardDragged.stackIndex===0 &&
				       ((cardDroppedOn.isBase && cardDragged.number===1) ||
					    (cardDragged.number===(cardDroppedOn.number+1) && cardDragged.suit==cardDroppedOn.suit));
			case("tableau"):
				var numFreeSpots = 1;
				var freeSpots = game.getSpots("cell");
				if(this._gameID!="SeahavenTowers" && this._gameID!="BakersGameKingsOnly" && this._gameID!="EightOff")
					A.append(freeSpots, game.getSpots("tableau"));
				for(var i=0;i<freeSpots.length;i++)
				{
					if(freeSpots[i].cards.length===0)
						numFreeSpots++;
				}
				
				if((this._gameID=="SeahavenTowers" || this._gameID=="BakersGameKingsOnly" || this._gameID=="EightOff") &&
				   cardDragged.number==13)
					numFreeSpots++;
				
				var isValidStack = true;
				for(var i=cardDragged["stackIndex"];i>=0;i--)
				{
					if(i==cardDragged["stackIndex"])
						continue;
					
					if(!this.isCardDropValid(cardDragged["spot"].cards[i], cardDragged["spot"].cards[i+1]))
						isValidStack = false;
				}
			
				if(this._gameID=="SeahavenTowers" || this._gameID=="BakersGameKingsOnly" || this._gameID=="EightOff")
					return (cardDroppedOn.isBase && spotClass==="tableau" && cardDragged["stackIndex"]===cardDragged["spot"].cards.length-1 && cardDragged["spot"]["class"]=="tableau") ||
					       (cardDroppedOn.isBase && cardDragged.number==13 && (cardDragged["stackIndex"]===0 || ((cardDragged["stackIndex"]+1)<numFreeSpots))) ||
						   ((cardDragged.number===(cardDroppedOn.number-1) && cardDragged.suit==cardDroppedOn.suit && ((cardDragged["stackIndex"]+1)<=numFreeSpots) && isValidStack));
				else if(this._gameID=="BakersGame")
					return (cardDroppedOn.isBase && spotClass==="tableau" && cardDragged["stackIndex"]===cardDragged["spot"].cards.length-1) ||
						   (cardDroppedOn.isBase && (cardDragged["stackIndex"]===0 || ((cardDragged["stackIndex"]+1)<numFreeSpots))) ||
						   (cardDragged.number===(cardDroppedOn.number-1) &&
							(cardDragged.suit===cardDroppedOn.suit) &&
							((cardDragged["stackIndex"]+1)<=numFreeSpots) && isValidStack);
				else
					return (cardDroppedOn.isBase && spotClass==="tableau" && cardDragged["stackIndex"]===cardDragged["spot"].cards.length-1) ||
						   (cardDroppedOn.isBase && (cardDragged["stackIndex"]===0 || ((cardDragged["stackIndex"]+1)<numFreeSpots))) ||
						   (cardDragged.number===(cardDroppedOn.number-1) &&
							((cardDragged.isBlack && cardDroppedOn.isRed) || (cardDragged.isRed && cardDroppedOn.isBlack)) &&
							((cardDragged["stackIndex"]+1)<=numFreeSpots) && isValidStack);
			case("cell"):
				return cardDragged.stackIndex===0 && cardDroppedOn.isBase;
		}
		
		return false;
	};
	
	/**
	 * This is optional. If present it will be called after a valid drop
	 * @param {Object} game
	 */
	this.postDropHandler = function(game)
	{
		var tableaus = game.getSpots("tableau");
		for(var i=0;i<tableaus.length;i++)
		{
			game.updateSpotCardHandlers(tableaus[i]);
		}
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
		
		var tableauIsFree = false;
		var numFreeSpots = 1;
		var freeSpots = game.getSpots("cell");
		if(this._gameID!="SeahavenTowers" && this._gameID!="BakersGameKingsOnly" && this._gameID!="EightOff")
			A.append(freeSpots, game.getSpots("tableau"));
		for(var i=0;i<freeSpots.length;i++)
		{
			if(freeSpots[i].cards.length===0)
			{
				numFreeSpots++;
				if(freeSpots[i]["class"]==="tableau")
					tableauIsFree = true;
			}
		}
		
		tableauIsFree = false;
		var tableaus = game.getSpots("tableau");
		for(var i=0;i<tableaus.length;i++)
		{
			if(tableaus[i].cards.length===0)
				tableauIsFree = true;
		}
		
		if(card["stackIndex"]==spot.cards.length-1 && tableauIsFree)
			return true;
		
		for(var i=card["stackIndex"];i>=0;i--)
		{
			if(i==card["stackIndex"])
				continue;
			
			if(!this.isCardDropValid(spot.cards[i], spot.cards[i+1]))
				return false;
		}
				
		if(card["stackIndex"]!==0 && (card["stackIndex"]+1)>numFreeSpots)
			return false;
		
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
		
		var spots = game.getSpots("tableau");
		A.append(spots, game.getSpots("cell"));
		
		for(var i=0;i<spots.length;i++)
		{
			var spot = spots[i];
			if(spot.cards.length<1)
				continue;
				
			var card = spot.cards[0];
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
