#include <iostream>
#include <queue>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <math.h>

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
#define SYSTEM "SYSTEM"

#define TEMP "TEMP"
#define BEGIN "BEGIN"
#define END "END"
#define LINK "LINK"

unordered_set<string> symbol_set{"cons", "quote", "list", "define", "car", "cdr", "eqv?", "equal?", "if", "cond", "begin", "clean-environment", "exit", "let", "lambda"}, 
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
        void isFLOAT(string &atom, bool rounding);
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
        void checkSymbol(ASTNode *cur, bool non_function, unordered_map<string, ASTNode*> local_map);
        
};

class Symbol {
    public:
        Symbol();
        ~Symbol();
        bool &getDefine() { return define; };
        int &getReturnValue() { return return_value; };
        void symbolCheck(ASTNode **root, ASTNode **parent, int deep);
        int countFunctionArg(ASTNode *root);
        bool isCONS(ASTNode *root, ASTNode *parent);
        bool isQUOTE(ASTNode *root, ASTNode *parent);
        bool isList(ASTNode *root, ASTNode *parent);
        bool isUserDefine(ASTNode *root, ASTNode *parent, int deep);
        void userDefined(ASTNode **root);
        void copyTree(ASTNode *root, ASTNode *defined_tree);
        bool isCAR(ASTNode **root, ASTNode **parent);
        bool isCDR(ASTNode **root, ASTNode **parent);
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
        bool isArithmetic(ASTNode *root, ASTNode **parent);
        bool isEqual(ASTNode *root, ASTNode *parent);
        bool isIf(ASTNode *root, ASTNode **parent);
        bool isCond(ASTNode *root, ASTNode **parent);
        bool isBegin(ASTNode *root, ASTNode **parent);
        bool isCleanEnvironment(ASTNode *root, ASTNode *parent, int deep);
        bool isExit(ASTNode *root, ASTNode *parent, int deep);
        bool isLet(ASTNode *root, ASTNode **parent);
        bool isLambda(ASTNode *root, ASTNode **parent);
    private:
        Atom atom;
        Error error;
        unordered_map<string, ASTNode*> local_map, lambda_map; // 區域變數庫與 lambda 變數庫
        int return_value;
        bool define, lambda;
        ASTNode *lambda_tree;
        void checkExpression(ASTNode **root, ASTNode **parent);
        void copyAndLink(ASTNode **root);
        void calculate(ASTNode *root, ASTNode **result, string &operand, bool &first);
        bool campare(ASTNode *first_element, ASTNode *second_element);
        void conditional(ASTNode **root);
        void procedure(ASTNode **root, ASTNode *parent);
        void localDefine(ASTNode *root);
        void checkLambda(ASTNode *root);
        ASTNode* useLambda(ASTNode *root);
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
            if ( c == '\\' && !backslash ) backslash = true; // 遇到反斜線
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
    symbol = Symbol();
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
    cout << "token: " << token << " " << type << " " << column << endl; // debug
    cout << "cur: " << cur->value << " " << cur->type << endl; // debug

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
        catch ( const char *msg ) {
            if ( msg == "Exit" ) return;
        }
        line = 0, column = 0;
        return;
    }
    else if ( dot ) {
        // 若前一個 Token 為 DOT ，則將該 Token 存入右節點
        cur->value = token;
        cur->type = type;
        cur->left = NULL;
        cur->right = NULL;
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
            try {
                optimizeAST(); // 指令建構完成，則輸出 AST
            }
            catch ( const std::runtime_error &msg ) {
                throw msg;
            }
            catch ( const char *msg ) {
                if ( msg == "Exit" ) return;
            }
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
    else if ( this->atom.isT(atom) ) type = T;
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
            if ( atom.isFLOAT(cur->value) ) atom.isFLOAT(cur->value, true); // 若為浮點數，則取小數點後三位
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
    /*檢查Atom是否為浮點數*/
    int len = atom.size(), decimal = 0;
    bool dot = false, interger = false;

    for ( int i = 0; i < len; ++i ) {
        // 數字開頭可以有正負號
        if ( i == 0 && (atom[i] == '+' || atom[i] == '-') ) 
            continue;
        else if ( !isdigit(atom[i]) ) {
            if ( atom[i] == '.' && !dot )
                dot = true;
            else return false; // 若含非數字或點字元，則不為浮點數
        }
        else {
            if ( decimal >= 3 ) {
                // 小數點後最多3位
                ++decimal;
                continue;
            } 
            interger = true;
            if ( dot ) ++decimal; // 計算小數位數
        }
    }

    if ( !interger || !dot  ) return false; // 若不含數字，則不為浮點數
    return true;
}

void Atom::isFLOAT(string &atom, bool rounding) {
    /*為浮點數，取浮點數後三位*/
    double num = stod(atom);
    num = round(num * 1000) / 1000; // 四捨五入至小數點後三位
    atom = to_string(num); // 轉換為字串
    for ( int i = 0; i < atom.size(); ++i ) {
        if ( atom[i] == '.' ) {
            // 若有小數點，則將其後的數字取出
            atom = atom.substr(0, i + 4); // 小數點後最多3位
            break;
        }
    }
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
        try {
            function.storeTokenInAST(token, line, column);
        }
        catch ( const std::runtime_error &msg ) {
            throw msg;
        }
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
        catch ( const char *msg ) {
            // clean-environment 不做其他回傳
            if ( msg == "Exit" ) throw msg;
            function.newAST();
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
    cout << "optimizeAST" << endl;
    while ( !ast.empty() ) backExpression(); // 將所有 S-expression 跳出至最外層

    stack<pair<ASTNode**, ASTNode**>> st;
    st.push({&root, NULL});

    if ( root->type == SYMBOL ) {
        // 若根節點為 SYMBOL ，則檢查是否存在
        try {
            symbol.symbolCheck(&root, NULL, 0); // 為單一 ATOM 時，直接輸出其 SYMBOL 的 value
            ASTNode *copy_temp = new ASTNode();
            symbol.copyTree(copy_temp, root); // 將該自定義結構複製
            root = copy_temp; // 給當前 AST 使用
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
            return;
        }
        printAST(); 
        return;
    }

    while ( !st.empty() ) {
        ASTNode **cur = st.top().first;
        ASTNode **parent = st.top().second;
        st.pop();
        if ( !parent ) {
            while ( (*cur)->left != NULL ) {
                st.push({&(*cur)->left, cur});
                cur = &(*cur)->left;
            }
        }
        else {
            try {
                symbol.symbolCheck(cur, parent, 0);
                ASTNode *copy_temp = new ASTNode();
                symbol.copyTree(copy_temp, *parent); // 將該自定義結構複製
                *parent = copy_temp; // 給當前 AST 使用
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
                return;
            }
        }

    }
    printAST(); 
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

void Error::checkSymbol(ASTNode *cur, bool non_function, unordered_map<string, ASTNode*> local_map) {
    /* 檢查 Symbol 是否為 CONS, QUOTE, LIST, 或自定義變數等 */

    if ( symbol_set.find(cur->value) == symbol_set.end() 
            && primitive_predicate.find(cur->value) == primitive_predicate.end() 
            && basic_arithmetic.find(cur->value) == basic_arithmetic.end() 
            && user_map.find(cur->value) == user_map.end()
            && local_map.find(cur->value) == local_map.end()
            && cur->value.find("<procedure " ) == std::string::npos ) {
        // 若該 SYMBOL 未定義，則拋出錯誤
        if ( cur->type == SYMBOL ) cout << "ERROR (unbound symbol) : ";
        else cout << "ERROR (attempt to apply non-function) : ";
        throw cur;
    }
    else if ( !non_function ) {
        // 若在函數位置卻為出現並未系統定義的 SYMBOL ，則拋出錯誤
        if ( cur->type == "QUOTE_DATA" ) {
            if ( cur->value.find("#<procedure " ) != std::string::npos ) return; // 若為系統定義的函數，則不需拋出錯誤
            cout << "ERROR (attempt to apply non-function) : ";
            throw cur;
        }
        else if ( local_map.find(cur->value) != local_map.end() ) {
            // 區域變數
            if ( local_map[cur->value]->type == SYSTEM ) return; // 若為系統定義的函數，則不需拋出錯誤
            cout << "ERROR (attempt to apply non-function) : ";
            throw local_map[cur->value];
        }
        else if ( user_map.find(cur->value) != user_map.end() ) {
            // 全域變數
            if ( user_map[cur->value]->type == SYSTEM ) return; // 若為系統定義的函數，則不需拋出錯誤
            cout << "ERROR (attempt to apply non-function) : ";
            throw user_map[cur->value];
        }
        
    }
    
}

Symbol::Symbol() {
    // cout << "Symbol created" << endl;    // debug
    atom = Atom();
    error = Error(); 
    define = false;
    lambda = false;
    return_value = 0;
    lambda_tree = NULL;
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

void Symbol::checkExpression(ASTNode **root, ASTNode **parent) {
    /* 檢查該 Expression 左子樹 */
    try {
        // 若為 S-expression ，則需檢查其內容
        if ( (*parent)->type == LEFT_PAREN ) {
            if ( root && (*root)->type == LEFT_PAREN ) checkExpression(&(*root)->left, root); // 若為左括弧，則檢查其內容
            symbolCheck(root, parent, 1);
        }
        else if ( (*parent)->type == SYMBOL )
            symbolCheck(parent, NULL, 1);
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( const char *msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }
}

void Symbol::copyAndLink(ASTNode **root) {
    /* 將該子樹複製一份回該點 */
    ASTNode *temp = new ASTNode();
    copyTree(temp, *root); // 將該自定義結構複製
    *root = temp;
}

void Symbol::symbolCheck(ASTNode **root, ASTNode **parent, int deep) {
    /* 檢查 Symbol 是否存在 */
    
    bool non_function = false;
    if ( !parent || !(*parent) ) non_function = true; // 若無父節點，則為單一 ATOM
    try {
        if ( !non_function && !isList(*parent) ) {
            cout << "ERROR (non-list) : ";
            throw *parent; // 若該 S-expression 內的資料不為 List ，則拋出錯誤
        }
        error.checkSymbol(*root, non_function, lambda ? lambda_map : local_map);
        procedure(root, non_function ? NULL : *parent); // 檢查是否為系統定義的函數
        if ( isCONS(*root, non_function ? NULL : *parent) ) cout << "CONS" << endl;
        else if ( isQUOTE(*root, non_function ? NULL : *parent) ) cout << "QUOTE" << endl;
        else if ( isList(*root, non_function ? NULL : *parent) ) cout << "LIST" << endl;
        else if ( isUserDefine(*root, non_function ? NULL : *parent, deep) ) cout << "DEFINE" << endl;
        else if ( isCAR(root, non_function ? NULL : parent) ) cout << "CAR" << endl;
        else if ( isCDR(root, non_function ? NULL : parent) ) cout << "CDR" << endl;
        else if ( checkPrimitivePredicate(*root, non_function ? NULL : *parent) ) cout << "PRIMITIVE PREDICATE" << endl;
        else if ( isArithmetic(*root, non_function ? NULL : parent) ) cout << "BASIC ARITHMETIC" << endl;
        else if ( isEqual(*root, non_function ? NULL : *parent) ) cout << "EQUAL" << endl;
        else if ( isIf(*root, non_function ? NULL : parent) ) cout << "IF" << endl;
        else if ( isCond(*root, non_function ? NULL : parent) ) cout << "COND" << endl;
        else if ( isBegin(*root, non_function ? NULL : parent) ) cout << "BEGIN" << endl;
        else if ( isCleanEnvironment(*root, non_function ? NULL : *parent, deep) ) cout << "CLEAN ENVIRONMENT" << endl;
        else if ( isExit(*root, non_function ? NULL : *parent, deep) ) return; 
        else if ( isLet(*root, non_function ? NULL : parent) ) cout << "LET" << endl;
        else if ( isLambda(*root, non_function ? NULL : parent) ) cout << "LAMBDA" << endl;
        else userDefined(root);
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( const char *msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        ASTNode *error_tree = new ASTNode();
        copyTree(error_tree, temp);
        throw error_tree; // 測試中 throw parent;
    }
    
}

bool Symbol::isCONS(ASTNode *root, ASTNode *parent) {
    /* 檢查 Symbol 是否為 CONS 並更新 AST */
    if ( root->value != "cons" ) return false;
    else if ( !parent || parent->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 CONS function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        root->type = SYSTEM;
        return true;
    }

    int arg = countFunctionArg(parent);   // 計算參數個數
    ASTNode *temp = parent->right; 
    try {
        if ( arg == 2 ) {
            ASTNode **fir_node = &parent->right->left;
            checkExpression(&(*fir_node)->left, fir_node); // 檢查第一個參數
            ASTNode **sec_node = &parent->right->right->left;
            checkExpression(&(*sec_node)->left, sec_node); // 檢查第二個參數
            parent->right->right = parent->right->right->left;   // 第二元素將與第一元素同階層
    
            // 將 dotted pair 提高一個階層放置
            parent->left = parent->right->left;
            parent->right = parent->right->right; 
        }
        else 
            throw std::runtime_error("ERROR (incorrect number of arguments) : cons\n");
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( const char *msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }

    return true;
    
}

bool Symbol::isQUOTE(ASTNode *root, ASTNode *parent) {
    /* 檢查 Symbol 是否為 QUOTE 並更新 AST */
    if ( root->value != "quote" ) return false;
    else if ( !parent || parent->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 QUOTE function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        root->type = SYSTEM;
        return true;
    }

    int arg = countFunctionArg(parent);   // 計算參數個數

    if ( arg != 1 ) 
        // 若參數個數不為1，則拋出錯誤
        throw std::runtime_error("ERROR (incorrect number of arguments) : quote\n");
    
    ASTNode *new_root = parent->right->left;    // 將 QUOTE 後的 S-expression 提高一個階層
    if ( new_root->type == NIL ) {
        parent->type = END;
        parent->value = "nil";
        parent->left = NULL;
        parent->right = NULL;
        return true;
    }
    else if ( new_root->type != LEFT_PAREN ) {
        // 該 S-expression 內的資料為 ATOM ，則需額外定義
        if ( new_root->type == SYMBOL ) parent->type = "QUOTE_DATA"; // 若為 SYMBOL ，則需額外定義
        else parent->type = new_root->type;
        parent->value = new_root->value;
        parent->left = NULL;
        parent->right = NULL;
        return true;
    }
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
        root->type = SYSTEM;
        return true;
    }
    
    int arg = countFunctionArg(parent);   // 計算參數個數
    ASTNode *temp = parent->right;
    try {
        while ( temp->type != END ) {
            // 檢查 List 內的每個左子樹
            checkExpression(&temp->left->left, &temp->left); // 檢查左子樹
            temp = temp->right;
        }
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( const char *msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }

    // 將 List 內容提高一個階層
    parent->left = parent->right->left;   
    parent->right = parent->right->right;
    if ( !parent->left && !parent->right ) {
        // 若該 list 為空，則將其轉為 nil
        parent->type = END;
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
        root->type = SYSTEM;
        return true;
    }

    if ( deep != 0 ) 
        // 若不在最上層，則報錯
        throw std::runtime_error("ERROR (level of DEFINE)\n");
    int arg = countFunctionArg(parent);   // 計算參數個數
    if ( arg != 2 ) {
        // 若參數個數不為2，則拋出錯誤
        cout << "ERROR (DEFINE format) : ";
        throw parent;
    }

    string key;
    try {
        key = parent->right->left->value; // 取得被定義的 SYMBOL
        if ( (parent->right->left->type != SYMBOL && parent->right->left->type != "QUOTE_DATA")  
            || symbol_set.find(key) != symbol_set.end() 
            || primitive_predicate.find(key) != primitive_predicate.end() 
            || basic_arithmetic.find(key) != basic_arithmetic.end() ) {
            // 若該 SYMBOL 為系統定義或不存在，則拋出錯誤
            cout << "ERROR (DEFINE format) : ";
            throw parent;
        }

        ASTNode *value = parent->right->right->left;
        checkExpression(&value->left, &value); // 檢查被定義的 SYMBOL 內的內容
        user_map[key] = value;    // 將定義過的 SYMBOL 加入 map 中
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( const char *msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }

    cout << key << " defined" << endl;
    define = true;
    return true;
}

void Symbol::userDefined(ASTNode **root) {
    /* 將 User Define 傳回並更新 AST */
    if ( (*root)->type == SYSTEM ) return; // 若該 SYMBOL 為系統定義過的，則不做任何處理
    // 若該 SYMBOL 為定義過的，則將其指向進當前 AST
    string key = (*root)->value;
    
    if ( !lambda && local_map.find(key) != local_map.end() )
        // 若該 SYMBOL 為當前區域定義過的，則將其指向進當前 AST
        *root = local_map[key]; // 將區域變數內容指向進當前 AST
    else if ( lambda && lambda_map.find(key) != lambda_map.end() ) 
        // 若該 SYMBOL 為當前 Lambda 定義過的，則將其指向進當前 AST
        *root = lambda_map[key]; // 將 Lambda 內容指向進當前 AST
    else 
        // 若該 SYMBOL 為全域變數定義過的，則將其指向進當前 AST
        *root = user_map[key]; // 將全域變數內容指向進當前 AST
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

bool Symbol::isCAR(ASTNode **root, ASTNode **parent) {
    /* 檢查 Symbol 是否為 CAR 並更新 AST */
    if ( (*root)->value != "car" ) return false;
    else if ( !parent || (*parent)->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 CAR function 功能，只留回傳功能
        (*root)->value = "#<procedure " + (*root)->value + ">";
        (*root)->type = SYSTEM;
        return true;
    }

    int arg = countFunctionArg(*parent);  // 計算參數個數

    try {
        if ( arg == 1 ) {
            // 若參數個數不為1，則拋出錯誤
            ASTNode *check_node = (*parent)->right->left;  // 分析參數
            checkExpression(&check_node->left, &check_node); // 檢查 Expression 內的內容
            if ( !check_node->left ) {
                // 若參數為 ATOM ，則報錯
                cout << "ERROR (car with incorrect argument type) : ";
                throw check_node;
            }
            *parent = check_node->left;  // 將結構內的第一筆資料作為回傳結果
        }
        else 
            throw std::runtime_error("ERROR (incorrect number of arguments) : car\n");
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( const char *msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }
    return true;
}

bool Symbol::isCDR(ASTNode **root, ASTNode **parent) {
    /* 檢查 Symbol 是否為 CDR 並更新 AST */
    if ( (*root)->value != "cdr" ) return false;
    else if ( !parent || (*parent)->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 CDR function 功能，只留回傳功能
        (*root)->value = "#<procedure " + (*root)->value + ">";
        (*root)->type = SYSTEM;
        return true;
    }

    int arg = countFunctionArg(*parent);   // 計算參數個數

    try {
        if ( arg == 1 ) {
            // 若參數個數不為1，則拋出錯誤
            ASTNode *check_node = (*parent)->right->left;  // 分析參數
            checkExpression(&check_node->left, &check_node); // 檢查 Expression 內的內容
            if ( !check_node->right ) {
                // 若左子樹為空，則報錯
                cout << "ERROR (cdr with incorrect argument type) : ";
                throw check_node;
            }
            *parent = check_node->right;  // 將 list 的右子樹做為 CDR 的輸出結果
        }
        else
            throw std::runtime_error("ERROR (incorrect number of arguments) : cdr\n");
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( const char *msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }

    return true;
    
}

bool Symbol::checkPrimitivePredicate(ASTNode *root, ASTNode *parent) {
    /* 檢查該 Symbol 是否為原始判斷式 */
    if ( primitive_predicate.find(root->value) == primitive_predicate.end() ) return false;
    else if ( !parent || parent->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 PrimitivePredicate function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        root->type = SYSTEM;
        return true;
    }

    int arg = countFunctionArg(parent);   // 計算參數個數

    try {
        if ( arg == 1 ) {
            // 若參數個數不為1，則拋出錯誤
            ASTNode *check_node = parent->right->left;  // 該位置應該為需確認的 S-expression
            checkExpression(&check_node->left, &check_node); // 檢查 Expression 內的內容
            copyAndLink(&check_node);
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
            
            parent->type = BOOL;
            parent->value = checkPrimitive ? "#t" : "nil"; // 若為原始判斷式，則將其轉為 #t 或 nil
            // 只將結果留下來，並刪除原本的 S-expression
            parent->left = NULL;
            parent->right = NULL;
        }
        else 
            throw std::runtime_error("ERROR (incorrect number of arguments) : " + root->value + "\n");
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( const char *msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }
    
    return true;
}

bool Symbol::isAtom(ASTNode *root) {
    /* 檢查該AST是否為 Atom */
    if ( !root->left && !root->right ) return true;
    return false;
}

bool Symbol::isPair(ASTNode *root) {
    /* 檢查該AST是否為 Pair */
    ASTNode *temp = root;
    if ( root->left && root->right )
        // 若為 Pair ，左右子樹皆不為空
        return true;
    return false;
}

bool Symbol::isList(ASTNode *root) {
    /* 檢查該AST是否為List */
    ASTNode *temp = root;
    while ( temp->type != END && temp->value != "nil" ) {
        if ( temp->type != LEFT_PAREN && temp->type != LINK ) 
            return false; // 若該AST並不為List，則返回false
        temp = temp->right;
    }
    return true;

}

bool Symbol::isNull(ASTNode *root) {
    /* 檢查該 AST 是否為 NIL */
    if ( root->type == NIL || root->type == END || (root->type == BOOL && root->value == "nil") ) return true;
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
    if ( root->type == BOOL || root->type == NIL || root->type == END || root->type == T ) return true;
    return false;
}

bool Symbol::isSymbol(ASTNode *root) {
    /* 檢查該 AST 是否為 Symbol */
    if ( root->type == SYMBOL || root->type == "QUOTE_DATA" ) return true;
    return false;
}

bool Symbol::isArithmetic(ASTNode *root, ASTNode **parent) {
    /* 檢查該 Symbol 是否為基礎運算式或比較運算式 */
    
    if ( basic_arithmetic.find(root->value) == basic_arithmetic.end() || 
    (root->value.size() > 14 && basic_arithmetic.find(root->value.substr(13, root->value.size() - 1)) == basic_arithmetic.end()) ) return false;
    else if ( !parent || (*parent)->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 Arithmetic function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        root->type = SYSTEM;
        return true;
    }

    int arg = countFunctionArg(*parent);   // 計算參數個數

    try {
        if ( (arg == 1 && root->value == "not") || (arg >= 2 && root->value != "not") ) {
            // 若為 not 運算式，參數個數需為 1，否則為基礎運算式，參數個數需大於等於 2
            ASTNode *count_node = (*parent)->right, *result = new ASTNode();  // 該位置應該為需計算的 S-expression
            string compare_result = "";
            bool first = false, compare = false;
            while ( count_node->type != END ) {
                checkExpression(&count_node->left->left, &count_node->left); // 檢查 Expression 內的內容
                // 計算結果
                calculate(count_node, &result, root->value, first);
                if ( (result->value == "nil" || result->value == "#t") && root->value != "or" && root->value != "and" ) {
                    // 若結果為 nil 或 #t，則該 function 為比較運算式
                    compare = true;
                    if ( result->value == "nil" )
                        // 若結果為 nil，則將比較結果設為 false
                        compare_result = "nil";
                    else if ( compare_result != "nil" )
                        // 若直到目前比較結果皆不為 nil，則將比較結果設為 true
                        compare_result = "#t";
                    result = count_node->left; // 將當前數字儲存
                }
                else if ( root->value == "or" && result->value != "nil" ) break; // 若為 or 運算式，則將結果不為 false 退出
                else if ( root->value == "and" && result->value == "nil" ) break; // 若為 and 運算式，則將結果為 false 退出
                count_node = count_node->right;  // 接續下一個數字
            }
            if ( compare ) {
                copyAndLink(&result); // 將該自定義結構複製
                result->value = compare_result; // 若為比較運算式，則將結果設為比較結果
            }
            
            // 將結果寫回根節點
            if ( atom.isINT(result->value) ) {
                // 若結果為整數，則將其轉為整數
                (*parent)->type = INT;
                (*parent)->value = result->value;
            }
            else if ( atom.isFLOAT(result->value) ) {
                // 若結果為浮點數，則將其轉為浮點數
                (*parent)->type = FLOAT;
                (*parent)->value = result->value;
            }
            else if ( atom.isSTRING(result->value) ) {
                // 若結果為字串，則將其轉為字串
                (*parent)->type = STRING;
                (*parent)->value = result->value;
            }
            else if ( atom.isT(result->value) || atom.isNIL(result->value) ) {
                // 若結果為 nil 或 #t，則將其轉為布林值
                (*parent)->type = BOOL;
                (*parent)->value = result->value;
            }
            else {
                *parent = result; // 若結果為其他型別，則將其轉為其他型別
                return true;
            }
            // 只將結果留下來，並刪除原本的 S-expression
            (*parent)->left = NULL;
            (*parent)->right = NULL;
        }
        else
            throw std::runtime_error("ERROR (incorrect number of arguments) : " + root->value + "\n");
    }
    catch ( const char *msg ) {
        throw msg;
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        ASTNode *temp2 = (*parent)->right;
        while ( temp2->type != END ) {
            // 將原本的 S-expression 內容刪除
            if ( temp2->left == temp ) {
                cout << "ERROR (" + root->value + " with incorrect argument type) : "; // 找出同階層的 Function，並以此作為錯誤輸出
                break;
            }
            temp2 = temp2->right;
        }
        throw temp;
    }
    
    return true;
}

void Symbol::calculate(ASTNode *root, ASTNode **result, string &operand, bool &first) {
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
        *result = root->left; // 將當前資料儲存
        if ( operand == "not" ) {
            ASTNode *temp = new ASTNode();
            temp->type = BOOL;
            // 若為 not，則將結果轉為布林值
            temp->value = ( (*result)->value == "nil" || (*result)->type == END ) ? "#t" : "nil";
            *result = temp; // 將結果設為布林值 
        }
        first = true; // 設定為已寫入第一筆資料
        return;
    }

    ASTNode *operate = root->left;
    copyAndLink(result); // 將當前資料複製一份，以避免覆蓋
    if ( operand == "+" ) {
        if ( atom.isINT(operate->value) && atom.isINT((*result)->value) ) 
            // 若兩者皆為整數，則將其轉為整數
            (*result)->value = to_string(stoi((*result)->value) + stoi(operate->value));
        else 
            // 若兩者皆為浮點數，則將其轉為浮點數
            (*result)->value = to_string(stod((*result)->value) + stod(operate->value));
    }
        
    else if ( operand == "-" ) {
        if ( atom.isINT(operate->value) && atom.isINT((*result)->value) ) 
            (*result)->value = to_string(stoi((*result)->value) - stoi(operate->value));
        else 
            (*result)->value = to_string(stod((*result)->value) - stod(operate->value));
    }
    else if ( operand == "*" ) {
        if ( atom.isINT(operate->value) && atom.isINT((*result)->value) ) 
            (*result)->value = to_string(stoi((*result)->value) * stoi(operate->value));
        else 
            (*result)->value = to_string(stod((*result)->value) * stod(operate->value));
    }
    else if ( operand == "/" ) {
        if ( stod(operate->value) == 0 ) {
            // 若除數為0，則拋出錯誤
            throw std::runtime_error("ERROR (division by zero) : /\n");
        }
        if ( atom.isINT(operate->value) && atom.isINT((*result)->value) ) 
            (*result)->value = to_string(stoi((*result)->value) / stoi(operate->value));
        else
            (*result)->value = to_string(stod((*result)->value) / stod(operate->value));
    }
    else if ( operand == "and" && (*result)->value != "nil" ) {
        if ( operate->type == END ) {
            // 若為 END ，則需將其轉為 nil
            (*result)->type = BOOL;
            (*result)->value = "nil";
        }
        else *result = operate;
    }
    else if ( operand == "or" && (*result)->value == "nil" ) {
        if ( operate->type == END ) {
            // 若為 END ，則需將其轉為 nil
            (*result)->type = BOOL;
            (*result)->value = "nil";
        }
        else *result = operate;
    }  
    else if ( operand == ">" )
        (*result)->value = ( stod((*result)->value) > stod(operate->value) ) ? "#t" : "nil";
    else if ( operand == "<" )
        (*result)->value = ( stod((*result)->value) < stod(operate->value) ) ? "#t" : "nil";
    else if ( operand == ">=" ) 
        (*result)->value = ( stod((*result)->value) >= stod(operate->value) ) ? "#t" : "nil";
    else if ( operand == "<=" ) 
        (*result)->value = ( stod((*result)->value) <= stod(operate->value) ) ? "#t" : "nil";
    else if ( operand == "=" ) 
        (*result)->value = ( stod((*result)->value) == stod(operate->value) ) ? "#t" : "nil";
    else if ( operand == "string-append" ) 
        // 若為字串相加，則將其轉為字串
        (*result)->value = (*result)->value.substr(0, (*result)->value.size() - 1) + operate->value.substr(1, operate->value.size() - 1); // 去除字串的引號
    else if ( operand == "string>?" ) 
        // 若為字串比較，則將其轉為布林值
        (*result)->value = ( (*result)->value > operate->value ) ? "#t" : "nil";
    else if ( operand == "string<?" ) 
        (*result)->value = ( (*result)->value < operate->value ) ? "#t" : "nil";
    else if ( operand == "string=?" ) 
        (*result)->value = ( (*result)->value == operate->value ) ? "#t" : "nil";
}

bool Symbol::isEqual(ASTNode *root, ASTNode *parent) {
    /* 檢查該 AST 是否為 equal */
    if ( root->value != "eqv?" && root->value != "equal?" ) return false;
    else if ( !parent || parent->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 EQUAL function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        root->type = SYSTEM;
        return true;
    }

    int arg = countFunctionArg(parent);   // 計算參數個數

    try {
        if ( arg == 2 ) {
            // 若參數個數不為2，則拋出錯誤
            ASTNode *first_element = parent->right->left, *second_element = parent->right->right->left;
            checkExpression(&first_element->left, &first_element); // 檢查 Expression 內的內容
            checkExpression(&second_element->left, &second_element); // 檢查 Expression 內的內容
            bool eqv = false, equal = false;
            if ( first_element == second_element ) eqv = true; // 若兩者皆指向相同記憶體位置， eqv 為 true
            else if ( first_element->type == STRING || second_element->type == STRING ) eqv = false; // 若有一者為字串，則 eqv 為 false
            else if ( isAtom(first_element) && isAtom(second_element) ) {
                if ( first_element->value == second_element->value ) eqv = true;
                else if ( isRealOrNumber(first_element) && isRealOrNumber(second_element) ) {
                    // 若兩者皆為數字，則將其轉為浮點數比較
                    if ( stod(first_element->value) == stod(second_element->value) ) eqv = true;
                }
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
            parent->left = NULL;
            parent->right = NULL;
        }
        else
            throw std::runtime_error("ERROR (incorrect number of arguments) : " + root->value + "\n");
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( const char *msg ) {
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

    if ( (first_element->type == END || first_element->value == "nil" ) && (second_element->type == END || second_element->value == "nil") ) return true;
    else if ( (first_element->type == LINK || first_element->type == LEFT_PAREN ) && (second_element->type == LINK || second_element->type == LEFT_PAREN) )
        // 若為連接用節點，則認為相同
        return campare(first_element->left, second_element->left) && campare(first_element->right, second_element->right);
    else if ( first_element->value != second_element->value ) {
        // 若為數字需額外處理
        if ( isRealOrNumber(first_element) && isRealOrNumber(second_element) ) {
            // 若兩者皆為數字，則將其轉為浮點數比較
            if ( stod(first_element->value) == stod(second_element->value) ) return true;
        }
        return false;
    }
    return campare(first_element->left, second_element->left) && campare(first_element->right, second_element->right);
}

bool Symbol::isIf(ASTNode *root, ASTNode **parent) {
    /* 檢查 Symbol 是否為 IF 並更新 AST */
    if ( root->value != "if" ) return false;
    else if ( !parent || (*parent)->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 IF function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        root->type = SYSTEM;
        return true;
    }

    int arg = countFunctionArg(*parent);   // 計算參數個數

    try {
        if ( arg == 2 || arg == 3 ) {
            // 若參數個數不為 2 或 3 ，則拋出錯誤
            ++return_value; // 計算回傳值 Function 個數
            ASTNode *check_node = (*parent)->right, *temp_root = new ASTNode() ;  // 該位置應該為需運行的 S-expression
            copyTree(temp_root, *parent); // 將該 S-expression 複製一份，報錯時使用
            conditional(&check_node); // 運行判別式
            --return_value;
            if ( !check_node && !return_value ) {
                // 判別結果無資料，則報錯
                cout << "ERROR (no return value) : ";
                throw temp_root;
            }
            *parent = check_node; // 將判別結果設為父節點
        }
        else
            throw std::runtime_error("ERROR (incorrect number of arguments) : if\n");
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( const char *msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }

    return true;
}

bool Symbol::isCond(ASTNode *root, ASTNode **parent) {
    /* 檢查 Symbol 是否為 COND 並更新 AST */
    if ( root->value != "cond" ) return false;
    else if ( !parent || (*parent)->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 COND function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        root->type = SYSTEM;
        return true;
    }

    int arg = countFunctionArg(*parent);   // 計算參數個數
    if ( arg < 1 ) {
        // 若參數個數不為1，則拋出錯誤
        cout << "ERROR (COND format) : ";
        throw *parent;
    }

    try {
        ASTNode *check_node = (*parent)->right, *temp_root = new ASTNode(), *result_statement = NULL;  // 該位置應該為需運行的 S-expression
        copyTree(temp_root, *parent); // 將該 S-expression 複製一份，報錯時使用
        ++return_value; // 計算回傳值 Function 個數

        while ( check_node->type != END ) {
            // 檢查 S-expression 內的每個左子樹
            if ( check_node->left->type != LEFT_PAREN ) {
                // 若左子樹不為 S-expression，則拋出錯誤
                cout << "ERROR (COND format) : ";
                throw temp_root;
            }
            else {
                ASTNode *temp = check_node->left;
                if ( temp->right->type == END ) {
                    // 若該判斷式內沒有右子樹，則拋出錯誤
                    cout << "ERROR (COND format) : ";
                    throw temp_root;
                }
                else if ( !isList(temp) ) {
                    // 若該判斷式具備右節點，則不符合格式
                    cout << "ERROR (COND format) : ";
                    throw temp_root;
                }
            }
            check_node = check_node->right; // 接續下一個判斷式
        }

        check_node = (*parent)->right;
        while ( check_node->type != END ) {
            ASTNode *temp = check_node->left->right; // 該位置為該判對式的結果
            if ( check_node->right->type == END && check_node->left->left->value == "else" ) {
                // 若為 else
                result_statement = temp; // 將其設為結果
                break;
            }
            else {
                checkExpression(&check_node->left->left->left, &check_node->left->left); // 檢查判斷式內的內容
                if ( !isNull(check_node->left->left) ) {
                    // 若判別結果不為 nil
                    result_statement = temp; // 將其設為結果
                    break;
                }

            }
            check_node = check_node->right; // 接續下一個 S-expression
        }

        ASTNode *temp = result_statement;
        while ( temp && temp->type != END  ) {
            // 檢查 S-expression 內的每個左子樹
            checkExpression(&temp->left->left, &temp->left); // 檢查 Expression 內的內容
            if ( temp->right->type == END )
                // 若為最後一筆 S-expression，則將其設為結果
                result_statement = temp->left;
            temp = temp->right; // 接續下一個 S-expression
        }
        --return_value;

        if ( !result_statement && !return_value ) {
            // 判別結果無資料，則報錯
            cout << "ERROR (no return value) : ";
            throw temp_root;
        }
        // 將結果寫回根節點
        *parent = result_statement; // 將判別結果設為父節點
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( const char *msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }
    return true;
}

void Symbol::conditional(ASTNode **root) {
    /* 檢查該是否回傳 statemet */
    ASTNode *check_node = (*root)->left;  // 該位置應該為判別式

    try {
        checkExpression(&check_node->left, &check_node); // 檢查 Expression 內的內容
        bool conditional = true;
        if ( check_node->value == "nil" || check_node->type == END )
            // 若條件為 nil，則將 conditional 設為 false
            conditional = false;

        ASTNode *first_statement = (*root)->right->left, *second_statement = (*root)->right->right->left;
        if ( conditional ) {
            // 若條件為 true，則將 S-expression 的左子樹設為第一筆 S-expression
            check_node = first_statement;
            checkExpression(&check_node->left, &check_node); // 檢查 Expression 內的內容
            (*root) = check_node;
        }
        else {
            if ( !second_statement )
                // 第二筆 S-expression 為空，則回傳空值
                (*root) = NULL;
            else {
                // 若條件為 false，則將 S-expression 的左子樹設為第二筆 S-expression
                check_node = second_statement;
                checkExpression(&check_node->left, &check_node); // 檢查 Expression 內的內容
                (*root) = check_node;
            }
        }
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( const char *msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }
    
}

bool Symbol::isBegin(ASTNode *root, ASTNode **parent) {
    /* 檢查 Symbol 是否為 BEGIN 並更新 AST */
    if ( root->value != "begin" ) return false;
    else if ( !parent || (*parent)->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 BEGIN function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        root->type = SYSTEM;
        return true;
    }

    int arg = countFunctionArg(*parent);   // 計算參數個數

    if ( arg < 1 ) {
        // 若參數個數不為1，則拋出錯誤
        string error_str = "ERROR (incorrect number of arguments) : begin\n";
        throw std::runtime_error(error_str);
    }

    try {
        ASTNode *check_node = (*parent)->right, *temp_root = new ASTNode();  // 該位置應該為需運行的 S-expression
        copyTree(temp_root, *parent); // 將該 S-expression 複製一份，報錯時使用
        ++return_value; // 計算回傳值 Function 個數

        while ( check_node->type != END ) {
            // 檢查 S-expression 內的每個左子樹
            checkExpression(&check_node->left->left, &check_node->left); // 檢查 Expression 內的內容

            if ( check_node->right->type == END ) 
                // 若為最後一筆 S-expression，則將其設為結果
                *parent = check_node->left;

            check_node = check_node->right;  // 接續下一個 S-expression
        }
        --return_value;
        if ( !(*parent) && !return_value ) {
            // 判別結果無資料，則報錯
            cout << "ERROR (no return value) : ";
            throw temp_root;
        }
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( const char *msg ) {
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
        root->type = SYSTEM;
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

void Symbol::procedure(ASTNode **root, ASTNode *parent) {
    /* 若為自定義 Symbol 函數，且內部資料為 Procedure 系統函數，則需進行轉換 */
    if ( user_map.find((*root)->value) != user_map.end() ) {
        if ( user_map[(*root)->value]->type == SYSTEM ) {
            if ( parent && parent->type == LEFT_PAREN ) {
                // 若父節點為 LEFT_PAREN ，則運行 procedure function 功能
                (*root)->value = user_map[(*root)->value]->value.substr(12, user_map[(*root)->value]->value.size() - 13); // 去除 <procedure 和 >
            }
            else {
                (*root)->value = user_map[(*root)->value]->value; // 若父節點不為 LEFT_PAREN ，則不做 procedure function 功能，只留回傳功能
                (*root)->type = SYSTEM; // 將其設為系統函數
            }
        }
    }
    else if ( (*root)->type == SYSTEM ) {
        if ( parent && parent->type == LEFT_PAREN ) {
            // 若父節點為 LEFT_PAREN ，則運行 procedure function 功能
            ASTNode *temp = new ASTNode();
            copyTree(temp, (*root)); // 將該 S-expression 複製一份，避免導致記憶體覆蓋
            *root = temp;
            (*root)->value = (*root)->value.substr(12, (*root)->value.size() - 13); // 去除 <procedure 和 >
        }
    }
}

bool Symbol::isExit(ASTNode *root, ASTNode *parent, int deep) {

    if ( root->value != "exit" ) return false;
    else if ( !parent || parent->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 DEFINE function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        root->type = SYSTEM;
        return true;
    }
    else if ( deep != 0 ) 
        // 若深度不為0，則不做 EXIT function 功能，只留回傳功能
        throw std::runtime_error("ERROR (level of EXIT)\n");
    int arg = countFunctionArg(parent);   // 計算參數個數

    if ( arg > 0 ) throw std::runtime_error("ERROR (incorrect number of arguments) : exit\n");
    else
        throw "Exit"; // 若深度為0，則拋出，並結束程式

    return true;
}

bool Symbol::isLet(ASTNode *root, ASTNode **parent) {
    /* 檢查 Symbol 是否為 LET 並更新 AST */
    if ( root->value != "let" ) return false;
    else if ( !parent || (*parent)->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 LET function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        root->type = SYSTEM;
        return true;
    }

    int arg = countFunctionArg(*parent);   // 計算參數個數

    try {
        if ( arg < 2 ) {
            // 若參數個數不為 2，則拋出錯誤
            cout << "ERROR (LET format) : ";
            throw *parent;
        }
        local_map.clear(); // 清除區域變數庫
        localDefine((*parent)->right->left); // 將區域變數寫入區域變數庫

        ASTNode *work_node = (*parent)->right->right; // 該位置應該為需運行的 S-expression
        while ( work_node->type != END ) {
            // 檢查並執行每個 S-expression
            checkExpression(&work_node->left->left, &work_node->left); // 檢查 Expression 內的內容
            if ( work_node->right->type == END )
                *parent = work_node->left; // 若為最後一筆 S-expression，則將其設為結果
            
            work_node = work_node->right; // 接續下一個 S-expression
        }
        local_map.clear(); // 清除區域變數庫
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( const char *msg ) {
        if ( msg == "ERROR (LET format) : " ) {
            // 若為 LET 格式錯誤，則拋出錯誤
            cout << msg;
            throw *parent;
        }
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }

    return true;
}

void Symbol::localDefine(ASTNode *root) {
    /* 將區域變數寫入區域變數庫 */

    try {
        while ( root->type != END && root->value != "nil" ) {
            if ( root->type != LEFT_PAREN && root->type != LINK )
                // 若該定義結構有右子樹，則拋出錯誤
                throw "ERROR (LET format) : ";

            ASTNode *define_node = root->left;
            if ( define_node->type != LEFT_PAREN )
                // 若左子樹不為 S-expression，則拋出錯誤
                throw "ERROR (LET format) : ";
            else if ( !isList(define_node) )
                // 若該判斷式具備右節點，則不符合格式
                throw "ERROR (LET format) : ";
    
            string key = define_node->left->value; // 取得被定義的 SYMBOL
            if ( (define_node->left->type != SYMBOL && define_node->left->type != "QUOTE_DATA")  
                || symbol_set.find(key) != symbol_set.end() 
                || primitive_predicate.find(key) != primitive_predicate.end() 
                || basic_arithmetic.find(key) != basic_arithmetic.end() )
                // 若該 SYMBOL 為系統定義或不存在，則拋出錯誤
                throw "ERROR (LET format) : ";

            ASTNode *value = define_node->right->left; // 取得被定義的 SYMBOL 的值
            if ( define_node->right->right->type != END )
                // 若該定義資料並非只有一個 S-expression，則拋出錯誤
                throw "ERROR (LET format) : ";
            checkExpression(&value->left, &value); // 檢查被定義的 SYMBOL 內的內容
            local_map[key] = value;    // 將定義過的 SYMBOL 加入 map 中

            root = root->right; // 接續下一個被定義的 SYMBOL
        }
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( const char *msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }

}

bool Symbol::isLambda(ASTNode *root, ASTNode **parent) {
    /* 檢查 Symbol 是否為 LAMBDA 並更新 AST */
    if ( root->value != "lambda" ) return false;
    else if ( !parent || (*parent)->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 LAMBDA function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        root->type = SYSTEM;
        return true;
    }

    int arg = countFunctionArg(*parent);   // 計算參數個數

    try {
        if ( lambda_tree ) {
            // 若 lambda_tree 存在，依照其 Function 回傳內容
            lambda = true; // 設定 lambda 標記
            *parent = useLambda((*parent)->right);
            lambda = false; // 清除 lambda 標記
            lambda_map.clear(); // 清除 lambda_map
        }
        else if ( arg < 2 ) {
            // 若參數個數小於 2，則拋出錯誤
            cout << "ERROR (LAMBDA format) : ";
            throw *parent;
        }
        else {
            // 創建 LAMBDA 樹，以供後續參數使用
            checkLambda((*parent)->right); // 檢查 LAMBDA 的格式
            (*parent)->value = "#<procedure " + root->value + ">";
            (*parent)->type = SYSTEM; // 將其設為系統函數
            (*parent)->left = NULL;
            (*parent)->right = NULL;
        }
            
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( const char *msg ) {
        if ( msg == "ERROR (LAMBDA format) : " ) {
            // 若為 LAMBDA 格式錯誤，則拋出錯誤
            cout << msg;
            throw *parent;
        }
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }

    return true;
}

void Symbol::checkLambda(ASTNode *root) {
    /* 檢查 LAMBDA 的格式 */
    ASTNode *define_node = root->left;
    if ( root->left->type != LEFT_PAREN && root->left->value != "nil" )
        // 若該定義參數位置不為該結構，則拋出錯誤
        throw "ERROR (LAMBDA format) : ";
    while ( define_node->type != END && define_node->value != "nil" ) {
        // 檢查 S-expression 內的每個左子樹
        string key = define_node->left->value; // 取得被定義的 SYMBOL
        if ( (define_node->left->type != SYMBOL && define_node->left->type != "QUOTE_DATA")  
                || symbol_set.find(key) != symbol_set.end() 
                || primitive_predicate.find(key) != primitive_predicate.end() 
                || basic_arithmetic.find(key) != basic_arithmetic.end() )
                // 若該 SYMBOL 為系統定義或不存在，則拋出錯誤
            throw "ERROR (LAMBDA format) : ";
        define_node = define_node->right; // 接續下一個被定義的 SYMBOL
    }
    lambda_tree = root;
}

ASTNode* Symbol::useLambda(ASTNode *root) {
    /* 依照參數回傳相應結果 */
    ASTNode *arg_node = root, *check_node = lambda_tree->left;

    try {
        while ( check_node->type != END && check_node->value != "nil" ) {
            // 檢查 S-expression 內的每個左子樹
            string key = check_node->left->value; // 取得被定義的 SYMBOL

            if ( arg_node->type == END )
                // 若該定輸入資料不足，則拋出錯誤
                throw std::runtime_error("ERROR (incorrect number of arguments) : lambda\n");
            ASTNode *value = arg_node->left; // 取得被定義的 SYMBOL 的值
            
            lambda_map[key] = value;    // 將定義過的 SYMBOL 加入 map 中
    
            check_node = check_node->right; // 接續下一個被定義的 SYMBOL
            arg_node = arg_node->right; // 接續下一個被定義內容
        }
        if ( arg_node->type != END )
            // 若該定輸入資料過多，則拋出錯誤
            throw std::runtime_error("ERROR (incorrect number of arguments) : lambda\n");

        ASTNode *function_node = lambda_tree->right; // 需運行的 S-expression
        while ( function_node->type != END ) {
            // 檢查 S-expression 內的每個左子樹
            checkExpression(&function_node->left->left, &function_node->left); // 檢查 Expression 內的內容
            if ( function_node->right->type == END )
                // 若為最後一筆 S-expression，則將其設為結果
                return function_node->left;
            function_node = function_node->right; // 接續下一個 S-expression
        }
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( const char *msg ) {
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }
    
    return NULL; // 若無法回傳，則回傳 NULL
}