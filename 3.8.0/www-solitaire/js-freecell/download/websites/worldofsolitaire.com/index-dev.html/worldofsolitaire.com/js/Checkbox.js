/* Checkbox.js - robert@cosmicrealms.com */

/**
 * Checkbox object for pretty radios
 */
var Checkbox = 
{
	_IMAGE_CHECKBOX_CHECKED : ((P.getBrowser()=="ie") ? "images/checkbox-checked.gif" : "images/checkbox-checked.png"),
	_IMAGE_CHECKBOX : ((P.getBrowser()=="ie") ? "images/checkbox.gif" : "images/checkbox.png"),
	
	check : function(element)
	{
		element.src = this._IMAGE_CHECKBOX_CHECKED;
	},

	uncheck : function(element)
	{
		element.src = this._IMAGE_CHECKBOX;
	}
};
