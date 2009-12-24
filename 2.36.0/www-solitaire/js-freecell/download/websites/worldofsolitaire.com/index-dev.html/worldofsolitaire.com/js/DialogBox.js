/*
 * DialogBox.js - Generic function for handling dialog box
 * Author - robert@cosmicrealms.com
 */

var DIALOG_BOX =
{
	showCount : 0
};

function DialogBox(panelID, options)
{
	var _panel = null;
	var _panelID = panelID;
	var _panelOptions = (typeof options!="undefined") ? options : {};

	/**
	 * Creates the intial panel
	 */	
	this.init = function()
	{
		var self = this;
		
		var dialogBoxOptions = {};
		dialogBoxOptions.close = (_panelOptions.hasOwnProperty("close") ? _panelOptions["close"] : true);
		dialogBoxOptions.draggable = (_panelOptions.hasOwnProperty("draggable") ? _panelOptions["draggable"] : true);
		dialogBoxOptions.underlay = "none";
		dialogBoxOptions.modal = false;
		dialogBoxOptions.visible = true;
		dialogBoxOptions.fixedcenter = true;
		dialogBoxOptions.constraintoviewport = true;
		dialogBoxOptions.monitorresize = (P.getBrowser()=="konqueror" ? false : true);
		
		E.add(_panelID, "keydown", function(e) { if(e.keyCode==27) { self.hide(); } });
		E.add(_panelID, "keydown", function(e) { if(e.keyCode==13) { self.handleEnterKey(); } });
		
		_panel = new YAHOO.widget.Panel(_panelID, dialogBoxOptions);
		_panel.hideEvent.subscribe(function() { self.hide(); });
	};
	
	/**
	 * Shows the dialog box
	 */
	this.show = function()
	{
		this.panelWidth = null;
		this.panelHeight = null;
	
		var dialogBox = document.getElementById(_panelID);

		if(_panel===null)
		{
			this.init();
			if(this.onInit!==null)
				this.onInit();
		}
						
		dialogBox.style.zIndex = 500 + DIALOG_BOX.showCount;
		dialogBox.parentNode.style.zIndex = 500 + DIALOG_BOX.showCount;
		DIALOG_BOX.showCount++;
						
		if(this.beforeShow!==null)
			this.beforeShow();

		if(_panelOptions.hasOwnProperty("percentageWidth"))
		{
			var availableWidth = D.width(document.body);
			this.panelWidth = Math.floor((availableWidth*_panelOptions["percentageWidth"]));
			
			if(_panelOptions.hasOwnProperty("maximumWidth"))
				this.panelWidth = Math.min(this.panelWidth, _panelOptions["maximumWidth"]);
			
			_panel.cfg.setProperty("width", this.panelWidth + "px");
		}

		if(_panelOptions.hasOwnProperty("percentageHeight"))
		{
			var availableHeight = D.height(document.body);
			this.panelHeight = Math.floor((availableHeight*_panelOptions["percentageHeight"]));
		
			if(_panelOptions.hasOwnProperty("maximumHeight"))
				this.panelHeight = Math.min(this.panelHeight, _panelOptions["maximumHeight"]);
		
			_panel.cfg.setProperty("height", this.panelHeight + "px");
		}
		
		var dialogBoxChildren = D.getElementsByTagName(dialogBox, "DIV");
		var dialogBoxHeader = dialogBoxChildren[0];
		var dialogBoxBody = dialogBoxChildren[1];
		var dialogBoxBodyChildren = D.getElementsByTagName(dialogBoxBody, "DIV");
		
		if(_panelOptions.hasOwnProperty("overflowable") && _panelOptions["overflowable"]===true)
		{
			dialogBoxBodyChildren[0].style.width = "100%";
			dialogBoxBodyChildren[0].style.height = "100%";
		}
		
		dialogBox.style.display = "block";
		
		if(_panelOptions.hasOwnProperty("overflowable") && _panelOptions["overflowable"]===true)
			dialogBoxBodyChildren[0].style.display = "block";
				
		_panel.render();			
		_panel.show();
		
		if(_panelOptions.hasOwnProperty("overflowable") && _panelOptions["overflowable"]===true)
		{
			var widthPadding = parseInt(YAHOO.util.Dom.getStyle(dialogBoxBody, "padding-left"), 10) + parseInt(YAHOO.util.Dom.getStyle(dialogBoxBody, "padding-right"), 10);
			var heightPadding = parseInt(YAHOO.util.Dom.getStyle(dialogBoxBody, "padding-top"), 10) + parseInt(YAHOO.util.Dom.getStyle(dialogBoxBody, "padding-bottom"), 10);
			
			dialogBoxBodyChildren[0].style.width = "" + (this.panelWidth-(P.getBrowser()=="konqueror" ? 10 : 2)-widthPadding) + "px";
			dialogBoxBodyChildren[0].style.height = "" + ((this.panelHeight-(D.height(dialogBoxHeader)) - 2)-heightPadding) + "px";
		}

		if(this.afterShow!==null)
			this.afterShow();
	};
	
	/**
	 * Hides the dialog box
	 */
	this.hide = function()
	{
		var dialogBox = document.getElementById(_panelID);
		var dialogBoxChildren = D.getElementsByTagName(dialogBox, "DIV");
		var dialogBoxBody = dialogBoxChildren[1];
		var dialogBoxBodyChildren = D.getElementsByTagName(dialogBoxBody, "DIV");		
		
		if(_panel!==null)
			_panel.hide();
			
		dialogBox.style.display = "none";
		
		if(_panelOptions.hasOwnProperty("overflowable") && _panelOptions["overflowable"]===true)
			dialogBoxBodyChildren[0].style.display = "none";
	};
	
	/**
	 * Will either hide or show the game chooser
	 */
	this.toggle = function()
	{
		if(!this.isShowing())
			this.show();
		else
			this.hide();
	};
	
	/**
	 * Is called when enter key is hit
	 */
	this.handleEnterKey = function()
	{
		if(this.onEnterPress!==null && this.isShowing())
			this.onEnterPress();
	};
	
	/**
	 * Returns true if the dialog box is showing or not
	 */
	this.isShowing = function()
	{
		return !(_panel===null || !_panel.cfg.getProperty("visible"));
	};
	
	this.panelWidth = null;
	this.panelHeight = null;
	this.beforeShow = null;
	this.afterShow = null;
	this.onInit = null;
	this.onEnterPress = null;
}

