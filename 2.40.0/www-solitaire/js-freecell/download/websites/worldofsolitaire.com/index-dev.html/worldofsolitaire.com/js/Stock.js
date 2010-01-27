/* Stock - robert@cosmicrealms.com */


var Stock = 
{
	/**
	 * Creates a standard stock of 52 randomized cards
	 */
	createStandard : function(count, validSuits)
	{
		var stock = [];
		
		if(typeof count=="undefined" || count===null)
			count = 1;
		
		for(var i=0;i<count;i++)
		{
			for(var m=0;m<52;m++)
			{
				var cardID = CARD.ids[m];
				if(typeof validSuits==="undefined" || validSuits.length===0 || validSuits.indexOf(CARD.suit(cardID))!=-1)
					stock.push(new Card(cardID));
			}
		}
						
		return A.randomize(stock, 7*(stock.length/52));
	}
};


