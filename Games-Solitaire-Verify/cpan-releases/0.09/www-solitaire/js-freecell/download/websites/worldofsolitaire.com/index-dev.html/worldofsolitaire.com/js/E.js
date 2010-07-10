/*
 * E.js - Event functions
 * Author - robert@cosmicrealms.com
 */

var E =
{
	add : function(element, eventType, eventCallback, eventArgument, eventScope) 
	{
		return YAHOO.util.Event.addListener(element, eventType, eventCallback, eventArgument, eventScope);
	},
	
	remove : function(element, eventType, eventCallback)
	{
		return YAHOO.util.Event.removeListener(element, eventType, eventCallback);
	},
	
	addToName : function(elementName, eventType, eventCallback, eventArgument, eventScope)
	{
		var elements = document.getElementsByName(elementName);
		
		for(var i=0;i<elements.length;i++)
		{
			E.add(elements[i], eventType, eventCallback, ((typeof eventArgument=="undefined") || (eventArgument===null)) ? elements[i] : eventArgument, eventScope);
		}
	},
	
	removeFromName : function(elementName, eventType, eventCallback)
	{
		var elements = document.getElementsByName(elementName);
		
		for(var i=0;i<elements.length;i++)
		{
			E.remove(elements[i], eventType, eventCallback);
		}
	},
	
	stop : function(e)
	{
		YAHOO.util.Event.stopEvent(e);
	},
	
	preventDefault : function(e)
	{
		YAHOO.util.Event.preventDefault(e);
	}
};
