/*
 * T.js - YUI Tree Functions
 * Author - robert@cosmicrealms.com
 */

var T =
{
	/**
	 * The function to convert a list into a tree
	 * @param {Object} listID
	 * @param {Object} treeID
	 */
	listToTree : function(listID, treeID)
	{
		var list = document.getElementById(listID);
		D.clearEmpty(list);
		
		var tree = new YAHOO.widget.TreeView(treeID);
		var root = tree.getRoot();
		
		T.listToTreeMakeBranches(root, list);
		
		tree.draw();
				
		return tree;
	},
	
	/**
	 * Recursive function that iterates over the list to make the tree
	 * @param {Object} treeNode
	 * @param {Object} listNode
	 */
	listToTreeMakeBranches : function(treeNode, listNode)
	{
		for(var i=0;i<listNode.childNodes.length;i++)
		{
			var item = listNode.childNodes[i];
			var nodeItem = new YAHOO.widget.TextNode(S.trim(item.childNodes[0].data), treeNode, false);
			var labelEl = nodeItem.getLabelEl();
			if(labelEl!==null)
				labelEl.style.fontWeight = "normal";

			if(item.title!==null && item.title.length>0)
				nodeItem.data = item.title;
			else
				nodeItem.data = null;
			
			if(item.childNodes.length>1)
			{
				if(labelEl!==null)
					labelEl.style.fontWeight = "normal";
					
				T.listToTreeMakeBranches(nodeItem, item.childNodes[1]);
			}
		}		
	},
	
	/**
	 * Returns all nodes in a tree
	 * @param {Object} node
	 */
	getAllNodes : function(node)
	{
		var nodes = [];
		nodes.push(node);
		
		if(node.hasOwnProperty("children") && node.children.length>0)
		{
			for(var i=0;i<node.children.length;i++)
			{
				A.append(nodes, T.getAllNodes(node.children[i]));
			}
		}
		
		return nodes;
	},
	
	/**
	 * Returns the node that matches the value
	 * @param {Object} node
	 * @param {Object} value
	 */
	getNodeWithDataMatching : function(node, value)
	{
		var allNodes = T.getAllNodes(node);
		for(var i=0;i<allNodes.length;i++)
		{
			if(allNodes[i].data===value)
				return allNodes[i];
		}
		
		return null;
	},
	
	/**
	 * Returns the node that has a label matching label
	 * @param {Object} node
	 * @param {Object} label
	 */
	getNodeWithLabelMatching : function(node, label)
	{
		var allNodes = T.getAllNodes(node);
		for(var i=0;i<allNodes.length;i++)
		{
			if(allNodes[i].label===label)
				return allNodes[i];
		}
		
		return null;
	},
	
	/**
	 * This will open all parents to the given node
	 * @param {Object} node
	 */
	openToNode : function(node)
	{
		node.parent.expand();
	},
	
	/**
	 * Will bold any folders
	 * @param {Object} node
	 */
	boldFolders : function(node)
	{
		var allNodes = T.getAllNodes(node);
		for(var i=0;i<allNodes.length;i++)
		{		
			var aNode = allNodes[i];
			var el = aNode.getEl();
			if(el===null)
				continue;
			
			if(!aNode.hasOwnProperty("children") || aNode.children.length<1)
				el.style.fontWeight = "normal";
			else
				el.style.fontWeight = "bold";
		}
	}
};
