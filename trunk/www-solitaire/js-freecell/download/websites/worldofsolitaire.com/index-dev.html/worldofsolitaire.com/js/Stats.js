/*
 * Stats.js - Game stats including loading/saving
 * Author - robert@cosmicrealms.com
 */

var Stats = null;

(function() {
	var _stats = {};
	var _cookieName = "solitaireStats";
	var _cookieDomain = ".worldofsolitaire.com";
	
	Stats = 
	{
		init : function()
		{
			var existingStatsString = C.get(_cookieName);
			if(existingStatsString!==null && existingStatsString.length>0)
				_stats = O.deserialize(existingStatsString);
		},
		
		/**
	 	 * Returns the option with the specified key
		 * @param {Object} key Option name
		 */
		get : function(key, defaultValue)
		{
			return (_stats.hasOwnProperty(key) ? _stats[key] : defaultValue);	
		},
		
		/**
		 * Sets the given stat
		 * @param {Object} key Option name
		 * @param {Object} value Option value
		 */
		set : function(key, value)
		{
			_stats[key] = value;
			
			C.set(_cookieName, O.serialize(_stats), C.DURATION_DECADE, _cookieDomain);
		},
		
		/**
		 * Increases the given stat by one
		 * @param {Object} key
		 */
		increment : function(key, amount)
		{
			if(typeof amount=="undefined" || amount===null)
				amount = 1;
			
			if(_stats.hasOwnProperty(key))
				this.set(key, (_stats[key]+amount));
		},
		
		/**
		 * Notifies the server of something
		 * @param {Object} key The key to post to
		 * @param {Object} postData Extra data to post
		 */
		notifyServer : function(key, postData)
		{
			if(CONSTANTS.IS_OFFLINE)
				return;
				
			YAHOO.util.Connect.asyncRequest("POST", key, {}, (typeof postData!=="undefined" && postData!==null) ? O.serialize(postData) : null);
		}
	};
})();


