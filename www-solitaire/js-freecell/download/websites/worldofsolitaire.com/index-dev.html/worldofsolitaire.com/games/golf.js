/* yukon.js - robert@cosmicrealms.com */

gGames["Golf"] = new golf("Golf");
gGames["GolfRelaxed"] = new golf("GolfRelaxed");
gGames["TriPeaks"] = new golf("TriPeaks");
gGames["TriPeaksStrict"] = new golf("TriPeaksStrict");

function golf(gameID)
{
	this._gameID = gameID;
	this._gameName = (gameID=="Golf" ? "Golf" :
					 (gameID=="TriPeaks" ? "Tri Peaks" :
					 (gameID=="TriPeaksStrict" ? "Tri Peaks Strict" :
	 			     (gameID=="GolfRelaxed" ? "Golf Relaxed" : "golf"))));
	
	this._board =
	{
		"width"  : (gameID=="TriPeaks" || gameID=="TriPeaksStrict" ? 10 : 7),
		"height" : (gameID=="TriPeaks" || gameID=="TriPeaksStrict" ? 4 : 2),
		"spots"  :
		[
			{
				"class"           : "stock",
			 	"x"               : [ 0.0 ],
				"y"		          : [ (gameID=="TriPeaks" || gameID=="TriPeaksStrict" ? 3.0 : 1.0) ],
				"stackMax"        : [  17 ],
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"clickable"       : { "faceUpOnly" : false },
				"moveOnClick"     : { "spot"                : "foundation",
								      "count"               : 1,
									  "moveOptions"         : { "turnFaceUp" : true },
								      "reverseUpdateTarget" : true }
			},
			{
				"class"		      : "foundation",
				"x"			      : [ 1.0 ],
				"y"			      : [ (gameID=="TriPeaks" || gameID=="TriPeaksStrict" ? 3.0 : 1.0) ],
				"stackMax"        : [  52 ],
				"sizeExempt"      : true,
				"reverseUpdate"   : true,
				"stackTypes"      :
				[ 
					{ "direction" : "E", "type" : "peek", "peekValue" : 0.50 }
				],
				"droppable"       : { }
			}
		]
	};
	
	if(gameID=="TriPeaks" || gameID=="TriPeaksStrict")
	{
		this._board["spots"].push(
			{
				"class"           : "tableau",
				"x"			      : [       1.5,           4.5,           7.5,
								          1.0, 2.0,      4.0, 5.0,      7.0, 8.0,
				                        0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5,
				                      0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0 ],
				"y"			      : [       0.0,           0.0,           0.0,
								          0.5, 0.5,      0.5, 0.5,      0.5, 0.5,
				                        1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
				                      1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5 ],
				"stackMax"	      : A.pushMany(Array(0), 1, 28),
				"startingZIndex"  : A.pushSequence(Array(0), 1, 28),
				"stackTypes"      : [ { "direction" : "", "type" : "deck" } ],
				"clickable"       : { "askGame" : true },
				"draggable"       : { "askGame" : true },
				"doubleclickable" : { "askGame" : true }
			});
	}
	else
	{
		this._board["spots"].push(
			{
				"class"           : "tableau",
				"x"			      : [ 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0 ],
				"y"			      : [ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 ],
				"stackMax"        : [   5,   5,   5,   5,   5,   5,   5 ],
				"stackTypes"      : [ { "direction" : "S", "type" : "peek", "extraSpacingPercentage" : 2.00 } ],
				"clickable"       : { },
				"draggable"       : { },
				"doubleclickable" : { }
			});
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
		options["turnFaceUp"] = false;
		options["moveSpeed"] = SOLITAIRE.CARD_DEAL_SPEED;
		
		if(this._gameID=="TriPeaks" || this._gameID=="TriPeaksStrict")
		{
			if(stockIndex<=27)
			{
				game.moveCard(card, "tableau", stockIndex, options);
				
				if(stockIndex===2)
				{
					for(var i=0;i<=2;i++) { game.updateSpot(game.getSpot("tableau", i)); }
				}
				else if(stockIndex==8)
				{
					for(var i=3;i<=8;i++) { game.updateSpot(game.getSpot("tableau", i)); }
				}
				else if(stockIndex==17)
				{
					for(var i=9;i<=17;i++) { game.updateSpot(game.getSpot("tableau", i)); }
				}
				else if(stockIndex==27)
				{
					for(var i=18;i<=27;i++) { game.updateSpot(game.getSpot("tableau", i)); }
				}
			}
			else if(stockIndex<=50)
			{
				game.moveCard(card, "stock", 0, options);
				if(stockIndex==50)
					game.updateSpot(game.getSpot("stock"));
			}
			else
			{
				options["afterMove"] = function() { self.stockDistributeFinished(game); };
	
				game.moveCard(card, "foundation", 0, options);
				game.updateSpot(game.getSpot("foundation"));
			}
		}
		else
		{
			if(stockIndex<=4)
			{
				game.moveCard(card, "tableau", 0, options);
				if(stockIndex==4)
					game.updateSpot(game.getSpot("tableau", 0));
			}
			else if(stockIndex<=9)
			{
				game.moveCard(card, "tableau", 1, options);
				if(stockIndex==9)
					game.updateSpot(game.getSpot("tableau", 1));
			}		
			else if(stockIndex<=14)
			{
				game.moveCard(card, "tableau", 2, options);
				if(stockIndex==14)
					game.updateSpot(game.getSpot("tableau", 2));
			}		
			else if(stockIndex<=19)
			{
				game.moveCard(card, "tableau", 3, options);
				if(stockIndex==19)
					game.updateSpot(game.getSpot("tableau", 3));
			}		
			else if(stockIndex<=24)
			{
				game.moveCard(card, "tableau", 4, options);
				if(stockIndex==24)
					game.updateSpot(game.getSpot("tableau", 4));
			}		
			else if(stockIndex<=29)
			{
				game.moveCard(card, "tableau", 5, options);
				if(stockIndex==29)
					game.updateSpot(game.getSpot("tableau", 5));
			}		
			else if(stockIndex<=34)
			{
				game.moveCard(card, "tableau", 6, options);
				if(stockIndex==34)
					game.updateSpot(game.getSpot("tableau", 6));
			}		
			else if(stockIndex<=50)
			{
				game.moveCard(card, "stock", 0, options);
				if(stockIndex==50)
					game.updateSpot(game.getSpot("stock"));
			}
			else
			{
				options["afterMove"] = function() { self.stockDistributeFinished(game); };
	
				game.moveCard(card, "foundation", 0, options);
				game.updateSpot(game.getSpot("foundation"));
			}
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
				game.turnCardFaceUp(tableau.cards[m], SOLITAIRE.CARD_DEAL_TURN_OVER_DELAY*(i*(this._gameID=="TriPeaks" || this._gameID=="TriPeaksStrict" ? 1 : 3)), (i==(tableaus.length-1) && m==(tableau.cards.length-1)) ? { "afterFaceUp" : function() { game.stockDistributionFinished(); } } : {});
			}
		}
		
		var foundation = game.getSpot("foundation");
		game.turnCardFaceUp(foundation.cards[0]);
	};
	
	/**
	 * This is optional. If present it should returns spots that need updated after a valid drop
	 * @param {Object} cardDragged
	 * @param {Object} cardDroppedOn
	 */
	this.getPostDropUpdateSpotList = function(cardDragged, cardDroppedOn)
	{
		var spotsToUpdate = [];
		
		if(this._gameID=="TriPeaks" || this._gameID=="TriPeaksStrict")
		{
			A.append(spotsToUpdate, this.getUpdateSpotList(cardDragged));
			A.append(spotsToUpdate, this.getUpdateSpotList(cardDroppedOn));
		}
		
		return spotsToUpdate;
	};
	
	/**
	 * Internal function to determine if a card is live or not
	 * @param {Object} card
	 */
	this.isCardLive = function(card, type)
	{
		if(this._gameID!="TriPeaks" && this._gameID!="TriPeaksStrict")
			return true;
			
		var spot = card.spot;
		var game = card.game;
		
		if(card._stackIndex>0)
			return false;
		
		if(spot["class"]=="stock" || spot["class"]=="foundation")
			return true;
		
		if(spot["spotYLoc"]==1.5)
			return true;
			
		var checkOffset = 1;
		if(spot["spotYLoc"]==0.0)
		{
			checkOffset += 2+spot["subIndex"];    // 0/+2  -  1/+3  -  2/+4
		}
		else if(spot["spotYLoc"]==0.5)
		{
			checkOffset = 4+Math.ceil((spot["subIndex"] / 2));
		}
		else if(spot["spotYLoc"]==1.0)
		{
			checkOffset = 9;
		}
		
		var coveringLeftSpot = game.getSpot("tableau", spot["subIndex"]+checkOffset);
		var coveringRightSpot = game.getSpot("tableau", spot["subIndex"]+checkOffset+1);
		if(coveringLeftSpot.cards.length>0 || coveringRightSpot.cards.length>0)
			return false;
		
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
	 * Internal work function to determine what spots to update based on card activity
	 * @param {Object} card
	 */
	this.getUpdateSpotList = function(card)
	{
		var spotsToUpdate = [];
		var spot = card.spot;
		var game = card.game;
		
		if(spot["class"]=="tableau" && spot["spotYLoc"]!==0.0)
		{
			var checkOffset = 9;
			if(spot["spotYLoc"]==0.5)
			{
				checkOffset = 1+Math.ceil((spot["subIndex"] / 2));	// 3/3  4/3   5/4  6/4   7/5  8/5
			}
			else if(spot["spotYLoc"]==1.0)
			{
				checkOffset = 2+Math.ceil((spot["subIndex"] / 3));	// 10/6   13/7    16/8
			}
			
			if(checkOffset>0)
			{
				var leftSpotCheck = game.getSpot("tableau", spot["subIndex"]-(checkOffset+1));
				var rightSpotCheck = game.getSpot("tableau", spot["subIndex"]-checkOffset);
				
				if(leftSpotCheck!==null && (spot["spotYLoc"]-leftSpotCheck["spotYLoc"])==0.5)
					spotsToUpdate.push(leftSpotCheck);
					
				if(rightSpotCheck!==null && (spot["spotYLoc"]-rightSpotCheck["spotYLoc"])==0.5)
					spotsToUpdate.push(rightSpotCheck);
			}
		}
		
		return spotsToUpdate;		
	};
	
	/**
	 * A card as been clicked, figure out what to do with it
	 * @param {Object} card
	 */
	this.cardClicked = function(card)
	{
		var game = card.game;
		
		if(this.tryPlayCard(card, true))
		{
			// Update our spots that this card was covering
			if(this._gameID=="TriPeaks" || this._gameID=="TriPeaksStrict")
			{
				var updateSpots = this.getUpdateSpotList(card);
				for(var i=0;i<updateSpots.length;i++)
				{
					game.updateSpot(updateSpots[i]);
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
			case("foundation"):
				if(this._gameID=="GolfRelaxed" || this._gameID=="TriPeaks")
					return ((cardDroppedOn.number+1===cardDragged.number) ||
						    (cardDroppedOn.number-1===cardDragged.number) ||
							(cardDroppedOn.number===1 && cardDragged.number===13) ||
							(cardDroppedOn.number===13 && cardDragged.number===1));
				else
					return ((cardDroppedOn.number+1===cardDragged.number) ||
						    (cardDroppedOn.number-1===cardDragged.number));
		}
		
		return false;
	};
		
	/**
	 * Called to determine if the game has been won or not
	 * @param {Object} game
	 */
	this.isGameWon = function(game)
	{
		var tableaus = game.getSpots("tableau");
		for(var i=0;i<tableaus.length;i++)
		{
			var tableau = tableaus[i];
			if(tableau.cards.length>0)
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
				
			var tableaus = game.getSpots("tableau");
			for(var i=0;i<tableaus.length;i++)
			{
				var tableau = tableaus[i];
				if(tableau.cards.length>1)
					return false;
			}
		}
		
		var tableaus = game.getSpots("tableau");
		for(var i=0;i<tableaus.length;i++)
		{
			var tableau = tableaus[i];
			if(tableau.cards.length<1)
				continue;
				
			var card = tableau.cards[0];
			if(!this.isCardLive(card, "draggable"))
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
		var foundation = game.getSpot("foundation");
		var targetCard = foundation.cards[0];
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
