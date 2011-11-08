/*
 * P.js - Platform functions
 * Author - robert@cosmicrealms.com
 */

var P =
{
    ua : navigator.userAgent.toLowerCase(),
	
	getBrowser : function()
	{
		if(this.ua.indexOf("nintendo wii")!=-1)
			return "wii";
		else if(this.ua.indexOf("opera")!=-1)
			return "opera";
		else if(this.ua.indexOf("msie 7")!=-1)
			return "ie7";
		else if(this.ua.indexOf("msie")!=-1)
			return "ie";
		else if(this.ua.indexOf("safari")!=-1 || this.ua.indexOf("applewebkit")!=-1)
			return "safari";
		else if(this.ua.indexOf("konqueror")!=-1)
			return "konqueror";
		else 
			return "gecko";
	}
};
