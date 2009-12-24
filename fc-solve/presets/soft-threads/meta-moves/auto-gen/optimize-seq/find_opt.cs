using System.Collections.Generic;
using System;
using System.IO;

class Input
{
	public int start_board;
	public int num_boards;
	
	public const string data_dir = "..";
	
	public List<int> blacklist;
	
	public Input(int new_start_board, int new_num_boards)
	{
		start_board = new_start_board;
		num_boards = new_num_boards;
	}
	
	public void read_scan_black_list()
	{
		blacklist = new List<int>();
		StreamReader f = new StreamReader(data_dir + "/scans-black-list.txt");
		
     	string line;
		
        while ((line = f.ReadLine()) != null) 
        {
      		blacklist.Add(Convert.ToInt32(line));
        }
	}
}

class Program
{
	const int start_board = 1;
	const int num_boards = 32000;
	
    static void Main(string[] args)
    {
		string cmd = ((args.Length >= 1) ? args[0] : "");
		if (cmd == "test_blacklist")
		{
			Input i = new Input(start_board, num_boards);
			i.read_scan_black_list();
			
			foreach (int blacked in i.blacklist)
			{
				Console.WriteLine(blacked);
			}
		}
        // List<double> myList = new List<double>();
		else
		{
        	Console.WriteLine("Hello World!");
		}
    }
}
