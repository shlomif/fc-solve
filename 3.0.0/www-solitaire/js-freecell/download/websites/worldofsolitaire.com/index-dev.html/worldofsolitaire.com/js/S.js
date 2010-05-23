/*
 * S.js - String functions
 * Author - robert@cosmicrealms.com
 */

var S =
{
	isWhiteSpace : function(text)
	{
		var onlyWhitespace = true;
		
		for(var i=0;i<text.length;i++)
		{
			if(text.charAt(i)=='\t')
				continue;
			if(text.charAt(i)=='\n')
				continue;
			if(text.charAt(i)=='\r')
				continue;
			if(text.charAt(i)==' ')
				continue;
			if(text.charCodeAt(i)==160)	// nbsp
				continue;
			
			onlyWhitespace = false;
		}
		
		return onlyWhitespace;	
	},

	dirname : function(text)
	{
		return text.replace(/(.*)\/.*$/, "$1");
	},
	
	basename : function(text, suffix)
	{
		var result = text.replace(/.*\//, "");
		
		if(suffix && S.endswith(result, suffix))
			return result.substring(0, (result.length-suffix.length));
		
		return result;
	},
	
	endswith : function(text, match)
	{
		text = S.reverse(text);
		match = S.reverse(match);
	
		if(text.indexOf(match)===0)
			return true;
	
		return false;	
	},
	
	reverse : function(text)
	{
		var i = text.length;
		var result = "";
	
		while(i>0)
		{
			result += text.substring((i-1), i);
			i--;
		}
	
		return result;	
	},
	
	lastIndexOf : function(text, match)
	{
		var lastLoc = S.reverse(text).indexOf(match);
		if(lastLoc==-1)
			return -1;
		
		return text.length-lastLoc;
	},
	
	repeat : function(text, count)
	{
	    count = parseInt(count, 10);
	    
	    if(isNaN(count) || !count)
	        count = 0;
	    
		var result="";
		for(var i=0;i<count;i++)
		{
			result+=text;
		}
	    
	    return result;	
	},
	
	trim : function(text, chars)
	{
		if(!chars)
			chars = " \t\n\r\u00a0";	// 00a0 is hex for 160 which is nbsp
		
		var c;
		var i;
		for(i=0,c=text.charAt(i);i<text.length && chars.indexOf(c)!=-1;i++,c=text.charAt(i))
		{
		}
		
		text = text.substring(i);
		
		for(i=(text.length-1),c=text.charAt(i);i>=0 && chars.indexOf(c)!=-1;i--,c=text.charAt(i))
		{
		}
		
		text = text.substring(0, (i+1));
		
		return text;	
	},
	
	replaceAll : function(text, find, replaceWith)
	{
		var replaceAllExpression = new RegExp(find, "g");
		
		return text.replace(replaceAllExpression, replaceWith);
	},
	
	dump : function(text)
	{
		var dumpString = "" + text.length + " [";

		for(var i=0;i<text.length;i++)
		{
			dumpString += "(" + text.charCodeAt(i) + ") " + text.charAt(i) + " \n";
		}
					
		dumpString += "]";
						
		return dumpString;
	}
};
