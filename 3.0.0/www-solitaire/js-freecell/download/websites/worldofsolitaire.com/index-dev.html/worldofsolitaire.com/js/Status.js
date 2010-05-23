/*
 * Status.js - Status panel functions
 * Author - robert@cosmicrealms.com
 */

var Status = null;

(function() {
	var _panel = null;
	var _maxSize = 0;
	var _shownBy = null;
	var _isBusy = false;
	
	Status = 
	{
		init : function()
		{
			var statusPanelOptions = {};
			statusPanelOptions.close = false;
			statusPanelOptions.draggable = true;
			statusPanelOptions.underlay = "none";
			statusPanelOptions.modal = false;
			statusPanelOptions.visible = true;
			statusPanelOptions.fixedcenter = true;
			statusPanelOptions.constraintoviewport = true;
			statusPanelOptions.zIndex = 900;
			statusPanelOptions.monitorresize = (P.getBrowser()=="konqueror" ? false : true);
		
			_panel = new YAHOO.widget.Panel("statusPanel", statusPanelOptions);
		},
		
		/**
		 * Inits the status panel with a given header and maximum size for the progress bar
		 * @param {Object} headerText
		 * @param {Object} maxSize
		 */
		show : function(headerText, maxSize, shownBy)
		{
			if(_panel===null)
				Status.init();
				
			_isBusy = true;
			
			if(typeof shownBy!=="undefined" && _shownBy===null)
				_shownBy = shownBy;
			
			var statusPanelHeader = document.getElementById("statusPanelHeader");
			D.clear(statusPanelHeader);
			statusPanelHeader.appendChild(document.createTextNode(headerText));
			
			var statusPanelBarText = document.getElementById("statusPanelBarText");
			D.clear(statusPanelBarText);
			
			_maxSize = maxSize;
			
			var statusPanelInfiniteBarContainer = document.getElementById("statusPanelInfiniteBarContainer");
			var statusPanelBarContainer = document.getElementById("statusPanelBarContainer");
			if(maxSize==-1)
			{
				statusPanelBarContainer.style.display = "none";
				statusPanelInfiniteBarContainer.style.display = "block";
			}
			else
			{
				statusPanelInfiniteBarContainer.style.display = "none";
				statusPanelBarContainer.style.display = "block";
			}

			var infiniteProgressBarImage = document.getElementById("infiniteProgressBarImage");
			var availableWidth = D.width(document.body);
			if(availableWidth<470)
			{
				statusPanelInfiniteBarContainer.style.width = "" + (availableWidth-150) + "px";
				statusPanelBarContainer.style.width = "" + (availableWidth-150) + "px";
				infiniteProgressBarImage.style.width = "" + (availableWidth-150) + "px";
				infiniteProgressBarImage.width = "" + (availableWidth-150) + "px";
			}
			else
			{
				statusPanelInfiniteBarContainer.style.width = "400px";
				statusPanelBarContainer.style.width = "400px";
				infiniteProgressBarImage.style.width = "auto";
			}
			
			var statusPanel = document.getElementById("statusPanel");
			statusPanel.style.display = "block";
			
			_panel.render();
			_panel.show();
		},
		
		/**
		 * This will hide the status panel
		 * @param {Object} shownByCheck OPTIONAL If set it will only hide if the status was shown by this id
		 */
		hide : function(shownByCheck)
		{
			if(typeof shownByCheck!=="undefined" && (_shownBy===null || _shownBy!==shownByCheck))
				return;
			
			_shownBy = null;
			_panel.hide();
			
			var statusPanel = document.getElementById("statusPanel");
			statusPanel.style.display = "none";
			
			_isBusy = false;
		},
		
		/**
		 * This returns whether or not status is busy
		 */
		isBusy : function()
		{
			return _isBusy;
		},
		
		/**
		 * Returns the max size of the status panel
		 */
		getMaxSize : function()
		{
			return _maxSize;
		},
		
		/**
		 * This will update the status panel with the text and current size
		 * @param {Object} statusText
		 * @param {Object} currentSize
		 */
		update : function(statusText, currentSize)
		{
			var statusPanelBarText = document.getElementById("statusPanelBarText");
			D.clear(statusPanelBarText);
			statusPanelBarText.appendChild(document.createTextNode(statusText));
			
			var statusPanelInfiniteBarContainer = document.getElementById("statusPanelInfiniteBarContainer");
			var statusPanelBarContainer = document.getElementById("statusPanelBarContainer");
			if(_maxSize==-1)
			{
				statusPanelBarContainer.style.display = "none";
				statusPanelInfiniteBarContainer.style.display = "block";
			}
			else
			{
				statusPanelInfiniteBarContainer.style.display = "none";
				statusPanelBarContainer.style.display = "block";
	
				var percentComplete = parseInt(100*(currentSize/_maxSize), 10);
				
				var statusPanelBar = document.getElementById("statusPanelBar");
				statusPanelBar.style.width = "" + percentComplete + "%";
				
				var statusPanelBarPercent = document.getElementById("statusPanelBarPercent");
				D.clear(statusPanelBarPercent);
				statusPanelBarPercent.appendChild(document.createTextNode("" + percentComplete + "%"));
				if(percentComplete<51)
					statusPanelBarPercent.style.color = "#000000";
				else
					statusPanelBarPercent.style.color = "#FFFFFF";
			}
		}
	};
})();


