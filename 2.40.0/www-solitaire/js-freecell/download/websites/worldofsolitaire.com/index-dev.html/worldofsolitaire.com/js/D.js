/*
 * D.js - DOM functions
 * Author - robert@cosmicrealms.com
 */

var D =
{
	clearEmpty : function(element, recurseLevel)
	{
		if(element && element.childNodes && element.childNodes.length && element.childNodes.length>0)
		{
			for(var i=element.childNodes.length-1;i>=0;i--)
			{
				var targetElement = element.childNodes[i];
				if(targetElement.nodeName=="#text" && S.isWhiteSpace(targetElement.data))
					element.removeChild(targetElement);
				else if(targetElement.nodeName=="#comment")
					element.removeChild(targetElement);
	
				if(typeof recurseLevel==="undefined" || recurseLevel===null || recurseLevel>0)
					D.clearEmpty(targetElement, ((typeof recurseLevel!=="undefined" && recurseLevel!==null) ? recurseLevel-1 : null));
			}
		}
	},
	
	clear : function(element)
	{
		if(!element)
			return;
	
		for(var i=element.childNodes.length-1;i>=0;i--)
		{
			element.removeChild(element.childNodes[i]);		
		}
	},
	
	xy : function(element)
	{
		return YAHOO.util.Dom.getXY(element);
	},
	
	region: function(element)
	{
		return YAHOO.util.Dom.getRegion(element);
	},
	
	width : function(element)
	{
		var elementRegion = YAHOO.util.Dom.getRegion(element);
		
		return elementRegion===false || !elementRegion.hasOwnProperty("right") || !elementRegion.hasOwnProperty("left") ? 0 : elementRegion.right - elementRegion.left;
	},
	
	height : function(element)
	{
		var elementRegion = YAHOO.util.Dom.getRegion(element);
		
		return elementRegion===false || !elementRegion.hasOwnProperty("bottom") || !elementRegion.hasOwnProperty("top") ? 0 : elementRegion.bottom - elementRegion.top;
	},
	
	center : function(element)
	{
		var elementWidth = D.width(element);
		var elementHeight = D.height(element);
		var viewportWidth = YAHOO.util.Dom.getViewportWidth();
		var viewportHeight = YAHOO.util.Dom.getViewportHeight();
		
		YAHOO.util.Dom.setXY(element, [ ((viewportWidth-elementWidth)/2), ((viewportHeight-elementHeight)/2)]);
	},
	
	getElementsByTagName : function(element, tagName)
	{
		if(!element)
			return [];
			
		var matchedElements = [];
			
		for(var i=0;i<element.childNodes.length;i++)
		{
			if(element.childNodes[i].nodeName===tagName)
				matchedElements.push(element.childNodes[i]);
		}
		
		return matchedElements;
	},
	
	getElementsByClassName : function(element, tagName, className)
	{
		if(!element)
			return [];
			
		var matchedElements = [];
		
		var containerTags = element.getElementsByTagName(tagName);
		for(var i=0;i<containerTags.length;i++)
		{
			if(containerTags[i].className==className)
				matchedElements.push(containerTags[i]);
		}
		
		return matchedElements;
	},
	
	get : function(argument)
	{
		return YAHOO.util.Dom.get(argument);
	}
};
