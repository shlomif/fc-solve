/*
 * Options.js - Game options including loading/saving
 * Author - robert@cosmicrealms.com
 */

var Options = null;

(function() {
	var _options = {};
	var _cookieName = "solitaireOptions";
	var _cookieDomain = ".worldofsolitaire.com";
	
	Options = 
	{
		init : function()
		{
			Options.refresh();
		},
		
		/**
		 * Will refresh the internal _options object from cookie value
		 */
		refresh : function()
		{
			var existingOptionsString = C.get(_cookieName);
			if(existingOptionsString!==null && existingOptionsString.length>0)
				_options = O.deserialize(existingOptionsString);
		},
		
		/**
	 	 * Returns the option with the specified key
		 * @param {Object} key Option name
		 */
		get : function(key, defaultValue)
		{
			return (_options.hasOwnProperty(key) ? _options[key] : defaultValue);	
		},
		
		/**
		 * Sets the given option
		 * @param {Object} key Option name
		 * @param {Object} value Option value
		 */
		set : function(key, value, skipCookie)
		{
			_options[key] = value;
			
			if(typeof skipCookie=="undefined" || skipCookie!==true)
				C.set(_cookieName, O.serialize(_options), C.DURATION_DECADE, _cookieDomain);
		}
	};
})();


