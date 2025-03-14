#include <iostream>
#include <queue>
#include <stack>

using namespace std;

#define LEFT_PAREN "LEFT_PAREN"
#define INT "INT"
#define STRING "STRING"
#define DOT "DOT"
#define FLOAT "FLOAT"
#define NIL "NIL"
#define T "T"
#define QUOTE "QUOTE"
#define SYMBOL "SYMBOL"

#define TEMP "TEMP"
#define BEGIN "BEGIN"
#define END "END"
#define LINK "LINK"

struct ASTNode {
    string type, value;
    ASTNode *left;
    ASTNode *right;
};

class Atom {
    public:
        Atom();
        ~Atom();
        bool isINT(string &atom);
        bool isSTRING(string &atom);
        bool isDOT(string &atom);
        bool isFLOAT(string &atom);
        bool isNIL(string &atom);
        bool isT(string &atom);
        bool isQUOTE(string &atom);
};

class Error {
    public:
        Error();
        ~Error();
        bool unexpectedTokenOfRightParen(ASTNode *cur, string &token, int &line, int &column, string &error);
        // bool unexpectedTokenOfAtomOrLeftParen(string &token, int &line, int &column);
        // bool noClosingQuote(int &line, int &column);
        // bool endOfFile();
};

class Function {
    public:
        Function();
        ~Function();
        ASTNode *getRoot() { return root; };
        bool getDot() { return dot; };
        int getQuote() { return quote; };
        void newAST();
        void checkTypeOfAtom(string &atom, string &type);
        void storeTokenInAST(string &token, int &line, int &column);
        void checkTokenToStore(string &token, int &line, int &column, char &sep);
        void newExpression(string &token, int &line, int &column);
        void backExpression();
        void quoteExpression(string &token, int &line, int &column);
        bool checkExit();
        void optimizeAST();
        void leafToRoot(ASTNode *root, int level);
        bool printAST();
    private:
        ASTNode *root;
        ASTNode *cur;
        stack<ASTNode*> ast;
        bool dot;
        int quote;
        Atom atom;
        Error error;
        string input;
        stack<pair<ASTNode*, int>> st;
};

class Separator {
    public:
        Separator(Function &function);
        ~Separator();
        void leftParen(string &token, int &line, int &column);
        void rightParen(string &token, int &nil, int &left_paren, int &line, int &column);
        void singleQuote(string &token, int &line, int &column);
        void doubleQuote(string &token, int &line, int &column, bool &str, bool &backslash, char &sep);
    private:
        Function &function;

};

class Reader {
    public:
        Reader(Function &function);
        ~Reader();
        void read();
        bool readNextLine(string &expr, int &line, int &nil);

        void saperateExprToToken(string &expr, int &line, int &nil);
    private:
        int left_paren;
        Separator separtor;
        Function &function;
};

int main() {
    //string num;   // 測資題號
    //getline(cin, num);
    Function function;
    Reader reader(function);
    reader.read();
    return 0;
}

Reader::Reader(Function &function) : function(function), separtor(function) {
    cout << "Reader created" << endl;
}

Reader::~Reader() {
    cout << "Reader destroyed" << endl;
}

void Reader::read() {

    cout << "Welcome to OurScheme!" << endl;
    cout << "\n> ";
    string expr;
    left_paren = 0; // 預設左括弧為 0
    int line = 0, nil = 0;    // 紀錄行數

    while ( true ) {
        // 重複讀取下一個S-expression
        try {
            if ( !readNextLine(expr, line, nil) ) break;   // 讀取到exit結束
        }
        catch ( const char *msg ) {
            cerr << msg;
            break;  // 讀取到EOF結束
        }
    }

    cout << "\nThanks for using OurScheme!" << endl;
    cout.flush(); // 避免緩衝區輸出並未顯示
}

bool Reader::readNextLine(string &expr, int &line, int &nil) {
    /*  讀取下一行輸入的資料
        若遇到EOF則拋出例外 */

    if ( !getline(cin, expr) ) throw "ERROR (no more input) : END-OF-FILE encountered"; // 讀到EOF
    ++line;    // 紀錄行數

    cout << "You entered: " << expr << " " << endl;    // debug
    try {
        saperateExprToToken(expr, line, nil);  // 將輸入的S-expression分割成Token
    }
    catch ( const std::runtime_error &msg ) {
        cerr << msg.what();
        cout << "\n> ";
        // 因錯誤而結束該S-expression，重新讀取下一行時需全部清空
        line = 0;
        left_paren = 0;
        function.newAST();
        return true;
    }
    catch ( const char *msg ) {
        if ( msg == "Exit" ) return false;
    }

    return true;

}

void Reader::saperateExprToToken(string &expr, int &line, int &nil) {
    /*  1: 將讀入的一行指令，以字元方式進行分析
        2: 並呼叫函數儲存至 AST
        3: 記錄行列數以因應錯誤 */
    int column = 0; // 紀錄目前字元列位置
    bool str = false, backslash = false;
    bool lineAreNotEmpty = false;   // 判斷該行行數是否需要被算入下一筆 Token
    string token;

    for ( auto &c : expr ) {
        ++column;
        if ( !line ) ++line;    // 避免該行的 S-expression 已結束，但該行後續還有資料
        if ( c != ' ' && c != ';' ) lineAreNotEmpty = true;
        if ( !str && c == '(' ) {
            ++left_paren;
            try {
                function.checkTokenToStore(token, line, column, c);
                separtor.leftParen(token, line, column);
            }
            catch ( const std::runtime_error &msg ) {
                throw msg;
            }
            nil = 0;
        }
        else if ( !str && c == ')' ) {
            --left_paren;
            try {
                function.checkTokenToStore(token, line, column, c);
                separtor.rightParen(token, nil, left_paren, line, column);
            }
            catch ( const std::runtime_error &msg ) {
                throw msg;
            }
            catch ( const char *msg ) {
                throw msg;
            }
        }
        else if ( !str && c == '\'' ) {
            // 遇到引號，將quote存入AST
            try {
                function.checkTokenToStore(token, line, column, c);
                separtor.singleQuote(token, line, column);
            }
            catch ( const std::runtime_error &msg ) {
                throw msg;
            }
        }
        else if ( !str && c == ' ' ) {
            // 遇到空白，將token存入AST
            try {
                function.checkTokenToStore(token, line, column, c);
            }
            catch ( const std::runtime_error &msg ) {
                throw msg;
            }
            token.clear();
        }
        else if ( !str && c == ';') {
            // 遇到註解，則該行後面的資料皆為註解
            break;
        }
        else if ( c == '\"' ) {
            // 遇到雙引號，將字串存入AST
            try {
                separtor.doubleQuote(token, line, column, str, backslash, c);
            }
            catch ( const std::runtime_error &msg ) {
                throw msg;
            }
            ++nil; // 該括弧並不為空
        } 
        else {
            if ( c == '\\') backslash = true; // 遇到反斜線
            else backslash = false;
            token += c;
            ++nil;
        }
    }

    if ( str ) {
        // 若字串在該行未結束，則拋出錯誤
        string error = "ERROR (no closing quote) : END-OF-LINE encountered at Line " + to_string(line) + " Column " + to_string(column + 1) + "\n";
        throw std::runtime_error(error);
    }
    else if ( !token.empty() ) {
        // 若該行最後一個 Token 未存入 AST ，則存入
        try {
            ++column;
            function.storeTokenInAST(token, line, column);
        }
        catch ( const std::runtime_error &msg ) {
            throw msg;
        }
        token.clear();
    }
    
    if ( left_paren || function.getQuote() ) return;    // 該 S-expression 未結束，繼續讀取下一行
    else if ( function.printAST() || lineAreNotEmpty ) line = 0;    // 當 AST 樹輸出結束，或該行早已輸出結果，則須更新行數
    
}

void Function::storeTokenInAST(string &token, int &line, int &column) {
    // cout << "stroeTokenInAST" << endl; // debug
    // 確認Token型態並更新其內容
    string type;
    checkTypeOfAtom(token, type);
    cout << "token: " << token << " " << type << " " << column << endl; // debug
    cout << "cur: " << cur->value << " " << cur->type << endl; // debug
    // cout << "dot: " << dot << endl; // debug
    
    if ( cur->type != BEGIN && cur->type != LEFT_PAREN && cur->type != TEMP && cur->type != "QUOTE_TEMP" ) {
        string error = "ERROR (unexpected token) : ')' expected when token at Line " + to_string(line) + " Column " + to_string(column - token.size()) + " is >>" + token + "<<\n";
        throw std::runtime_error(error); 
    }
    if ( type == DOT ) {
        // 若該token為DOT，則下一個token為另一個S-expression
        if ( cur->type == BEGIN || cur->type == LEFT_PAREN || dot || ast.top()->type == "QUOTE_TEMP" ) {
            // 前方並未存在S-expression
            string error = "ERROR (unexpected token) : atom or '(' expected when token at Line " + to_string(line) + " Column " + to_string(column - 1) + " is >>" + token + "<<\n";
            throw std::runtime_error(error);
        }
        dot = true;
        return;
    }
    else if ( (token == "nil" || token == ")")  && ( cur->value == "(" || cur->value == "DOT_LEFT_PAREN" || dot ) ) {
        if ( token == ")" && cur->value == "(" ) {
            // 左右括弧內部資料空成立，則該位置為 nil
            cur->type = NIL; 
            cur->value = "nil"; 
            // 為葉節點
            cur->left = NULL;
            cur->right = NULL;
        }
        else if ( cur->value == "DOT_LEFT_PAREN" || dot ) {
            // nil 將存在右子樹，則不需儲存
            cur->value = "nil";
            cur->type = END;
            dot = false;
        }
        else {
            // 輸入字串 nil or #f
            if ( cur->type == TEMP || cur->type == "QUOTE_TEMP" ) 
                cur->type = LINK; // debug

            ASTNode *new_node = new ASTNode();
            new_node->value = token;
            new_node->type = type;
            // 為葉節點
            new_node->left = NULL;
            new_node->right = NULL;

            // 將新節點存入AST，並更新目前節點位置
            cur->left = new_node;
            cur->right = new ASTNode();
            cur = cur->right;

            // 預設該位置為AST結尾節點
            cur->type = TEMP;
        }
        return;
    }

    // 將1筆token存入新節點

    if ( dot ) {
        // 若前一個token為DOT，則將該token存入右節點
        cur->value = token;
        cur->type = type;
        cur->left = NULL;
        cur->right = new ASTNode();
        cur->right->type = END; // 預設該位置為AST結尾節點
        dot = false;
        return;
    }

    ASTNode *new_node = new ASTNode();
    new_node->value = token;
    new_node->type = type;
    new_node->left = NULL;
    new_node->right = NULL;

    if ( cur->type == BEGIN ) {
        // 若當前token為空，則將該token存入左節點
        // cout << "BEGIN" << endl;   // debug
        cur->value = token;
        cur->type = type;
        cur->left = NULL;
        cur->right = NULL;
        printAST();
        line = 0;
        column = 0;
        return;
    }
    // 將新節點存入AST，並更新目前節點位置
    if ( cur->type == TEMP || cur->type == "QUOTE_TEMP" ) 
        cur->type = LINK; // debug
    cur->left = new_node;
    cur->right = new ASTNode();
    cur = cur->right;

    // 預設該位置為AST結尾節點
    cur->type = TEMP;
    
    if ( type != QUOTE && quote > 0 && !ast.empty() && ast.top()->type == "QUOTE_TEMP" ) {
        // 若前一個S-expression結束，且上一個節點為END，則再回到上一個S-expression
        cur->type = END;
        backExpression();
        if ( ast.empty() ) {
            printAST();
            line = 0, column = 0;
        }
    }
}

Function::Function() {
    // cout << "Function created" << endl;  // debug
    atom = Atom();
    error = Error();
    root = new ASTNode();   // 建立AST
    root->type = BEGIN;
    root->value = "";
    cur = root;
    dot = false;
    quote = false;
}

Function::~Function() {
    // cout << "Function destroyed" << endl;   // debug
}

void Function::newAST() {
    // cout << "newAST" << endl; // debug
    // 建立新的AST
    delete root;
    root = new ASTNode();
    root->type = BEGIN;
    root->value = "";
    cur = root;
    dot = false;
    quote = false;
}

bool Function::checkExit() {
    // 檢查是否為exit的AST定義
    if ( root->left == NULL || root->right == NULL ) return false;  // 避免單一 Atom 的 S-expression 造成錯誤
    else if ( root->left->value == "exit" && root->right->type == END ) return true;
    return false;
}

void Function::checkTypeOfAtom(string &atom, string &type) {
    /* 檢查Token型態，並將設定其型態 */
    if ( this->atom.isINT(atom) ) type = INT;
    else if ( this->atom.isSTRING(atom) ) type = STRING;
    else if ( this->atom.isDOT(atom) ) type = DOT;
    else if ( this->atom.isFLOAT(atom) ) type = FLOAT;
    else if ( this->atom.isNIL(atom) ) type = NIL;
    else if ( this->atom.isT(atom) ) type = T;
    else if ( this->atom.isQUOTE(atom) ) type = QUOTE;
    else type = SYMBOL;
}

void Function::checkTokenToStore(string &token, int &line, int &column, char &sep) {
    /* 在 seperater 使用前，檢查 Token 是否蘊含先前讀入的資料
        若有，則將該資料存入 AST，無論如何，皆會將 Token 改為使其分割的 Seperator */
    if ( !token.empty() ) {
        try {
            storeTokenInAST(token, line, column);
        }
        catch ( const std::runtime_error &msg ) {
            throw msg;
        }
        if ( line == 0 ) line = 1;  // 若儲存完即輸出，若該行讀入還有資料，則須設定行數為 1
        if ( column == 0 ) column = 1;  // 若儲存完即輸出，若該行讀入還有資料，則須設定列數為 1
    }
    token = sep;

}

Atom::Atom() {
    // cout << "Atom created" << endl;  // debug
}

Atom::~Atom() {
    // cout << "Atom destroyed" << endl;    // debug
}

bool Atom::isINT(string &atom) {
    /*檢查Atom是否為整數，並依其正負更改其顯示*/
    int len = atom.size();
    bool interger = false;
    string num;

    for ( int i = 0; i < len; ++i ) {
        // 數字開頭可以有正負號
        if ( i == 0 && (atom[i] == '+' || atom[i] == '-') ) {
            if ( atom[i] == '-' ) num += '-';
            continue;
        }
        else if ( !isdigit(atom[i]) ) return false; // 若含非數字字元，則不為整數
        else {
            num += atom[i];
            interger = true;
        }
    }

    if ( !interger ) return false;  // 若不含數字，則不為整數
    atom = num;
    return true;
}

bool Atom::isDOT(string &atom) {
    /*檢查Atom是否為點*/
    if ( atom.size() == 1 && atom[0] == '.' ) return true;  // 若Atom僅含點一個字元，則為點
    return false;
}

bool Atom::isSTRING(string &atom) {
    /*檢查Atom是否為字串，並依照其函數更新字串內容*/
    if ( atom[0] != '\"' ) return false;
    string rebuild = "";
    bool backslash = false;

    for ( auto &c : atom ) {
        if ( c == '\\' && !backslash ) backslash = true; // 遇到反斜線
        else {
            if ( backslash ) {
                // 若前一位為反斜線，則將其形成的函數進行轉換
                switch (c)
                {
                case 'n':
                    rebuild += '\n';
                    break;
                case '\"':
                    rebuild += '\"';
                    break;
                case 't':
                    rebuild += '\t';
                    break;
                case '\\':
                    rebuild += '\\';
                    break;
                default:
                    rebuild += '\\';
                    rebuild += c;
                    break;
                }
                backslash = false;
            }
            else rebuild += c;
        }
    }
    atom = rebuild;
    return true;
}

bool Atom::isFLOAT(string &atom) {
    /*檢查Atom是否為浮點數，並取浮點數後三位*/
    int len = atom.size(), decimal = 0;
    string num;
    bool dot = false, interger = false, round = false;

    for ( int i = 0; i < len; ++i ) {
        // 數字開頭可以有正負號
        if ( i == 0 && (atom[i] == '+' || atom[i] == '-') ) {
            if ( atom[i] == '-' ) num += '-';
            continue;
        }
        else if ( !isdigit(atom[i]) ) {
            if ( atom[i] == '.' && !dot ) {
                dot = true;
                if ( !interger ) num += '0'; // 若並未輸入整數位，則補0
                num += '.';
            }
            else return false; // 若含非數字或點字元，則不為浮點數
        }
        else {
            if ( decimal >= 3 ) {
                // 小數點後最多3位
                if ( decimal == 3 && atom[i] >= '5' ) round = true; // 四捨五入
                ++decimal;
                continue;
            } 
            num += atom[i];
            interger = true;
            if ( dot ) ++decimal; // 計算小數位數
        }
    }

    if ( !interger ) return false; // 若不含數字，則不為浮點數

    while ( decimal < 3 ) {
        // 若小數位數不足3位，則補0
        num += '0';
        ++decimal;
    }
    if ( round ) num[num.size() - 1] += 1; // 四捨五入
    atom = num;
    return true;
}

bool Atom::isNIL(string &atom) {
    /*檢查Atom是否為空，並修改其內容*/
    if ( atom == "nil" || atom == "#f" ) {
        atom = "nil";
        return true;
    }
    return false;
}

bool Atom::isT(string &atom) {
    /*檢查Atom是否為T，並修改其內容*/
    if ( atom == "t" || atom == "#t" ) {
        atom = "#t";
        return true;
    }
    return false;
}

bool Atom::isQUOTE(string &atom) {
    /*檢查Atom是否為引號，並修改其內容*/
    if ( atom == "'" ) {
        atom = "quote";
        return true;
    }
    return false;
}

Separator::Separator(Function &function) : function(function) {
    // cout << "Separtor created" << endl;  // debug
}

Separator::~Separator() {
    // cout << "Separtor destroyed" << endl;    // debug
}

void Separator::leftParen(string &token, int &line, int &column) {
    /* 當遇見新的左括弧，即產生新的Exprssion */

    try {
        function.newExpression(token, line, column);
        token.clear();
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
}

void Separator::rightParen(string &token, int &nil, int &left_paren, int &line, int &column) {
    /*  遇見右括弧，可能為
        1: 結束該S-expression -> 若剛好使用完左括弧，及輸出結果，反之則回到外層 S-expression
        2: nil
    */
    if ( function.getDot() || left_paren < 0 ) {
        // 若左括弧不存在或前一個資料為 DOT ，則該右括弧為錯誤
        string error = "ERROR (unexpected token) : atom or '(' expected when token at Line " + to_string(line) + " Column " + to_string(column) + " is >>" + token + "<<\n";
        throw std::runtime_error(error);
    }
    else if ( !nil ) {
        // 當括弧內沒有資料時，則該位置為nil
        function.storeTokenInAST(token, line, column);
        ++nil;  // 外層括弧並不為空
    }
    
    // 若該 S-expression 結束，回到外層 S-expression
    function.backExpression();
    
    if ( !left_paren ) {
        // 整筆 S-expression 結束
        if ( function.checkExit() ) throw "Exit"; // 若為exit，則結束
        function.optimizeAST(); //debug
        //function.printAST();
        line = 0, column = 0; // 重新另一個S-expression
    }
    token.clear();

}

void Separator::singleQuote(string &token, int &line, int &column) {
    /*  遇到單引號，即內部為另一筆 S-expression 
        與左括弧相似，需建造樹的新母節點 */

    try {
        function.quoteExpression(token, line, column);
        function.storeTokenInAST(token, line, column);
        token.clear();
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
}

void Separator::doubleQuote(string &token, int &line, int &column, bool &str, bool &backslash, char &sep) {
    /*  遇到雙引號，將字串存入AST
        若遇到反斜線則將雙引號公用消除，反之則該字串結束 */
    try {
        if ( !str ) function.checkTokenToStore(token, line, column, sep);   // 若為雙引號的開頭，則將前面的 token 存入 AST

        if ( str && !backslash ) {
            // 若遇到反斜線則將雙引號公用消除，反之則該字串結束
            token += sep;
            function.storeTokenInAST(token, line, column);
            token.clear();
            str = false;
        }
        else {
            if ( token.size() != 1 ) token += sep; // 避免雙引號重複添加
            str = true;
            backslash = false;
        }
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }

}

void Function::newExpression(string &token, int &line, int &column) {
    /* 遇見新的左括弧，即產生新的Exprssion
        左括弧永遠是在每個 S-expression 的根節點 */
    string error_str;
    if ( error.unexpectedTokenOfRightParen(cur, token, line, column, error_str) ) {
        // 檢查是否有缺少右括弧的錯誤
        throw std::runtime_error(error_str); 
    } 

    if ( dot ) {
        // dot後面將接上另一個具有括弧的S-Expression，則該 S-expression 與前一個 S-expression 屬於相同的層級
        cur->value = "DOT_LEFT_PAREN";
        ast.push(cur); 
        dot = false;
        return;
    }

    if ( cur->type == BEGIN ) {
        // 該 S-expression 的第一個位置，即 AST 的根節點
        cur->type = LEFT_PAREN;
        cur->value = token;
        cur->right = new ASTNode();
        cur->right->type = TEMP;
        ast.push(cur->right);
        return;
    }
    // 外層有 S-expression，則在其內部建立新的 S-expression
    cur->left = new ASTNode();
    cur->right = new ASTNode();
    cur->right->type = TEMP; // 該位置為對外層 S-expression 的下一個 AST 寫入節點
    if ( cur->type != LEFT_PAREN ) cur->type = LINK; // debug
    ast.push(cur->right); // 留存前一個S-expression目前的結尾節點
    
    cur = cur->left;   // 新的S-expression起點
    cur->type = LEFT_PAREN;
    cur->value = token;

}

void Function::backExpression() {
    /*  跳出外圈S-expression的位置，因該內部S-expression以結束
        須回到上層AST結尾節點，需跳出至外層不為 QUOTE S-expression 或沒有更外層 */
    do {
        if ( cur->type == TEMP || cur->type == "QUOTE_TEMP" ) cur->type = END;  // 為該 S-expression 結尾
        if ( cur->type == "QUOTE_TEMP" ) --quote; // 跳出外層為 QUOTE 的 S-expression 
        cur = ast.top();
        ast.pop();
    } while ( !ast.empty() && ast.top()->type == "QUOTE_TEMP" );
}

void Function::quoteExpression(string &token, int &line, int &column) {
    /*  遇到單引號，即內部為另一筆 S-expression 
        與左括弧相似，需建造樹的新母節點 */
    string error_str;
    if ( error.unexpectedTokenOfRightParen(cur, token, line, column, error_str) ) {
        // 檢查是否有缺少右括弧的錯誤
        throw std::runtime_error(error_str); 
    }

    ++quote;    // 計算 QUOTE 的層數
    if ( cur->type == BEGIN ) {
         // 該 S-expression 的第一個位置，即 AST 的根節點
        cur->type = LEFT_PAREN;
        cur->value = "(";
        cur->right = new ASTNode();
        cur->right->type = "QUOTE_TEMP";
        ast.push(cur->right);
        return;
    }
    // 外層有 S-expression，則在其內部建立新的 S-expression
    cur->left = new ASTNode();
    cur->right = new ASTNode();
    cur->right->type = "QUOTE_TEMP"; // 該位置為對外層 S-expression 的下一個 AST 寫入節點
    ast.push(cur->right); // 留存外層 S-expression 目前的結尾節點
    // 新的 S-expression 起點
    cur = cur->left;   
    cur->type = LEFT_PAREN;
    cur->value = "(";

}

bool Function::printAST() {
    /* 印出AST */
    if ( root->type == BEGIN ) return false;    // 若無資料，則不需印出
    while ( !ast.empty() ) backExpression();

    stack<pair<ASTNode*, string>> st;
    st.push({root, ""});
    int left_paren = 0, temp = 0;
    bool beforeIsParen = false;

    while ( !st.empty() ) {
        ASTNode *cur = st.top().first;
        string direction = st.top().second; // 紀錄該節點為左節點或右節點
        st.pop();
        if ( cur->type != LINK && cur->type != TEMP && cur->type != "QUOTE_TEMP" && cur->value != "DOT_LEFT_PAREN" ) {
            // 除了連結節點外，其他節點皆需印出
            if ( cur->type == END ) --left_paren; // 結束一個S-expression，則縮排
            if ( !beforeIsParen ) {
                if ( direction == "right" && cur->type != END && cur->value != "(" ) {
                    // 若該節點為右節點，且內容不為左括弧以及結束節點，則印出點
                    for ( int i = 0; i < left_paren; ++i ) cout << "  ";
                    cout << "." << endl;
                }
                for ( int i = 0; i < left_paren; ++i ) cout << "  ";
            }
            else beforeIsParen = false;

            if ( cur->value == "(" ) {
                ++left_paren;
                cout << cur->value << " ";
                beforeIsParen = true; // 做為左括弧的下一筆資料，不需換行。因此標記
            }
            else if ( cur->type == END ) cout << ")" << endl;
            else cout << cur->value << endl;
        }
        // 遍歷子樹
        if ( cur->right ) st.push({cur->right, "right"});
        if ( cur->left ) st.push({cur->left, "left"});
    }
    cout << "\n> ";
    newAST();
    return true;
}

void Function::optimizeAST() {
    // 整理AST
    cout << "optimizeAST" << endl;
    leafToRoot(root, 0);
    bool dot = false;
    printAST();
}

void Function::leafToRoot(ASTNode *root, int level) {

    // 將AST由葉節點往根節點整理
    cout << root->value << " " << root->type << " " << level << endl;
    st.push({root, level});
    if ( root->left ) leafToRoot(root->left, level + 1);
    if ( root->right ) leafToRoot(root->right, level + 1);
    
}

Error::Error() {
    // cout << "Error created" << endl;  // debug
}

Error::~Error() {
    // cout << "Error destroyed" << endl;    // debug
}

bool Error::unexpectedTokenOfRightParen(ASTNode *cur, string &token, int &line, int &column, string &error) {
    /* 檢查是否為缺少右括弧的情況 */
    if ( cur->type != BEGIN && cur->type != LEFT_PAREN && cur->type != TEMP && cur->type != "QUOTE_TEMP" ) {
        // 走到有存取資料或連接位置，即該 S-expression 須結束，故缺少右括弧
        // 右子樹結束，於同一層卻又遇到左括弧，即為錯誤
        error = "ERROR (unexpected token) : ')' expected when token at Line " + to_string(line) + " Column " + to_string(column) + " is >>" + token + "<<\n";
        return true;
    } 
    return false;

}