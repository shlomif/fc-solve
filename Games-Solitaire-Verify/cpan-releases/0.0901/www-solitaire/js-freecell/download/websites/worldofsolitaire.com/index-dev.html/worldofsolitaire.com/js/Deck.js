/* Deck.js - robert@cosmicrealms.com */

var DECKINFO =
{
	"paris" : {
		"name"    : "Paris",
		"artistName" : "Public Domain",
		"artistWebsite" : "http://rahga.com/svg/",
		"peekX"   : 0.10,
		"sizes"   : [ "39x60", "54x84", "75x116", "99x154", "128x199", "141x219", "316x492", "46x71", "64x99", "87x135", "160x249", "172x267", "184x286", "196x305", "212x330", "230x358", "246x383", "262x407", "282x439", "300x467" ] },
	"ornamental" : {
		"name"    : "Ornamental",
		"artistName" : "Nicu Buculei",
		"artistWebsite" : "http://nicubunu.ro/cards/",
		"peekX"   : 0.14,
		"peekY"   : 0.15,
		"sizes"   : [ "39x53", "54x73", "75x101", "99x134", "128x174", "141x191", "417x567", "46x62", "64x87", "87x118", "32x43", "25x33", "22x29", "160x217", "172x233", "184x250", "196x266", "212x288", "230x312", "246x334", "262x356", "282x383", "300x407" ] },
	"dondorf" : {
		"name"    : "Dondorf's \"Swiss Costumes\"",
		"artistName" : "Public Domain",
		"artistWebsite" : "http://rahga.com/svg/",
		"peekX"   : 0.10,
		"hasFlip" : true,
		"sizes"   : [ "39x60", "54x84", "75x116", "99x154", "128x199", "141x219", "158x246", "46x71", "64x99", "87x135" ] },
	"oxygen" : {
		"name"    : "Oxygen",
		"artistName" : "KDE",
		"artistWebsite" : "http://games.kde.org",
		"peekX"   : 0.10,
		"sizes"   : [ "39x54", "54x75", "75x105", "99x138", "128x179", "141x197", "302x423", "46x64", "64x89", "87x121", "160x224", "172x240", "184x257", "196x274", "212x296", "230x322", "246x344", "262x366", "282x394" ] },
	"oxygenwhite" : {
		"name"    : "Oxygen - White",
		"artistName" : "KDE",
		"artistWebsite" : "http://games.kde.org",
		"peekX"   : 0.10,
		"sizes"   : [ "39x54", "54x75", "75x105", "99x138", "128x179", "141x197", "302x423", "46x64", "64x89", "87x121", "160x224", "172x240", "184x257", "196x274", "212x296", "230x322", "246x344", "262x366", "282x394" ] },
	"anglo" : {
		"name"    : "Anglo-American",
		"artistName" : "Gnome",
		"artistWebsite" : "http://www.gnome.org/projects/gnome-games/",
		"peekX"   : 0.10,
		"sizes"   : [ "39x60", "54x84", "75x116", "99x154", "128x199", "141x219", "405x630", "46x71", "64x99", "87x135", "160x249", "172x267", "184x286", "196x305", "212x330", "230x358", "246x383", "262x407", "282x439", "300x467" ] },
	"majestic" : {
		"name"	  : "Majestic",
		"artistName" : "Holly Bell",
		"artistWebsite" : "http://www.wild-melody.com/",
		"peekX"   : 0.10,
		"sizes"   : [ "39x53", "54x74", "75x103", "99x136", "128x176", "141x193", "652x897", "46x63", "64x88", "87x119", "160x220", "172x236", "184x253", "196x269", "212x291", "230x316", "246x338", "262x360", "282x387", "300x412" ] },
	"animals" : {
		"name"	  : "Animals",
		"artistName" : "Meo-Tor",
		"artistWebsite" : "http://meo-toronto.sitesled.com/",
		"peekX"   : 0.10,
		"sizes"   : [ "39x60", "54x84", "75x116", "99x154", "128x199", "141x219", "560x871", "46x71", "64x99", "87x135", "160x249", "172x267", "184x286", "196x305", "212x330", "230x358", "246x383", "262x407", "282x439", "300x467" ] },
	"template_paris" : {
		"name"	  : "Template - Paris",
		"artistName" : "",
		"artistWebsite" : "",
		"peekX"   : 0.10,
		"sizes"   : [ "39x60", "54x84", "75x116", "99x154", "128x199", "141x219", "316x492", "46x71", "64x99", "87x135", "160x249", "172x267", "184x286", "196x305", "212x330", "230x358", "246x383", "262x407", "282x439", "300x467" ] }
};

//var gAllDeckURLS = {};

function Deck(deckID)
{
	this._deckID = deckID;
	this._deckIDCustomTag = null;
	this._deckName = "";
	this._deckArtistName = "";
	this._deckArtistWebsite = "";
	this._deckSizes = [];
	this._loadFinishHandler = null;
	this._preloadImageList = [];
	this._preloadFinishHandler = null;
	this._preloadCardCount = 0;
	this._preloadDeckMaxSize = null;
	this._hasFlip = false;
	this._peekX = 0.20;
	this._peekY = 0.10;

 	/**
 	 * Begins loading this deck
 	 * @param {Object} loadFinishHandler Function to call when loading is complete
 	 */
	this.loadDeck = function()
	{
		var self = this;
		
		if(this._deckID.indexOf("template")===0 && this._deckID.indexOf("-")!=-1)
		{
			this._deckIDCustomTag = this._deckID.substring(this._deckID.indexOf("-")+1);
			this._deckID = this._deckID.substring(0, this._deckID.indexOf("-"));
		}
		
		var deckInfo = DECKINFO[this._deckID];
		
		this._deckName = deckInfo["name"];
		this._deckArtistName = deckInfo["artistName"];
		this._deckArtistWebsite = deckInfo["artistWebsite"];
		
		if(deckInfo.hasOwnProperty("hasFlip") && deckInfo["hasFlip"]===true)
			this._hasFlip = true;
		
		if(deckInfo.hasOwnProperty("peekX"))
			this._peekX = deckInfo["peekX"];

		if(deckInfo.hasOwnProperty("peekY"))
			this._peekY = deckInfo["peekY"];
		
		for(var i=0;i<deckInfo["sizes"].length;i++)
		{
			var deckInfoSize = deckInfo["sizes"][i];
			var sizeWidthHeight = deckInfoSize.split("x");
			if(sizeWidthHeight.length!=2 || sizeWidthHeight[0].length<1 || sizeWidthHeight[1].length<1)
			{
				this._deckSizes.push({});
				continue;
			}
				
			var cardURLs = CARD.ids.slice(0);
			var moreCardURLs = [];
			for(var z=0;z<cardURLs.length;z++)
			{
				var cardNumber = CARD.number(cardURLs[z]);
				if((cardNumber===11 || cardNumber===12 || cardNumber===13) && this._hasFlip===true)
					moreCardURLs.push(this.createCardURL(cardURLs[z], i, this._hasFlip));
				cardURLs[z] = this.createCardURL(cardURLs[z], i);
			}
			if(moreCardURLs.length>0)
				cardURLs = A.append(cardURLs, moreCardURLs);
	
			this._deckSizes.push({ "size" : sizeWidthHeight, "cardURLs" : cardURLs, "isPreloaded" : false });
		}
	};
	
	/**
	 * Begins preloading of the given sizeNumber
	 * @param {Object} sizeNumber The size number to preload
	 */
	this.preloadDeckSize = function(sizeNumber, cardScale, preloadFinishHandler)
	{
		var self = this;
		
		if(typeof preloadFinishHandler!=="undefined")
			this._preloadFinishHandler = preloadFinishHandler;
		else
			this._preloadFinishHandler = null;
		
		if(this._deckSizes.length<=sizeNumber)
			return;
			
		var deckSizeInfo = this._deckSizes[sizeNumber];
		
		Status.show("Loading", -1, "preloadDeck");
		
		if(deckSizeInfo["isPreloaded"])
		{
			Status.hide("preloadDeck");
			
			if(this._preloadFinishHandler!==null)
				setTimeout(function() { self._preloadFinishHandler(); }, 0);
			return;
		}

		this._preloadDeckMaxSize = deckSizeInfo["cardURLs"].length;
		
		deckSizeInfo["isPreloaded"] = true;

		// We have a valid deck size
		this._preloadCardCount = 0;
		
		var imageObjects = [];
		
		for(var m=0;m<this._preloadDeckMaxSize;m++)			
		{
			imageObjects[m] = new Image();
			imageObjects[m].onload = function() { self.updatePreloadStatus(); };
			imageObjects[m].onerror = function() { self.updatePreloadStatus(); };
			imageObjects[m].onabort = function() { self.updatePreloadStatus(); };
			
			imageObjects[m].src = deckSizeInfo["cardURLs"][m];
		}

		setTimeout(function() { self.preloadDeckTooLongTimerElapsed(); }, 5000);
	};
	
	/**
	 * Called often to see if our images have been preloaded or not yet
	 */
	this.updatePreloadStatus = function()
	{
		var self = this;
		
		this._preloadCardCount ++;

		if(this._preloadCardCount==this._preloadDeckMaxSize)
		{
			Status.show("Loading", -1, "preloadDeck");
			
			setTimeout(function() { self.preloadDeckImagesFinished(); }, 0);
			return;
		}

		Status.update("", this._preloadCardCount);
	};
	
	/**
	 * Called after some time elapsed and a real progress bar is shown
	 */
	this.preloadDeckTooLongTimerElapsed = function()
	{
		if(this._preloadCardCount<this._preloadDeckMaxSize)
		{
			Status.show("Loading", this._preloadDeckMaxSize, "preloadDeck");
			Status.update("", this._preloadCardCount);
		}
	};
	
	/**
	 * Called after a slight delay when all images have finished
	 */
	this.preloadDeckImagesFinished = function()
	{
		var self = this;
		
		Status.hide("preloadDeck");
				
		if(this._preloadFinishHandler!==null)
			setTimeout(function() { self._preloadFinishHandler(); }, 0);
	};
		
	/**
	 * Creates a cardURL For this deck given the cardID and a sizeNumber
	 * @param {Object} cardID
	 * @param {Object} sizeNumber
	 */
	this.createCardURL = function(cardID, sizeNumber, flip)
	{
		if(typeof sizeNumber=="undefined" || sizeNumber===null)
			sizeNumber = 0;
		
		var cardNumber = CARD.number(cardID);
		
		var deckStartPath = "decks/" + this._deckID;
		if(this._deckIDCustomTag!=null)
		{
			deckStartPath = "customdecks/" + this._deckIDCustomTag;
		}
		
		var cardURL = deckStartPath + "/" + sizeNumber + "/" + ((typeof flip!="undefined" && flip===true && this._hasFlip===true && (cardNumber===11 || cardNumber===12 || cardNumber===13)) ? "f" : "") + cardID + (P.getBrowser()=="ie" ? ".gif" : ".png");
		
		if(P.getBrowser()=="gecko")
		{
			//gAllDeckURLS[cardURL] = cardURL + "?z=" + md5(cardURL);
			if(geckoCachedCardURLs.hasOwnProperty(cardURL))
			{
				cardURL = geckoCachedCardURLs[cardURL];
			}
			else
			{
				//alert("cardURL [" + cardURL + "] does not have a gecko cached card url");
				cardURL += "?z=" + md5(cardURL);
			}
		}
		
		return cardURL;		
	};
	
	/**
	 * Get's a pixel width for this card given a sizeNumber and a scale percentage
	 * @param {Object} sizeNumber
	 * @param {Object} scale
	 */
	this.getCardWidth = function(sizeNumber, scale)
	{
		if(typeof scale=="undefined" || scale===null)
			scale = 1.00;
			
		return Math.floor((parseFloat(this._deckSizes[sizeNumber]["size"][0])*parseFloat(scale)));
	};

	/**
	 * Get's a pixel height for this card given a sizeNumber and a scale percentage
	 * @param {Object} sizeNumber
	 * @param {Object} scale
	 */
	this.getCardHeight = function(sizeNumber, scale)
	{
		if(typeof scale=="undefined" || scale===null)
			scale = 1.00;
			
		return Math.floor((parseFloat(this._deckSizes[sizeNumber]["size"][1])*parseFloat(scale)));
	};
	
	/**
	 * Returns a size number that is best suited for the passed in an idealHeight
	 * @param {Object} idealHeight
	 */
	this.calculateSizeNumber = function(idealHeight)
	{
		idealHeight = Math.floor(idealHeight);
		//console.log("Ideal Height: %d", idealHeight);
		
		var winningSize = 0;
		var smallestDifference = this._deckSizes[0]["size"][1]-idealHeight;
		if(smallestDifference<0)
			smallestDifference = 65535;
		
		//console.log("Starting smallestDifference: %d", smallestDifference);
		
		for(var i=1;i<this._deckSizes.length;i++)
		{
			var deckSize = this._deckSizes[i];
			var testDifference = (Options.get("optionCardSize")=="largest") ? this._deckSizes[i]["size"][1]-idealHeight : idealHeight-this._deckSizes[i]["size"][1];
			//console.log("%d (%d) testDifference %d", i, this._deckSizes.length, testDifference);
			if(testDifference<0)
				testDifference = 65535;
			
			if(testDifference<smallestDifference)
			{
				winningSize = i;
				smallestDifference = (Options.get("optionCardSize")=="largest") ? this._deckSizes[i]["size"][1]-idealHeight : idealHeight-this._deckSizes[i]["size"][1];
			}
		}
		
		//console.log("Winning Size: %d", winningSize);
		
		return winningSize;
	};
	
	this.fitInside = function(idealCardHeight, idealCardWidth)
	{
		var deck = this;
		var deckSizeNum = -1;	
		var cardScale=1.00;
		var lowestHeightDiff = 99999;
		
		for(var i=0;i<DECKINFO[this._deckID]["sizes"].length;i++)
		{
			var cardHeight = deck.getCardHeight(i);
			var cardWidth = deck.getCardWidth(i);
			
			if(cardHeight>idealCardHeight || cardWidth>idealCardWidth)
				continue;
			
			var heightDiff = idealCardHeight-cardHeight;
			var widthDiff = idealCardWidth-cardWidth;
			
			if(heightDiff<lowestHeightDiff)
			{
				lowestHeightDiff = heightDiff;
				deckSizeNum = i;
			}
		}
		
		if(deckSizeNum==-1)
		{
			for(var i=0;i<DECKINFO[this._deckID]["sizes"].length;i++)
			{
				var cardHeight = deck.getCardHeight(i);
				var cardWidth = deck.getCardWidth(i);
				
				var heightDiff = cardHeight-idealCardHeight;
				var widthDiff = cardWidth-idealCardWidth;
				
				if(heightDiff<lowestHeightDiff)
				{
					lowestHeightDiff = heightDiff;
					deckSizeNum = i;
				}
			}
			
			var cardHeight = deck.getCardHeight(deckSizeNum, cardScale);
			while(cardHeight>idealCardHeight)
			{
				cardScale-=0.05
				cardHeight = deck.getCardHeight(deckSizeNum, cardScale);
			}
		}
		
		var results = {};
		results["sizeNum"] = deckSizeNum;
		results["scale"] = cardScale;
		
		return results;
	};
}
