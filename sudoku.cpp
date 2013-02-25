#include <iostream>
#include <fstream>

using namespace std;

class sudoku_puzzle
{

#define SIZE       9      

public:
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
			_good_file = false;
		}
	}
	
	bool solve()
	{
		if(!_good_file)
			return false;
		if(!is_valid_puzzle())
			return false;
			
		return true;
	}
	
	void print_puzzle()
	{
		for(int i = 0; i < SIZE; i++)
		{
			for(int j = 0; j < SIZE; j++)
				cout << _rows[i][j] << " ";
			cout << endl;
		}
	}
	
private:
	bool _good_file;
	int _rows[SIZE][SIZE];
	
	bool set_row(string &row, int index)
	{
		if(row.size() != SIZE)
			return false;
		for(unsigned i = 0; i < row.size(); i++)
		{
			if(!isdigit(row[i]))
				return false;
			_rows[index][i] = get_int_value(row[i]);
		}
		return true;
	}
	
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
	
	bool is_valid_puzzle()
	{
		for(int i = 0; i < SIZE; i++)
		{
			if(!is_valid_row(i))
				return false;
		}
		return true;
	}
	
	bool is_valid_row(int row)
	{
		for(int i = 0; i < SIZE; i++)
		{
			if(_rows[row][i] != 0)
			{
				for(int j = SIZE-1; j > i; j--)
				{
					if(_rows[row][i] == _rows[row][j])
						return false;
				}
			}
		}
		return true;
	}
};

int main(int argc, char *argv[])
{
	if(argc == 1)
		return 0;
		
	sudoku_puzzle sd(argv[1]);
	
	if(!sd.solve())
	{
		cout << endl << "Invalid puzzle" << endl << endl;
		system("pause");
		return 0;
	}
	
	sd.print_puzzle();
	
	system("pause");
	
	return 1;
}
