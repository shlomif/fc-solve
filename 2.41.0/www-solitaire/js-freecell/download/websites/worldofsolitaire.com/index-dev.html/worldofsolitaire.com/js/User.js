/*
 * User.js - User data
 * Author - robert@cosmicrealms.com
 */

var User = null;

(function() {
	var _user = { userID : 0 };
	var _isLoggedIn = false;
	var _cookieName = "solitaireUser";
	var _cookieDomain = ".worldofsolitaire.com";
	var _registerUser = null;
	var _loginUser = null;
	var _originalUser = null;
	var _usernameInfoTooltip;
	var _lastOptionUpdate = null;
	var _saveUserOptionInterval = 2000;
	
	User = 
	{
		init : function()
		{
			if(!C.areCookiesEnabled())
			{
				var usernameInfoValue = document.getElementById("usernameInfoValue");
				usernameInfoValue.appendChild(document.createTextNode("cookies are disabled"));
				
				return;
			}
			
			this.refresh();
			
			var tooltipOptions = { context   		: "usernameInfo",
								   showdelay 		: 100,
								   hidedelay 		: 200,
								   autodismissdelay : 15000,
								   zIndex			: 901,
								   text      	    : "Registering ensures your statistics are kept forever.<br>" +
								   					  "It also allows you to carry your statistics to different computers." };
			_usernameInfoTooltip = new YAHOO.widget.Tooltip("_usernameInfoTooltip", tooltipOptions);

			E.add("usernameInfoLogoutLink", "click", User.handleLogout, null, User);
			E.add("usernameInfoLoginLink", "click", User.handleLoginUser, null, User);
			E.add("usernameInfoRegisterLink", "click", User.handleRegisterUser, null, User);
			
			_registerUser = new DialogBox("registerUser");
			E.add("registerUserSubmit", "click", User.handleRegisterUserSubmit, null, User);
			E.add("registerUserUsername", "keyup", User.updateRegisterUserDialog, null, User);
			E.add("registerUserPassword", "keyup", User.updateRegisterUserDialog, null, User);
			E.add("registerUserPasswordVerify", "keyup", User.updateRegisterUserDialog, null, User);
			E.add("registerUserEmail", "keyup", User.updateRegisterUserDialog, null, User);
			_registerUser.onEnterPress = function() { User.handleRegisterUserSubmit(); };
			
			_loginUser = new DialogBox("loginUser");
			E.add("loginUserSubmit", "click", User.handleLoginUserSubmit, null, User);
			E.add("loginUserUsername", "keyup", User.updateLoginUserDialog, null, User);
			E.add("loginUserPassword", "keyup", User.updateLoginUserDialog, null, User);
			_loginUser.onEnterPress = function() { User.handleLoginUserSubmit(); };

			var userID = User.get("userID", null);
			var username = User.get("username", null);
			var password = User.get("password", null);		
			var psession = User.get("psession", null);		
			var hash = User.get("hash", null);

			if(username!==null && username.length>0 &&
			   password!==null && password.length>0)
			{
				User.login(username, password);
			}
			else if(username!==null && username.length>0 &&
					psession!==null && psession.length>0)
			{
				User.loginWithPSession(username, psession);
			}
			else
			{
				if(userID===null || hash===null || hash.length!=64)
				{
					User.set("userID", 0, true);
					userID = 0;
					C.unset(_cookieName, _cookieDomain);
				}
				
				if(userID===0)
				{
					User.getNewUserID();
				}
				else
				{
					User.updateHeader();
				}
			}
		},
		
		/**
		 * Will get a new user ID
		 */
		getNewUserID : function(callbackFunction)
		{
			YAHOO.util.Connect.asyncRequest("POST", "newUser", { success  : User.getNewUserIDSuccess,
																 failure  : User.getNewUserIDFailure,
																 argument : typeof callbackFunction=="undefined" ? null : callbackFunction });
		},
		
		/**
		 * This is called when getting a new user id succeed
		 * @param {Object} data
		 */
		getNewUserIDSuccess : function(data)
		{
			var responseData = { userID : 0, hash : 0 };
			
			if(typeof data!=="undefined" && data!==null &&
			   data.hasOwnProperty("responseText") &&
			   data["responseText"]!==null && data["responseText"].length>0)
				responseData = O.deserialize(data["responseText"]);
			
			var userID = 0;
			if(responseData.hasOwnProperty("userID"))
				userID = parseInt(responseData["userID"], 10);
			if(isNaN(userID))
				userID = 0;
			User.set("userID", userID);
			
			if(responseData.hasOwnProperty("hash"))
				User.set("hash", responseData["hash"]);
				
			_originalUser = null;
			User.refresh();
			
			User.updateHeader();
			
			if(data.hasOwnProperty("argument") && data.argument!==null)
				data.argument();
		},
		
		/**
		 * This is called when getting a new user id fails
		 * @param {Object} data
		 */
		getNewUserIDFailure : function(data)
		{
			
		},
		
		/**
		 * Called to update the top right header with the correct info
		 */
		updateHeader : function()
		{
			var userID = User.get("userID", 0);
			if(userID<=0)
				return;

			var username = User.get("username", null);
			
			var usernameInfoValue = document.getElementById("usernameInfoValue");		
			D.clear(usernameInfoValue);
			
			if(username===null || username.length<1)
			{
				//usernameInfoValue.appendChild(document.createTextNode("User #" + I.formatWithCommas(userID)));
			}
			else
			{
				usernameInfoValue.appendChild(document.createTextNode(username));
			}
			
			var usernameInfoLoginLink = document.getElementById("usernameInfoLoginLink");
			var usernameInfoRegisterLink = document.getElementById("usernameInfoRegisterLink");
			var usernameInfoLogoutLink = document.getElementById("usernameInfoLogoutLink");
			
			if(_isLoggedIn===true)
			{
				usernameInfoLoginLink.style.display = "none";
				usernameInfoRegisterLink.style.display = "none";
				usernameInfoLogoutLink.style.display = "inline";
			}
			else
			{
				usernameInfoLoginLink.style.display = "inline";
				usernameInfoRegisterLink.style.display = "inline";
				usernameInfoLogoutLink.style.display = "none";
			}
		},
		
		/**
		 * Called when user clicks on the register link
		 */
		handleRegisterUser : function()
		{
			_registerUser.show();
			
			var registerUserUsername = document.getElementById("registerUserUsername");
			registerUserUsername.disabled = false;

			var registerUserPassword = document.getElementById("registerUserPassword");
			registerUserPassword.disabled = false;

			var registerUserPasswordVerify= document.getElementById("registerUserPasswordVerify");
			registerUserPasswordVerify.disabled = false;

			var registerUserEmail = document.getElementById("registerUserEmail");
			registerUserEmail.disabled = false;
			
			var registerUserSubmit = document.getElementById("registerUserSubmit");
			registerUserSubmit.disabled = false;
			registerUserSubmit.value = "Register User";
						
			registerUserUsername.focus();
			
			User.updateRegisterUserDialog();
		},
		
		/**
		 * This is called whenever data in the register user input boxes changes
		 */
		updateRegisterUserDialog : function()
		{
			var validDocument = true;
			
			var registerUserErrors = document.getElementById("registerUserErrors");
			D.clear(registerUserErrors);
			
			var registerUserUsername = document.getElementById("registerUserUsername");
			if(registerUserUsername.value.length<3)
			{
				validDocument = false;
				registerUserErrors.appendChild(document.createTextNode("Username must be at least 3 letters"));
				registerUserErrors.appendChild(document.createElement("BR"));
			}
			
			if(registerUserUsername.value.indexOf(" ")!=-1)
			{
				validDocument = false;
				registerUserErrors.appendChild(document.createTextNode("Username cannot contain spaces"));
				registerUserErrors.appendChild(document.createElement("BR"));
			}
			else
			{
				var allValidChars = true;
				for(var i=0;i<registerUserUsername.value.length;i++)
				{
					var checkCode = registerUserUsername.value.charCodeAt(i);
					if(checkCode<33 || checkCode>126)
					{
						allValidChars = false;
						break;
					}
				}
				if(allValidChars!==true)
				{
					validDocument = false;
					registerUserErrors.appendChild(document.createTextNode("Username cannot contain extended characters"));
					registerUserErrors.appendChild(document.createElement("BR"));
				}
			}
			
			var registerUserPassword = document.getElementById("registerUserPassword");
			if(registerUserPassword.value.length<5)
			{
				validDocument = false;
				registerUserErrors.appendChild(document.createTextNode("Password must be at least least 4 letters"));
				registerUserErrors.appendChild(document.createElement("BR"));
			}
			
			var registerUserPasswordVerify = document.getElementById("registerUserPasswordVerify");
			if(registerUserPassword.value!=registerUserPasswordVerify.value)
			{
				validDocument = false;
				registerUserErrors.appendChild(document.createTextNode("Password and Verify Password must match"));
				registerUserErrors.appendChild(document.createElement("BR"));
			}
			
			var registerUserEmail = document.getElementById("registerUserEmail");
			if(registerUserEmail.value.length<6 || registerUserEmail.value.indexOf("@")===-1)
			{
				validDocument = false;
				registerUserErrors.appendChild(document.createTextNode("E-mail must be valid"));
				registerUserErrors.appendChild(document.createElement("BR"));
			}			
		
			var registerUserSubmit = document.getElementById("registerUserSubmit");
			if(validDocument===true)
			{
				registerUserSubmit.disabled = false;
			}
			else
			{
				registerUserSubmit.disabled = true;
			}
			
			return validDocument;
		},
		
		/**
		 * This is called when the user hits the submit button the register user screen
		 */
		handleRegisterUserSubmit : function()
		{
			if(!User.updateRegisterUserDialog())
				return;
			
			var postObject = {};
			
			var registerUserUsername = document.getElementById("registerUserUsername");
			postObject["username"] = registerUserUsername.value;
			registerUserUsername.disabled = true;

			var registerUserPassword = document.getElementById("registerUserPassword");
			postObject["password"] = registerUserPassword.value;
			registerUserPassword.disabled = true;

			var registerUserPasswordVerify = document.getElementById("registerUserPasswordVerify");
			registerUserPasswordVerify.disabled = true;

			var registerUserEmail = document.getElementById("registerUserEmail");
			postObject["email"] = registerUserEmail.value;
			registerUserEmail.disabled = true;
			
			var registerUserSubmit = document.getElementById("registerUserSubmit");
			registerUserSubmit.disabled = true;
			registerUserSubmit.value = "Please Wait...";
			
			YAHOO.util.Connect.asyncRequest("POST", "registerUser", { success : function(data) { User.registerUserFinish(data); }, failure : function(data) { User.registerUserFinish(data); } }, O.serialize(postObject));
		},
		
		/**
		 * This is called when user registration finishes, with error or with success
		 */
		registerUserFinish : function(data)
		{
			if(!data.hasOwnProperty("responseText") ||
			   (data["responseText"]!==null && data["responseText"].length!==0))
			{
				if(data["responseText"]=="UserID is Already Registered")
				{
					// Get a new user ID, register again
					User.set("userID", 0);
					User.getNewUserID(function() { User.handleRegisterUserSubmit(); });
					return;
				}
				
				var registerUserServerErrors = document.getElementById("registerUserServerErrors");
				D.clear(registerUserServerErrors);
				if(data.hasOwnProperty("responseText"))
				{
					registerUserServerErrors.appendChild(document.createTextNode(data["responseText"]));
				}
				else
				{
					registerUserServerErrors.appendChild(document.createTextNode("Registration Failed"));
				}
				
				this.handleRegisterUser();
				return;
			}
			
			var registerUserUsername = document.getElementById("registerUserUsername");
			User.set("username", registerUserUsername.value);

			var registerUserEmail = document.getElementById("registerUserEmail");
			User.set("email", registerUserEmail.value);
			
			_isLoggedIn = true;
			
			var registerUserServerErrors = document.getElementById("registerUserServerErrors");
			D.clear(registerUserServerErrors);
			
			_registerUser.hide();
			
			this.updateHeader();
		},
		
		/**
		 * This function is called when the user hits the logout button
		 */
		handleLogout : function()
		{
			this.setAll(_originalUser);

			User.set("psession", "");
			User.set("username", "");
			
			_isLoggedIn = false;			
			
			this.updateHeader();
		},
		
		/**
		 * Called when user clicks on the login link
		 */
		handleLoginUser : function()
		{
			_loginUser.show();
						
			var loginUserUsername = document.getElementById("loginUserUsername");
			loginUserUsername.disabled = false;

			var loginUserPassword = document.getElementById("loginUserPassword");
			loginUserPassword.disabled = false;

			var loginUserSubmit = document.getElementById("loginUserSubmit");
			loginUserSubmit.disabled = false;
			loginUserSubmit.value = "Login";
			
			var loginUserUsername = document.getElementById("loginUserUsername");
			loginUserUsername.focus();
			
			User.updateLoginUserDialog();
		},
		
		/**
		 * This is called whenever data in the login user input boxes changes
		 */
		updateLoginUserDialog : function()
		{
			var validDocument = true;
			
			var loginUserErrors = document.getElementById("loginUserErrors");
			D.clear(loginUserErrors);
			
			var loginUserUsername = document.getElementById("loginUserUsername");
			if(loginUserUsername.value.length<3)
			{
				validDocument = false;
				loginUserErrors.appendChild(document.createTextNode("Username must be at least 3 letters"));
				loginUserErrors.appendChild(document.createElement("BR"));
			}
			
			var loginUserPassword = document.getElementById("loginUserPassword");
			if(loginUserPassword.value.length<5)
			{
				validDocument = false;
				loginUserErrors.appendChild(document.createTextNode("Password must be at least least 4 letters"));
				loginUserErrors.appendChild(document.createElement("BR"));
			}	
		
			var loginUserSubmit = document.getElementById("loginUserSubmit");
			if(validDocument===true)
			{
				loginUserSubmit.disabled = false;
			}
			else
			{
				loginUserSubmit.disabled = true;
			}
			
			return validDocument;
		},
		
		/**
		 * This is called when the user hits the submit button the login user screen
		 */
		handleLoginUserSubmit : function()
		{
			if(!User.updateLoginUserDialog())
				return;
						
			var loginUserUsername = document.getElementById("loginUserUsername");
			var username = loginUserUsername.value;
			loginUserUsername.disabled = true;

			var loginUserPassword = document.getElementById("loginUserPassword");
			var password = loginUserPassword.value;
			loginUserPassword.disabled = true;
			
			var loginUserSubmit = document.getElementById("loginUserSubmit");
			loginUserSubmit.disabled = true;
			loginUserSubmit.value = "Please Wait...";

			this.login(username, password, function(data) { User.handleLoginUserFinish(data); });
		},		
		
		/**
		 * Called when dialog started login comes back good or bad
		 * @param {Object} data
		 */
		handleLoginUserFinish : function(data)
		{
			if(!data.hasOwnProperty("responseText") ||
			   (data["responseText"]!==null && data["responseText"].length!==0))
			{
				var loginUserServerErrors = document.getElementById("loginUserServerErrors");
				D.clear(loginUserServerErrors);
				if(data.hasOwnProperty("responseText"))
				{
					loginUserServerErrors.appendChild(document.createTextNode(data["responseText"]));
				}
				else
				{
					loginUserServerErrors.appendChild(document.createTextNode("Login Failed"));
				}
				
				this.handleLoginUser();
				return;
			}
			
			var loginUserServerErrors = document.getElementById("loginUserServerErrors");
			D.clear(loginUserServerErrors);

			_loginUser.hide();
			
			this.loginFinish(data);
			this.getUserOptions(function() { SOLITAIRE.refreshUI(); });			
		},
				
		/**
		 * Will attempt to login with the given username and password
		 * @param {Object} username
		 * @param {Object} password
		 */
		login : function(username, password, callbackFunction)
		{
			var postObject = {};
			postObject["username"] = username;
			postObject["password"] = password;
			
			if(typeof callbackFunction=="undefined" || callbackFunction===null)
				callbackFunction = function(data) { User.loginFinish(data); };
			
			YAHOO.util.Connect.asyncRequest("POST", "login", { success : callbackFunction, failure : callbackFunction }, O.serialize(postObject));
		},
		
		/**
		 * Will attempt to login with the given username and psession
		 * @param {Object} username
		 * @param {Object} psession
		 */
		loginWithPSession : function(username, psession, callbackFunction)
		{
			var postObject = {};
			postObject["username"] = username;
			postObject["psession"] = psession;
			
			if(typeof callbackFunction=="undefined" || callbackFunction===null)
				callbackFunction = function(data) { User.loginFinish(data); };
			
			YAHOO.util.Connect.asyncRequest("POST", "login", { success : callbackFunction, failure : callbackFunction }, O.serialize(postObject));
		},
		
		/**
		 * Called when an attempt to login works
		 * @param {Object} data
		 */
		loginFinish : function(data)
		{
			if(!data.hasOwnProperty("responseText") ||
			   (data["responseText"]!==null && data["responseText"].length!==0))
			{
				this.handleLogout();
				return;
			}
			
			this.refresh();
			
			_isLoggedIn = true;
			
			this.updateHeader();
			
			var optionHasSavedOnce = Options.get("optionHasSavedOnce", false);
			if(!optionHasSavedOnce)
			{
				if(User.saveUserOptions())
					Options.set("optionHasSavedOnce", true);
			}			
		},
		
		/**
		 * Called when the user has changed an option and it should be saved remotely
		 */
		saveUserOptions : function()
		{
			var userID = User.get("userID", null);	
			var userID = User.get("userID", null);	
			var username = User.get("username", null);
			var psession = User.get("psession", null);		
			var hash = User.get("hash", null);
			
			if(userID===null || userID===0 ||
			   hash===null || hash.length!=64 ||
			   username===null || username.length<1 ||
			   psession===null || psession.length<1)
			{
				return false;
			}
			
			if(_lastOptionUpdate===null)
			{
				_lastOptionUpdate = new Date();
				setTimeout(function() { User.saveUserOptionPerform(); }, _saveUserOptionInterval);				
			}
			else
			{
				_lastOptionUpdate = new Date();
			}
			
			return true;
		},
		
		/**
		 * Called after a timeout in order to not flood server with option updates
		 */
		saveUserOptionPerform : function()
		{
			var rightNow = new Date();
			var msElapsed = parseInt(((rightNow.getTime()-_lastOptionUpdate.getTime())), 10);
			if(msElapsed<_saveUserOptionInterval)
			{
				setTimeout(function() { User.saveUserOptionPerform(); }, _saveUserOptionInterval);
				return;
			}
			
			_lastOptionUpdate = null;
			
			YAHOO.util.Connect.asyncRequest("POST", "saveUserOptions", {});
		},
		
		/**
		 * Will get the options for the given user
		 * * @param {Object} callbackFunction Function to call when done
		 */
		getUserOptions : function(callbackFunction)
		{
			var userID = User.get("userID", null);	
			var username = User.get("username", null);
			var psession = User.get("psession", null);		
			var hash = User.get("hash", null);
			
			if(userID===null || userID===0 ||
			   hash===null || hash.length!=64 ||
			   username===null || username.length<1 ||
			   psession===null || psession.length<1)
			{
				callbackFunction();
				return;
			}
			
			var userCallback = function(data) { User.getUserOptionsFinish(data); };
				
			YAHOO.util.Connect.asyncRequest("POST", "getUserOptions", { success : userCallback, failure : userCallback, argument : callbackFunction });
		},
		
		/**
		 * Called when getting the user options finish
		 * @param {Object} data
		 */
		getUserOptionsFinish : function(data)
		{
			Options.refresh();
			
			if(data.hasOwnProperty("argument") && data.argument!==null)
				data.argument();
		},
		
		/**
		 * Refreshes our internal array with the latest cookie value
		 */
		refresh : function()
		{
			var existingUserString = C.get(_cookieName);
			if(existingUserString!==null && existingUserString.length>0)
			{
				_user = O.deserialize(existingUserString, false, false);
				
				if(_originalUser===null)
					_originalUser = _user;
			}
		},
		
		/**
		 * Sets all the key values in an object
		 * @param {Object} obj
		 */
		setAll : function(obj)
		{
			for(var key in obj)
			{
				User.set(key, obj[key]);
			}
		},
		
		/**
	 	 * Returns the option with the specified key
		 * @param {Object} key Option name
		 */
		get : function(key, defaultValue)
		{
			return (_user.hasOwnProperty(key) ? _user[key] : defaultValue);	
		},
		
		/**
		 * Sets the given username key/value
		 * @param {Object} key Option name
		 * @param {Object} value Option value
		 */
		set : function(key, value, skipCookieSetting)
		{
			_user[key] = value;
			
			if(typeof skipCookieSetting==="undefined" || skipCookieSetting!==true)
				C.set(_cookieName, O.serialize(_user), C.DURATION_DECADE, _cookieDomain);
		}
	};
})();


