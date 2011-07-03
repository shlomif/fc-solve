/* Card.js - robert@cosmicrealms.com */

function Card(cardID)
{
	// public
	this.id = cardID;
	this.name = CARD.name(this.id);
	this.number = CARD.number(this.id);
	this.suit = CARD.suit(this.id);
	this.spot = null;
	this.isRed = CARD.isRed(this.id);
	this.isBlack = CARD.isBlack(this.id);
	this.isJoker = CARD.isJoker(this.id);
	this.isBase = this.id.indexOf("base")===0;
	this.finalZIndex = 0;
	this.moveOptions = {};

	// private	
	this._domCard = null;
	this._deck = null;
	this._deckSize = 0;
	this._x = null;
	this._y = null;
	this._stackIndex = null;
	this._scale = 1.00;
	this._cardHeight = null;
	this._cardWidth = null;
	this._isFaceUp = false;		// Or will be face up once animation is complete
	this._dragDrop = null;
	this._dragStartHandler = null;
	this._dragHandler = null;
	this._dragStopHandler = null;
	this._droppedHandler = null;
	this._mouseOverHandler = null;
	this._mouseOutHandler = null;
	this._dragSpace = document.body;
	this._doFlip = false;
	
	/**
	 * Creates the actual card including appending it to the DOM
	 * @param {Object} deck Which deck to use to create the card
	 */
	this.createCard = function(deck, deckSize, offscreenCoordinates)
	{
		var self = this;
		
		if(typeof offscreenCoordinates=="undefined" || offscreenCoordinates===null)
			offscreenCoordinates = [-50, -50];
		
		this._deck = deck;
		this._deckSize = deckSize;
		this._cardHeight = Math.floor(this._deck.getCardHeight(this._deckSize, this._scale));
		this._cardWidth = Math.floor(this._deck.getCardWidth(this._deckSize, this._scale));
		
		if(this._deck._hasFlip===true && I.random(0, 1)===1)
			this._doFlip = true;
		
		var domCard = document.createElement("IMG");
		domCard.src = this._deck.createCardURL(!this.isBase ? "back" : this.id, this._deckSize, this._doFlip);
		domCard.alt = CARD.name(!this.isBase ? "back" : this.id);
		domCard.title = "";
		domCard.className = "card";
		domCard.style.visibility = "visible";
		domCard.style.top = "" + (((offscreenCoordinates[1]<0) ? (this._cardHeight)*-1 : this._cardHeight) + offscreenCoordinates[1]) + "px";
		domCard.style.left = "" + (((offscreenCoordinates[0]<0) ? (this._cardWidth)*-1 : this._cardWidth) + offscreenCoordinates[0]) + "px";
		domCard.style.width = "" + this._cardWidth + "px";
		domCard.style.height = "" + this._cardHeight + "px";
		domCard.style.zIndex = 0;
		
		var rightNow = new Date();
		domCard.id = "card" + CARD.cardsCreatedCount;
		
		document.body.appendChild(domCard);
		
		this._domCard = domCard;
		
		this._dragDrop = new YAHOO.util.DD(domCard.id, "enabled");
		this._dragDrop.startDrag = function(x, y) { self.dragStart(x, y); };
		this._dragDrop.onDrag = function(e) { self.drag(e); };
		this._dragDrop.onMouseUp = function(e) { self.dragStop(e); };
		this._dragDrop.onDragDrop = function(e, DDArray) { self.dropped(e, DDArray); };
		this.disableDragging();
		this.disableDropping();
		
		E.add(domCard, "mouseover", function(e) { self.mouseOver(e); });
		E.add(domCard, "mouseout", function(e) { self.mouseOut(e); });
		
		CARD.cardsCreated[this._domCard.id] = this;
		CARD.cardsCreatedCount++;
	};
	
	/**
	 * Called when card drag starts
	 * @param {Object} x The X point where the mousedown event was
	 * @param {Object} y The Y point where the mousedown event was
	 */
	this.dragStart = function(x, y)
	{			
		var self = this;
		
		var cardXY = D.xy(this._domCard);
		var cardHeight = D.height(this._domCard);
		var cardWidth = D.width(this._domCard);
		var dragSpaceXY = D.xy(this._dragSpace);
		var dragSpaceHeight = D.height(this._dragSpace);
		var dragSpaceWidth = D.width(this._dragSpace);
				
		this._dragDrop.resetConstraints();
		this._dragDrop.setXConstraint(cardXY[0]-dragSpaceXY[0], ((dragSpaceXY[0]+dragSpaceWidth)-cardXY[0])-cardWidth);
		this._dragDrop.setYConstraint(cardXY[1]-dragSpaceXY[1], ((dragSpaceXY[1]+dragSpaceHeight)-cardXY[1])-cardHeight);

		if(this._dragStartHandler!==null)
			this._dragStartHandler(self);		
	};
	
	/**
	 * Called while draggin
	 * @param {Object} e The mousemove event
	 */
	this.drag = function(e)
	{
		var self = this;
		
		if(this._dragHandler!==null)
			this._dragHandler(self);		
	};
	
	/**
	 * Called when dragging has finished
	 * @param {Object} e The mouseup event
	 */
	this.dragStop = function(e)
	{
		var self = this;
		
		if(this._dragStopHandler!==null)
			this._dragStopHandler(self);
	};
	
	/**
	 * Called when user mouses over this card
	 * @param {Object} e
	 */
	this.mouseOver = function(e)
	{
		var self = this;

		if(this._mouseOverHandler!==null)
			this._mouseOverHandler(self);
	};
	
	/**
	 * Called when user mouses out this card
	 * @param {Object} e
	 */
	this.mouseOut = function(e)
	{
		var self = this;
		
		if(this._mouseOutHandler!==null)
			this._mouseOutHandler(self);
	};
		
	/**
	 * Called when the card being dragged is dropped
	 * @param {Object} e The mouseup event
	 * @param {Object} potentials An array of objects that the card being dragged touches
	 */
	this.dropped = function(e, potentials)
	{
		potentials.sort(function(a, b)
					    {
							if(!a.overlap && b.overlap)
								return 1;
							if(a.overlap && !b.overlap)
								return -1;
							if(!a.overlap && !b.overlap)
								return 0;
							if(a.overlap.getArea() < b.overlap.getArea())
								return 1;
							if(a.overlap.getArea() > b.overlap.getArea())
								return -1;

							return 0;
						});

		for(var i=0;i<potentials.length;i++)
		{
			var potential = potentials[i];
			var potentialCardDom = potential.getEl();
			
			if(this._droppedHandler!==null)
			{
				if(this._droppedHandler(this, CARD.cardsCreated[potentialCardDom.id]))
					break;
			}
		}
	};
	
	/**
	 * Updates a card with a new deck, size and scale
	 * @param {Object} deck
	 * @param {Object} deckSize
	 * @param {Object} scale
	 */
	this.updateCard = function(deck, deckSize, scale)
	{
		if(typeof deck=="undefined" || deck===null)
			deck = this._deck;
		this._deck = deck;
		
		if(typeof deckSize=="undefined" || deckSize===null)
			deckSize = this._deckSize;
		this._deckSize = deckSize;

		if(typeof scale=="undefined" || scale===null)
			scale = this._scale;
		this._scale = scale;

		this._cardHeight = Math.floor(this._deck.getCardHeight(this._deckSize, this._scale));
		this._cardWidth = Math.floor(this._deck.getCardWidth(this._deckSize, this._scale));
		
		this._domCard.style.width = "" + this._cardWidth + "px";
		this._domCard.style.height = "" + this._cardHeight + "px";
		
		this._domCard.src = this._deck.createCardURL((!this._isFaceUp && !this.isBase) ? "back": this.id, this._deckSize, this._doFlip);
		this._domCard.alt = CARD.name((!this._isFaceUp && !this.isBase) ? "back": this.id);
	};
		
	/**
	 * Sets a percentage on how much to reduce or enlarge a card by
	 * @param {Object} scale Percentage to scale the card by
	 */
	this.setScale = function(scale)
	{
		this._scale = scale;
		
		this._cardHeight = Math.floor(this._deck.getCardHeight(this._deckSize, this._scale));
		this._cardWidth = Math.floor(this._deck.getCardWidth(this._deckSize, this._scale));
		
		this._domCard.style.width = "" + this._cardWidth + "px";
		this._domCard.style.height = "" + this._cardHeight + "px";
		
		return this;
	};
	
	/**
	 * This function will turn a card face up
	 */
	this.turnFaceUp = function(delay, options)
	{
		var self = this;
		
		this._isFaceUp = true;
		
		if(typeof delay=="undefined" || delay===null)
			delay = 0;
		
		if(typeof options=="undefined" || options===null)
			options = {};
			
		var selfOptions = options;
					
		if(delay==-1)
		{
			self._domCard.src = self._deck.createCardURL(self.id, self._deckSize, this._doFlip);
			self._domCard.alt = self.name;
			if(selfOptions.hasOwnProperty("afterFaceUp"))
				selfOptions["afterFaceUp"]();
		}
		else if(delay>0)
		{
			setTimeout(function() { self.turnFaceUp(0, selfOptions); }, delay);
		}
		else
		{
			var liftSizeX = parseInt((this._cardWidth*0.08), 10);
			var liftSizeY = parseInt((this._cardHeight*0.08), 10);
	
			var growAttributes = {  width : { by : liftSizeX },
			                       height : { by : liftSizeY } };
			var growAnimation = new YAHOO.util.Anim(this._domCard, growAttributes, SOLITAIRE.CARD_TURN_OVER_SPEED);
			growAnimation.onComplete.subscribe(
				function()
				{
					var oCardTwo = self;
					var oSelfOptionsTwo = selfOptions;
					
					var squeezeAttributes = { width : { to : 0 } };
					var squeezeAnimation = new YAHOO.util.Anim(self._domCard, squeezeAttributes, SOLITAIRE.CARD_TURN_OVER_SPEED);
					squeezeAnimation.onComplete.subscribe(
						function()
						{
							var oCardThree = oCardTwo;
							var oSelfOptionsThree = oSelfOptionsTwo;
							
							oCardTwo._domCard.src = oCardTwo._deck.createCardURL(oCardTwo.id, oCardTwo._deckSize, this._doFlip);
							oCardTwo._domCard.alt = oCardTwo.name;
							
							var expandAttributes = { width : { to : oCardTwo._cardWidth+liftSizeX } };
							var expandAnimation = new YAHOO.util.Anim(oCardTwo._domCard, expandAttributes, SOLITAIRE.CARD_TURN_OVER_SPEED);
							expandAnimation.onComplete.subscribe(
								function()
								{
									var oSelfOptionsFour = oSelfOptionsThree;
									var shrinkAttributes = { width : { to : oCardThree._cardWidth },
									                        height : { to : oCardThree._cardHeight } };
									var shrinkAnimation = new YAHOO.util.Anim(oCardThree._domCard, shrinkAttributes, SOLITAIRE.CARD_TURN_OVER_SPEED);
									if(oSelfOptionsFour.hasOwnProperty("afterFaceUp"))
										shrinkAnimation.onComplete.subscribe(function() { oSelfOptionsFour["afterFaceUp"](); });
									shrinkAnimation.animate();
								});
								
							expandAnimation.animate();
						});
						
					squeezeAnimation.animate();
				});
				
			growAnimation.animate();
		}
		
		return this;
	};
	
	/**
	 * This function will turn a card face down
	 */
	this.turnFaceDown = function(delay, options)
	{
		var self = this;
		
		this._isFaceUp = false;
		
		if(typeof delay=="undefined" || delay===null)
			delay = 0;

		if(typeof options=="undefined" || options===null)
			options = {};
		
		var selfOptions = options;
			
		if(delay==-1)
		{
			self._domCard.src = this._deck.createCardURL("back", this._deckSize, this._doFlip);
			self._domCard.alt = "back";
			if(selfOptions.hasOwnProperty("afterFaceDown"))
				selfOptions["afterFaceDown"]();
		}
		else if(delay>0)
		{
			setTimeout(function() { self.turnFaceUp(0, selfOptions); }, delay);
		}
		else
		{
			var liftSizeX = parseInt((this._cardWidth*0.08), 10);
			var liftSizeY = parseInt((this._cardHeight*0.08), 10);
	
			var growAttributes = {  width : { by : liftSizeX },
			                       height : { by : liftSizeY } };
			var growAnimation = new YAHOO.util.Anim(this._domCard, growAttributes, SOLITAIRE.CARD_TURN_OVER_SPEED);
			growAnimation.onComplete.subscribe(
				function()
				{
					var oCardTwo = self;
					var oSelfOptionsTwo = selfOptions;
					
					var squeezeAttributes = { width : { to : 0 } };
					var squeezeAnimation = new YAHOO.util.Anim(self._domCard, squeezeAttributes, SOLITAIRE.CARD_TURN_OVER_SPEED);
					squeezeAnimation.onComplete.subscribe(
						function()
						{
							var oCardThree = oCardTwo;
							var oSelfOptionsThree = oSelfOptionsTwo;
							
							oCardTwo._domCard.src = oCardTwo._deck.createCardURL("back", oCardTwo._deckSize, this._doFlip);
							oCardTwo._domCard.alt = "back";
							
							var expandAttributes = { width : { to : oCardTwo._cardWidth+liftSizeX } };
							var expandAnimation = new YAHOO.util.Anim(oCardTwo._domCard, expandAttributes, SOLITAIRE.CARD_TURN_OVER_SPEED);
							expandAnimation.onComplete.subscribe(
								function()
								{
									var oSelfOptionsFour = oSelfOptionsThree;
									var shrinkAttributes = { width : { to : oCardThree._cardWidth },
									                        height : { to : oCardThree._cardHeight } };
									var shrinkAnimation = new YAHOO.util.Anim(oCardThree._domCard, shrinkAttributes, SOLITAIRE.CARD_TURN_OVER_SPEED);
									if(oSelfOptionsFour.hasOwnProperty("afterFaceDown"))
										shrinkAnimation.onComplete.subscribe(function() { oSelfOptionsFour["afterFaceDown"](); });
									shrinkAnimation.animate();
								});
								
							expandAnimation.animate();
						});
						
					squeezeAnimation.animate();
				});
				
			growAnimation.animate();
		}		
		
		return this;
	};
	
	/**
	 * Returns true if the card is face up (or will be soon)
	 */
	this.isFaceUp = function()
	{
		return this._isFaceUp;
	};
	
	/**
	 * Returns true if the card is currently, right now, as we speak, face up or not
	 */
	this.isCurrentlyFaceUp = function()
	{
		return (this._domCard.src.indexOf("back")===-1);
	};
	
	/**
	 * This function sets the zIndex of the DOM card
	 * @param {Object} zIndex What zIndex to set the card to
	 */
	this.setZIndex = function(zIndex)
	{
		if(typeof zIndex=="undefined" || zIndex===null || isNaN(zIndex))
			zIndex = this.finalZIndex;

		if(!this.hasOwnProperty("isDragging") || this.isDragging!==true)
			this._domCard.style.zIndex = zIndex;
	};
	
	/**
	 * This will move a card to somewhere on the screen
	 * @param {Object} screenX The screen X coordinate on where to move the card to
	 * @param {Object} screenY The screen Y coordinate on where to move the card to
	 * @param {Object} delay How long to wait before moving, -1 to disable animation
	 * @param {Object} options Additional options valid currently:
	 *                         turnFaceUp        - Boolean  - If set to true, the card will be turned face up after it's moved
	 *                         turnFaceDown      - Boolean  - If set to true, the card will be turned face down after it's moved
	 *                         afterFaceUp       - function - If set, this function will be called after turning face up
	 *                         afterFaceDown     - function - If set, this function will be called after turning face down
	 *                         skipTurnAnimation - Boolean  - If set to true, no animation will be used to turn the card
	 */
	this.moveTo = function(screenX, screenY, delay, options)
	{
		var self = this;
		
		this.show();
		
		if((P.getBrowser()=="ie" || P.getBrowser()=="ie7") && this.isBase && !SOLITAIRE.tinyScreen)
		{
			screenX -= 2;
			screenY -= 2;
		}
				
		if(typeof options=="undefined" || options===null)
			options = {};
					
		var moveSpeed = SOLITAIRE.CARD_MOVE_SPEED;
		if(options.hasOwnProperty("moveSpeed"))
			moveSpeed = parseFloat(options["moveSpeed"]);
		if(options.hasOwnProperty("moveSpeedFunction"))
			moveSpeed = parseFloat(options.moveSpeedFunction());
			
		var movePoint = [ screenX, screenY ];
		var moveAttributes = { points : { to : movePoint } };
		var moveAnimation = new YAHOO.util.Motion(this._domCard, moveAttributes, moveSpeed, YAHOO.util.Easing.easeOut);
		
		var turnOverDelay = null;
		if(options.hasOwnProperty("turnOverDelay"))
			turnOverDelay = options["turnOverDelay"];

		if(options.hasOwnProperty("preAnimation"))
			moveAnimation.onStart.subscribe(function() { options["preAnimation"](); });

		moveAnimation.onComplete.subscribe(function() { CARD.cardsCurrentlyAnimating[self._domCard.id] = null; });
		
		if(options.hasOwnProperty("postAnimation"))
			moveAnimation.onComplete.subscribe(options["postAnimation"]);

		if(options.hasOwnProperty("turnFaceUp") && options["turnFaceUp"]===true)
		{
			this._isFaceUp = true;
			moveAnimation.onComplete.subscribe(function() { self.turnFaceUp(options.hasOwnProperty("skipTurnAnimation") && options["skipTurnAnimation"]===true ? -1 : turnOverDelay); if(options.hasOwnProperty("afterFaceUp")) { options["afterFaceUp"](); } });
		}
			
		if(options.hasOwnProperty("turnFaceDown") && options["turnFaceDown"]===true)
		{
			this._isFaceUp = false;
			moveAnimation.onComplete.subscribe(function() { self.turnFaceDown(options.hasOwnProperty("skipTurnAnimation") && options["skipTurnAnimation"]===true ? -1 : turnOverDelay); if(options.hasOwnProperty("afterFaceDown")) { options["afterFaceDown"](); } });
		}
		
		if(options.hasOwnProperty("afterMove") && options["afterMove"]!==null)
			moveAnimation.onComplete.subscribe(function() { options["afterMove"](); });
						
		if(typeof delay=="undefined" || delay===null)
		{
			CARD.cardsCurrentlyAnimating[self._domCard.id] = moveAnimation;
			moveAnimation.animate();
		}
		else if(delay==-1)
		{			
			if(options.hasOwnProperty("postAnimation"))
				options["postAnimation"]();

			this._domCard.style.top = "" + screenY + "px";
			this._domCard.style.left = "" + screenX + "px";
			
			if(options.hasOwnProperty("turnFaceUp") && options["turnFaceUp"]===true)
			{
				self.turnFaceUp(options.hasOwnProperty("skipTurnAnimation") && options["skipTurnAnimation"]===true ? -1 : turnOverDelay);
				if(options.hasOwnProperty("afterFaceUp"))
					options["afterFaceUp"]();
			}
			
			if(options.hasOwnProperty("turnFaceDown") && options["turnFaceDown"]===true)
			{
				self.turnFaceDown(options.hasOwnProperty("skipTurnAnimation") && options["skipTurnAnimation"]===true ? -1 : turnOverDelay);
				if(options.hasOwnProperty("afterFaceDown"))
					options["afterFaceDown"]();
			}
			
			if(options.hasOwnProperty("afterMove"))
				setTimeout(function() { options["afterMove"](); }, 0);
		}
		else
		{
			moveAnimation.delay = delay;
			CARD.cardAnimations.push(moveAnimation);
			CARD.animateStart();
		}
	};
				
	/**
	 * Returns the CARD ID for this card
	 */
	this.getID = function()
	{
		return this.id;
	};
	
	/**
	 * Returns the DOM object representing this card (the IMG)
	 */
	this.getDomCard = function()
	{
		return this._domCard;
	};
	
	/**
	 * Sets the function to call when starting the drag
	 * @param {Object} dragStartHandler
	 */
	this.setDragStartHandler = function(dragStartHandler)
	{
		this._dragStartHandler = dragStartHandler;
	};	
	/**
	 * Sets the function to call while dragging
	 * @param {Object} dragHandler
	 */
	this.setDragHandler = function(dragHandler)
	{
		this._dragHandler = dragHandler;
	};
	
	/**
	 * Sets the function to call when the users mouses over a card
	 * @param {Object} mouseOverHandler
	 */
	this.setMouseOverHandler = function(mouseOverHandler)
	{
		this._mouseOverHandler = mouseOverHandler;
	};

	/**
	 * Sets the function to call when the users mouses out of a card
	 * @param {Object} mouseOutHandler
	 */
	this.setMouseOutHandler = function(mouseOutHandler)
	{
		this._mouseOutHandler = mouseOutHandler;
	};
	
	/**
	 * Sets the function to call when the user let's go of a card
	 * @param {Object} dragStopHandler
	 */
	this.setDragStopHandler = function(dragStopHandler)
	{
		this._dragStopHandler = dragStopHandler;
	};

	/**
	 * This is called to set an external drag dropped handler
	 * @param {Object} dragDroppedHandler
	 */
	this.setDroppedHandler = function(droppedHandler)
	{
		this._droppedHandler = droppedHandler;
	};
	
	/**
	 * This will hide the card
	 */
	this.hide = function()
	{
		this._domCard.style.visibility = "hidden";
	};
	
	/**
	 * This will show the card
	 */
	this.show = function()
	{
		this._domCard.style.visibility = "visible";
	};

	/**
	 * Will put this object in the right drag drop group depending on enable/disable status
	 */	
	this.updateGroup = function()
	{
		if(this._dragDrop.isLocked() && !this._dragDrop.isTarget)
		{
			this._dragDrop.removeFromGroup("enabled");
			this._dragDrop.addToGroup("disabled");
		}
		else
		{
			this._dragDrop.removeFromGroup("disabled");
			this._dragDrop.addToGroup("enabled");
		}
	};
		
	/**
	 * This will prevent the card from being dragged
	 */
	this.disableDragging = function()
	{
		this._dragDrop.lock();
		this.updateGroup();
	};
	
	/**
	 * This will allow the card to be dragged
	 */
	this.enableDragging = function()
	{
		this._dragDrop.unlock();
		this.updateGroup();
	};
	
	/**
	 * This will prevent things from being dropped onto this card
	 */
	this.disableDropping = function()
	{
		this._dragDrop.isTarget = false;
		this.updateGroup();
	};
	
	/**
	 * This will allow things to be dropped onto this card
	 */
	this.enableDropping = function()
	{
		this._dragDrop.isTarget = true;
		this.updateGroup();
	};
	
	/**
	 * Will limit a card to only going certain places
	 * @param {Object} dragSpace
	 */
	this.setDragSpace = function(dragSpace)
	{
		this._dragSpace = dragSpace;
	};
}

/**
 * Static CARD object for various things
 */
var CARD = 
{
	ids : [
			"1c", 
			"1d", 
			"1h", 
			"1s", 
			"2c", 
			"2d", 
			"2h", 
			"2s", 
			"3c", 
			"3d", 
			"3h", 
			"3s", 
			"4c", 
			"4d", 
			"4h", 
			"4s", 
			"5c", 
			"5d", 
			"5h", 
			"5s", 
			"6c", 
			"6d", 
			"6h", 
			"6s", 
			"7c", 
			"7d", 
			"7h", 
			"7s", 
			"8c", 
			"8d", 
			"8h", 
			"8s", 
			"9c", 
			"9d", 
			"9h", 
			"9s", 
			"10c", 
			"10d", 
			"10h", 
			"10s", 
			"11c", 
			"11d", 
			"11h", 
			"11s", 
			"12c", 
			"12d", 
			"12h", 
			"12s", 
			"13c", 
			"13d", 
			"13h", 
			"13s", 
			"bj", 
			"rj",
			"back", 
			"base",
			"base_redeal",
			"base_stop",
			"base_clear",
			"base_foundation"
		  ],
	
	cardAnimations : [],
	cardsCurrentlyAnimating : {},

	cardsCreated : {},
	cardsCreatedCount : 0,
	
	/**
	 * Start animation, only proceeds if only one animation is in the queue
	 */
	animateStart : function()
	{
		var self = this;
		
		if(this.cardAnimations.length===1)
			setTimeout(function() { self.animateNext(); }, this.cardAnimations[0].delay);
	},
	
	/**
	 * Animates the next card in the animation queue
	 */
	animateNext : function()
	{
		var self = this;
		
		var moveAnimation = (this.cardAnimations.splice(0, 1))[0];
		var DOMID = moveAnimation.getEl().id;
		var card = CARD.getCardMatchingDOMID(DOMID);
		
		if(CARD.cardsCurrentlyAnimating.hasOwnProperty(DOMID) &&
		   CARD.cardsCurrentlyAnimating[DOMID]!==null)
		{
			var existingAnimation = CARD.cardsCurrentlyAnimating[DOMID];
			if(existingAnimation.attributes.points.to[0]==moveAnimation.attributes.points.to[0] &&
			   existingAnimation.attributes.points.to[1]==moveAnimation.attributes.points.to[1])
			{
				// TODO: I should add any different onCompletes for moveAnimation onto existingAnimation
				
				if(this.cardAnimations.length>0)
					setTimeout(function() { self.animateNext(); }, this.cardAnimations[0].delay);
				//else
				//	CARD.runAfterAnimations(function() { CARD.updateZIndexes(DOMID); });

				return;
			}
			
		  	existingAnimation.stop();
		}

		CARD.cardsCurrentlyAnimating[DOMID] = moveAnimation;
		moveAnimation.animate();
		
		if(this.cardAnimations.length>0)
			setTimeout(function() { self.animateNext(); }, this.cardAnimations[0].delay);
	},
	
	/**
	 * Stops animation for a given card
	 * @param {Object} cardDOMID
	 */
	animateStop : function(cardDOMID)
	{
		if(!CARD.cardsCurrentlyAnimating.hasOwnProperty(cardDOMID) || CARD.cardsCurrentlyAnimating[cardDOMID]===null)
			return;
		
		CARD.cardsCurrentlyAnimating[cardDOMID].stop();
		CARD.cardsCurrentlyAnimating[cardDOMID] = null;
	},
	
	/**
	 * Can be called with a function and it will run the function after all animations are finished
	 * @param {Object} theFunction
	 */
	runAfterAnimations : function(theFunction, delay)
	{
		if(typeof delay=="undefined")
			delay = null;
			
		for(var key in CARD.cardsCurrentlyAnimating)
		{
			if(CARD.cardsCurrentlyAnimating[key]!==null)
			{
				var timeoutID = setTimeout(function() { CARD.runAfterAnimations(theFunction, delay); }, 300);
				return timeoutID;
			}
		}
		
		if(delay===null)
			theFunction();
		else
			setTimeout(theFunction, delay);
		
		return null;
	},

	/**
	 * Returns the cardID matching the given domID
	 * @param {Object} domID
	 */
	getCardMatchingDOMID : function(domID)
	{
		for(var key in CARD.cardsCreated)
		{
			var card = CARD.cardsCreated[key];
			var domCard = card.getDomCard();		
			if(domCard.id==domID)
				return card;
		}
		
		return null;
	},
	
	/**
	 * Returns true if any cards are currently being animated
	 */
	isCurrentlyAnimating : function()
	{
		for(var key in CARD.cardsCurrentlyAnimating)
		{
			if(CARD.cardsCurrentlyAnimating[key]!==null)
				return true;
		}
		
		return false;
	},
	
	/**
	 * Given a cardID it will return a readable version
	 * @param {Object} cardID
	 */
	name : function(cardID)
	{
		if(typeof cardID=="object")
			cardID = cardID.getID();
	
		if(cardID=="bj")
			return "Black Joker";
		else if(cardID=="rj")
			return "Red Joker";
		else if(cardID=="back")
			return "Card Back";
		else if(cardID.indexOf("base")===0)
			return "Card Base";
		else
		{
			var cardNumber = CARD.number(cardID);
			if(cardNumber==1)
				cardNumber = "Ace";
			else if(cardNumber==11)
				cardNumber = "Jack";
			else if(cardNumber==12)
				cardNumber = "Queen";
			else if(cardNumber==13)
				cardNumber = "King";
		
			if(cardID.indexOf("c")!=-1)
				return cardNumber + " of Clubs";
			else if(cardID.indexOf("h")!=-1)
				return cardNumber + " of Hearts";
			else if(cardID.indexOf("d")!=-1)
				return cardNumber + " of Diamonds";
			else if(cardID.indexOf("s")!=-1)
				return cardNumber + " of Spades";
		}
		
		return "Unknown Card";		
	},
	
	suit : function(cardID)
	{
		if(typeof cardID=="object")
			cardID = cardID.getID();

		if(cardID=="bj")
			return "j";
		else if(cardID=="rj")
			return "j";
		else if(cardID=="back")
			return "back";
		else if(cardID.indexOf("base")===0)
			return "base";
		else
		{
			if(cardID.indexOf("c")!=-1)
				return "c";
			else if(cardID.indexOf("h")!=-1)
				return "h";
			else if(cardID.indexOf("d")!=-1)
				return "d";
			else if(cardID.indexOf("s")!=-1)
				return "s";
		}
		
		return "";
	},
	
	/**
	 * Returns true if the cardID is a Heart, Diamond or red joker
	 * @param {Object} cardID
	 */
	isRed : function(cardID)
	{
		if(typeof cardID=="object")
			cardID = cardID.getID();
	
		return cardID.indexOf("d")!=-1 || cardID.indexOf("h")!=-1 || cardID=="rj";
	},
	
	/**
	 * Returns true if the cardID is a Spade, Club or black joker
	 * @param {Object} cardID
	 */
	isBlack : function(cardID)
	{
		if(typeof cardID=="object")
			cardID = cardID.getID();
	
		return cardID.indexOf("s")!=-1 || cardID.indexOf("c")!=-1  || cardID=="bj";
	},
	
	/**
	 * Returns true if the cardID is a joker (red or black)
	 * @param {Object} cardID
	 */
	isJoker : function(cardID)
	{
		if(typeof cardID=="object")
			cardID = cardID.getID();
	
		return cardID.indexOf("j")!=-1;
	},
	
	/**
	 * Returns the number of the given cardID
	 * @param {Object} cardID
	 */
	number : function(cardID)
	{
		if(typeof cardID=="object")
			cardID = cardID.getID();
		
		if(cardID=="back" || cardID.indexOf("base")===0)
			return "";
	
		var cardNumber = "";
		for(var i=0;i<cardID.length;i++)
		{
			if(!isNaN(parseInt(cardID.charAt(i), 10)))
				cardNumber += cardID.charAt(i);
		}
		
		cardNumber = parseInt(cardNumber, 10);
		
		return cardNumber;
	},
	
	/**
	 * Will update call cards with new deck or sizes
	 * @param {Object} deck The deck to use
	 * @param {Object} deckSize The size number
	 * @param {Object} scale How much to scale the cards
	 */
	updateAllCards : function(deck, deckSize, scale)
	{
		for(var key in CARD.cardsCreated)
		{
			CARD.cardsCreated[key].updateCard(deck, deckSize, scale);
		}
	},
	
	/**
	 * This will remove all cards from the game
	 */
	removeAllCards : function()
	{
		for(var key in CARD.cardsCreated)
		{
			var card = CARD.cardsCreated[key];
			
			card._dragDrop.unreg();
			
			var domCard = card.getDomCard();
			
			E.remove(domCard, "mouseover");
			E.remove(domCard, "mouseout");
			
			document.body.removeChild(domCard);
		}		
		
		CARD.cardsCreated = {};
	}
};
