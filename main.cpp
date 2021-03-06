#include <iostream>
#include <string>
#include <cstdlib>

#define DEBUG_MODE
#undef DEBUG_MODE

using namespace std;

// Markings
const short int INCLUDE = 0000;
const short int EXCLUDE = 0010;
const short int MARKED = 0100;

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
  int llow = 2, lupp = 19;

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
  board_width = IntegerInput("Board width? [6] >> ", 6);
  board_width = abs(board_width);

  board_height = IntegerInput("Board height? [6] >> ", 6);
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
    tiles[i] = INCLUDE;
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
    printf(" %3d ", j);
  }
  cout << endl;

  cout << "   Target|";
  for (int j = 0; j < board_width; j++) {
    printf(" %3d ", sums[board_width + j]);
  }
  cout << endl;
  
  // Upper dashes
  cout << "Row|-----|";
  for (int j = 0; j < board_width; j++) {
    cout << "-----";
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
      
      if (!(tiles[index] & EXCLUDE)) {
        col_sums[j] += current;
        row_sum += current;
      }

      if (flag & EXCLUDE){ cout << "-";}
      else {cout << " ";}

      if (flag & MARKED){ printf("[%2d]", current);}
      else {printf(" %2d ", current);}
    }
    printf("| %3d", row_sum);
    cout << endl;
  }

  cout << "          -";
  for (int j = 0; j < board_width; j++) {
    cout << "-----";
  }
  cout << endl;

  cout << "         ";
  for (int j = 0; j < board_width; j++) {
    printf(" %4d", col_sums[j]);
  }
  cout << endl;

  delete[] col_sums;
}

bool CheckCommand(string cmd, string input) {
  return cmd.compare(input) == 0;
}

void MarkTile(string cmd) {
  short int xor_operand;

  int mark_position = cmd.find("*");
  if (mark_position > cmd.size()) {
    mark_position = cmd.find("-");
    xor_operand = EXCLUDE;
  } else {
    xor_operand = MARKED;
  }

  int dash_position = cmd.find("-", mark_position + 1);

  if (dash_position < cmd.size() && mark_position < cmd.size()) {
    string first = cmd.substr(mark_position + 1, dash_position);
    string second = cmd.substr(dash_position + 1, cmd.size());

    #ifdef DEBUG_MODE
    cout << "First part " << first;
    cout << " Second part " << second << endl;
    #endif

    cout << endl;

    int row;
    int col;

    // Mark all column if row is missing.
    try {
      row = stod(first);
    } catch (const exception& x) {
      try {
        col = stod(second);
      } catch (exception& x) {
        cout << "Can't understand the command." << endl;
        cout << endl << endl;
        return;
      }
      if (abs(col) < board_width) {
        #ifdef DEBUG_MODE
        cout << "Marking whole column " << col << endl;
        #endif

        short int index;
        for (int i = 0; i < board_height; i++) {
          index = i * board_width + col;
          
          #ifdef DEBUG_MODE
          printf("Row: %d, Col: %d, Index: %d", i, col, index);
          cout << endl;
          #endif

          if ((!(tiles[index] & MARKED) && xor_operand != MARKED) ||
              xor_operand == MARKED){
            #ifdef DEBUG_MODE
            cout << "tile is not marked" << endl;
            #endif

            tiles[index] ^= xor_operand;
            cout << "Toggled row: " << i;
            cout << " and col: " << col;
            cout << endl << endl;
          }
        }

        return;

      } else {
        cout << "Col number is out of bounds." << endl;
        cout << endl << endl;
        return;
      }
    }

    // Since no error is caught for row conversion above, we test col again.
    // If it's missing, we mark the whole row.
    try {
      col = stod(second);
    } catch(exception& x) {
      if (abs(row) < board_height) {
        #ifdef DEBUG_MODE
        cout << "Marking whole row " << col << endl;
        #endif

        short int index;
        for (int j = 0; j < board_width; j++) {
          index = row * board_width + j;

          #ifdef DEBUG_MODE
          printf("Row: %d, Col: %d, Index: %d", row, j, index);
          cout << endl;
          #endif

          if ((!(tiles[index] & MARKED) && xor_operand != MARKED) ||
              xor_operand == MARKED){
            #ifdef DEBUG_MODE
            cout << "tile is not marked" << endl;
            #endif

            tiles[index] ^= xor_operand;
            cout << "Toggled row: " << row;
            cout << " and col: " << j;
            cout << endl << endl;
          }
        }

        return;

      } else {
        cout << "Row number is out of bounds." << endl;
        cout << endl << endl;
        return;
      }
    }

    // At this point, both row and col are there so we proceed as normal.
    short int index = row * board_width + col;
    #ifdef DEBUG_MODE
    printf("Row: %d, Col: %d, Index: %d", row, col, index);
    cout << endl;
    #endif
    
    if ((!(tiles[index] & MARKED) && xor_operand != MARKED) ||
        xor_operand == MARKED){
      if (abs(row) < board_height && abs(col) < board_width) {
        #ifdef DEBUG_MODE
        cout << "tile is not marked" << endl;
        #endif

        cout << endl;

        tiles[index] ^= xor_operand;
        cout << "Toggled row: " << row;
        cout << " and col: " << col;
        cout << endl << endl;

      }
      else {
        cout << "Row or col number is out of bounds." << endl;
        cout << endl << endl;
        return;
      }
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
      if (!(tiles[index] & EXCLUDE)) {
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

  cout << "Rullover" << endl;

  #ifdef DEBUG_MODE
  srand(42);
  #else
  srand(time(NULL));
  #endif

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

    MarkTile(input);
    
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