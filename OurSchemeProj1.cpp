#include <iostream>
#include <queue>
#include <stack>
#include <unordered_set>
#include <unordered_map>

using namespace std;

#define LEFT_PAREN "LEFT_PAREN"
#define INT "INT"
#define STRING "STRING"
#define BOOL "BOOL"
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

unordered_set<string> symbol_set{"cons", "quote", "list", "define", "car", "cdr", "eqv?", "equal?", "if", "cond", "begin", "clean-environment", "exit"}, 
                    primitive_predicate{"atom?", "pair?", "list?", "null?", "integer?", "real?", "number?", "string?", "boolean?", "symbol?"},
                    basic_arithmetic{"+" , "-" , "*" , "/", "not", "and", "or", ">", ">=", "<", "<=", "=", "string-append", "string>?", "string<?", "string=?"};

struct ASTNode {
    string type, value;
    ASTNode *left;
    ASTNode *right;
};

unordered_map<string, ASTNode*> user_map{};

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
        bool unexpectedTokenOfRightParen(ASTNode *cur, string &token, int &line, int &column, string &error, bool isSeperator);
        // bool unexpectedTokenOfAtomOrLeftParen(string &token, int &line, int &column);
        // bool noClosingQuote(int &line, int &column);
        // bool endOfFile();
        void checkSymbol(ASTNode *cur, bool non_function);
        
};

class Symbol {
    public:
        Symbol();
        ~Symbol();
        bool &getDefine() { return define; };
        void symbolCheck(ASTNode *root, ASTNode *parent, int deep);
        int countFunctionArg(ASTNode *root);
        bool isCONS(ASTNode *root, ASTNode *parent);
        bool isQUOTE(ASTNode *root, ASTNode *parent);
        bool isList(ASTNode *root, ASTNode *parent);
        bool isUserDefine(ASTNode *root, ASTNode *parent, int deep);
        void userDefined(ASTNode *root);
        void copyTree(ASTNode *root, ASTNode *defined_tree);
        bool isCAR(ASTNode *root, ASTNode *parent);
        bool isCDR(ASTNode *root, ASTNode *parent);
        bool checkPrimitivePredicate(ASTNode *root, ASTNode *parent);
        bool isAtom(ASTNode *root);
        bool isPair(ASTNode *root);
        bool isList(ASTNode *root);
        bool isNull(ASTNode *root);
        bool isInteger(ASTNode *root);
        bool isRealOrNumber(ASTNode *root);
        bool isString(ASTNode *root);
        bool isBoolean(ASTNode *root);
        bool isSymbol(ASTNode *root);
        bool isArithmetic(ASTNode *root, ASTNode *parent);
        bool isEqual(ASTNode *root, ASTNode *parent);
        bool isIf(ASTNode *root, ASTNode *parent);
        bool isCond(ASTNode *root, ASTNode *parent);
        bool isBegin(ASTNode *root, ASTNode *parent);
        bool isCleanEnvironment(ASTNode *root, ASTNode *parent, int deep);
        bool isExit(ASTNode *root, ASTNode *parent);
    private:
        Atom atom;
        Error error;
        bool define;
        void calculate(ASTNode *root, string &operand, string &result, bool &first);
        bool campare(ASTNode *first_element, ASTNode *second_element);
        void conditional(ASTNode **root, string &operand);
        void procedure(ASTNode *root, ASTNode *parent);
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
        void commonStore(string &token, string &type);
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
        Symbol symbol;
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
    string num;   // 測資題號
    getline(cin, num);
    Function function;
    Reader reader(function);
    reader.read();
    return 0;
}

Reader::Reader(Function &function) : function(function), separtor(function) {
    //cout << "Reader created" << endl;
}

Reader::~Reader() {
    //cout << "Reader destroyed" << endl;
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
            cout << msg;
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

    //cout << "You entered: " << expr << " " << endl;    // debug
    try {
        saperateExprToToken(expr, line, nil);  // 將輸入的S-expression分割成Token
    }
    catch ( const std::runtime_error &msg ) {
        cout << msg.what();
        cout << "\n> ";
        // 因錯誤而結束該S-expression，重新讀取下一行時需全部清空
        line = 0;
        left_paren = 0;
        function.newAST();
        return true;
    }
    catch ( const char *msg ) {
        if ( msg == "Exit" ) return false;

        // 因錯誤而結束該S-expression，重新讀取下一行時需全部清空
        line = 0;
        left_paren = 0;
        function.newAST();
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
            // 遇到引號，將 quote 存入 AST
            try {
                function.checkTokenToStore(token, line, column, c);
                separtor.singleQuote(token, line, column);
            }
            catch ( const std::runtime_error &msg ) {
                throw msg;
            }
        }
        else if ( !str && c == ' ' ) {
            // 遇到空白，將 token 存入 AST
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

Function::Function() {
    // cout << "Function created" << endl;  // debug
    atom = Atom();
    error = Error();
    symbol = Symbol();
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

void Function::storeTokenInAST(string &token, int &line, int &column) {
    /*  將 Token 存入 AST
        並更新目前節點位置 */
    string type;
    checkTypeOfAtom(token, type);   // 確認 Token 型態並更新其內容
    //cout << "token: " << token << " " << type << " " << column << endl; // debug
    //cout << "cur: " << cur->value << " " << cur->type << endl; // debug

    string error_str;
    if ( error.unexpectedTokenOfRightParen(cur, token, line, column, error_str, false) ) throw std::runtime_error(error_str);   // 若前方並未存在 S-expression，則拋出錯誤
    if ( type == DOT ) {
        // 若該 Token 為 DOT ，則下一個 Token 為另一個 S-expression
        if ( cur->type == BEGIN || cur->type == LEFT_PAREN || dot || ast.top()->type == "QUOTE_TEMP" ) {
            // 前方並未存在 S-expression
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
            commonStore(token, type);
        }
        return;
    }

    // 將 1 筆 Token 存入新節點
    if ( cur->type == BEGIN ) {
        // 若該 S-expression 為單一 Atom
        cur->value = token;
        cur->type = type;
        cur->left = NULL;
        cur->right = NULL;
        try {
            optimizeAST(); // 指令建構完成，則輸出 AST
        }
        catch ( const std::runtime_error &msg ) {
            throw msg;
        }
        line = 0, column = 0;
        return;
    }
    else if ( dot ) {
        // 若前一個 Token 為 DOT ，則將該 Token 存入右節點
        cur->value = token;
        cur->type = type;
        cur->left = NULL;
        cur->right = new ASTNode();
        cur->right->type = END; // 預設該位置為AST結尾節點
        dot = false;
        return;
    }
    else commonStore(token, type);    // 將新節點存入 AST ，並更新目前節點位置

    if ( type != QUOTE && quote > 0 && !ast.empty() && ast.top()->type == "QUOTE_TEMP" ) {
        // 因 QUOTE 建立的 S-expression 已滿足，則跳出至外層 S-expression
        cur->type = END;
        backExpression();
        if ( ast.empty() ) {
            // 若無外層 S-expression ，表示指令建構完成，則輸出 AST
            optimizeAST();
            line = 0, column = 0;
        }
    }
}

void Function::checkTypeOfAtom(string &atom, string &type) {
    /* 檢查Token型態，並將設定其型態 */
    if ( this->atom.isINT(atom) ) type = INT;
    else if ( this->atom.isSTRING(atom) ) type = STRING;
    else if ( this->atom.isDOT(atom) ) type = DOT;
    else if ( this->atom.isFLOAT(atom) ) type = FLOAT;
    else if ( this->atom.isNIL(atom) ) type = NIL;
    else if ( this->atom.isT(atom) ) type = BOOL;
    else if ( this->atom.isQUOTE(atom) ) type = QUOTE;
    else type = SYMBOL;
}

void Function::commonStore(string &token, string &type) {
    /* 將Token存入AST */
    if ( cur->type == TEMP || cur->type == "QUOTE_TEMP" ) 
        cur->type = LINK;   // 將該節點設置為連結節點

    ASTNode *new_node = new ASTNode();
    new_node->value = token;
    new_node->type = type;
    // 為葉節點
    new_node->left = NULL;
    new_node->right = NULL;

    // 將新節點存入 AST ，並更新目前節點位置
    cur->left = new_node;
    cur->right = new ASTNode();
    cur = cur->right;

    // 預設該位置為下個連結節點
    cur->type = TEMP;
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

void Function::newExpression(string &token, int &line, int &column) {
    /*  遇見新的左括弧，即產生新的Exprssion
        左括弧永遠是在每個 S-expression 的根節點 */
    string error_str;
    if ( error.unexpectedTokenOfRightParen(cur, token, line, column, error_str, true) ) {
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
        cur->value = "(";
        cur->right = new ASTNode();
        cur->right->type = (token == "(" ? TEMP : "QUOTE_TEMP");
        ast.push(cur->right);
        return;
    }
    // 外層有 S-expression，則在其內部建立新的 S-expression
    cur->left = new ASTNode();
    cur->right = new ASTNode();
    cur->right->type = (token == "(" ? TEMP : "QUOTE_TEMP"); // 該位置為對外層 S-expression 的下一個 AST 寫入節點
    if ( cur->type != LEFT_PAREN ) cur->type = LINK; // debug
    ast.push(cur->right); // 留存前一個S-expression目前的結尾節點
    
    cur = cur->left;   // 新的S-expression起點
    cur->type = LEFT_PAREN;
    cur->value = "(";

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
    newExpression(token, line, column);
    ++quote;    // 計算 QUOTE 的層數
}

bool Function::printAST() {
    /* 印出AST */
    if ( root->type == BEGIN ) return false;    // 若無資料，則不需印出
    else if ( symbol.getDefine() ) {
        // 若有新的定義，則不須印出
        symbol.getDefine() = false;
        cout << "\n> ";
        newAST();
        return true;
    }
    
    while ( !ast.empty() ) backExpression();

    stack<pair<ASTNode*, string>> st;
    st.push({root, ""});
    int left_paren = 0, temp = 0;
    bool beforeIsParen = false;

    while ( !st.empty() ) {
        ASTNode *cur = st.top().first;
        string direction = st.top().second; // 紀錄該節點為左節點或右節點
        st.pop();
        // 遍歷子樹
        if ( cur->right ) st.push({cur->right, "right"});
        if ( cur->left ) st.push({cur->left, "left"});

        if ( (cur->type == END || cur->value == "nil") && direction == "right") --left_paren; // 結束一個S-expression，則縮排
        if ( !beforeIsParen && (direction != "right" || (!cur->left && !cur->right) ) ) {
            if ( direction == "right" && cur->type != END && cur->value != "nil" && !cur->left && !cur->right ) {
                // 若該節點為右節點，且內容不為左括弧以及結束節點，則印出點
                for ( int i = 0; i < left_paren; ++i ) cout << "  ";
                cout << "." << endl;
            }
            
            for ( int i = 0; i < left_paren; ++i ) cout << "  ";
        }
        else beforeIsParen = false;

        if ( direction != "right" && cur->left && cur->right ) {
            ++left_paren;
            cout << "( ";
            beforeIsParen = true; // 做為左括弧的下一筆資料，不需換行。因此標記
        }
        else if ( (cur->type == END || cur->value == "nil") && direction != "left" ) {
            // 若該節點為右節點且為 END 或 nil，則印出右括弧
            if ( direction != "right" ) cout << "nil" << endl;
            else cout << ")" << endl;
        }
        else if ( !cur->left && !cur->right ) {
            cout << cur->value << endl;
            if ( direction == "right" ) {
                --left_paren;
                for ( int i = 0; i < left_paren; ++i ) cout << "  ";
                cout << ")" << endl; // 若該節點為右節點結尾，則印出右括弧
            }    
        }
    }
    cout << "\n> ";
    newAST();
    return true;
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
        try {
            function.optimizeAST(); // 整理AST
        }
        catch ( const std::runtime_error &msg ) {
            throw msg;
        }
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

void Function::optimizeAST() {
    // 整理AST
    //cout << "optimizeAST" << endl;
    int defined_count = user_map.size();
    leafToRoot(root, 0);
    stack<pair<ASTNode*, ASTNode*>> st;
    st.push({root, NULL});

    if ( root->type == SYMBOL ) {
        // 若根節點為 SYMBOL ，則檢查是否存在
        try {
            symbol.symbolCheck(root, NULL, 0); // 為單一 ATOM 時，直接輸出其 SYMBOL 的 value
        }
        catch ( const std::runtime_error &msg ) {
            cout << msg.what();
            cout << "\n> ";
            // 因錯誤而結束該S-expression，重新讀取下一筆 S-expression
            newAST();
            return;
        }
        catch ( const char *msg ) {
            throw msg;
        }
        catch ( ASTNode *temp ) {
            root = temp;
            printAST();
            throw "ERROR of non-function || ERROR (non-list)";
        
        }
        leafToRoot(root, 0);
        printAST(); 
        return;
    }


    while ( !st.empty() ) {
        ASTNode *cur = st.top().first;
        ASTNode *parent = st.top().second;
        st.pop();
        if ( parent == NULL || parent->right == cur ) {
            while ( cur->left != NULL ) {
                st.push({cur->left, cur});
                cur = cur->left;
            }
        }
        else {
            try {
                symbol.symbolCheck(cur, parent, 0);
            }
            catch ( const std::runtime_error &msg ) {
                cout << msg.what();
                cout << "\n> ";
                // 因錯誤而結束該S-expression，重新讀取下一筆 S-expression
                newAST();
                return;
            }
            catch ( const char *msg ) {
                throw msg;
            }
            catch ( ASTNode *temp ) {
                root = temp;
                printAST();
                throw "ERROR of non-function || ERROR (non-list)";
            }
        }

    }
    leafToRoot(root, 0);
    printAST(); 
}

void Function::leafToRoot(ASTNode *root, int level) {

    // 將AST由葉節點往根節點整理
    //cout << root->value << " " << root->type << " " << level << endl;
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

bool Error::unexpectedTokenOfRightParen(ASTNode *cur, string &token, int &line, int &column, string &error, bool isSeperator) {
    /* 檢查是否為缺少右括弧的情況 */
    if ( cur->type != BEGIN && cur->type != LEFT_PAREN && cur->type != TEMP && cur->type != "QUOTE_TEMP" ) {
        // 走到有存取資料或連接位置，即該 S-expression 須結束，故缺少右括弧
        // 右子樹結束，於同一層卻又遇到左括弧，即為錯誤
        if ( isSeperator )
            error = "ERROR (unexpected token) : ')' expected when token at Line " + to_string(line) + " Column " + to_string(column) + " is >>" + token + "<<\n";
        else 
            error = "ERROR (unexpected token) : ')' expected when token at Line " + to_string(line) + " Column " + to_string(column - token.size()) + " is >>" + token + "<<\n";
        return true;
    } 
    return false;

}

void Error::checkSymbol(ASTNode *cur, bool non_function) {
    /* 檢查 Symbol 是否為 CONS, QUOTE, LIST */

    if ( symbol_set.find(cur->value) == symbol_set.end() 
            && primitive_predicate.find(cur->value) == primitive_predicate.end() 
            && basic_arithmetic.find(cur->value) == basic_arithmetic.end() 
            && user_map.find(cur->value) == user_map.end()
            && cur->value.find("<procedure " ) == std::string::npos ) {
        // 若該 SYMBOL 未定義，則拋出錯誤
        string err_str;
        if ( cur->type == SYMBOL ) err_str = "ERROR (unbound symbol) : " + cur->value + "\n";
        else err_str = "ERROR (attempt to apply non-function) : " + cur->value + "\n";
        throw std::runtime_error(err_str);
    }
    if ( cur->type != SYMBOL && cur->type != QUOTE && cur->type != "QUOTE_DATA" ) 
        // 若在函數位置卻不是 SYMBOL ，則拋出錯誤
        throw std::runtime_error("ERROR (attempt to apply non-function) : " + cur->value + "\n");  
    else if ( non_function && user_map.find(cur->value) != user_map.end() ) {
        // 若在函數位置卻為出現並未系統定義的 SYMBOL ，則拋出錯誤
        if ( user_map[cur->value]->value.find("<procedure " ) != std::string::npos ) {
            // cur->value = user_map[cur->value]->value.substr(12, user_map[cur->value]->value.size() - 13); // 去除 <procedure 和 >
            return;
        }
        cout << "ERROR (attempt to apply non-function) : ";
        throw user_map[cur->value];
    }
    
}

Symbol::Symbol() {
    // cout << "Symbol created" << endl;    // debug
    atom = Atom();
    error = Error();
    define = false;
}

Symbol::~Symbol() {
    // cout << "Symbol destroyed" << endl;  // debug
}

int Symbol::countFunctionArg(ASTNode *root) {
    /* 計算函數的參數個數 */
    int count = 0;
    ASTNode *temp = root->right;
    while ( temp->type != END ) {
        ++count;
        temp = temp->right;
    }
    return count;

}

void Symbol::symbolCheck(ASTNode *root, ASTNode *parent, int deep) {
    /* 檢查 Symbol 是否存在 */
    
    try {
        error.checkSymbol(root, parent ? true : false);
        procedure(root, parent); // 檢查是否為系統定義的函數
        if ( isCONS(root, parent) ) return; 
        else if ( isQUOTE(root, parent) ) return; 
        else if ( isList(root, parent) ) return; 
        else if ( isUserDefine(root, parent, deep) ) return; 
        else if ( isCAR(root, parent) ) return; 
        else if ( isCDR(root, parent) ) return; 
        else if ( checkPrimitivePredicate(root, parent) ) return; 
        else if ( isArithmetic(root, parent) ) return; 
        else if ( isEqual(root, parent) ) return; 
        else if ( isIf(root, parent) ) return; 
        else if ( isCond(root, parent) ) return; 
        else if ( isBegin(root, parent) ) return; 
        else if ( isCleanEnvironment(root, parent, deep) ) return;
        else if ( isExit(root, parent) ) return; 
        else userDefined(root);
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( const char *msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        if ( root != temp ) {
            copyTree(root, temp);
            throw root;
        }
        throw parent;
    }
    
}

bool Symbol::isCONS(ASTNode *root, ASTNode *parent) {
    /* 檢查 Symbol 是否為 CONS 並更新 AST */
    if ( root->value != "cons" ) return false;
    else if ( !parent || parent->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 CONS function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        return true;
    }

    int arg = countFunctionArg(parent);   // 計算參數個數
    // cout << "arg: " << arg << endl;  // debug
    if ( arg != 2 ) 
        // 若參數個數不為2，則拋出錯誤
        throw std::runtime_error("ERROR (incorrect number of arguments) : cons\n");

    ASTNode *temp = parent->right; 
    try {
        bool checkList = isList(temp);
        if ( !checkList ) {
            // 若該 S-expression 內的資料不為 List ，則拋出錯誤
            cout << "ERROR (non-list) : ";
            throw root;
        }
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }

    if ( arg == 2 ) {
        ASTNode *fir_node = parent->right->left;
        if ( fir_node->type == LEFT_PAREN || fir_node->type == SYMBOL ) {
            // 若第一元素為 S-expression ，則需檢查其內容
            try {
                if ( fir_node->type == LEFT_PAREN ) symbolCheck(fir_node->left, fir_node, 1);
                else symbolCheck(fir_node, NULL, 1);
            }
            catch ( const std::runtime_error &msg ) {
                throw msg;
            }

        }
        ASTNode *sec_node = parent->right->right->left;
        if ( sec_node->type == LEFT_PAREN || sec_node->type == SYMBOL ) {
            // 若第二元素為 S-expression ，則需檢查其內容
            try {
                if ( sec_node->type == LEFT_PAREN ) symbolCheck(sec_node->left, sec_node, 1);
                else symbolCheck(sec_node, NULL, 1);
            }
            catch ( const std::runtime_error &msg ) {
                throw msg;
            }
            // 移除其左括弧並將其提高一個階層
            if ( sec_node->type == LEFT_PAREN ) {
                parent->right->right->left = sec_node->left;
                parent->right->right->right = sec_node->right;
            }
            else parent->right->right = sec_node;   // 第二元素將與第一元素同階層
        }
        else parent->right->right = sec_node;   // 第二元素將與第一元素同階層

        // 將 dotted pair 提高一個階層放置
        parent->type = "QUOTE_TEMP";
        parent->left = parent->right->left;
        parent->right = parent->right->right; 

        if ( parent->right->right == NULL ) {
            // 若右子樹為 ATOM ，則需額外添加結束節點
            if ( parent->right->type == NIL ) {
                // 若右子樹為 nil ，則需將其轉為 END
                parent->right->type = END;
                parent->right->value = "";
            }
            else {
                parent->right->right = new ASTNode();
                parent->right->right->type = END;
            }
        }
        else if ( parent->right->type == "QUOTE_TEMP" ) {
            // 若右子樹為 QUOTE ，則需將其降階
            parent->right->type = LINK;
            parent->right->value = ""; 
        }

        try {
            // 檢查 CONS 內的 SYMBOL 是否為定義過的
            if ( parent->left->type == SYMBOL ) error.checkSymbol(parent->left, false);
            if ( parent->right->type == SYMBOL ) error.checkSymbol(parent->right, false);
        }
        catch ( const std::runtime_error &msg ) {
            throw msg;
        }
        catch ( ASTNode *temp ) {
            throw temp;
        }
    }
    return true;
    
}

bool Symbol::isQUOTE(ASTNode *root, ASTNode *parent) {
    /* 檢查 Symbol 是否為 QUOTE 並更新 AST */
    if ( root->value != "quote" ) return false;
    else if ( !parent || parent->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 QUOTE function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        return true;
    }

    int arg = countFunctionArg(parent);   // 計算參數個數
    //cout << "arg: " << arg << endl;  // debug
    if ( arg != 1 ) 
        // 若參數個數不為1，則拋出錯誤
        throw std::runtime_error("ERROR (incorrect number of arguments) : quote\n");
    
    ASTNode *new_root = parent->right->left;    // 將 QUOTE 後的 S-expression 提高一個階層
    if ( new_root->type == NIL ) {
        parent->type = NIL;
        parent->value = "nil";
        parent->left = NULL;
        parent->right = NULL;
        return true;
    }
    else if ( new_root->type != LEFT_PAREN ) {
        // 該 S-expression 內的資料為 ATOM ，則需額外定義
        parent->type = "QUOTE_DATA";
        parent->value = new_root->value;
        parent->left = NULL;
        parent->right = NULL;
        return true;
    }
    parent->type = "QUOTE_TEMP";
    parent->right = new_root->right;
    parent->left = new_root->left;
    return true;
    
    
}

bool Symbol::isList(ASTNode *root, ASTNode *parent) {
    /* 檢查 Symbol 是否為 List 並更新 AST */
    if ( root->value != "list" ) return false;
    else if ( !parent || parent->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 LIST function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        return true;
    }
    
    int arg = countFunctionArg(parent);   // 計算參數個數
    // cout << "arg: " << arg << endl;  // debug

    ASTNode *temp = parent->right;
    try {
        ASTNode *new_root = new ASTNode(); // 新建一個 AST 以供檢查
        copyTree(new_root, temp); // 複製一份 AST 以供檢查
        bool checkList = isList(new_root);
        if ( !checkList ) {
            // 若該 S-expression 內的資料不為 List ，則拋出錯誤
            cout << "ERROR (non-list) : ";
            throw root;
        }
        isList(temp);
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }

    while ( temp->type != END ) {
        // 檢查 List 內的每個左子樹
        if ( temp->left->type == SYMBOL ) {
            // 若左子樹為 S-expression ，則需檢查其內容
            try {
                symbolCheck(temp->left, NULL, 1); // 若為 ATOM 且為 SYMBOL ，則檢查 SYMBOL 是否為定義過的
            }
            catch ( const std::runtime_error &msg ) {
                throw msg;
            }
        }
        temp = temp->right;
    }

    // 將 List 內容提高一個階層
    parent->type = "QUOTE_TEMP";
    parent->left = parent->right->left;   
    parent->right = parent->right->right;
    if ( !parent->left && !parent->right ) {
        parent->type = BOOL;
        parent->value = "nil";
    }
    return true;
    
}

bool Symbol::isUserDefine(ASTNode *root, ASTNode *parent, int deep) {
    /* 檢查 Symbol 是否為 Define 並更新 AST */
    if ( root->value != "define" ) return false;
    else if ( !parent || parent->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 DEFINE function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        return true;
    }

    if ( deep != 0 )
        throw std::runtime_error("ERROR (level of DEFINE)\n");

    int arg = countFunctionArg(parent);   // 計算參數個數
    // cout << "arg: " << arg << endl;  // debug
    if ( arg != 2 || parent->right->left->type != SYMBOL ) {
        // 若參數個數不為2，則拋出錯誤
        cout << "ERROR (DEFINE format) : ";
        throw root;
    }

    string key;
    try {
        bool checkList = isList(parent->right);
        if ( !checkList ) {
            // 若該 S-expression 內的資料不為 List ，則拋出錯誤
            cout << "ERROR (non-list) : ";
            throw root;
        }

        if ( parent->right->left->type == SYMBOL ) key = parent->right->left->value;
        if ( symbol_set.find(key) != symbol_set.end() ) {
            // 若該 SYMBOL 為系統定義，則拋出錯誤
            cout << "ERROR (DEFINE format) : ";
            throw root;
        }

        /*
        ASTNode *temp = parent->right->right;
         while ( temp->type != END ) {
            // 檢查 List 內的每個左子樹
            if ( temp->left->type == LEFT_PAREN ) {
                // 若左子樹為 S-expression ，則需檢查其內容
                ASTNode *symbol_node = temp->left->left;
                if ( symbol_node->value.find("<procedure ") == std::string::npos ) 
                    symbolCheck(symbol_node, temp->left);
            }
            else if ( temp->left->type == SYMBOL ) {
                // 若為 ATOM 且為 SYMBOL ，則檢查 SYMBOL 是否為定義過的
                error.checkSymbol(temp->left, false);
                if ( user_map.find(temp->left->value) != user_map.end() ) temp->left = user_map[temp->left->value]; // 將其內容複寫一份進當前 AST
                else if ( temp->left->value.find("<procedure ") == std::string::npos ) symbolCheck(temp->left, NULL);
            }
            temp = temp->right;
        } */
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }

    ASTNode *value = parent->right->right->left;
    if ( symbol_set.find(value->value) != symbol_set.end() || primitive_predicate.find(value->value) != primitive_predicate.end() || basic_arithmetic.find(value->value) != basic_arithmetic.end() )
        symbolCheck(value, NULL, 1); // 若為 ATOM 且為 SYMBOL ，則檢查 SYMBOL 是否為定義過的
    else if ( value->type == SYMBOL ) value = user_map[value->value]; // 若被參考值為使用者參數，則將其內容複寫一份進當前 AST
    user_map[key] = value;    // 將定義過的 SYMBOL 加入 map 中
    cout << key << " defined" << endl;
    define = true;
    return true;
}

void Symbol::userDefined(ASTNode *root) {
    /* 將 User Define 傳回並更新 AST */
    try {
        error.checkSymbol(root, false);
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }
    if ( root->value.find("<procedure " ) != std::string::npos ) return; // 若該 SYMBOL 為系統定義過的，則不做任何處理
    // 若該 SYMBOL 為定義過的，則將其內容複寫一份進當前 AST
    string key = root->value;
    
    root = user_map[key]; // 將其內容被當前 AST 指向
    // copyTree(root, user_map[key]);
}

void Symbol::copyTree(ASTNode *root, ASTNode *defined_tree) {
    /* 從 defined_tree 複製 AST 至當前 root */

    root->type = defined_tree->type;
    root->value = defined_tree->value;
    if ( defined_tree->left ) {
        root->left = new ASTNode();
        copyTree(root->left, defined_tree->left);
    }
    if ( defined_tree->right ) {
        root->right = new ASTNode();
        copyTree(root->right, defined_tree->right);
    }
}

bool Symbol::isCAR(ASTNode *root, ASTNode *parent) {
    /* 檢查 Symbol 是否為 CAR 並更新 AST */
    if ( root->value != "car" ) return false;
    else if ( !parent || parent->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 CAR function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        return true;
    }

    int arg = countFunctionArg(parent);  // 計算參數個數
    // cout << "arg: " << arg << endl;  // debug

    if ( arg == 1 ) {
        // 若參數個數不為1，則拋出錯誤
        ASTNode *check_node = parent->right->left;  // 該位置應該為 LEFT_PAREN 或自定義 SYMBOL
        if ( check_node->type == LEFT_PAREN || check_node->type == SYMBOL ) {
            try {
                if ( check_node->type == LEFT_PAREN ) symbolCheck(check_node->left, check_node, 1);
                else symbolCheck(check_node, NULL, 1);
            }
            catch ( const std::runtime_error &msg ) {
                throw msg;
            }
            catch ( ASTNode *temp ) {
                throw temp;
            }
            check_node = parent->right->left->left;  // 將 list 的左子樹做為 CAR 的輸出結果
            if ( !check_node ) 
                // 若左子樹為空，則報錯
                throw std::runtime_error("ERROR (car with incorrect argument type) : " + parent->right->left->value + "\n");
        }
        else {
            throw std::runtime_error("ERROR (car with incorrect argument type) : " + check_node->value + "\n");
        }

        // 將結果寫回根節點
        if ( check_node->type == LINK ) {
            check_node->type = "QUOTE_TEMP";
            check_node->value = "(";
        }
        else if ( check_node->type != LEFT_PAREN && check_node->type != "QUOTE_TEMP" ) {
            check_node->type = "QUOTE_DATA"; // 若為單一 ATOM ，則需額外定義
            check_node->right = NULL;
            check_node->left = NULL;
        }
        parent->type = check_node->type;
        parent->value = check_node->value;
        parent->left = check_node->left;
        parent->right = check_node->right;
    }
    else {
        try {
            bool checkList = isList(parent->right);
            if ( !checkList ) {
                cout << "ERROR (non-list) : ";
                throw root;
            }
        }
        catch ( const std::runtime_error &msg ) {
            throw msg;
        }
        catch ( ASTNode *temp ) {
            throw temp;
        }
        throw std::runtime_error("ERROR (incorrect number of arguments) : car\n");
    }
    return true;
}

bool Symbol::isCDR(ASTNode *root, ASTNode *parent) {
    /* 檢查 Symbol 是否為 CDR 並更新 AST */
    if ( root->value != "cdr" ) return false;
    else if ( !parent || parent->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 CDR function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        return true;
    }

    int arg = countFunctionArg(parent);   // 計算參數個數
    // cout << "arg: " << arg << endl;  // debug

    if ( arg == 1 ) {
        // 若參數個數不為1，則拋出錯誤
        ASTNode *check_node = parent->right->left;  // 該位置應該為 LEFT_PAREN 或自定義 SYMBOL
        if ( check_node->type == LEFT_PAREN || check_node->type == SYMBOL ) {
            try {
                if ( check_node->type == LEFT_PAREN ) symbolCheck(check_node->left, check_node, 1);
                else symbolCheck(check_node, NULL, 1);
            }
            catch ( const std::runtime_error &msg ) {
                throw msg;
            }
            catch ( ASTNode *temp ) {
                throw temp;
            }
            check_node = parent->right->left->right;  // 將 list 的右子樹做為 CDR 的輸出結果
            if ( !check_node ) 
                // 若左子樹為空，則報錯
                throw std::runtime_error("ERROR (car with incorrect argument type) : " + parent->right->left->value + "\n");
        }
        else {
            throw std::runtime_error("ERROR (cdr with incorrect argument type) : " + check_node->value + "\n");
        }
        // 將結果寫回根節點
        if ( check_node->type == LINK ) {
            check_node->type = "QUOTE_TEMP";
            check_node->value = "(";
        }
        else if ( check_node->type != LEFT_PAREN && check_node->type != "QUOTE_TEMP" ) {
            check_node->type = "QUOTE_DATA"; // 若為單一 ATOM ，則需額外定義
            check_node->right = NULL;
            check_node->left = NULL;
        }
        parent->type = check_node->type;
        parent->value = check_node->value;
        parent->left = check_node->left;
        parent->right = check_node->right;
    }
    else {
        try {
            bool checkList = isList(parent->right);
            if ( !checkList ) {
                cout << "ERROR (non-list) : ";
                throw root;
            }
        }
        catch ( const std::runtime_error &msg ) {
            throw msg;
        }
        catch ( ASTNode *temp ) {
            throw temp;
        }
        throw std::runtime_error("ERROR (incorrect number of arguments) : cdr\n");
    }
    return true;
}

bool Symbol::checkPrimitivePredicate(ASTNode *root, ASTNode *parent) {
    /* 檢查該 Symbol 是否為原始判斷式 */
    if ( primitive_predicate.find(root->value) == primitive_predicate.end() ) return false;
    else if ( !parent || parent->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 PrimitivePredicate function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        return true;
    }

    int arg = countFunctionArg(parent);   // 計算參數個數
    //cout << "arg: " << arg << endl;  // debug

    try {
        if ( arg == 1 ) {
            // 若參數個數不為1，則拋出錯誤
            ASTNode *check_node = parent->right->left;  // 該位置應該為需確認的 S-expression
            if ( check_node->type == LEFT_PAREN || check_node->type == SYMBOL ) {
                if ( check_node->type == LEFT_PAREN ) symbolCheck(check_node->left, check_node, 1);
                else symbolCheck(check_node, NULL, 1);
            }
            // 檢查該 S-expression 是否為原始判斷式
            bool checkPrimitive = false;
            if (root->value == "atom?") 
                checkPrimitive = isAtom(check_node);
            else if (root->value == "pair?") 
                checkPrimitive = isPair(check_node);
            else if (root->value == "list?") 
                checkPrimitive = isList(check_node);
            else if ( root->value == "null?") 
                checkPrimitive = isNull(check_node);
            else if (root->value == "integer?") 
                checkPrimitive = isInteger(check_node);
            else if (root->value == "real?" || root->value == "number?") 
                checkPrimitive = isRealOrNumber(check_node);
            else if (root->value == "string?") 
                checkPrimitive = isString(check_node);
            else if (root->value == "boolean?") 
                checkPrimitive = isBoolean(check_node);
            else if (root->value == "symbol?") 
                checkPrimitive = isSymbol(check_node);
            
            if ( checkPrimitive ) {
                parent->type = BOOL;
                parent->value = "#t";
            }
            else {
                parent->type = BOOL;
                parent->value = "nil";
            }
            // 只將結果留下來，並刪除原本的 S-expression
            ASTNode *temp = parent->right;
            parent->right = NULL;
            delete temp; // 刪除原本的右子樹
            temp = parent->left;
            parent->left = NULL;
            delete temp; // 刪除原本的左子樹
        }
        else {
            bool checkList = isList(parent->right);
            if ( !checkList ) {
                cout << "ERROR (non-list) : ";
                throw root;
            }
            string errer_str = "ERROR (incorrect number of arguments) : " + root->value + "\n";
            throw std::runtime_error(errer_str);
        }

    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }
    
    return true;
}

bool Symbol::isAtom(ASTNode *root) {
    /* 檢查該AST是否為 Atom */
    if ( root->type == SYMBOL || root->type == INT || root->type == FLOAT || root->type == STRING || root->type == NIL ) return true;
    return false;
}

bool Symbol::isPair(ASTNode *root) {
    /* 檢查該AST是否為 Pair */
    ASTNode *temp = root;
    if ( root->type == "QUOTE_TEMP" )
        // 若為 QUOTE ，則該 AST 內部的 S-expression 為 Pair
        return true;
    return false;
}

bool Symbol::isList(ASTNode *root) {
    /* 檢查該AST是否為List */
    ASTNode *temp = root;

    while ( temp->type != END ) {
        try {
            if ( temp->type != LINK && temp->type != "QUOTE_TEMP" ) return false; // 若該AST並不為List，則返回false
            else if ( temp->left->type == LEFT_PAREN ) symbolCheck(temp->left->left, temp->left, 1); // 檢查該 AST 內部的 S-expression
        }
        catch ( const std::runtime_error &msg ) {
            throw msg;
        }
        catch ( ASTNode *temp ) {
            throw temp;
        }
        temp = temp->right;
    }
    return true;

}

bool Symbol::isNull(ASTNode *root) {
    /* 檢查該 AST 是否為 NIL */
    if ( root->type == NIL ) return true;
    return false;
}

bool Symbol::isInteger(ASTNode *root) {
    /* 檢查該 AST 是否為 Integer */
    if ( root->type == INT ) return true;
    return false;
}

bool Symbol::isRealOrNumber(ASTNode *root) {
    /* 檢查該 AST 是否為 Real */
    if ( root->type == INT || root->type == FLOAT ) return true;
    return false;
}

bool Symbol::isString(ASTNode *root) {
    /* 檢查該 AST 是否為 String */
    if ( root->type == STRING ) return true;
    return false;
}

bool Symbol::isBoolean(ASTNode *root) {
    /* 檢查該 AST 是否為 Boolean */
    if ( root->type == BOOL || root->type == NIL ) return true;
    return false;
}

bool Symbol::isSymbol(ASTNode *root) {
    /* 檢查該 AST 是否為 Symbol */
    if ( root->type == SYMBOL || root->type == "QUOTE_DATA" ) return true;
    return false;
}

bool Symbol::isArithmetic(ASTNode *root, ASTNode *parent) {
    /* 檢查該 Symbol 是否為基礎運算式或比較運算式 */

    if ( basic_arithmetic.find(root->value) == basic_arithmetic.end() || 
    (root->value.size() > 14 && basic_arithmetic.find(root->value.substr(13, root->value.size() - 1)) == basic_arithmetic.end()) ) return false;
    else if ( !parent || parent->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 Arithmetic function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        return true;
    }

    int arg = countFunctionArg(parent);   // 計算參數個數
    // cout << "arg: " << arg << endl;  // debug

    try {
        if ( (arg == 1 && root->value == "not") || (arg >= 2 && root->value != "not") ) {
            // 若參數個數小於2，則拋出錯誤
            ASTNode *count_node = parent->right;  // 該位置應該為需計算的 S-expression
            string result, compare_result = "";
            bool first = false, compare = false;
            while ( count_node->type != END ) {
                if ( count_node->left->type == LEFT_PAREN || count_node->left->type == SYMBOL ) {
                    if ( count_node->left->type == LEFT_PAREN ) {
                        stack<ASTNode *> s;
                        ASTNode *temp = count_node->left;
                        s.push(temp);
                        while ( temp->left->type == LEFT_PAREN ) {
                            s.push(temp->left);
                            temp = temp->left;
                        }
                        while ( !s.empty() ) {
                            // 處理多階層的 S-expression
                            temp = s.top();
                            symbolCheck(temp->left, temp, 1);
                            s.pop();
                        }
                    }
                    else symbolCheck(count_node->left, NULL, 1);
                }
                // 計算結果
                calculate(count_node, root->value, result, first);
                if ( (result == "nil" || result == "#t") && root->value != "or" ) {
                    // 若結果為 nil 或 #t，則該 function 為比較運算式
                    compare = true;
                    if ( result == "nil" )
                        // 若結果為 nil，則將比較結果設為 false
                        compare_result = "nil";
                    else if ( compare_result != "nil" )
                        // 若結果不為 nil，則將比較結果設為 true
                        compare_result = "#t";
                    result = count_node->left->value; // 將當前數字儲存
                }

                count_node = count_node->right;  // 接續下一個數字
            }
            if ( root->value == "and" ) {
                // 若為 and 運算式，且過程中結果並未有 nil，則結果為最後一個元素
                if ( compare_result == "nil") result = compare_result;
            }
            else if ( compare ) result = compare_result; // 若為比較運算式，則將結果設為比較結果
            
            // 將結果寫回根節點
            if ( atom.isINT(result) ) {
                // 若結果為整數，則將其轉為整數
                parent->type = INT;
                parent->value = result;
            }
            else if ( atom.isFLOAT(result) ) {
                // 若結果為浮點數，則將其轉為浮點數
                parent->type = FLOAT;
                parent->value = result;
            }
            else if ( atom.isSTRING(result) ) {
                // 若結果為字串，則將其轉為字串
                parent->type = STRING;
                parent->value = result;
            }
            else if ( atom.isT(result) || atom.isNIL(result) ) {
                parent->type = BOOL;
                parent->value = result;
            }
            // 只將結果留下來，並刪除原本的 S-expression
            ASTNode *temp = parent->right;
            parent->right = NULL;
            delete temp; // 刪除原本的右子樹
            temp = parent->left;
            parent->left = NULL;
            delete temp; // 刪除原本的左子樹
        }
        else {
            /* bool checkList = isList(parent->right);
            if ( !checkList ) {
                cout << "ERROR (non-list) : ";
                throw root;
            } */
            string errer_str = "ERROR (incorrect number of arguments) : " + root->value + "\n";
            throw std::runtime_error(errer_str);
        }
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        string error_str = "ERROR (" + root->value + " with incorrect argument type) : " + temp->value + "\n";
        throw std::runtime_error(error_str);
    }
    
    return true;
}

void Symbol::calculate(ASTNode *root, string &operand, string &result, bool &first) {
    /* 計算結果 */
    if ( !atom.isINT(root->left->value) && !atom.isFLOAT(root->left->value) && 
        (operand == "+" || operand == "-" || operand == "*" || operand == "/" || operand == ">" || operand == "<" || operand == ">=" || operand == "<=" || operand == "=" ) )
        // 若左子樹不為數字，則拋出錯誤
        throw root->left;
    else if ( !atom.isSTRING(root->left->value) && (operand == "string-append" || operand == "string>?" || operand == "string<?" || operand == "string=?" ) )
        // 若左子樹不為字串，則拋出錯誤
        throw root->left;

    if ( !first ) {
        // 並未寫入第一筆資料
        result = root->left->value; // 將當前數字儲存
        if ( operand == "not" ) result = ( result == "nil" ) ? "#t" : "nil"; // 若為 not，則將結果轉為布林值
        first = true; // 設定為已寫入第一筆資料
        return;
    }

    string operate = root->left->value;

    if ( operand == "+" ) {
        if ( atom.isINT(operate) && atom.isINT(result) ) 
            // 若兩者皆為整數，則將其轉為整數
            result = to_string(stoi(result) + stoi(operate));
        else 
            // 若兩者皆為浮點數，則將其轉為浮點數
            result = to_string(stof(result) + stof(operate));
    }
        
    else if ( operand == "-" ) {
        if ( atom.isINT(operate) && atom.isINT(result) ) 
            result = to_string(stoi(result) - stoi(operate));
        else 
            result = to_string(stof(result) - stof(operate));
    }
    else if ( operand == "*" ) {
        if ( atom.isINT(operate) && atom.isINT(result) ) 
            result = to_string(stoi(result) * stoi(operate));
        else 
            result = to_string(stof(result) * stof(operate));
    }
    else if ( operand == "/" ) {
        if ( stoi(operate) == 0 ) {
            // 若除數為0，則拋出錯誤
            throw std::runtime_error("ERROR (division by zero) : /\n");
        }
        if ( atom.isINT(operate) && atom.isINT(result) ) 
            result = to_string(stoi(result) / stoi(operate));
        else
            result = to_string(stof(result) / stof(operate));
    }
    else if ( operand == "and" && result != "nil" ) {
        if ( root->left->type != BOOL && root->left->type != NIL ) result = operate;
        else result = ( operate == "#t" ) ? "#t" : "nil";
    }
    else if ( operand == "or" && result == "nil" ) {
        if ( root->left->type != BOOL && root->left->type != NIL ) result = operate;
        else result = ( operate == "#t" ) ? "#t" : "nil";
    }
    else if ( operand == ">" ) 
        result = ( stof(result) > stof(operate) ) ? "#t" : "nil";
    else if ( operand == "<" ) 
        result = ( stof(result) < stof(operate) ) ? "#t" : "nil";
    else if ( operand == ">=" ) 
        result = ( stof(result) >= stof(operate) ) ? "#t" : "nil";
    else if ( operand == "<=" ) 
        result = ( stof(result) <= stof(operate) ) ? "#t" : "nil";
    else if ( operand == "=" ) 
        result = ( stof(result) == stof(operate) ) ? "#t" : "nil";
    else if ( operand == "string-append" )
        // 若為字串相加，則將其轉為字串
        result = result.substr(0, result.size() - 1) + operate.substr(1, operate.size() - 1); // 去除字串的引號
    else if ( operand == "string>?" ) 
        // 若為字串比較，則將其轉為布林值
        result = ( result > operate ) ? "#t" : "nil";
    else if ( operand == "string<?" ) 
        result = ( result < operate ) ? "#t" : "nil";
    else if ( operand == "string=?" ) 
        result = ( result == operate ) ? "#t" : "nil";
}

bool Symbol::isEqual(ASTNode *root, ASTNode *parent) {
    /* 檢查該 AST 是否為 equal */
    if ( root->value != "eqv?" && root->value != "equal?" ) return false;
    else if ( !parent || parent->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 EQUAL function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        return true;
    }

    int arg = countFunctionArg(parent);   // 計算參數個數
    // cout << "arg: " << arg << endl;  // debug

    try {
        if ( arg == 2 ) {
            // 若參數個數不為2，則拋出錯誤
            ASTNode *first_element = parent->right->left, *second_element = parent->right->right->left;
            bool eqv = false, equal = false;
            if ( first_element->type == STRING || second_element->type == STRING ) eqv = false;
            else if ( isAtom(first_element) && isAtom(second_element) && first_element->value == second_element->value ) eqv = true;
            else if ( user_map.find(first_element->value) != user_map.end() && user_map.find(second_element->value) != user_map.end()
                        && user_map[first_element->value] == user_map[second_element->value] ) 
                // 若兩者皆為自定義 SYMBOL，且指向內容相同
                eqv = true;
    
            ASTNode *check_node = parent->right;
            while ( check_node->type != END ) {
                if ( check_node->left->type == LEFT_PAREN || check_node->left->type == SYMBOL ) {
                    if ( check_node->left->type == LEFT_PAREN ) symbolCheck(check_node->left->left, check_node->left, 1);
                    else symbolCheck(check_node->left, NULL, 1);
                }
                check_node = check_node->right;  // 接續下一個 S-expression
            }
            equal = campare(first_element, second_element); // 檢查兩個 S-expression 是否相同
            
            // 將結果寫回根節點
            if ( root->value == "eqv?" ) {
                // 若為 eqv?，則將結果設為 eqv 的結果
                parent->type = BOOL;
                parent->value = ( eqv ) ? "#t" : "nil";
            }
            else if ( root->value == "equal?" ) {
                // 若為 equal?，則將結果設為 equal 的結果
                parent->type = BOOL;
                parent->value = ( equal ) ? "#t" : "nil";
            }
            // 只將結果留下來，並刪除原本的 S-expression
            ASTNode *temp = parent->right;
            parent->right = NULL;
            delete temp; // 刪除原本的右子樹
            temp = parent->left;
            parent->left = NULL;
            delete temp; // 刪除原本的左子樹
        }
        else {
            bool checkList = isList(parent->right);
            if ( !checkList ) {
                cout << "ERROR (non-list) : ";
                throw root;
            }
            string error_str = "ERROR (incorrect number of arguments) : " + root->value + "\n";
            throw std::runtime_error(error_str);
        }
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }
    
    return true;
}

bool Symbol::campare(ASTNode *first_element, ASTNode *second_element) {
    /* 檢查該 AST 是否為相同 */
    if ( first_element == NULL && second_element == NULL ) return true;
    else if ( first_element == NULL || second_element == NULL ) return false;

    if ( first_element->type == END && second_element->type == END ) return true;
    else if ( first_element->value != second_element->value ) return false;
    return campare(first_element->left, second_element->left) && campare(first_element->right, second_element->right);
}

bool Symbol::isIf(ASTNode *root, ASTNode *parent) {
    /* 檢查 Symbol 是否為 IF 並更新 AST */
    if ( root->value != "if" ) return false;
    else if ( !parent || parent->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 IF function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        return true;
    }

    int arg = countFunctionArg(parent);   // 計算參數個數
    // cout << "arg: " << arg << endl;  // debug

    try {
        if ( arg == 2 || arg == 3 ) {
            // 若參數個數不為 2 或 3 ，則拋出錯誤
            ASTNode *check_node = parent->right, *temp_root = new ASTNode() ;  // 該位置應該為需運行的 S-expression
            copyTree(temp_root, parent); // 將該 S-expression 複製一份，報錯時使用
            
            conditional(&check_node, root->value); // 運行判別式
            if ( !check_node ) {
                // 判別結果無資料，則報錯
                cout << "ERROR (no return value) : ";
                throw temp_root;
            }

            parent->type = check_node->type;
            parent->value = check_node->value;
            parent->left = check_node->left;
            parent->right = check_node->right;
        }
        else {
            bool checkList = isList(parent->right);
            if ( !checkList ) {
                cout << "ERROR (non-list) : ";
                throw root;
            }
            string error_str = "ERROR (incorrect number of arguments) : if\n";
            throw std::runtime_error(error_str);
        }
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }

    return true;
}

bool Symbol::isCond(ASTNode *root, ASTNode *parent) {
    /* 檢查 Symbol 是否為 COND 並更新 AST */
    if ( root->value != "cond" ) return false;
    else if ( !parent || parent->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 COND function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        return true;
    }

    int arg = countFunctionArg(parent);   // 計算參數個數
    // cout << "arg: " << arg << endl;  // debug

    if ( arg < 1 ) {
        // 若參數個數不為1，則拋出錯誤
        cout << "ERROR (COND format) : ";
        throw root;
    }

    try {
        ASTNode *check_node = parent->right, *temp_root = new ASTNode(), *result_statement = NULL;  // 該位置應該為需運行的 S-expression
        copyTree(temp_root, parent); // 將該 S-expression 複製一份，報錯時使用
            
        while ( check_node->type != END ) {
            // 檢查 S-expression 內的每個左子樹
            if ( check_node->left->type != LEFT_PAREN ) {
                // 若左子樹不為 S-expression，則拋出錯誤
                cout << "ERROR (COND format) : ";
                throw temp_root;
            }
            else {
                ASTNode *temp = check_node->left;
                while ( temp->type != END ) {
                    // 檢查 S-expression 內的每個左子樹
                    if ( temp->type != LEFT_PAREN && temp->type != LINK ) {
                        // 若子樹具備右節點，則不符合格式
                        cout << "ERROR (COND format) : ";
                        throw temp_root;
                    }
                    temp = temp->right;
                }
            }
            if ( check_node->right->type != END && check_node->left->left->value != "else" ) {
                // 有先處理判斷式，只留下 True 的 S-expression
                if ( check_node->left->left->type == LEFT_PAREN ) symbolCheck(check_node->left->left->left, check_node->left->left, 1);
            }
            check_node = check_node->right; // 接續下一個 S-expression
        }

        check_node = parent->right;
        while ( check_node->type != END ) {
            if ( check_node->right->type == END && check_node->left->left->value == "else" ) {
                // 若為 else，則將其設為最後一筆 S-expression
                ASTNode *temp = check_node->left;
                while ( temp->right->type != END ) {
                    temp = temp->right; // 接續下一個 S-expression
                }
                if ( !result_statement ) result_statement = temp; // 若結果不為空，則將其設為結果
                break;
            }
            else if ( check_node->left->left->value != "nil" ) conditional(&check_node->left, root->value); // 運行判別式
            else check_node->left = NULL; // 該 S-expression 不須運作

            if ( check_node->left && !result_statement ) {
                result_statement = check_node->left; // 若結果不為空，則將其設為結果
                break;
            }

            check_node = check_node->right; // 接續下一個 S-expression
        }

        if ( !result_statement ) {
            // 判別結果無資料，則報錯
            cout << "ERROR (no return value) : ";
            throw temp_root;
        }
        ASTNode *temp = result_statement;
        while ( temp->right && temp->type != END  ) {
            // 檢查 S-expression 內的每個左子樹
            if ( temp->left->type == LEFT_PAREN || temp->left->type == SYMBOL ) {
                if ( temp->left->type == LEFT_PAREN ) symbolCheck(temp->left->left, temp->left, 1);
                else symbolCheck(temp->left, NULL, 1);
            }
            
            if ( temp->right->type == END ) {
                // 若為最後一筆 S-expression，則將其設為結果
                result_statement = temp->left;
            }
            temp = temp->right; // 接續下一個 S-expression
        }
        if ( temp->type == SYMBOL ) userDefined(temp); // 若為自定義 Symbol，則將其轉為對應的 AST

        parent->type = result_statement->type;
        parent->value = result_statement->value;
        parent->left = result_statement->left;
        parent->right = result_statement->right;
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }
    return true;
}

void Symbol::conditional(ASTNode **root, string &operand) {
    /* 檢查該是否回傳 statemet */
    ASTNode *check_node = (*root)->left;  // 該位置應該為需運行的 S-expression

    try {
        if ( check_node->type == LEFT_PAREN || check_node->type == SYMBOL ) {
            // 檢查 S-expression 內的每個左子樹
            if ( check_node->type == LEFT_PAREN ) symbolCheck(check_node->left, check_node, 1);
            else symbolCheck(check_node, NULL, 1);
        }

        bool conditional = true;
        check_node = (*root)->left;
        if ( check_node->value == "nil" )
            // 若條件為 nil，則將 conditional 設為 false
            conditional = false;

        ASTNode *first_statement = (*root)->right, *second_statement = (*root)->right->right->left;
        if ( conditional ) {
            // 若條件為 true，則將 S-expression 的左子樹設為第一筆 S-expression
            if ( operand == "cond" ) {
                // 若為 cond，則直接將該筆資料回傳
                (*root) = first_statement;
                return;
            }
            check_node = first_statement->left;
            if ( check_node->type == LEFT_PAREN || check_node->type == SYMBOL ) {
                // 檢查 S-expression 內的每個左子樹
                if ( check_node->type == LEFT_PAREN ) symbolCheck(check_node->left, check_node, 1);
                else symbolCheck(check_node, NULL, 1);
            }
            (*root) = check_node;
        }
        else {
            if ( !second_statement ) {
                // 第二筆 S-expression 為空，則回傳空值
                (*root) = NULL;
            }
            else {
                // 若條件為 false，則將 S-expression 的左子樹設為第二筆 S-expression
                check_node = second_statement;
                if ( check_node->type == LEFT_PAREN || check_node->type == SYMBOL ) {
                    // 檢查 S-expression 內的每個左子樹
                    if ( check_node->type == LEFT_PAREN ) symbolCheck(check_node->left, check_node, 1);
                    else symbolCheck(check_node, NULL, 1);
                }
                (*root) = check_node;
            }
        }
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }
    
    
}

bool Symbol::isBegin(ASTNode *root, ASTNode *parent) {
    /* 檢查*/
    if ( root->value != "begin" ) return false;
    else if ( !parent || parent->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 BEGIN function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        return true;
    }

    int arg = countFunctionArg(parent);   // 計算參數個數
    // cout << "arg: " << arg << endl;  // debug

    if ( arg < 1 ) {
        // 若參數個數不為1，則拋出錯誤
        string error_str = "ERROR (incorrect number of arguments) : begin\n";
        throw std::runtime_error(error_str);

    }
    bool checkList = isList(parent->right);
    if ( !checkList ) {
        cout << "ERROR (non-list) : ";
        throw root;
    }

    try {
        ASTNode *check_node = parent->right, *temp_root = new ASTNode();  // 該位置應該為需運行的 S-expression
        copyTree(temp_root, parent); // 將該 S-expression 複製一份，報錯時使用
            
        while ( check_node->type != END ) {
            // 檢查 S-expression 內的每個左子樹
            if ( check_node->left->type == SYMBOL ) {
                symbolCheck(check_node->left, NULL, 1);
            }

            if ( check_node->right->type == END ) {
                // 若為最後一筆 S-expression，則將其設為結果
                parent->type = check_node->left->type;
                parent->value = check_node->left->value;
                parent->left = check_node->left->left;
                parent->right = check_node->left->right;
            }

            check_node = check_node->right;  // 接續下一個 S-expression
        }


    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }
    return true;
}

bool Symbol::isCleanEnvironment(ASTNode *root, ASTNode *parent, int deep) {

    if ( root->value != "clean-environment" ) return false;
    else if ( !parent || parent->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 CleanEnvironment function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        return true;
    }

    int arg = countFunctionArg(parent);   // 計算參數個數

    try {
        if ( arg == 0 && deep == 0 ) {
            // 若參數個數不為0，則拋出錯誤
            // 清除環境變數
            user_map.clear();
            cout << "environment cleaned" << endl;
            cout << "\n> ";
            throw "";
        }
        else {
            bool checkList = isList(parent->right);
            if ( !checkList ) {
                cout << "ERROR (non-list) : ";
                throw root;
            }
            string error_str = "ERROR (incorrect number of arguments) : clean-environment\n";
            if ( deep != 0 ) error_str = "ERROR (level of CLEAN-ENVIRONMENT)\n";
            throw std::runtime_error(error_str);
        }
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }
    return true;
}

void Symbol::procedure(ASTNode *root, ASTNode *parent) {

    if ( user_map.find(root->value) != user_map.end() ) {
        if ( user_map[root->value]->value.find("<procedure " ) != std::string::npos ) {
            if ( parent && parent->type == LEFT_PAREN ) {
                // 若父節點為 LEFT_PAREN ，則運行 procedure function 功能
                root->value = user_map[root->value]->value.substr(12, user_map[root->value]->value.size() - 13); // 去除 <procedure 和 >
            }
            else root->value = user_map[root->value]->value; // 若父節點不為 LEFT_PAREN ，則不做 procedure function 功能，只留回傳功能
        }
    }
    else if ( root->value.find("<procedure " ) != std::string::npos ) {
        if ( parent && parent->type == LEFT_PAREN ) {
            // 若父節點為 LEFT_PAREN ，則運行 procedure function 功能
            root->value = root->value.substr(12, root->value.size() - 13); // 去除 <procedure 和 >
        }
    }
}

bool Symbol::isExit(ASTNode *root, ASTNode *parent) {

    if ( root->value != "exit" ) return false;
    else if ( !parent || parent->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 DEFINE function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        return true;
    }
    else {
        throw std::runtime_error("ERROR (incorrect number of arguments) : exit\n"); // 拋出錯誤
    }
}