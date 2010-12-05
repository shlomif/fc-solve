/*
 * DeckView.js - Decks View functions
 * Author - robert@cosmicrealms.com
 */

var DeckView = new DialogBox("deckView", { "percentageWidth"  : P.getBrowser()==="ie7" || P.getBrowser()==="ie" ? 0.85 : 0.85,
											     "percentageHeight" : P.getBrowser()==="ie7" || P.getBrowser()==="ie" ? 0.95 : 0.95,
											     "overflowable"     : true});
							
/**
 * This is called when the decks view is first initialized
 */													   
DeckView.onInit = function()
{
	DeckView.customDecksHandled = false;
	DeckView.tabView = new YAHOO.widget.TabView('deckViewTabs');
	DeckView.tabView.addListener("activeTabChange", DeckView.tabChanged);
	
	DeckView.chooseTree = T.listToTree("deckViewChooseList", "deckViewChooseTree");
	
	var deckViewChooseList = document.getElementById("deckViewChooseList");
	deckViewChooseList.parentNode.removeChild(deckViewChooseList);
	
	DeckView.chooseTree.subscribe("labelClick", DeckView.chooseDeckClicked);
	DeckView.chooseTree.subscribe("expandComplete", function() { T.boldFolders(DeckView.chooseTree.getRoot()); });
	DeckView.chooseTreeSelectedNode = null;
	
	E.add("deckViewChooseCancelButton", "click", DeckView.hide, null, DeckView);
	E.add("deckViewCreateCancelButton", "click", DeckView.hide, null, DeckView);
	E.add("deckViewCreateSaveButton", "click", DeckView.createSaveHandler, null, DeckView);
	
	var deckViewChooseShowDropdown = document.getElementById("deckViewChooseShowDropdown");
	E.add(deckViewChooseShowDropdown, "change", DeckView.chooseShowDropdownChanged);
	
	var deckViewCreateDecksSelect = document.getElementById("deckViewCreateDecksSelect");
	E.add(deckViewCreateDecksSelect, "change", DeckView.createDecksSelectChanged);
	
	var deckViewCreateWhichCard = document.getElementById("deckViewCreateWhichCard");
	E.add(deckViewCreateWhichCard, "change", DeckView.deckViewCreateWhichCardChanged);
	
	E.add("deckViewCreateWhichImage", "keyup", DeckView.deckViewCreateWhichImageChanged);
	
	var deckViewCreateCardImageScaleThumbContainer = document.getElementById("deckViewCreateCardImageScaleThumbContainer");
	var targetThumbLocation = 200-Math.floor((1.0*(200/1.0)));
	deckViewCreateCardImageScaleThumbContainer.style.top = targetThumbLocation + "px";
		
	DeckView.deckViewCreateImageScaleSlider = YAHOO.widget.Slider.getVertSlider("deckViewCreateCardImageScaleContainer", "deckViewCreateCardImageScaleThumbContainer", targetThumbLocation, 200-targetThumbLocation);
	//DeckView.deckViewCreateImageScaleSlider.setValue(1.0, true); 
	DeckView.deckViewCreateImageScaleSlider.subscribe("change", function(offset) { DeckView.deckViewCreateCardImageScaleHandler(offset); });
};

/**
 * Called before the dialog is shown
 */
DeckView.beforeShow = function()
{
	if(!DeckView.customDecksHandled && !CONSTANTS.IS_OFFLINE)
		YAHOO.util.Connect.asyncRequest("POST", "getCustomDecks", { success : DeckView.getCustomDecksFinished, failure: DeckView.getCustomDecksFinished });
	
	var chooseNodeToSelect = DeckView.chooseTreeSelectedNode;
	
	if(chooseNodeToSelect===null)
	{
		chooseNodeToSelect = T.getNodeWithDataMatching(DeckView.chooseTree.getRoot(), Options.get("optionDeck"));
		DeckView.chooseTreeSelectedNode = chooseNodeToSelect;
	}
	
	if(chooseNodeToSelect!==null)
		T.openToNode(chooseNodeToSelect);
	
	Menu.deselect();
};

/**
 * This is called when getting a list of custom decks returns
 * @param {Object} data
 */
DeckView.getCustomDecksFinished = function(data)
{
	if(data===null || !data.hasOwnProperty("responseText") || data.responseText===null || data.responseText.length<1)
		return;
	
	if(!DeckView.customDecksHandled)
	{
		var deckViewCreateDecksSelect = document.getElementById("deckViewCreateDecksSelect");
		
		try
		{
			DeckView.customDecks = eval("(" + data["responseText"] + ")");
			
			// Add to the create deck section
			for(var i=0;i<DeckView.customDecks.length;i++)
			{
				var deckObject = DeckView.customDecks[i];
				
				var deckOption = document.createElement("OPTION");
				deckOption.value = deckObject["deckID"];
				deckOption.appendChild(document.createTextNode(deckObject["name"]));
				deckViewCreateDecksSelect.appendChild(deckOption);
			}
			
			// Add to our tree for selection
			if(DeckView.customDecks.length>0)
			{
				var chooseTreeRoot = DeckView.chooseTree.getRoot();
				
				var yourCreationsNode = new YAHOO.widget.TextNode("Your Creations", chooseTreeRoot, false);
				yourCreationsNode.data = null;
				
				for(var i=0;i<DeckView.customDecks.length;i++)
				{
					var deckObject = DeckView.customDecks[i];
					
					var deckNode = new YAHOO.widget.TextNode(deckObject["name"], yourCreationsNode, false);
					deckNode.data = deckObject["deckID"];
				}
				
				DeckView.chooseTree.draw();
				T.boldFolders(chooseTreeRoot);
			}
		}
		catch(e)
		{
			
		}
			
		DeckView.customDecksHandled = true;
	}
};

/**
 * Whenever the decks view is shown, this is called after it's visible
 */
DeckView.afterShow = function()
{
	DeckView.resize();
	
	var deckViewTabsContentContainer = document.getElementById("deckViewTabsContentContainer");
	var deckViewTabsContentContainerChildren = D.getElementsByTagName(deckViewTabsContentContainer, "DIV");
	var deckViewContents = document.getElementById("deckViewContents");
	var deckViewTabsContainer = document.getElementById("deckViewTabsContainer");

	for(var i=0;i<deckViewTabsContentContainerChildren.length;i++)
	{
		var tabContainer = deckViewTabsContentContainerChildren[i];

		var containerHeight = (D.height(deckViewContents) - D.height(deckViewTabsContainer));
		containerHeight -= (P.getBrowser()=="ie" ? 40 : 20);
		tabContainer.style.height = containerHeight + "px";
		
		// Statistcs view clears fields here? Should we clear stuff too?
	}
	
	if(P.getBrowser()==="gecko" || P.getBrowser()==="opera")
	{
		var deckViewChooseShowDropdownLabel = document.getElementById("deckViewChooseShowDropdownLabel");
		deckViewChooseShowDropdownLabel.style.position = "relative";
		deckViewChooseShowDropdownLabel.style.top = "-4px";
	}
	
	DeckView.tabChanged();
};

/**
 * This is called after someone switches a tab (also called explicitly when the dialog is shown to handle the pre-selected tab)
 * @param {Object} e
 */
DeckView.tabChanged = function(e)
{
	var activeIndex = DeckView.tabView.get("activeIndex");

	if(activeIndex===0)
		DeckView.chooseDeckClicked(DeckView.chooseTreeSelectedNode);
	else
		DeckView.createDecksSelectChanged();
};

/**
 * Called when a deck is clicked
 * @param {Object} node
 */
DeckView.chooseDeckClicked = function(node)
{
	if(node.data===null)
		return;
		
	if(DeckView.chooseTreeSelectedNode!==null)
	{
		YAHOO.util.Dom.removeClass(DeckView.chooseTreeSelectedNode.getEl(), "treeSelectedRow");
		if(P.getBrowser()=="safari")
			YAHOO.util.Dom.addClass(DeckView.chooseTreeSelectedNode.getEl(), "treeNonSelectedRow");
	}

	DeckView.chooseTreeSelectedNode = node;
	YAHOO.util.Dom.addClass(DeckView.chooseTreeSelectedNode.getEl(), "treeSelectedRow");
	
	var deckViewChooseRight = document.getElementById("deckViewChooseRight");
	deckViewChooseRight.style.visibility = "hidden";
	
	DeckView.resize();
	
	deckViewChooseRight.style.visibility = "";

	setTimeout(function() { DeckView.chooseShowDropdownChanged(); }, 0);
};

/**
 * This is called when the choose shown dropdown is changed
 * @param {Object} e
 */
DeckView.chooseShowDropdownChanged = function(e)
{
	var deckViewChooseShowDropdown = document.getElementById("deckViewChooseShowDropdown");
	var selectedShowValue = deckViewChooseShowDropdown.options[deckViewChooseShowDropdown.selectedIndex].value;
	var cardsIDsToShow = selectedShowValue.split(",");
	var squareRoute = Math.floor(Math.sqrt(cardsIDsToShow.length));
	var leftOverCount = cardsIDsToShow.length-(squareRoute*squareRoute);
	
	var deckViewChooseRightMiddle = document.getElementById("deckViewChooseRightMiddle");
	D.clear(deckViewChooseRightMiddle);
	
	var cardsToShowTable = document.createElement("TABLE");
	cardsToShowTable.id = "deckViewChooseCardsToShowTable";
	
	var cardsToShowTableBody = document.createElement("TBODY");
	
	for(var y=0;y<squareRoute;y++)
	{
		var cardsToShowRow = document.createElement("TR");
		
		for(var x=0;x<squareRoute+(leftOverCount/squareRoute);x++)
		{
			var cardsToShowCell = document.createElement("TD");
			if(y===0&&x===0)
				cardsToShowCell.id = "deckViewChooseCardsToShowFirstCell";
				
			cardsToShowRow.appendChild(cardsToShowCell);
		}
		
		cardsToShowTableBody.appendChild(cardsToShowRow);
	}
	
	cardsToShowTable.appendChild(cardsToShowTableBody);
	
	deckViewChooseRightMiddle.appendChild(cardsToShowTable);
	
	var deckViewChooseCardsToShowFirstCell = document.getElementById("deckViewChooseCardsToShowFirstCell");
	
	var idealCardHeight = D.height(deckViewChooseCardsToShowFirstCell)-(2*squareRoute);
	var idealCardWidth = D.width(deckViewChooseCardsToShowFirstCell)-(2*(squareRoute+(leftOverCount/squareRoute)));
	
	var currentlySelectedDeckID = DeckView.chooseGetSelectedDeck();
	var deck = new Deck(currentlySelectedDeckID);
	deck.loadDeck();
	
	var artistName = deck._deckArtistName;
	var artistWebsite = deck._deckArtistWebsite;
	
	if(DeckView.customDecks)
	{
		for(var i=0;i<DeckView.customDecks.length;i++)
		{
			var deckObject = DeckView.customDecks[i];
			if(deckObject["deckID"]!=currentlySelectedDeckID)
				continue;
				
			artistName = deckObject["artistName"];
			artistWebsite = deckObject["artistWebsite"];
		}
	}
	
	var deckViewChooseArtistName = document.getElementById("deckViewChooseArtistName");
	D.clear(deckViewChooseArtistName);
	deckViewChooseArtistName.appendChild(document.createTextNode(artistName));
	
	var deckViewChooseArtistWebsite = document.getElementById("deckViewChooseArtistWebsite");
	D.clear(deckViewChooseArtistWebsite);
	var websiteAnchor = document.createElement("A");
	websiteAnchor.href = artistWebsite;
	websiteAnchor.target = "_blank";
	websiteAnchor.appendChild(document.createTextNode(artistWebsite));
	deckViewChooseArtistWebsite.appendChild(websiteAnchor);
	
	var deckSizeFitResults = deck.fitInside(idealCardHeight, idealCardWidth);

	var deckSizeNum = deckSizeFitResults["sizeNum"];	
	var cardScale = deckSizeFitResults["scale"];
	
	var cardLoc = 0;
	for(var y=0;cardLoc<cardsIDsToShow.length && y<cardsToShowTableBody.childNodes.length;y++)
	{
		var cardsToShowRow = cardsToShowTableBody.childNodes[y];
		for(var x=0;cardLoc<cardsIDsToShow.length && x<cardsToShowRow.childNodes.length;x++)
		{
			var cardsToShowCell = cardsToShowRow.childNodes[x];
			
			var cardToShowImageURL = deck.createCardURL(cardsIDsToShow[cardLoc], deckSizeNum);
			cardLoc++;
			
			var cardToShowImage = document.createElement("IMG");
			cardToShowImage.src = cardToShowImageURL;
			
			if(cardScale!=1.00)
			{
				var cardHeight = Math.floor(deck.getCardHeight(deckSizeNum, cardScale));
				var cardWidth = Math.floor(deck.getCardWidth(deckSizeNum, cardScale));
		
				cardToShowImage.style.width = "" + cardWidth + "px";
				cardToShowImage.style.height = "" + cardHeight + "px";
			}
			
			cardsToShowCell.appendChild(cardToShowImage);
		}
	}
};

/**
 * Returns the currently selected deck
 */
DeckView.chooseGetSelectedDeck = function()
{
	return DeckView.chooseTreeSelectedNode.data;
};

/**
 * This is called when we need to render the right createRight content based on what was clicked in the decks list of the create tab
 * @param {Object} e
 * @param {Object} ignored
 */
DeckView.createDecksSelectChanged = function(e, ignored)
{
	DeckView.resize();
	
	var deckViewCreateDecksSelect = document.getElementById("deckViewCreateDecksSelect");
	var selectedDeckValue = deckViewCreateDecksSelect.options[deckViewCreateDecksSelect.selectedIndex].value;

	if(selectedDeckValue=="new")
	{
		DeckView.customDeck = DeckView.createNewCustomDeck();
	}
	else
	{
		var currentCard = "11c";
		if(typeof DeckView.customDeck!="undefined")
			currentCard = DeckView.customDeck["currentCard"];
			
		for(var i=0;i<DeckView.customDecks.length;i++)
		{
			var deckObject = DeckView.customDecks[i];
			if(deckObject["deckID"]!=selectedDeckValue)
				continue;
			
			DeckView.customDeck = DeckView.customDecks[i];
			DeckView.customDeck["currentCard"] = currentCard;
			
			var deckViewCreateWhichCard = document.getElementById("deckViewCreateWhichCard");
			for(var i=0;i<deckViewCreateWhichCard.options.length;i++)
			{
				var cardID = deckViewCreateWhichCard.options[i].value;
				DeckView.customDeck["cards"][cardID]["templateHeight"] = 0;
				DeckView.customDeck["cards"][cardID]["imageLastLeftTop"] = [ 0, 0 ];
				DeckView.customDeck["cards"][cardID]["imageOriginalWidth"] = 0;
				DeckView.customDeck["cards"][cardID]["imageOriginalHeight"] = 0;
				DeckView.customDeck["cards"][cardID]["imageWidth"] = 0;
				DeckView.customDeck["cards"][cardID]["imageHeight"] = 0;
			}
		}
	}
	
	// Update the display with our deck info
	var deckViewCreateDeckName = document.getElementById("deckViewCreateDeckName");
	deckViewCreateDeckName.value = DeckView.customDeck["name"];

	var deckViewCreateDeckPublic = document.getElementById("deckViewCreateDeckPublic");
	deckViewCreateDeckPublic.checked = DeckView.customDeck["public"];

	var deckViewCreateDeckArtistName = document.getElementById("deckViewCreateDeckArtistName");
	deckViewCreateDeckArtistName.value = DeckView.customDeck["artistName"];

	var deckViewCreateDeckArtistWebsite = document.getElementById("deckViewCreateDeckArtistWebsite");
	deckViewCreateDeckArtistWebsite.value = DeckView.customDeck["artistWebsite"];
	
	var deckViewCreateWhichCard = document.getElementById("deckViewCreateWhichCard");
	for(var i=0;i<deckViewCreateWhichCard.options.length;i++)
	{
		if(deckViewCreateWhichCard.options[i].value===DeckView.customDeck["currentCard"])
		{
			deckViewCreateWhichCard.options[i].selected = true;
			deckViewCreateWhichCard.selectedIndex = i;
		}
		else
		{
			deckViewCreateWhichCard.options[i].selected = false;
		}
	}
	
	DeckView.deckViewCreateWhichCardChanged();
};

/**
 * Creates a new custom deck
 */
DeckView.createNewCustomDeck = function()
{
	var customDeck = {};
	
	customDeck["name"] = "untitled";
	customDeck["public"] = true;
	customDeck["artistName"] = "";
	customDeck["artistWebsite"] = "";
	customDeck["cards"] = {};
	customDeck["currentCard"] = "11c";
	customDeck["templateID"] = "template_paris";
	
	var deckViewCreateWhichCard = document.getElementById("deckViewCreateWhichCard");
	for(var i=0;i<deckViewCreateWhichCard.options.length;i++)
	{
		var card = {};
		card["templateSizeNum"] = 0;
		card["templateHeight"] = 0;
		card["image"] = "";
		card["imageScale"] = 1.0;
		card["imageLastLeftTop"] = [ 0, 0 ];
		card["imageLeftTop"] = [ 0, 0 ];
		card["imageOriginalWidth"] = 0;
		card["imageOriginalHeight"] = 0;
		card["imageWidth"] = 0;
		card["imageHeight"] = 0;
		
		customDeck["cards"][deckViewCreateWhichCard.options[i].value] = card;
	}
	
	return customDeck;
};

/**
 * Called when which card to edit changes
 * @param {Object} e
 * @param {Object} ignored
 */
DeckView.deckViewCreateWhichCardChanged = function(e, ignored)
{
	var deckViewCreateWhichCard = document.getElementById("deckViewCreateWhichCard");
	var currentCardID = deckViewCreateWhichCard.options[deckViewCreateWhichCard.selectedIndex].value;
	DeckView.customDeck["currentCard"] = currentCardID;
	
	var deckViewCreateWhichImage = document.getElementById("deckViewCreateWhichImage");
	deckViewCreateWhichImage.value = DeckView.customDeck["cards"][currentCardID]["image"];

	// Clear our existing images, calculate and put in the new ones
	var deckViewCreateCardImageContainer = document.getElementById("deckViewCreateCardImageContainer");
	D.clear(deckViewCreateCardImageContainer);
	
	var deckViewCreateCardContainer = document.getElementById("deckViewCreateCardContainer");

	var idealCardHeight = D.height(deckViewCreateCardContainer);
	var idealCardWidth = D.width(deckViewCreateCardContainer);

	var deck = new Deck(DeckView.customDeck["templateID"]);
	deck.loadDeck();

	var deckSizeFitResults = deck.fitInside(idealCardHeight, idealCardWidth);

	var deckSizeNum = deckSizeFitResults["sizeNum"];	
	var templateCardScale = deckSizeFitResults["scale"];
	
	var templateImage = document.createElement("IMG");
	templateImage.id = "deckViewCreateCardTemplate";
	templateImage.src = deck.createCardURL(currentCardID, deckSizeNum);
	
	var templateCardHeight = Math.floor(deck.getCardHeight(deckSizeNum, templateCardScale));
	var templateCardWidth = Math.floor(deck.getCardWidth(deckSizeNum, templateCardScale));
	if(templateCardScale!=1.00)
	{
		templateImage.style.width = "" + templateCardWidth + "px";
		templateImage.style.height = "" + templateCardHeight + "px";
	}
	
	var deckViewCreateButtonsContainer = document.getElementById("deckViewCreateButtonsContainer");

	deckViewCreateCardImageContainer.style.top = "" + ((idealCardHeight-templateCardHeight)/2) + "px"
	deckViewCreateCardImageContainer.style.left = "" + (((idealCardWidth-templateCardWidth)/2)-50) + "px"
	deckViewCreateCardImageContainer.style.height = "" + templateCardHeight + "px";
	deckViewCreateCardImageContainer.style.width = "" + templateCardWidth + "px";

	deckViewCreateCardImageContainer.appendChild(templateImage);
	
	// Has our ideal size changed?
	DeckView.customDeck["cards"][currentCardID]["templateSizeNum"] = deckSizeNum;
	
	if(DeckView.customDeck["cards"][currentCardID]["templateHeight"]===0 ||
	   DeckView.customDeck["cards"][currentCardID]["templateHeight"]===templateCardHeight)
	{
		DeckView.customDeck["cards"][currentCardID]["templateHeight"] = templateCardHeight;
	}
	else
	{
		var templateHeightDifferencePercentage = (templateCardHeight-DeckView.customDeck["cards"][currentCardID]["templateHeight"])/DeckView.customDeck["cards"][currentCardID]["templateHeight"];
		DeckView.customDeck["cards"][currentCardID]["imageScale"] += DeckView.customDeck["cards"][currentCardID]["imageScale"]*templateHeightDifferencePercentage;
		DeckView.customDeck["cards"][currentCardID]["imageLeftTop"][0] += DeckView.customDeck["cards"][currentCardID]["imageLeftTop"][0]*templateHeightDifferencePercentage;
		DeckView.customDeck["cards"][currentCardID]["imageLeftTop"][1] += DeckView.customDeck["cards"][currentCardID]["imageLeftTop"][1]*templateHeightDifferencePercentage;
	}
	
	// Create our actual image now
	var cardImage = document.createElement("IMG");
	cardImage.id = "deckViewCreateCardImage";
	cardImage.onload = function() { DeckView.deckViewCreateCardImageLoaded(); };
	cardImage.onerror = function() { DeckView.deckViewCreateCardImageLoaded(); };
	cardImage.onabort = function() { DeckView.deckViewCreateCardImageLoaded(); };
	cardImage.style.left = "" + (DeckView.customDeck["cards"][currentCardID]["imageLeftTop"][0]) + "px";
	cardImage.style.top = "" + (DeckView.customDeck["cards"][currentCardID]["imageLeftTop"][1]) + "px";
	cardImage.src = "images/empty.gif";
	
	deckViewCreateCardImageContainer.appendChild(cardImage);

	// Create our mask (the corners and extreme edges)	
	var maskImage = document.createElement("IMG");
	var maskImageURL = deck.createCardURL("base", deckSizeNum);
	maskImageURL = S.replaceAll(maskImageURL, "base", "mask");
	maskImage.id = "deckViewCreateMaskImage";
	maskImage.src = maskImageURL;
	deckViewCreateCardImageContainer.appendChild(maskImage);
	
	// Create the area where we can drag
	var deckViewCreateDragArea = document.createElement("DIV");
	deckViewCreateDragArea.id = "deckViewCreateDragArea";
	deckViewCreateCardImageContainer.appendChild(deckViewCreateDragArea);
	
	DeckView.createCardImageDragDropStartXY = D.xy(deckViewCreateDragArea);
	
	if(typeof DeckView.createCardImageDragDrop!="undefined" && DeckView.createCardImageDragDrop!==null)
		DeckView.createCardImageDragDrop.unreg();
		
	DeckView.createCardImageDragDrop = new YAHOO.util.DD(deckViewCreateDragArea.id, "cardImageDragDrop");
	DeckView.createCardImageDragDrop.onMouseUp = function(e) { DeckView.createCardImageDragStopped(e); };
	DeckView.createCardImageDragDrop.onDrag = function(e) { DeckView.createCardImageDragged(e); };
	DeckView.createCardImageDragDrop.startDrag = function(x, y) { DeckView.createCardImageDragStarted(x, y); };
	
	DeckView.deckViewCreateWhichImageChanged(null, true);
};

/**
 * This is called whenever the card image changes
 * @param {Object} e
 */
DeckView.deckViewCreateWhichImageChanged = function(e, forceLoad)
{
	var deckViewCreateWhichImage = document.getElementById("deckViewCreateWhichImage");
	
	var currentCardID = DeckView.customDeck["currentCard"];
	if(forceLoad!==true && DeckView.customDeck["cards"][currentCardID]["image"]==deckViewCreateWhichImage.value)
		return;
		
	DeckView.customDeck["cards"][currentCardID]["image"] = deckViewCreateWhichImage.value;
	
	var deckViewCreateCardImage = document.getElementById("deckViewCreateCardImage");
	deckViewCreateCardImage.style.width = "auto";
	deckViewCreateCardImage.style.height = "auto";
	
	Status.show("Loading Image...", -1, "loadCustomDeckImage");
	
	deckViewCreateCardImage.src = DeckView.customDeck["cards"][currentCardID]["image"];
};

/**
 * Called when the image has laoded
 * @param {Object} e
 */
DeckView.deckViewCreateCardImageLoaded = function(e)
{
	var deckViewCreateCardImage = document.getElementById("deckViewCreateCardImage");
		
	var imageWidth = D.width(deckViewCreateCardImage);
	var imageHeight = D.height(deckViewCreateCardImage);
	
	Status.hide();
	
	if(imageWidth<=1 || imageHeight<=1)
		return;
		
	var currentCardID = DeckView.customDeck["currentCard"];
	DeckView.customDeck["cards"][currentCardID]["imageOriginalHeight"] = imageHeight;
	DeckView.customDeck["cards"][currentCardID]["imageOriginalWidth"] = imageWidth;
	
	var deckViewCreateCardImageScaleThumbContainer = document.getElementById("deckViewCreateCardImageScaleThumbContainer");
	
	var targetThumbLocation = 200-Math.floor((DeckView.customDeck["cards"][currentCardID]["imageScale"]*(200/1.0)));
	deckViewCreateCardImageScaleThumbContainer.style.top = targetThumbLocation + "px";
	//DeckView.deckViewCreateImageScaleSlider.setValue(DeckView.customDeck["cards"][currentCardID]["imageScale"], true, false, true);
	
	DeckView.deckViewCreateCardImageScaleHandler();
};

/**
 * This is called when someone stops sliding the image scaler
 * @param {Object} offsetFromStart
 */
DeckView.deckViewCreateCardImageScaleHandler = function(offset)
{
	var deckViewCreateCardImageScaleThumbContainer = document.getElementById("deckViewCreateCardImageScaleThumbContainer");
	var deckViewCreateCardImageScaleThumbContainerXY = D.xy(deckViewCreateCardImageScaleThumbContainer);
	
	var deckViewCreateCardImageScaleContainer = document.getElementById("deckViewCreateCardImageScaleContainer");
	var deckViewCreateCardImageScaleContainerXY = D.xy(deckViewCreateCardImageScaleContainer);
	
	if(deckViewCreateCardImageScaleThumbContainerXY===false)
		return;
		
	var currentCardID = DeckView.customDeck["currentCard"];
	
	var position = (deckViewCreateCardImageScaleThumbContainerXY[1]-deckViewCreateCardImageScaleContainerXY[1]);
	DeckView.customDeck["cards"][currentCardID]["imageScale"] = ((200-position) / (200/1.0));

	var deckViewCreateCardImage = document.getElementById("deckViewCreateCardImage");
	if(deckViewCreateCardImage===null ||
	   DeckView.customDeck["cards"][currentCardID]["imageOriginalHeight"]<1 ||
	   DeckView.customDeck["cards"][currentCardID]["imageOriginalWidth"]<1)
		return;
	
	var newImageHeight = Math.floor(DeckView.customDeck["cards"][currentCardID]["imageOriginalHeight"]*DeckView.customDeck["cards"][currentCardID]["imageScale"]);
	var newImageWidth = Math.floor(DeckView.customDeck["cards"][currentCardID]["imageOriginalWidth"]*DeckView.customDeck["cards"][currentCardID]["imageScale"]);

	deckViewCreateCardImage.style.width = "" + newImageWidth + "px";
	deckViewCreateCardImage.style.height = "" + newImageHeight + "px";
};

/**
 * This is called when dragging begins
 * @param {Object} x
 * @param {Object} y
 */
DeckView.createCardImageDragStarted = function(x, y)
{
	var deckViewCreateCardImage = document.getElementById("deckViewCreateCardImage");
	var deckViewCreateCardImageXY = D.xy(deckViewCreateCardImage);	
	var deckViewCreateCardImageHeight = D.height(deckViewCreateCardImage);
	var deckViewCreateCardImageWidth = D.width(deckViewCreateCardImage);
	
	var deckViewCreateDragArea = document.getElementById("deckViewCreateDragArea");
	var deckViewCreateDragAreaXY = D.xy(deckViewCreateDragArea);
	var deckViewCreateDragAreaHeight = D.height(deckViewCreateDragArea);
	var deckViewCreateDragAreaWidth = D.width(deckViewCreateDragArea);
	
	var remainingSpaceRight = deckViewCreateCardImageWidth-((deckViewCreateDragAreaXY[0]+deckViewCreateDragAreaWidth)-deckViewCreateCardImageXY[0])
	var remainingSpaceLeft = deckViewCreateDragAreaXY[0]-deckViewCreateCardImageXY[0];
	
	var remainingSpaceTop = deckViewCreateCardImageHeight-((deckViewCreateDragAreaXY[1]+deckViewCreateDragAreaHeight)-deckViewCreateCardImageXY[1])
	var remainingSpaceBottom = deckViewCreateDragAreaXY[1]-deckViewCreateCardImageXY[1];

	DeckView.createCardImageDragDrop.resetConstraints();
	DeckView.createCardImageDragDrop.setXConstraint(remainingSpaceRight, remainingSpaceLeft);
	DeckView.createCardImageDragDrop.setYConstraint(remainingSpaceTop, remainingSpaceBottom);
};

/**
 * This is called while the user is dragging the card
 * @param {Object} e
 */
DeckView.createCardImageDragged = function(e)
{
	var deckViewCreateDragArea = document.getElementById("deckViewCreateDragArea");
	var cardImageDragDropCurrentXY = D.xy(deckViewCreateDragArea);
	
	var deckViewCreateCardImage = document.getElementById("deckViewCreateCardImage");
	
	var currentCardID = DeckView.customDeck["currentCard"];
	
	DeckView.customDeck["cards"][currentCardID]["imageLastLeftTop"][0] = (cardImageDragDropCurrentXY[0]-DeckView.createCardImageDragDropStartXY[0]);
	DeckView.customDeck["cards"][currentCardID]["imageLastLeftTop"][1] = (cardImageDragDropCurrentXY[1]-DeckView.createCardImageDragDropStartXY[1]);
	
	deckViewCreateCardImage.style.left = "" + (DeckView.customDeck["cards"][currentCardID]["imageLeftTop"][0]+DeckView.customDeck["cards"][currentCardID]["imageLastLeftTop"][0]) + "px";
	deckViewCreateCardImage.style.top =  "" + (DeckView.customDeck["cards"][currentCardID]["imageLeftTop"][1]+DeckView.customDeck["cards"][currentCardID]["imageLastLeftTop"][1]) + "px";
};

/**
 * This is called when the user lets go of 
 * @param {Object} e
 */
DeckView.createCardImageDragStopped = function(e)
{
	var deckViewCreateDragArea = document.getElementById("deckViewCreateDragArea");
	deckViewCreateDragArea.style.top = "0px";
	deckViewCreateDragArea.style.left = "0px";
	
	var currentCardID = DeckView.customDeck["currentCard"];
	
	var deckViewCreateCardImage = document.getElementById("deckViewCreateCardImage");
	DeckView.customDeck["cards"][currentCardID]["imageLeftTop"][0] = (DeckView.customDeck["cards"][currentCardID]["imageLeftTop"][0]+DeckView.customDeck["cards"][currentCardID]["imageLastLeftTop"][0]);
	DeckView.customDeck["cards"][currentCardID]["imageLeftTop"][1] = (DeckView.customDeck["cards"][currentCardID]["imageLeftTop"][1]+DeckView.customDeck["cards"][currentCardID]["imageLastLeftTop"][1]);
};

/**
 * This is called when the user hits 'Save Deck'
 * @param {Object} e
 */
DeckView.createSaveHandler = function(e)
{
	// Show progress bar
	// Post to saveDeck
	// Save deck needs to: save/read database data, create/update the deck images
	Status.show("Saving Deck...", -1, "saveDeck");
	
	var postOptions = {};
	
	var deckViewCreateDeckName = document.getElementById("deckViewCreateDeckName");
	DeckView.customDeck["name"] = deckViewCreateDeckName.value;

	var deckViewCreateDeckArtistName = document.getElementById("deckViewCreateDeckArtistName");
	DeckView.customDeck["artistName"] = deckViewCreateDeckArtistName.value;

	var deckViewCreateDeckArtistWebsite = document.getElementById("deckViewCreateDeckArtistWebsite");
	DeckView.customDeck["artistWebsite"] = deckViewCreateDeckArtistWebsite.value;
	
	postOptions["name"] = DeckView.customDeck["name"];
	postOptions["public"] = DeckView.customDeck["public"] ? "true" : "false";
	postOptions["artistName"] = DeckView.customDeck["artistName"];
	postOptions["artistWebsite"] = DeckView.customDeck["artistWebsite"];
	postOptions["templateID"] = DeckView.customDeck["templateID"];
	
	if(DeckView.customDeck.hasOwnProperty("deckID"))
		postOptions["existingDeckID"] = DeckView.customDeck["deckID"];
	
	var deckViewCreateWhichCard = document.getElementById("deckViewCreateWhichCard");
	for(var i=0;i<deckViewCreateWhichCard.options.length;i++)
	{
		var cardID = deckViewCreateWhichCard.options[i].value;
		var currentCard = DeckView.customDeck["cards"][cardID];
		
		postOptions[cardID+"templateSizeNum"] = currentCard["templateSizeNum"];
		postOptions[cardID+"image"] = currentCard["image"];
		postOptions[cardID+"imageScale"] = currentCard["imageScale"];
		postOptions[cardID+"imageLeft"] = currentCard["imageLeftTop"][0];
		postOptions[cardID+"imageTop"] = currentCard["imageLeftTop"][1];
	}

	YAHOO.util.Connect.asyncRequest("POST", "saveCustomDeck", { success : DeckView.createSaveFinished, failure: DeckView.createSaveFinished }, O.serialize(postOptions));		
};

/**
 * This is called when the 'Save Deck' operation returns. Either as success or failure
 * @param {Object} data
 */
DeckView.createSaveFinished = function(data)
{
	Status.hide();
	
	if(!data.hasOwnProperty("responseText") || data.responseText===null || data.responseText.length<1)
	{
		alert("An unknown error took place trying to save the deck.")
		return;
	}
	
	if(data.responseText.indexOf("-")===-1 || data.responseText.indexOf("/")===-1)
	{
		alert("An error took place trying to save the deck:\n" + data.responseText);
		return;
	}
	
	var deckViewCreateDecksSelect = document.getElementById("deckViewCreateDecksSelect");

	// Need to update or add it to the select and to our DeckView.customDecks	
	if(DeckView.customDeck.hasOwnProperty("deckID"))
	{
		// Update
		for(var i=0;i<deckViewCreateDecksSelect.options.length;i++)
		{
			var deckOption = deckViewCreateDecksSelect.options[i];
			if(deckOption.value===DeckView.customDeck["deckID"])
			{
				D.clear(deckOption);
				deckOption.appendChild(document.createTextNode(DeckView.customDeck["name"]));
			}
		}
		
		var deckNode = T.getNodeWithDataMatching(DeckView.chooseTree.getRoot(), DeckView.customDeck["deckID"]);
		if(deckNode!==null)
			deckNode.label = DeckView.customDeck["name"];
		
		DeckView.chooseTree.draw();
	}
	else
	{
		// New
		DeckView.customDeck["deckID"] = data.responseText;
		
		DeckView.customDecks.push(DeckView.customDeck);
		
		var deckOption = document.createElement("OPTION");
		deckOption.value = DeckView.customDeck["deckID"];
		deckOption.appendChild(document.createTextNode(DeckView.customDeck["name"]));
		deckViewCreateDecksSelect.appendChild(deckOption);

		deckOption.selected = true;
		deckViewCreateDecksSelect.selectedIndex = deckViewCreateDecksSelect.options.length-1;
		
		// Add to our tree for selection
		if(DeckView.customDecks.length>0)
		{
			var chooseTreeRoot = DeckView.chooseTree.getRoot();
			
			var yourCreationsNode = T.getNodeWithLabelMatching(chooseTreeRoot, "Your Creations");
			if(yourCreationsNode==null)
			{
				yourCreationsNode = new YAHOO.widget.TextNode("Your Creations", chooseTreeRoot, false);
				yourCreationsNode.data = null;
			}
			
			var deckNode = new YAHOO.widget.TextNode(DeckView.customDeck["name"], yourCreationsNode, false);
			deckNode.data = DeckView.customDeck["deckID"];
			
			DeckView.chooseTree.draw();
			T.boldFolders(chooseTreeRoot);
		}
	}
};

/**
 * Will resize the deck choose area as needed
 */
DeckView.resize = function()
{
	if(this.panelHeight===null || this.panelWidth===null)
		return;
	
	var deckViewHeader = document.getElementById("deckViewHeader");
	var deckViewTabsContainer = document.getElementById("deckViewTabsContainer");
	
	var finalHeight = this.panelHeight-(D.height(deckViewHeader))-(D.height(deckViewTabsContainer))-27;

	// Resize Choose tab
	var deckViewChooseLeft = document.getElementById("deckViewChooseLeft");
	deckViewChooseLeft.style.height = finalHeight + "px";
	
	var deckViewChooseRight = document.getElementById("deckViewChooseRight");
	deckViewChooseRight.style.height = finalHeight + "px";
	
	var bottomHeight = D.height(document.getElementById("deckViewChooseOKButton")) + 10;
	
	var deckViewChooseRightBottomRight = document.getElementById("deckViewChooseRightBottomRight");
	deckViewChooseRightBottomRight.style.height = bottomHeight + "px";
	
	var deckViewChooseRightBottomLeft = document.getElementById("deckViewChooseRightBottomLeft");
	deckViewChooseRightBottomLeft.style.height = bottomHeight + "px";
	
	var deckViewChooseRightTop = document.getElementById("deckViewChooseRightTop");
	var deckViewChooseRightTopHeight = D.height(deckViewChooseRightTop);
	
	var deckViewChooseRightMiddle = document.getElementById("deckViewChooseRightMiddle");
	var middleHeight = ((finalHeight-bottomHeight)-deckViewChooseRightTopHeight);
	middleHeight = Math.max(middleHeight, 30);
	deckViewChooseRightMiddle.style.height = "" + middleHeight + "px";

	// Resize create tab
	var deckViewCreateLeft = document.getElementById("deckViewCreateLeft");
	deckViewCreateLeft.style.height = finalHeight + "px";
	
	var deckViewCreateRight = document.getElementById("deckViewCreateRight");
	deckViewCreateRight.style.height = finalHeight + "px";

	var bottomHeight = D.height(document.getElementById("deckViewCreateSaveButton")) + 10;
	
	var deckViewCreateButtonsContainer = document.getElementById("deckViewCreateButtonsContainer");
	deckViewCreateButtonsContainer.style.height = bottomHeight + "px";
	
	var deckViewCreateFieldsTable = document.getElementById("deckViewCreateFieldsTable");
		
	var deckViewCreateCardContainer = document.getElementById("deckViewCreateCardContainer");
	var middleHeight = (((finalHeight)-D.height(deckViewCreateFieldsTable))-15);
	middleHeight = Math.max(middleHeight, 30);
	deckViewCreateCardContainer.style.height = "" + middleHeight + "px";
	deckViewCreateCardContainer.style.lineHeight = "" + middleHeight + "px";
	
	var deckViewCreateCardImageScaleContainerWithLabels = document.getElementById("deckViewCreateCardImageScaleContainerWithLabels");
	deckViewCreateCardImageScaleContainerWithLabels.style.top = "" + ((middleHeight-D.height(deckViewCreateCardImageScaleContainerWithLabels))/2) + "px";
};
