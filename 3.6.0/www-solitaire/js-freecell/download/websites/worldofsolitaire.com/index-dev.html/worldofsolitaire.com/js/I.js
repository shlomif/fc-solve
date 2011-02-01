/*
 * I.js - Integer functions
 * Author - robert@cosmicrealms.com
 */

var I =
{
	random : function(min, max)
	{
		if(min===max)
			return min;
			
		if(typeof min=="undefined" || min===null)
			min = 0;

		if(typeof max=="undefined" || max===null)
			max = min+1;
		
		return Math.floor(Math.random() * (max - min + 1)) + min;
	},
	
	secondsAsClock : function(secondsElapsed, omitMinutes)
	{
		var clockText = "";
		if(secondsElapsed>=(60*60))
		{
			var hourText = "" + parseInt((secondsElapsed / (60*60)), 10);
			clockText += hourText + ":";
			secondsElapsed %= (60*60);
			secondsElapsed = parseInt(secondsElapsed, 10);
		}
		
		if(secondsElapsed>=(60))
		{
			var minuteText = "" + parseInt((secondsElapsed / 60), 10);
			clockText += (minuteText.length===0 ? "00" : (minuteText.length==1) ? "0" : "") + minuteText + ":";
			secondsElapsed %= 60;
			secondsElapsed = parseInt(secondsElapsed, 10);
		}
		else if(typeof omitMinutes==="undefined" || omitMinutes!==true)
			clockText += "00:";
		
		var secondText = "" + secondsElapsed;
		clockText += (secondText.length===0 ? "00" : (secondText.length==1) ? "0" : "") + secondText;
	
		return clockText;
	},
	
	secondsAsHumanReadable : function(secondsElapsed, includeTrailingZeros)
	{
		var clock = I.secondsAsClock(secondsElapsed, true);
		var clockParts = clock.split(":");
		var humanText = "";
		
		if(typeof includeTrailingZeros=="undefined")
			includeTrailingZeros = false;
			
		if(clockParts.length===3)
		{
			var part = parseInt(clockParts[0], 10);
			if(isNaN(part))
				part = 0;
			if(part>8760)
			{
				part /= 8760;
				part = Math.floor(part);
				if(isNaN(part))
					part = 0;
				humanText += I.formatWithCommas(part) + " year" + (part>1 || part===0 ? "s, " : ", ");
				
				part = parseInt(clockParts[0], 10);
				if(isNaN(part))
					part = 0;
				part %= 8760;
				part /= 24;
				part = Math.floor(part);
				if(isNaN(part))
					part = 0;
				humanText += I.formatWithCommas(part) + " day" + (part>1 || part===0 ? "s" : "");
			}
			else if(part>24)
			{
				part /= 24;
				part = Math.floor(part);
				if(isNaN(part))
					part = 0;
				humanText += I.formatWithCommas(part) + " day" + (part>1 || part===0 ? "s, " : ", ");
				
				part = parseInt(clockParts[0], 10);
				if(isNaN(part))
					part = 0;
				part %= 24;
				part = Math.floor(part);
				if(isNaN(part))
					part = 0;
				humanText += I.formatWithCommas(part) + " hour" + (part>1 || part===0 ? "s" : "");
			}
			else
			{
				humanText += I.formatWithCommas(part) + " hour" + (part>1 || part===0 ? "s, " : ", ");	
				
				part = parseInt(clockParts[1], 10);
				if(isNaN(part))
					part = 0;
				humanText += I.formatWithCommas(part) + " minute" + (part>1 || part===0 ? "s" : "");		
			}
		}
		else if(clockParts.length===2)
		{
			var part = parseInt(clockParts[0], 10);
			if(isNaN(part))
				part = 0;
			humanText += I.formatWithCommas(part) + " minute" + (part>1 || part===0 ? "s, " : ", ");
			
			part = parseInt(clockParts[1], 10);
			if(isNaN(part))
				part = 0;
			humanText += I.formatWithCommas(part) + " second" + (part>1 || part===0 ? "s" : "");
		}
		else if(clockParts.length===1)
		{
			var part = parseInt(clockParts[0], 10);
			if(isNaN(part))
				part = 0;
			humanText += I.formatWithCommas(part) + " second" + (part>1 || part===0 ? "s" : "");
		}

		return humanText;
	},
	
	formatWithCommas : function(num)
	{
		var result = "" + num;
		
		if(result.indexOf(",")!==-1 || result.length<4)
			return result;
		
		var reversedResult = S.reverse(result);
		var result = "";
		
		for(var i=0;i<reversedResult.length;i++)
		{
			if(i>0 && i%3===0)
				result += ",";
			
			result += reversedResult.charAt(i);
		}
		
		return S.reverse(result);
	}
};
