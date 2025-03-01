#include<iostream>
#include <queue>

using namespace std;

struct ASTNode {
    string value;
    ASTNode *left;
    ASTNode *right;
};

class Function {
    public:
        Function();
        ~Function();
        void checkTypeOfToken(string &token);
        bool isINT(string &token);
        bool isDOT(string &token);
        bool isFLOAT(string &token);
        bool isNIL(string &token);
        bool isT(string &token);
        bool isQUOTE(string &token);
    private:
        string input;
};

class Reader {
    public:
        Reader();
        ~Reader();
        void read();
        void saperateExprToToken(string &expr);
        void storeTokenInAST(string &token);
    private:
        Function function;
        int left_paren;
        ASTNode *root;
        ASTNode *cur;
};

int main() {
    Reader reader;
    reader.read();
    return 0;
}

Reader::Reader() {
    left_paren = 0;
    root = new ASTNode();
    function = Function();
    cout << "Reader created" << endl;
}

Reader::~Reader() {
    cout << "Reader destroyed" << endl;
}

void Reader::read() {

    cout << "Welcome to OurScheme!" << endl;
    cout << "> ";
    char ch;    // 讀入字元
    string expr;
    cur = root;
    while ( getline(cin, expr) ) {
        cout << "You entered: " << ch << " " << expr << endl;
        if ( expr.empty() ) continue; // 該行並未輸入資料
        saperateExprToToken(expr);
        if ( !left_paren ) cout << "> ";
    }
    cout << "Goodbye!" << endl;
}

void Reader::saperateExprToToken(string &expr) {
    cout << "saperateExprToToken" << endl;
    int nil = 0; // 判斷是否為空括弧
    string token;
    for ( auto &c : expr ) {
        if ( c == '(' ) {
            ++left_paren;
            nil = 0;
        }
        else if ( c == ')' ) {
            --left_paren;
            if ( !nil && token.empty() ) {
                // 遇到空括弧，將nil存入AST
                cout << "nil" << endl;
                token = "nil";
                storeTokenInAST(token);
                ++nil;
            }
            else if ( !token.empty() ) {
                // 遇到右括弧時，還有資料未寫入AST
                storeTokenInAST(token);
            }
            token.clear();
        }
        else if ( c == '\'' ) {
            // 遇到引號，將quote存入AST
            token = "'";
            storeTokenInAST(token);
            token.clear();
        }
        else if ( c == ' ' ) {
            // 遇到空白，將token存入AST
            if ( token.empty() ) continue;
            storeTokenInAST(token);
            token.clear();
            ++nil; // 該括弧並不為空
        }
        else token += c;
    }

    if ( !token.empty() ) {
        storeTokenInAST(token);
        token.clear();
    }
}

void Reader::storeTokenInAST(string &token) {
    cout << "stroeTokenInAST" << endl;
    // 確認Token型態並更新其內容
    function.checkTypeOfToken(token);
    cout << "token: " << token << endl; // debug
    // 將1筆token存入新節點
    ASTNode *new_node = new ASTNode();
    new_node->value = token;
    new_node->left = NULL;
    new_node->right = NULL;

    // 將新節點存入AST，並更新目前節點位置
    cur->left = new_node;
    cur->right = new ASTNode();
    cur = cur->right;
}

Function::Function() {
    cout << "Function created" << endl;
}

Function::~Function() {
    cout << "Function destroyed" << endl;
}

void Function::checkTypeOfToken(string &token) {
    // 分辨Token的型態
    cout << "checkTypeOfToken" << endl;
    if ( isINT(token) ) cout << "INT" << endl;
    else if ( isDOT(token) ) cout << "DOT" << endl;
    else if ( isFLOAT(token) ) cout << "FLOAT" << endl;
    else if ( isNIL(token) ) cout << "NIL" << endl;
    else if ( isT(token) ) cout << "T" << endl;
    else if ( isQUOTE(token) ) cout << "QUOTE" << endl;
    else cout << "SYMBOL" << endl;
}

bool Function::isINT(string &token) {
    /*檢查Token是否為整數*/
    int len = token.size();
    string num;

    for ( int i = 0; i < len; ++i ) {
        // 數字開頭可以有正負號
        if ( i == 0 && (token[i] == '+' || token[i] == '-') ) {
            if ( token[i] == '-' ) num += '-';
            continue;
        }
        else if ( !isdigit(token[i]) ) return false;
        else num += token[i];
    }

    token = num;
    return true;
}

bool Function::isDOT(string &token) {
    /*檢查Token是否為點*/
    int len = token.size();
    if ( len == 1 && token[0] == '.' ) return true;
    return false;
}

bool Function::isFLOAT(string &token) {
    /*檢查Token是否為浮點數*/
    int len = token.size(), decimal = 0;
    string num;
    bool dot = false, interger = false;

    for ( int i = 0; i < len; ++i ) {
        // 數字開頭可以有正負號
        if ( i == 0 && (token[i] == '+' || token[i] == '-') ) {
            if ( token[i] == '-' ) num += '-';
            continue;
        }
        else if ( !isdigit(token[i]) ) {
            if ( token[i] == '.' && !dot ) {
                dot = true;
                if ( !interger ) num += '0'; // 若並未輸入整數位，則補0
                num += '.';
            }
            else return false;
        }
        else {
            if ( decimal >= 3 ) continue; // 小數點後最多3位
            num += token[i];
            interger = true;
            if ( dot ) ++decimal; // 計算小數位數
        }
    }

    while ( decimal < 3 ) {
        // 若小數位數不足3位，則補0
        num += '0';
        ++decimal;
    }

    token = num;
    return true;
}

bool Function::isNIL(string &token) {
    /*檢查Token是否為空*/
    if ( token == "nil" || token == "#f" ) return true;
    return false;
}

bool Function::isT(string &token) {
    /*檢查Token是否為T*/
    if ( token == "t" || token == "#t" ) {
        token = "#t";
        return true;
    }
    return false;
}

bool Function::isQUOTE(string &token) {
    /*檢查Token是否為引號*/
    if ( token == "'" ) return true;
    return false;
}