#include <iostream>
#include <fstream>

using namespace std;

class sudoku_puzzle
{

#define RC_SIZE       	9	// size of row and column
#define SECTOR_SIZE   	3	// size of sector
#define RCS_SUM			45	// this is the sum of any full row, col, or sector

public:
	//constructs a sudoku_puzzle populating it's puzzle from a given file
	sudoku_puzzle(char *file)
	{
		string row;
		ifstream myfile (file);
		int index = 0;
		if (myfile.is_open())
		{
			while ( myfile.good() )
			{
				getline (myfile,row);
				if(!set_row(row,index))
				{
					_good_file = false;
					myfile.close();
					return;
				}
				index++;
			}
			myfile.close();
			_good_file = true;
		}
		else
		{
			err_msg = file;
			err_msg.append(" does not exist");
			_good_file = false;
		}
	}
	
	//initiates the solve routine
	bool solve()
	{
		if(!_good_file)
			return false;
		if(!is_valid_puzzle())
			return false;
		if(!is_puzzle_solved())
		{
			do_obvious_check();
		}
		return true;
	}
	
	//prints the state of the puzzle
	void print_puzzle()
	{
		for(int i = 0; i < RC_SIZE; i++)
		{
			for(int j = 0; j < RC_SIZE; j++)
				cout << _cells[i][j] << " ";
			cout << endl;
		}
	}
	
	string get_err_msg() { return err_msg; }
	
private:
	bool _good_file;
	int _cells[RC_SIZE][RC_SIZE];
	string err_msg;
	
	//this method iterates through the whole puzzle calling
	//obvious_row_check(i);
	//obvious_col_check(i);
	bool do_obvious_check()
	{
		bool updated_puzzle = false;
		bool temp_result = false;
		for(int i = 0; i < RC_SIZE; i++)
		{
			temp_result = obvious_row_check(i);
			if(!updated_puzzle && temp_result)
				updated_puzzle = true;
			temp_result = obvious_col_check(i);
			if(!updated_puzzle && temp_result)
				updated_puzzle = true;
		}
		return updated_puzzle;
	}
	
	//this method checks to see if the row of index "int row"
	//has only one empty cell, if so it identifies the empty cell, then
	//assignes the proper number and returns true signaling that a cell has
	//been updated, otherwise it returns false
	bool obvious_row_check(int row)
	{
		int empty_cells = 0;
		int empty_index = 0;
		for(int c = 0; c < RC_SIZE; c++)
		{
			if(_cells[row][c]==0)
			{
				empty_cells++;
				empty_index = c;
			}
			if(empty_cells > 1)
				return false;
		}
		if(empty_cells==0)
			return false;
			
		int final_val = RCS_SUM;
		for(int c = 0; c < RC_SIZE; c++)
		{
			final_val -= _cells[row][c];
		}
		_cells[row][empty_index] = final_val;
		return true;
	}
	
	//this method checks to see if the column of index "int col"
	//has only one empty cell, if so it identifies the empty cell, then
	//assignes the proper number and returns true signaling that a cell has
	//been updated, otherwise it returns false
	bool obvious_col_check(int col)
	{
		int empty_cells = 0;
		int empty_index = 0;
		for(int r = 0; r < RC_SIZE; r++)
		{
			if(_cells[r][col]==0)
			{
				empty_cells++;
				empty_index = r;
			}
			if(empty_cells > 1)
				return false;
		}
		if(empty_cells==0)
			return false;
			
		int final_val = RCS_SUM;
		for(int r = 0; r < RC_SIZE; r++)
		{
			final_val -= _cells[r][col];
		}
		_cells[empty_index][col] = final_val;
		return true;
	}
	
	//parse a string to set a row of the puzzle at position index
	bool set_row(string &row, int index)
	{
		if(row.size() != RC_SIZE)
		{
			err_msg =  "invalid row lenght";
			return false;
		}
		for(unsigned i = 0; i < row.size(); i++)
		{
			if(!isdigit(row[i]))
				return false;
			_cells[index][i] = get_int_value(row[i]);
		}
		return true;
	}
	
	//get integer value represented by char c
	int get_int_value(char c)
	{
		int val = 0;
		switch(c)
		{
		case '1': val = 1; break;
		case '2': val = 2; break;
		case '3': val = 3; break;
		case '4': val = 4; break;
		case '5': val = 5; break;
		case '6': val = 6; break;
		case '7': val = 7; break;
		case '8': val = 8; break;
		case '9': val = 9; break;
		}
		return val;
	}
	
	//checks if the puzzle has emptry cells (cells with 0)
	bool is_puzzle_solved()
	{
		for(int c = 0; c < RC_SIZE; c++)
			for(int r = 0; r < RC_SIZE; r++)
				if(_cells[r][c]==0)
					return false;
		return true;
	}
	
	//validates the whole puzzle
	bool is_valid_puzzle()
	{
		int row = 0, col = 0;
		for(int i = 0; i < RC_SIZE; i++)
		{
			if(!is_valid_row(_cells[i],false))
				return false;
			if(!is_valid_column(i))
				return false;
			if(!is_valid_quadrant(row,col))
				return false;
			col+=SECTOR_SIZE;
			if(col==RC_SIZE)
			{
				col=0;
				row+=SECTOR_SIZE;
			}
		}
		
		return true;
	}
	
	//validates a row in a given array of ints
	//a valid row is one that does not have repeted numbers
	//except for 0 with denotes an empty cell
	//the paramater isquad is just to better address the error msg
	bool is_valid_row(int *row, bool isquad)
	{
		for(int i = 0; i < RC_SIZE; i++)
		{
			if(row[i] != 0)
			{
				for(int j = RC_SIZE-1; j > i; j--)
				{
					if(row[i] == row[j])
					{
						if(isquad)
							err_msg = "the puzzle contains invalid quadrant(s)";
						else
							err_msg = "the puzzle contains invalid row(s)";
						return false;
					}
				}
			}
		}
		return true;
	}
	
	//validates a column in _cells
	//a valid column is one that does not have repeted numbers
	//except for 0 with denotes an empty cell
	bool is_valid_column(int col)
	{
		for(int i = 0; i < RC_SIZE; i++)
		{
			if(_cells[i][col] != 0)
			{
				for(int j = RC_SIZE-1; j > i; j--)
				{
					if(_cells[i][col] == _cells[j][col])
					{
						err_msg = "the puzzle contains invalid column(s)";
						return false;
					}
				}
			}
		}
		return true;
	}
	
	//validates a sector in _cells
	//a sector is a 3x3 square
	//a valid sector is one that does not have repeted numbers
	//except for 0 with denotes an empty cell
	bool is_valid_quadrant(int quad_row, int quad_col)
	{
		int vals[9];
		int index = 0;
		for(int r = quad_row; r < quad_row+SECTOR_SIZE; r++)
		{
			for(int c = quad_col; c < quad_col+SECTOR_SIZE; c++)
			{
				vals[index] = _cells[r][c];
				index++;
			}
		}
		return is_valid_row(vals,true);
	}
};

//program entry point to solve a sudoku puzzle
//the program takes a file name as parameter
int main(int argc, char *argv[])
{
	if(argc == 1)
	{
		cout << "program requires file name as parameter" << endl;
		system("pause");
		return 0;
	}
		
	sudoku_puzzle sd(argv[1]);
	
	if(!sd.solve())
	{
		cout << sd.get_err_msg() << endl;
		system("pause");
		return 0;
	}
	
	sd.print_puzzle();
	
	system("pause");
	
	return 1;
}
