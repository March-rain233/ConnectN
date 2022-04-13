#include<iostream>
#include<vector>
#include<map>
#include<string>
#include<random>
#include<stdlib.h>
#include<Windows.h>
using namespace std;
typedef unsigned long long ulong;
struct Item {
    int depth;
    int alpha;
    int beta;
};
map<ulong, Item> CloseList;
const int N = 3;
const int Width = 3;
const int Height = 3;
const int Limit = 11;
const int WinCost = 10000;
int Player = 1;
ulong BoardHash[Height][Width][3];
enum class Status {
    Draw,
    O,
    X,
    Pending,
};
void CreateBoardHash() {
    std::default_random_engine e;
    e.seed(time(0));
    for (int i = 0; i < Height; ++i) {
        for (int j = 0; j < Width; ++j) {
            for (int k = 0; k < 3; ++k) {
                BoardHash[i][j][k] = e();
            }
        }
    }
}
ulong ZobristHash(const int* board) {
    ulong res = 0;
    for (int i = 0; i < Height; ++i) {
        for (int j = 0; j < Width; ++j) {
            res ^= board[i * Width + j];
        }
    }
    return res;
}
WCHAR Chess2Char(int num) {
    switch (num) {
    case 1:
        return 'O';
    case 2:
        return 'X';
    default:
        return ' ';
    }
}
int CheckLine(const int* board, int start, int step) {
    int res = 3;
    for (int i = 0; i < N; ++i) {
        res &= board[start + i * step];
    }
    return res;
}
Status CheckStep(const int* board, int x, int y) {
    int temp;
    bool left = x - N <= -1;
    bool right = x + N >= Width;
    if (right) {
        temp = CheckLine(board, y * Width + x, 1);
        if (temp) return (Status)temp;
    }
    if (left) {
        temp = CheckLine(board, y * Width + x, -1);
        if (temp) return (Status)temp;
    }
    if (y - N <= -1) {
        temp = CheckLine(board, y * Width + x, -Width);
        if (temp) return (Status)temp;
        if (right) {
            temp = CheckLine(board, y * Width + x, -Width + 1);
            if (temp) return (Status)temp;
        }
        if (left) {
            temp = CheckLine(board, y * Width + x, -Width - 1);
            if (temp) return (Status)temp;
        }
    }
    if (y + N >= Width) {
        temp = CheckLine(board, y * Width + x, Width);
        if (temp) return (Status)temp;
        if (right) {
            temp = CheckLine(board, y * Width + x, Width + 1);
            if (temp) return (Status)temp;
        }
        if (left) {
            temp = CheckLine(board, y * Width + x, Width - 1);
            if (temp) return (Status)temp;
        }
    }
    for (int i = 0; i < Width * Height; ++i) 
        if (!board)
            return Status::Draw;
    return Status::Pending;
}
Status Check(const int* board) {
    bool blank = 1;
    for (int i = 0; i < Height; ++i) {
        for (int j = 0; j < Width; ++j) {
            blank &= !!board[i * Width + j];
            if (board[i * Width + j]) {
                int temp;
                if (j + N <= Width) {
                    temp = CheckLine(board, i * Width + j, 1);
                    if (temp) return (Status)temp;
                }
                if (i + N <= Height) {
                    temp = CheckLine(board, i * Width + j, Width);
                    if (temp) return (Status)temp;
                    if (j + N <= Width) {
                        temp = CheckLine(board, i * Width + j, Width + 1);
                        if (temp) return (Status)temp;
                    }
                    if (j - N >= -1) {
                        temp = CheckLine(board, i * Width + j, Width - 1);
                        if (temp) return (Status)temp;
                    }
                }
            }
        }
    }
    if (!blank) return Status::Draw;
    return Status::Pending;
}
void OutputBoard(const int* board) {
    const WCHAR column = '|';
    const WCHAR row = '-';
    auto drawline = [&row]()->void {
        wcout << "  ";
        wcout << '+';
        for (int i = 0; i < Width; ++i) {
            wcout << row;
            wcout << row;
            wcout << row;
            wcout << '+';
        }
        wcout << endl;
    };
    wcout << "   ";
    for (int i = 0; i < Width; ++i) {
        wcout << " " << i + 1 << "  ";
    }
    wcout << endl;
    drawline();
    for (int i = 0; i < Height; ++i) {
        wcout << i + 1 << " ";
        wcout << column;
        for (int j = 0; j < Width; ++j) {
            wcout << ' ' << Chess2Char(board[i * Width + j]) << ' ' << column;
        }
        wcout << endl;
        drawline();
    }
}
int Eval(const int* board, int turn) {
    int h = 0;
    
    return h;
}
int AlphaBeta(int* board, ulong hash, int turn, int alpha, int beta, int depth) {
    Status sta = Check(board);
    if (sta != Status::Draw) {
        if (sta == Status::Pending) return 0;
        if ((int)sta == Player) return -WinCost;
        return WinCost;
    }
    if (depth >= Limit) {
        return Eval(board, turn);
    }
    if (Player != turn) {
        for (int i = 0; i < Width * Height; ++i) {
            if (!board[i]) {
                board[i] = turn;
                int x = i % Width;
                int y = i / Width;
                ulong newHash = hash ^ BoardHash[y][x][0] ^ BoardHash[y][x][turn];
                int temp = AlphaBeta(board, newHash, turn ^ 3, alpha, beta, depth + 1);
                board[i] = 0;
                alpha = alpha < temp ? temp : alpha;
                if (alpha > beta) return alpha;
            }
        }
        return alpha;
    }
    else {
        for (int i = 0; i < Width * Height; ++i) {
            if (!board[i]) {
                board[i] = turn;
                int x = i % Width;
                int y = i / Width;
                ulong newHash = hash ^ BoardHash[y][x][0] ^ BoardHash[y][x][turn];
                int temp = AlphaBeta(board, newHash, turn ^ 3, alpha, beta, depth + 1);
                board[i] = 0;
                beta = beta > temp ? temp : beta;
                if (alpha > beta) return beta;
            }
        }
        return beta;
    }
}
int Solve(int* board, ulong hash, int turn) {
    int maxH = INT_MIN;
    int maxIndex = -1;
    for (int i = 0; i < Width * Height; ++i) {
        if (board[i]) continue;
        board[i] = turn;
        int x = i % Width;
        int y = i / Width;
        ulong newHash = hash ^ BoardHash[y][x][0] ^ BoardHash[y][x][turn];
        int h = AlphaBeta(board, newHash, turn ^ 3, INT_MIN, INT_MAX, 1);
        board[i] = 0;
        if (h > maxH) {
            maxH = h;
            maxIndex = i;
        }
    }
    return maxIndex;
}
int main() {
    wcout.imbue(locale("chs"));
    CreateBoardHash();
    int* board = new int[Width * Height];
    memset(&board[0], 0, Width * Height * sizeof(int));
    ulong hash = ZobristHash(board);
    int turn = 1;
    Status sta;
    do {
        system("cls");
        if (Player == turn) {
            wcout << L"玩家回合 ";
        }
        else {
            wcout << L"AI回合 ";
        }
        wcout << L"当前执子方:" << Chess2Char(turn) << endl;
        OutputBoard(board);
        if (Player == turn) {
            int x, y;
            wcout << L"输入着子坐标:";
            while (1) {
                cin >> x >> y;
                x -= 1;
                y -= 1;
                if (x < 0 || y < 0 || x >= Width || y >= Height
                    || board[y * Width + x]) {
                    wcout << L"错误输入,请重新输入：";
                }
                else break;
            }
            board[y * Width + x] = turn;
            hash ^= BoardHash[y][x][0] ^ BoardHash[y][x][turn];
        }
        else {
            int index = Solve(board, hash, turn);
            int x = index & Width;
            int y = index / Width;
            board[index] = turn;
            hash ^= BoardHash[y][x][0] ^ BoardHash[y][x][turn];
            //Sleep(300);
        }
        turn ^= 3;
        sta = Check(board);
    } while (sta == Status::Draw);
    system("cls");
    wcout << L"——游戏结束——" << endl;
    OutputBoard(board);
    wcout << L"当前获胜方:";
    if (sta == Status::Pending) {
        wcout << L"平局" << endl;
    }
    else {
        if (Player == (int)sta) {
            wcout << L"玩家" << endl;
        }
        else {
            wcout << L"AI" << endl;
        }
    }
    return 0;
}