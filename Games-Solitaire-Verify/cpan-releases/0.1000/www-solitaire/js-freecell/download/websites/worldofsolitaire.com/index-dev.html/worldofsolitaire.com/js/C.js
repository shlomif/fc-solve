/*
 * C.js - Cookie functions
 * Author - robert@cosmicrealms.com
 */

var C =
{
	DURATION_MINUTE : (60),
	DURATION_HOUR : ((60)*60),
	DURATION_DAY : (((60)*60)*24),
	DURATION_MONTH : ((((60)*60)*24)*31),
	DURATION_YEAR : ((((60)*60)*24)*365),
	DURATION_DECADE : (((((60)*60)*24)*365)*10),
	
	set : function(name, value, duration, domain)
	{
		var cookieText = encodeURIComponent(name) + "=" + encodeURIComponent(value) + ";";

		if(typeof duration!=='undefined' && duration!==null)
		{
			var rightNow = new Date();
			var durationDate = new Date();
			durationDate.setTime(rightNow.getTime()+(duration*1000));
			
			cookieText += " expires=" + durationDate.toGMTString() +";";
		}
		
		cookieText += " path=/;";
		
		if(typeof domain!=='undefined' && domain!==null)
			cookieText += " domain=" + domain + ";";
		
		document.cookie = cookieText;
	},
	
	unset : function(name, domain)
	{
		var cookieText = encodeURIComponent(name) + "=;";
		cookieText += " expires=Fri, 31 Dec 1999 23:59:59 GMT;";
		cookieText += " path=/;";
		
		if(typeof domain!=='undefined' && domain!==null)
			cookieText += " domain=" + domain + ";";
		
		document.cookie = cookieText;
	},
	
	get : function(name)
	{
		var cookies = document.cookie.split("; ");
		
		for(var i=0;i<cookies.length;i++)
		{
			if(cookies[i].length<1)
				continue;
				
			var cookieParts = cookies[i].split("=");
			if(cookieParts[0]==encodeURIComponent(name))
				return decodeURIComponent(cookieParts[1]);
		}
		
		return null;
	},
	
	isSet : function(name)
	{
		if(C.get(name)!==null)
			return true;
		
		return false;
	},
	
	areCookiesEnabled : function(domain)
	{
		C.set("areCookiesEnabledTest", "123", C.DURATION_HOUR, ((typeof domain!=="undefined") ? domain : null));
		var value = C.get("areCookiesEnabledTest");
		C.unset("areCookiesEnabledTest", ((typeof domain!=="undefined") ? domain : null));
		
		if(value==="123")
			return true;
		
		return false;
	}
};
