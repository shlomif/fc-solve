/* Radio.js - robert@cosmicrealms.com */

/**
 * Radio object for pretty radios
 */
var Radio = 
{
	_IMAGE_RADIO_CHECKED : ((P.getBrowser()=="ie") ? "images/radio-checked.gif"  : "images/radio-checked.png"),
	_IMAGE_RADIO : ((P.getBrowser()=="ie") ? "images/radio.gif" : "images/radio.png"),
	
	checkMatchingValue : function(name, value)
	{
		var elements = document.getElementsByName(name);
		
		var checkedRadio = null;
		for(var i=0;i<elements.length;i++)
		{
			if(elements[i].value===value)
				checkedRadio = elements[i];
				
			elements[i].src = ((elements[i].value===value) ? this._IMAGE_RADIO_CHECKED : this._IMAGE_RADIO);
		}
		
		return checkedRadio;
	},
	
	check : function(element)
	{
		var elements = document.getElementsByName(element.name);
		for(var i=0;i<elements.length;i++)
		{
			elements[i].src = ((elements[i]===element) ? this._IMAGE_RADIO_CHECKED : this._IMAGE_RADIO);
		}
	}
};
