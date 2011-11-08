/*
 * O.js - Object functions
 * Author - robert@cosmicrealms.com
 */

var O = 
{
	clone : function(object)
	{
		var newObject = {};
		
		for(var key in object)
		{
			newObject[key] = object[key];
		}
		
		return newObject;
	},
	
	merge : function(oldObject, newObject, keepOld)
	{
		if(typeof keepOld==="undefined" || keepOld===null)
			keepOld = false;
		
		for(var key in newObject)
		{
			if(!oldObject.hasOwnProperty(key) || keepOld!==true)
				oldObject[key] = newObject[key];
		}
	},
	
	serialize : function(object)
	{
		var result = "";
		
		if(typeof object==="undefined" && object===null)
			return result;
		
		for(var key in object)
		{
			if(result.length>0)
				result += "&";
				
			result += encodeURIComponent(key) + "=" + encodeURIComponent(object[key]);
		}
		
		return result;
	},
	
	deserialize : function(string, convertBooleans, convertNumbers)
	{
		var result = {};
		
		if(typeof string==="undefined" || string===null)
			return result;
			
		if(typeof convertBooleans==="undefined" || convertBooleans===null)
			convertBooleans = true;

		if(typeof convertNumbers==="undefined" || convertNumbers===null)
			convertNumbers = true;

		var parts = string.split("&");
		for(var i=0;i<parts.length;i++)
		{
			var partNameValue = parts[i].split("=");
			var partName = decodeURIComponent(partNameValue[0]);
			var partValue = "";
			if(partNameValue.length>1)
			{
				partValue = decodeURIComponent(partNameValue[1]);
				if(convertBooleans)
					partValue = (partValue==="true" ? true : (partValue==="false" ? false : partValue));
				if(convertNumbers)
				{
					var tryNumber = parseFloat(partValue);
					if(!isNaN(tryNumber))
						partValue = tryNumber;
				}
			}

			result[partName] = partValue;
		}
		
		return result;
	}
};