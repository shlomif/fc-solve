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
	
	public const string data_dir
		= "/home/shlomi/progs/freecell/trunk/fc-solve/presets/soft-threads/meta-moves/auto-gen";
	
	
	public List<int> blacklist;
	public List<InputScan> scans;
	
	public int [,] scans_data;
	
	public Input(int new_start_board, int new_num_boards)
	{
		start_board = new_start_board;
		num_boards = new_num_boards;
	}
	
	public string data_file_path(string basename)
	{
		return data_dir + "/" + basename;
	}
	
	public void read_scan_black_list()
	{
		blacklist = new List<int>();
		StreamReader f = new StreamReader(data_file_path("scans-black-list.txt"));
		
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
		StreamReader f = new StreamReader(data_file_path("scans.txt"));
		
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
	
	public void read_data()
	{
		read_scans_file();
		
		scans_data = new int [scans.Count,num_boards];
		
		int scan_idx = 0;
		foreach (InputScan scan in scans)
		{
			string file_path = 
				data_file_path("data/" + scan.id + ".data.bin");
			
			BinaryReader binReader =
			    new BinaryReader(File.Open(file_path, FileMode.Open));
			
			{
				int start, num, iters_limit;
			
				start = binReader.ReadInt32();
				num = binReader.ReadInt32();
				iters_limit = binReader.ReadInt32();
				
				if (start != start_board)
				{
					throw new ArgumentException("Start board does not match in " + scan.id);
				}
				
				if (num != num_boards)
				{
					throw new ArgumentException("Num boards does not match in " + scan.id);
				}
				
				if (iters_limit != 100000)
				{
					throw  new ArgumentException("Iters limits is wrong in scan No. " + scan.id);
				}
			}
			
			for (int board_idx = 0; board_idx < num_boards ; board_idx++)
			{
				scans_data[scan_idx,board_idx] = binReader.ReadInt32();				
			}

			scan_idx++;
		}
	}
}

class Quota_Allocation
{
	public int scan_idx;
	public int quota;
	
	public Quota_Allocation(int new_scan_idx, int new_quota)
	{
		scan_idx = new_scan_idx;
		quota = new_quota;
	}
	
	public Quota_Allocation(Quota_Allocation other)
	{
		scan_idx = other.scan_idx;
		quota = other.quota;
	}
}

class Process
{
	protected Input input;
	
	public Process(Input new_input)
	{
		input = new_input;
	}
	
	public void SampleRun()
	{
		List<Quota_Allocation> allocations = new List<Quota_Allocation>();
			
		int scans_num = input.scans.Count;
		
		InputScan [] scans = new InputScan[scans_num];
		
		/* Calculate the scans array */
		{
			int scan_idx = 0;
			foreach (InputScan scan in input.scans)
			{
				scans[scan_idx] = scan;
				scan_idx++;
			}
		}
				
		// This is temporary - naturally they will later be sorted out.
		const int quota_iters_num = 5000;
		
		int [,] running_scans_data = input.scans_data;
		
		
		int num_boards = input.num_boards;
		
		const int quota_step = 350;
		
		int quota = quota_step;
		
		for (int quota_idx = 0; quota_idx < quota_iters_num ; quota_idx++)
		{
			int max_solved_boards = 0;
			int max_solved_scan_idx = -1;
			
			/*
			 * Find the scan which solves the largest number of boards.
			 * */
			for (int scan_idx = 0; scan_idx < scans_num ; scan_idx++)
			{
				int solved_boards = 0;
				
				for (int board_idx = 0; board_idx < num_boards ; board_idx++)
				{
					int datum = running_scans_data[scan_idx, board_idx];
					
					if ((datum > 0) && (datum <= quota))
					{
						solved_boards++;
					}
				}

#if false
				Console.WriteLine ("Scan idx is " + scan_idx);
				Console.WriteLine(
					string.Format(
				    	"Solved {0} boards at scan {1} at quota_idx {2}",
				        solved_boards, scans[scan_idx].id, quota_idx
				    )
			    );
#endif
				
				if (solved_boards > max_solved_boards)
				{
					max_solved_boards = solved_boards;
					max_solved_scan_idx = scan_idx;
				}
			}
			
			if (max_solved_boards > 0)
			{
				allocations.Add(new Quota_Allocation(max_solved_scan_idx, quota));
				
				/* Update the iterations */
				
				int [,] new_running_scans_data =
					new int [scans_num , num_boards-max_solved_boards];
				int target_idx = 0;
				
				for(int board_idx = 0; board_idx < num_boards; board_idx++)
				{
					int source_max_datum = running_scans_data[max_solved_scan_idx,board_idx];
					if ((source_max_datum < 0) || (source_max_datum > quota))
					{
						for (int scan_idx = 0; scan_idx < scans_num ; scan_idx++)
						{
							int source_datum =
								running_scans_data[scan_idx,board_idx];
						
							new_running_scans_data[scan_idx, target_idx] = 
								((max_solved_scan_idx == scan_idx)
									? (source_datum - quota) : source_datum
								);
						}
						
						target_idx++;
					}
				}
				
				/* This is an assertion. */
				if (target_idx != num_boards-max_solved_boards)
				{
					throw new ApplicationException("target_idx is not max_solved_boards");
				}
				
				running_scans_data = new_running_scans_data;
			    num_boards = target_idx;
				
				quota = quota_step;
			}
			else
			{
				quota += quota_step;
			}
		}
		
		List<Quota_Allocation> rled_allocs = new List<Quota_Allocation>();
		{
			/* RLE the allocations */
			
			Quota_Allocation to_add = null;
			foreach (Quota_Allocation item in allocations)
			{
				if (to_add == null)
				{
					to_add = new Quota_Allocation(item);
				}
				else
				{
					if (to_add.scan_idx == item.scan_idx)
					{
						to_add.quota += item.quota;
					}
					else
					{
						rled_allocs.Add(to_add);
						to_add = new Quota_Allocation(item);
					}
				}
			}
			rled_allocs.Add(to_add);
		}
		

		bool is_first = true;
		foreach (Quota_Allocation quota_a in rled_allocs)
		{
			if (! is_first)
			{
				Console.Write(",");
			}
			
			Console.Write(
				"" +quota_a.quota + "@" + scans[quota_a.scan_idx].id
			);
			
			is_first = false;
		}
		Console.WriteLine("");
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
		else if (cmd == "test_lookup_iters")
		{
			Input input = new Input(start_board, num_boards);
			input.read_data();
			
			int scan_id = Convert.ToInt32(args[1]);
			int board_idx = Convert.ToInt32(args[2]);
			
			int scan_idx = 0;
			foreach (InputScan scan in input.scans)
			{
				if (scan.id == scan_id)
				{
					break;
				}
				scan_idx++;
			}
			
			Console.WriteLine(input.scans_data[scan_idx , (board_idx-input.start_board)]);
		}
        else if (cmd == "test_process_sample_run")
        {
            Input input = new Input(start_board, num_boards);
            input.read_data();

            Process p = new Process(input);
            p.SampleRun();
        }
        // List<double> myList = new List<double>();
		else
		{
        	Console.WriteLine("Hello World!");
		}
    }
}
