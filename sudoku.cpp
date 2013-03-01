#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

//class responsable for solving a sudoku puzzle
class sudoku_puzzle
{

// a few definitions

#define	RCS_SIZE		9	// size of a row, column, or sector
#define	TOP_LEFT		0	// top left sector
#define	TOP_MID			1	// top middle sector
#define	TOP_RIGHT		2	// top right sector
#define	MID_LEFT		3	// middle left sector
#define	MID_MID			4	// middle middle sector
#define	MID_RIGHT		5	// middle right sector
#define	BOTTOM_LEFT		6	// bottom left sector
#define	BOTTOM_MID		7	// bottom middle sector
#define	BOTTOM_RIGHT	8	// bottom right sector

public:

	//constructs a sudoku_puzzle object
	//populating it's cells from a given file
	sudoku_puzzle(char *file)
	{
		string row;
		ifstream myfile(file);

		set_cell_coordinates();

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
			_err_msg = file;
			_err_msg.append(" does not exist");
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
		
		int iter = 100;
		while(!is_puzzle_solved() && iter > 0)
		{
			update_cells_according_to_potentials();
			if(is_puzzle_solved())
				break;
			update_cells_single_position();
			iter--;
		}

		return true;
	}

	//prints the state of the puzzle
	void print_puzzle()
	{
		bool has_potentials = false;
		for(int r = 0; r < RCS_SIZE; r++)
		{
			for(int c = 0; c < RCS_SIZE; c++)
			{
				if(!has_potentials && _cells[r][c]._potential_values.size() > 0)
					has_potentials = true;
				if(_cells[r][c]._value == 0) 
					cout << " " << " ";
				else
					cout << _cells[r][c]._value << " ";
			}
			cout << endl;
		}
		if(has_potentials)
		{
			cout << endl << "potentials" << endl;
			for(int r = 0; r < RCS_SIZE; r++)
			{
				int c;
				for(c = 0; c < RCS_SIZE; c++)
				{
					if(_cells[r][c]._value == 0) 
					{
						if(_cells[r][c]._potential_values.size() > 0)
						{
							cout << "cell [" << r << "," << c << "] = {";
							for(unsigned i = 0; i < _cells[r][c]._potential_values.size()-1; i++)
							{
								cout << _cells[r][c]._potential_values[i] << ", ";
							}
							cout <<  _cells[r][c]._potential_values.back() << "}" << endl;
						}
					}
				}
				cout << endl;
			}
		}

		print_potentials_to_file();

	}

	void print_potentials_to_file()
	{
		ofstream myfile;
		myfile.open ("C:\\Dev-Cpp\\Projects\\sudoku\\potentials.txt");

		for(int r = 0; r < RCS_SIZE; r++)
		{
			for(int c = 0; c < RCS_SIZE; c++)
			{
				if(_cells[r][c]._value == 0) 
					myfile << " " << " ";
				else
					myfile << _cells[r][c]._value << " ";
			}
			myfile << endl;
		}

		myfile << endl;

		for(int r = 0; r < RCS_SIZE; r++)
		{
			for(int c = 0; c < RCS_SIZE; c++)
			{
				char num[5];
				string line;
				for(unsigned i = 0; i < _cells[r][c]._potential_values.size(); i++)
				{
					itoa(_cells[r][c]._potential_values[i],num,10);
					line.push_back(num[0]);
				}
				myfile << setfill ('-') << setw (10);
				myfile << setw(10) << left << line;
				if(c == 2 || c == 5)
					myfile << "|";
			}
			myfile << endl;
			if(r == 2 || r == 5)
				myfile << endl;
		}
		myfile.close();
	}

	//prints the last error message
	string get_err_msg() { return _err_msg; }

private:

	//represents a cell in the puzzle
	struct cell
	{
		int _value, _sector, _position, _row, _col;
		vector<int> _potential_values;
	};

	bool 	_good_file;
	string	_err_msg;
	cell	_cells[9][9];
	cell	*_cell_sectors[9][9];

//-- methods pertinent to the actual puzzle solving start here --------------------------------

	//updates the potential values that can be assigned to each cell
	void update_potentials()
	{
		for(int sec = 0; sec < RCS_SIZE; sec++)
		{
			vector<int> sec_missing_vals = get_sector_missing_values(sec);
			for(int pos = 0; pos < RCS_SIZE; pos++)
			{
				cell *cell = _cell_sectors[sec][pos]; 	// temp pointer so I dont have to write
														// _cell_sectors[sec][pos] every time
				cell->_potential_values.clear();
				if(cell->_value == 0)
				{
					for(unsigned i = 0; i < sec_missing_vals.size(); i++)
					{
						if(!does_row_conatin_value(cell, sec_missing_vals[i]) &&
							!does_col_conatin_value(cell,sec_missing_vals[i]))
						{
							cell->_potential_values.push_back(sec_missing_vals[i]);
						}
					}
				}
			}
		}
	}

	//this method evaluates the potential values in each sector
	//if there is a cell that has a potential values that is
	//not found in the rest of the sector the method assigns that
	//potential value to its respective cell and then clears that
	//cell's potential_values vector because the value has been assigned
	void update_cells_single_position()
	{
		for(int sec = 0; sec < RCS_SIZE; sec++)
		{
			for(int pos = 0; pos < RCS_SIZE; pos++)
			{
				cell *cel = _cell_sectors[sec][pos];
				if(cel->_value == 0)
				{
					vector<int> sec_potentials = get_sector_potentials(sec,pos);
					for(unsigned i = 0; i < cel->_potential_values.size(); i++)
					{
						if(!contains(sec_potentials,cel->_potential_values[i]))
						{
							cel->_value = cel->_potential_values[i];
							cel->_potential_values.clear();
							update_potentials();
						}
					}
				}
			}
		}
	}

	//this method retrieves a vector with potential values of a given sector
	//except for the potential values of the given position
	vector<int> get_sector_potentials(int sector, int position)
	{
		vector<int> sector_potentials;
		for(int pos = 0; pos < RCS_SIZE; pos++)
		{
			cell *cel = _cell_sectors[sector][pos];
			if(cel->_value == 0 && pos != position)
			{
				for(unsigned i = 0; i < cel->_potential_values.size(); i++)
				{
					if(!contains(sector_potentials,cel->_potential_values[i]))
					{
						sector_potentials.push_back(cel->_potential_values[i]);
					}
				}
			}
		}
		return sector_potentials;
	}

	//this metthod assigns a value to the cell acourding to its potential values,
	//if the potential_values vector only contains one element 
	//that means only that value can be assigned at that cell
	void update_cells_according_to_potentials()
	{
		update_potentials();
		for(int r = 0; r < RCS_SIZE; r++)
		{
			for(int c = 0; c < RCS_SIZE; c++)
			{
				if(_cells[r][c]._potential_values.size() == 1)
				{
					_cells[r][c]._value = _cells[r][c]._potential_values[0];
					_cells[r][c]._potential_values.clear();
					update_cells_according_to_potentials();
				}
			}
		}
	}

	//retrieves a vector with the missing values of the sector
	vector<int> get_sector_missing_values(int sector)
	{
		vector<int> vals;
		vector<int> missing_vals;
		for(int pos = 0; pos < RCS_SIZE; pos++)
		{
			if(_cell_sectors[sector][pos]->_value != 0)
				vals.push_back(_cell_sectors[sector][pos]->_value);
		}
		for(int i = 1; i < RCS_SIZE+1; i++)
		{
			if(!contains(vals,i))
				missing_vals.push_back(i);
		}
		return missing_vals;
	}

	//determines if the given vector contains the given value
	bool contains(const vector<int>& v, int val)
	{
		for(unsigned i = 0; i < v.size(); i++)
		{
			if(v[i] == val)
				return true;
		}
		return false;
	}

	//determines if a row of the puzzle has the value 
	//contained in the given cell based in the cell's row
	bool does_row_conatin_value(cell *cel, int value)
	{
		for(int c = 0; c < RCS_SIZE; c++)
		{
			if(_cells[cel->_row][c]._value == value)
				return true;
		}
		return false;
	}

	//determines if a column of the puzzle has the value 
	//contained in the given cell based in the cell's column
	bool does_col_conatin_value(cell *cel, int value)
	{
		for(int r = 0; r < RCS_SIZE; r++)
		{
			if(_cells[r][cel->_col]._value == value)
				return true;
		}
		return false;
	}

	//checks if the puzzle has emptry cells (cells with 0)
	bool is_puzzle_solved()
	{
		for(int c = 0; c < RCS_SIZE; c++)
			for(int r = 0; r < RCS_SIZE; r++)
				if(_cells[r][c]._value == 0)
					return false;
		return true;
	}

//-- puzzle initialization methods start here -------------------------------------------------

	//sets the sector, position, row, and column in each cell
	void set_cell_coordinates()
	{
		int position = 0, offset = 0;
		for(int r = 0; r < RCS_SIZE; r++)
		{
			for(int c = 0; c < RCS_SIZE; c++)
			{
				if((r >= 0 && r < 3) && (c >= 0 && c < 3))
					_cells[r][c]._sector = TOP_LEFT;
				else if((r >= 0 && r < 3) && (c >= 3 && c < 6))
					_cells[r][c]._sector = TOP_MID;
				else if((r >= 0 && r < 3) && (c >= 6 && c < 9))
					_cells[r][c]._sector = TOP_RIGHT;
				else if((r >= 3 && r < 6) && (c >= 0 && c < 3))
					_cells[r][c]._sector = MID_LEFT;
				else if((r >= 3 && r < 6) && (c >= 3 && c < 6))
					_cells[r][c]._sector = MID_MID;
				else if((r >= 3 && r < 6) && (c >= 6 && c < 9))
					_cells[r][c]._sector = MID_RIGHT;
				else if((r >= 6 && r < 9) && (c >= 0 && c < 3))
					_cells[r][c]._sector = BOTTOM_LEFT;
				else if((r >= 6 && r < 9) && (c >= 3 && c < 6))
					_cells[r][c]._sector = BOTTOM_MID;
				else if((r >= 6 && r < 9) && (c >= 6 && c < 9))
					_cells[r][c]._sector = BOTTOM_RIGHT;

				_cells[r][c]._position = position + offset;
				_cells[r][c]._row = r;
				_cells[r][c]._col = c;
				_cell_sectors[_cells[r][c]._sector][_cells[r][c]._position] = &_cells[r][c];
				position++;
				if(position == 3)
					position = 0;

			}
			offset += 3;
			if(offset == 9)
				offset = 0;
		}
	}

	//parse a string to set a row of the puzzle at position index
	bool set_row(string &row, int index)
	{
		if(row.size() != RCS_SIZE)
		{
			_err_msg =  "invalid row lenght";
			return false;
		}
		for(unsigned c = 0; c < row.size(); c++)
		{
			if(!isdigit(row[c]))
				return false;
			_cells[index][c]._value = get_int_value(row[c]);
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

//-- puzzle validation methods start here -----------------------------------------------------

	//validates the whole puzzle
	bool is_valid_puzzle()
	{
		for(int i = 0; i < RCS_SIZE; i++)
		{
			if(!is_valid_row(i))
				return false;
			if(!is_valid_column(i))
				return false;
			if(!is_valid_sector(i))
				return false;
		}
		return true;
	}

	//validates the row in a given array of cells
	//a valid row is one that does not have repeated numbers
	//except for 0 which denotes an empty cell
	//the paramater issector is just to better address the error msg
	bool is_valid_row(int row)
	{
		for(int c = 0; c < RCS_SIZE; c++)
		{
			if(_cells[row][c]._value != 0)
			{
				for(int j = RCS_SIZE-1; j > c; j--)
				{
					if(_cells[row][c]._value == _cells[row][j]._value)
					{
						_err_msg = "the puzzle contains invalid row(s)";
						return false;
					}
				}
			}
		}
		return true;
	}

	//validates a column in _cells
	//a valid column is one that does not have repeated numbers
	//except for 0 with denotes an empty cell
	bool is_valid_column(int col)
	{
		for(int r = 0; r < RCS_SIZE; r++)
		{
			if(_cells[r][col]._value != 0)
			{
				for(int j = RCS_SIZE-1; j > r; j--)
				{
					if(_cells[r][col]._value == _cells[j][col]._value)
					{
						_err_msg = "the puzzle contains invalid column(s)";
						return false;
					}
				}
			}
		}
		return true;
	}

	//validates a sector in _cells
	//a sector is a 3x3 square
	//a valid sector is one that does not have repeated numbers
	//except for 0 with denotes an empty cell
	bool is_valid_sector(int sector)
	{
		cell vals[9];
		for(int c = 0; c < RCS_SIZE; c++)
		{
			if(_cell_sectors[sector][c]->_value != 0)
			{
				for(int j = RCS_SIZE-1; j > c; j--)
				{
					if(_cell_sectors[sector][c]->_value == _cell_sectors[sector][j]->_value)
					{
						_err_msg = "the puzzle contains invalid sector(s)";
						return false;
					}
				}
			}
		}
		return true;
	}
};

//program entry point to solve a sudoku puzzle
//the program takes a file name as parameter
int main(int argc, char *argv[])
{
	if(argc == 1)
	{
		cout << "program requires file name as parameter" << endl;
		return 0;
	}

	sudoku_puzzle sd(argv[1]);

	if(!sd.solve())
	{
		cout << sd.get_err_msg() << endl;
		return 0;
	}

	sd.print_puzzle();

	return 1;
}
