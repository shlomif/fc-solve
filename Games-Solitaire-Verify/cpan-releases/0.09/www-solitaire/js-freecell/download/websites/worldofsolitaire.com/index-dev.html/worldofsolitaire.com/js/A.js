/*
 * A.js - Array functions
 * Author - robert@cosmicrealms.com
 */

var A = 
{
	randomize : function(array, times)
	{
		if(typeof times=="undefined" || times===null)
			times = 7;
			
		for(var z=0;z<times;z++)
		{
			for(var i=0;i<array.length;i++)
			{
				var newLoc = I.random(0, (array.length-1));
				var curVal = array[i];
				array[i] = array[newLoc];
				array[newLoc] = curVal;
			}
		}
		
		return array;
	},
	
	find : function(array, match)
	{
		for(var i=0;i<array.length;i++)
		{
			if(array[i]==match)
				return i;
		}
	
		return -1;		
	},
	
	pushMany : function(array, value, count)
	{
		for(var i=0;i<count;i++)
		{
			array.push(value);
		}
		
		return array;
	},
	
	equalTo : function(arrayOne, arrayTwo)
	{
		if(arrayOne.length!=arrayTwo.length)
			return false;
		
		for(var i=0;i<arrayOne.length;i++)
		{
			if(arrayOne[i]!=arrayTwo[i])
				return false;
		}
		
		return true;
	},
	
	clone : function(array)
	{
		var newArray = [];
		
		for(var i=0;i<array.length;i++)
		{
			newArray.push(array[i]);
		}
		
		return newArray;
	},
	
	append : function(array, moreArray)
	{
		for(var i=0;i<moreArray.length;i++)
		{
			array.push(moreArray[i]);
		}
		
		return array;
	},
	
	pushSequence : function(array, startAt, endAt)
	{
		if(endAt>startAt)
		{
			for(var i=startAt;i<=endAt;i++)
			{
				array.push(i);
			}
		}
		else if(endAt<startAt)
		{
			for(var i=endAt;i>=startAt;i--)
			{
				array.push(i);
			}
		}
		else
		{
			array.push(startAt);
		}
		
		return array;
	},
	
	sum : function(array)
	{
		for(var i=0,sum=0;i<array.length;sum+=array[i],i++)
		;
		
		return sum;
	}
};