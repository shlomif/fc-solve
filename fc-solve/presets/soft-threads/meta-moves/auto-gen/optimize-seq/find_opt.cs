using System.Collections.Generic;
using System;
using System.IO;

class InputScan
{
	public int id;
	public string cmd_line;
	
	public InputScan(int new_id, string new_cmd_line)
	{
		id = new_id;
		cmd_line = new_cmd_line;
	}
	
	static public InputScan from_line(string line)
	{
		string [] e = line.Split('\t');
		return new InputScan(Convert.ToInt32(e[0]), e[1]);
	}
}

class Input
{
	public int start_board;
	public int num_boards;
	
	public const string data_dir = "..";
	
	public List<int> blacklist;
	public List<InputScan> scans;
	
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
	
	public void read_scans_file()
	{
		scans = new List<InputScan>();
		read_scan_black_list();
		StreamReader f = new StreamReader(data_dir + "/scans.txt");
		
		string line;

        while ((line = f.ReadLine()) != null) 
        {
			InputScan scan = InputScan.from_line(line);
			
			if (! blacklist.Exists(i => i == scan.id))
			{
				scans.Add(scan);
			}
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
		else if (cmd == "test_lookup_scan_cmd_line_by_id")
		{
			Input input = new Input(start_board, num_boards);
			input.read_scans_file();
			
			int id = Convert.ToInt32(args[1]);
			
			InputScan scan = input.scans.Find(e => e.id == id);
			Console.WriteLine(scan.cmd_line);
		}
        // List<double> myList = new List<double>();
		else
		{
        	Console.WriteLine("Hello World!");
		}
    }
}
