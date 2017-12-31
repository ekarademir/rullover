#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

// Markings
const short int INCLUDE = 0;
const short int EXCLUDE = 1;

short int board_width, board_height, limit_lower, limit_upper;
short int *board;
short int *tiles;
short int *mask;
int *sums;

string
Input(string question)
{
  string answer;
  
  cout << question;
  getline(cin, answer);

  return answer;
}

int
IntegerInput(string question, int def=0)
{
  string t = Input(question);
  int ret;
  try {
    ret = stoi(t);
  }
  catch (const exception& x) {
    ret = def;
  }

  return ret;
}

void GetLimits()
{
  string t;
  int llow = 1, lupp = 15;

  string question = "Numbers range - inclusive, less than or 100 - ? [";
  question += to_string(llow);
  question += "-";
  question += to_string(lupp);
  question += "] >> ";

  t = Input(question);
  int p = t.find('-');
  
  if (p > 0) {
    int len_left = p - 0;
    int len_right = t.size() - p - 1;

    try {
      limit_lower = stoi(t.substr(0, len_left));
    }
    catch (const exception& x) {
      limit_lower = llow;
    }

    try {
      limit_upper = stoi(t.substr(p + 1, len_right));
    }
    catch (const exception& x) {
      limit_upper = lupp;
    }
    
  } else {
    limit_lower = llow;
    limit_upper = lupp;
  }

  limit_lower = abs(limit_lower);
  limit_upper = abs(limit_upper);

  if (limit_lower > limit_upper) {
    int t = limit_lower;
    limit_lower = limit_upper;
    limit_upper = t;
  }

  if (limit_lower >= 100) {limit_lower = 99;}
  if (limit_upper >= 100) {limit_upper = 99;}

}

void
FillBoard()
{
  int arr_size = board_height * board_width;

  short int t;
  short int range = limit_upper - limit_lower + 1;

  for (int i = 0; i < arr_size; i++) {
    t = limit_lower + rand() % range;
    board[i] = t;
  }

  for (int i = 0; i < arr_size; i++) {
    t = rand() % 2;
    mask[i] = t;
  }

  short int index;
  for (int i = 0; i < board_height; i++) {
    for (int j = 0; j < board_width; j++) {
      index = i * board_width + j;
      if (mask[index] == 0) {
        sums[i] += board[index];
        sums[board_height + j] += board[index];
      }
    }
  }
}

void
SetBoard()
{
  board_width = IntegerInput("Board width? [10] >> ", 10);
  board_width = abs(board_width);

  board_height = IntegerInput("Board height? [10] >> ", 10);
  board_height = abs(board_height);

  board = new short int[board_height * board_width];
  tiles = new short int[board_height * board_width];
  mask = new short int[board_height * board_width];
  sums = new int[board_height + board_width];

  for (int i  = 0; i < board_height + board_width; i++) {
    sums[i] = 0;
  }

  for (int i = 0; i < board_height * board_width; i++) {
    board[i] = 0;
    tiles[i] = 0;
    mask[i] = 0;
  }
  
  GetLimits();

  FillBoard();
}

void
UnsetBoard()
{
  delete[] board;
  delete[] tiles;
  delete[] mask;
  delete[] sums;
}

void
PrintBoard()
{
  // Column numbers
  cout << "      Col ";
  for (int j = 0; j < board_width; j++) {
    printf("%3d ", j);
  }
  cout << endl;

  cout << "   Target|";
  for (int j = 0; j < board_width; j++) {
    printf("%3d ", sums[board_width + j]);
  }
  cout << endl;
  
  // Upper dashes
  cout << "Row|-----|";
  for (int j = 0; j < board_width; j++) {
    cout << "----";
  }
  cout << endl;

  short int current;
  short int flag;
  short int not_included;
  short int index;

  int *col_sums = new int[board_width];
  for (int j  = 0; j < board_width; j++) {
    col_sums[j] = 0;
  }

  for (int i = 0; i < board_height; i++) {
    
    printf("%3d  %4d|", i, sums[i]);
    
    int row_sum = 0;
    for (int j = 0; j < board_width; j++) {
      index = i * board_width + j;
      current = board[index];
      flag = tiles[index];
      not_included = mask[index];
      
      if (tiles[index] == INCLUDE) {
        col_sums[j] += current;
        row_sum += current;
      }

      if (flag == EXCLUDE){ cout << "-";}
      else {cout << " ";}

      printf("%2d ", current);
    }
    printf("| %3d", row_sum);
    cout << endl;
  }

  cout << "          -";
  for (int j = 0; j < board_width; j++) {
    cout << "----";
  }
  cout << endl;

  cout << "         ";
  for (int j = 0; j < board_width; j++) {
    printf("%4d", col_sums[j]);
  }
  cout << endl;

  delete[] col_sums;
}

bool CheckCommand(string cmd, string input) {
  return cmd.compare(input) == 0;
}

void CheckTile(string cmd) {
  int dash_position = cmd.find("-");

  if (dash_position < cmd.size()) {
    string first = cmd.substr(0, dash_position);
    string second = cmd.substr(dash_position + 1, cmd.size());

    cout << endl;

    int row;
    int col;
    try {
      row = stod(first);
      col = stod(second);
    } catch (const exception& x) {
      cout << "Invalid row or col number" << endl;
      cout << endl << endl;
      return;
    }

    if (abs(row) < board_height && abs(col) < board_width) {
      short int address = row * board_width + col;
      if (tiles[address] == EXCLUDE) {
        tiles[address] = INCLUDE;
      }
      else if (tiles[address] == INCLUDE) {
        tiles[address] = EXCLUDE;
      }

      cout << "Toggled row: " << row;
      cout << " and col: " << col;
      cout << endl << endl;

    }
    else {
      cout << "Invalid row or col number" << endl;
      cout << endl << endl;
      return;
    }
  }
}


bool CheckGame() {

  short int len = board_height + board_width;
  int *t_sums = new int[len];

  for (int i  = 0; i < len; i++) {
    t_sums[i] = 0;
  }

  short int index;
  for (int i = 0; i < board_height; i++) {
    for (int j = 0; j < board_width; j++) {
      index = i * board_width + j;
      if (tiles[index] == INCLUDE) {
        t_sums[i] += board[index];
        t_sums[board_height + j] += board[index];
      }
    }
  }

  for (int i  = 0; i < len; i++) {
    if (t_sums[i] != sums[i]) {
      delete[] t_sums;
      return false;
    }
  }

  delete[] t_sums;
  return true;
}


int
main() 
{

  string prompt = ">>> ";
  string input;

  cout << "Rullo" << endl;

  srand(42);

  SetBoard();

  string cmd;
  bool loop = true;
  while (loop) {
    PrintBoard();
    cout << prompt;
    getline(cin, input);

    if (CheckCommand("exit", input)) {
      loop = false;
    }

    CheckTile(input);
    
    if(CheckGame()) {
      cout << endl << endl;
      cout << "     YOU WON!!!!";
      cout << endl << endl;
      loop = false;
    }
  }

  UnsetBoard();
  

  return 0;
}