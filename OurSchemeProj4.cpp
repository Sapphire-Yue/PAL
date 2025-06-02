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
#define ERROR "ERROR"

#define TEMP "TEMP"
#define BEGIN "BEGIN"
#define END "END"
#define LINK "LINK"

unordered_set<string> symbol_set{"cons", "quote", "list", "define", "car", "cdr", "eqv?", "equal?", "if", "cond", "begin", "clean-environment", "exit", "let", "lambda", "read", "write", "display-string", "newline", "symbol->string", "number->string", "eval", "set!"}, 
                    primitive_predicate{"atom?", "pair?", "list?", "null?", "integer?", "real?", "number?", "string?", "boolean?", "symbol?", "error-object?"},
                    basic_arithmetic{"+" , "-" , "*" , "/", "not", "and", "or", ">", ">=", "<", "<=", "=", "string-append", "string>?", "string<?", "string=?"};

struct ASTNode {
    string type, value;
    ASTNode *left;
    ASTNode *right;
};

unordered_map<string, ASTNode*> user_map{}, user_function_map{}; // 使用者定義的變數與函數

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
        void checkSymbol(ASTNode *cur, bool non_function, stack<pair<unordered_map<string, ASTNode*>, bool>> local_map);
};

class Symbol {
    public:
        Symbol();
        ~Symbol();
        bool &getDefine() { return define; };
        int &getRead() { return read; };
        queue<ASTNode*> &getReadQueue() { return read_queue; };
        void symbolCheck(ASTNode **root, ASTNode **parent, int deep);
        int countFunctionArg(ASTNode *root);
        bool isCONS(ASTNode *root, ASTNode *parent);
        bool isQUOTE(ASTNode *root, ASTNode **parent);
        bool isList(ASTNode *root, ASTNode *parent);
        bool isUserDefine(ASTNode *root, ASTNode *parent, int deep);
        void userDefined(ASTNode **root, ASTNode **parent);
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
        bool isErrorObject(ASTNode *root);
        bool isArithmetic(ASTNode *root, ASTNode **parent);
        bool isEqual(ASTNode *root, ASTNode *parent);
        bool isIf(ASTNode *root, ASTNode **parent);
        bool isCond(ASTNode *root, ASTNode **parent);
        bool isBegin(ASTNode *root, ASTNode **parent);
        bool isCleanEnvironment(ASTNode *root, ASTNode *parent, int deep);
        bool isExit(ASTNode *root, ASTNode *parent, int deep);
        bool isLet(ASTNode *root, ASTNode **parent);
        bool isLambda(ASTNode *root, ASTNode **parent);
        bool isLambdaArg(ASTNode *root, ASTNode **parent);
        void isLambdaProcedure(ASTNode *root, ASTNode **parent);
        bool isRead(ASTNode *root, ASTNode **parent);
        bool isWrite(ASTNode *root, ASTNode **parent);
        bool isChangeToString(ASTNode *root, ASTNode **parent);
        bool isEval(ASTNode *root, ASTNode **parent);
        bool isSet(ASTNode *root, ASTNode **parent);
    private:
        Atom atom;
        Error error;
        stack<pair<unordered_map<string, ASTNode*>, bool>> local_map; // 區域變數庫
        bool define, eval;
        int read;
        queue<ASTNode*> read_queue; // 用於處理讀取的 ASTNode 指標
        unordered_map<string, ASTNode*> lambda_tree;
        void checkExpression(ASTNode **root, ASTNode **parent);
        void copyAndLink(ASTNode **root);
        void calculate(ASTNode *root, ASTNode **result, string &operand, bool &first);
        bool campare(ASTNode *first_element, ASTNode *second_element);
        void conditional(ASTNode **root);
        void procedure(ASTNode **root, ASTNode *parent);
        void localDefine(ASTNode *root);
        void checkLambda(ASTNode *root);
        ASTNode* useLambda(ASTNode *root, string &key);
        void defineFunction(string &key, ASTNode *parent);
        void writeFunction(ASTNode *root);
};

class Reader; // 前置宣告，因 Function 需要使用 Reader

class Function {
    public:
        Function();
        ~Function();
        ASTNode **getRoot() { return &root; };
        bool getDot() { return dot; };
        int getQuote() { return quote; };
        int getRead() { return read; };
        stack<ASTNode*> &getAST() { return ast; };
        Atom &getAtom() { return atom; };
        void newAST();
        void checkTypeOfAtom(string &atom, string &type);
        void storeTokenInAST(string &token, int &line, int &column, Reader &reader, Symbol &symbol);
        void checkTokenToStore(string &token, int &line, int &column, char &sep, Reader &reader, Symbol &symbol);
        void commonStore(string &token, string &type);
        void newExpression(string &token, int &line, int &column);
        void backExpression();
        void quoteExpression(string &token, int &line, int &column);
        bool checkExit();
    private:
        ASTNode *root;
        ASTNode *cur;
        stack<ASTNode*> ast;
        ASTNode *read_tree; // 用於讀取的 ASTNode 樹
        bool dot;
        int quote;
        Atom atom;
        Error error;
        string input;
        int read;
        queue<ASTNode**> read_queue; // 用於處理讀取的 ASTNode 指標
};

class Separator; // 前置宣告，因 Reader 需要使用 Separator

class Reader {
    public:
        Reader(Function &function);
        ~Reader();
        bool readNextLine(string &expr, int &line, int &nil, Separator &separator, Function &function, Symbol &symbol);
        void saperateExprToToken(string &expr, int &line, int &nil, Separator &separator, Function &function, Symbol &symbol);
    private:
        int left_paren;
        Function &function;
};

class Separator {
    public:
        Separator(Function &function);
        ~Separator();
        void leftParen(string &token, int &line, int &column);
        void rightParen(string &token, int &nil, int &left_paren, int &line, int &column, Reader &reader, Symbol &symbol);
        void singleQuote(string &token, int &line, int &column, Reader &reader, Symbol &symbol);
        void doubleQuote(string &token, int &line, int &column, bool &str, bool &backslash, char &sep, Reader &reader, Symbol &symbol);
    private:
        Function &function;
};

void interface(Reader &reader, Separator &separator, Function &function, Symbol &symbol);
void optimizeAST(Reader &reader, Function &function, Symbol &symbol);
bool printAST(Function &function, Symbol &symbol);

int main() {
    //string num;   // 測資題號
    //getline(cin, num);
    Function function;
    Reader reader(function);
    Separator separator(function); // 用於分割 S-expression 的 Separator

    Function function2; // 用於優化 AST 的 Function
    Reader reader2(function2);
    Symbol symbol;

    interface(reader, separator, function, symbol); // 讀取使用者輸入的 S-expression
    return 0;
}

Reader::Reader(Function &function) : function(function) {
    cout << "Reader created" << endl;
    left_paren = 0; // 預設左括弧為 0
}

Reader::~Reader() {
    cout << "Reader destroyed" << endl;
}

void interface(Reader &reader, Separator &separator, Function &function, Symbol &symbol) {

    cout << "Welcome to OurScheme!" << endl;
    cout << "\n> ";
    string expr;
    int line = 0, nil = 0;    // 紀錄行數

    while ( true ) {
        // 重複讀取下一個S-expression
        try {
            if ( !reader.readNextLine(expr, line, nil, separator, function, symbol) ) break;   // 讀取到exit結束
        }
        catch ( const char *msg ) {
            cerr << msg;
            break;  // 讀取到EOF結束
        }
    }

    cout << "\nThanks for using OurScheme!" << endl;
    cout.flush(); // 避免緩衝區輸出並未顯示
}

bool Reader::readNextLine(string &expr, int &line, int &nil, Separator &separator, Function &function, Symbol &symbol) {
    /*  讀取下一行輸入的資料
        若遇到EOF則拋出例外 */

    if ( !getline(cin, expr) ) throw "ERROR (no more input) : END-OF-FILE encountered"; // 讀到EOF
    ++line;    // 紀錄行數

    cout << "You entered: " << expr << " " << endl;    // debug
    try {
        saperateExprToToken(expr, line, nil, separator, function, symbol);  // 將輸入的S-expression分割成Token
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

void Reader::saperateExprToToken(string &expr, int &line, int &nil, Separator &separator, Function &function, Symbol &symbol) {
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
                function.checkTokenToStore(token, line, column, c, *this, symbol);
                separator.leftParen(token, line, column);
            }
            catch ( const std::runtime_error &msg ) {
                throw msg;
            }
            nil = 0;
        }
        else if ( !str && c == ')' ) {
            --left_paren;
            try {
                function.checkTokenToStore(token, line, column, c, *this, symbol);
                separator.rightParen(token, nil, left_paren, line, column, *this, symbol);
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
                function.checkTokenToStore(token, line, column, c, *this, symbol);
                separator.singleQuote(token, line, column, *this, symbol);
            }
            catch ( const std::runtime_error &msg ) {
                throw msg;
            }
        }
        else if ( !str && c == ' ' ) {
            // 遇到空白，將 token 存入 AST
            try {
                function.checkTokenToStore(token, line, column, c, *this, symbol);
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
                separator.doubleQuote(token, line, column, str, backslash, c, *this, symbol);
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
            function.storeTokenInAST(token, line, column, *this, symbol);
        }
        catch ( const std::runtime_error &msg ) {
            throw msg;
        }
        token.clear();
    }
    
    if ( left_paren || function.getQuote() ) return;    // 該 S-expression 未結束，繼續讀取下一行
    else if ( lineAreNotEmpty ) line = 0;    // 當 AST 樹輸出結束，或該行早已輸出結果，則須更新行數
    
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
    read = 0;
    read_tree = NULL;
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

void Function::storeTokenInAST(string &token, int &line, int &column, Reader &reader, Symbol &symbol) {
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
            optimizeAST(reader, *this, symbol); // 指令建構完成，則輸出 AST
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
                optimizeAST(reader, *this, symbol); // 指令建構完成，則輸出 AST
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

void Function::checkTokenToStore(string &token, int &line, int &column, char &sep, Reader &reader, Symbol &symbol) {
    /* 在 seperater 使用前，檢查 Token 是否蘊含先前讀入的資料
        若有，則將該資料存入 AST，無論如何，皆會將 Token 改為使其分割的 Seperator */
    if ( !token.empty() ) {
        try {
            storeTokenInAST(token, line, column, reader, symbol);
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

bool printAST(Function &function, Symbol &symbol) {
    /* 印出 AST */
    ASTNode *root = *function.getRoot();
    if ( root->type == BEGIN ) return false;    // 若無資料，則不需印出
    else if ( symbol.getDefine() ) {
        // 若有新的定義，則不須印出
        symbol.getDefine() = false;
        cout << "\n> ";
        function.newAST();
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
            if ( direction != "right" )
                cout << "nil" << endl;
            else 
                cout << ")" << endl;
        }
        else if ( !cur->left && !cur->right ) {
            if ( function.getAtom().isFLOAT(cur->value) ) function.getAtom().isFLOAT(cur->value, true); // 若為浮點數，則取小數點後三位
            cout << cur->value << endl;

            if ( direction == "right" ) {
                --left_paren;
                for ( int i = 0; i < left_paren; ++i ) cout << "  ";
                cout << ")" << endl; // 若該節點為右節點結尾，則印出右括弧
            }    
        }
    }

    cout << "\n> ";
    function.newAST();
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

void Separator::rightParen(string &token, int &nil, int &left_paren, int &line, int &column, Reader &reader, Symbol &symbol) {
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
            function.storeTokenInAST(token, line, column, reader, symbol);
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
            optimizeAST(reader, function, symbol); // 整理AST
        }
        catch ( const std::runtime_error &msg ) {
            throw msg;
        }
        catch ( const char *msg ) {
            // clean-environment 不做其他回傳
            if ( msg == "Exit" ) throw msg;
            function.newAST();
        }
        line = 0, column = 0; // 重新另一個S-expression
    }
    token.clear();

}

void Separator::singleQuote(string &token, int &line, int &column, Reader &reader, Symbol &symbol) {
    /*  遇到單引號，即內部為另一筆 S-expression 
        與左括弧相似，需建造樹的新母節點 */

    try {
        function.quoteExpression(token, line, column);
        function.storeTokenInAST(token, line, column, reader, symbol);
        token.clear();
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
}

void Separator::doubleQuote(string &token, int &line, int &column, bool &str, bool &backslash, char &sep, Reader &reader, Symbol &symbol) {
    /*  遇到雙引號，將字串存入AST
        若遇到反斜線則將雙引號公用消除，反之則該字串結束 */
    try {
        if ( !str ) function.checkTokenToStore(token, line, column, sep, reader, symbol);   // 若為雙引號的開頭，則將前面的 token 存入 AST

        if ( str && !backslash ) {
            // 若遇到反斜線則將雙引號公用消除，反之則該字串結束
            token += sep;
            function.storeTokenInAST(token, line, column, reader, symbol);
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

void optimizeAST(Reader &reader, Function &function, Symbol &symbol) {
    // 整理 AST
    while ( !function.getAST().empty() ) function.backExpression(); // 將所有 S-expression 跳出至最外層

    ASTNode *root = *function.getRoot(), *root_temp = new ASTNode();
    stack<pair<ASTNode**, ASTNode**>> st;
    stack<ASTNode*> error_node;
    st.push({&root, NULL});
    symbol = Symbol(); // 清空 Symbol 的定義
    symbol.copyTree(root_temp, root); // 將該自定義結構複製，報錯時使用

    if ( root->type == SYMBOL ) {
        // 若根節點為 SYMBOL ，則檢查是否存在
        try {
            symbol.symbolCheck(&root, NULL, 0); // 為單一 ATOM 時，直接輸出其 SYMBOL 的 value
            symbol.isLambdaProcedure(root, NULL); // 檢查是否為 lambda 式
            ASTNode *copy_temp = new ASTNode();
            symbol.copyTree(copy_temp, root); // 將該自定義結構複製
            root = copy_temp; // 給當前 AST 使用
        }
        catch ( const std::runtime_error &msg ) {
            cout << msg.what();
            cout << "\n> ";
            // 因錯誤而結束該S-expression，重新讀取下一筆 S-expression
            function.newAST();
            return;
        }
        catch ( const char *msg ) {
            throw msg;
        }
        catch ( ASTNode *temp ) {
            root = temp;
            *function.getRoot() = root; // 更新 AST 的根節點
            printAST(function, symbol);
            return;
        }
        *function.getRoot() = root; // 更新 AST 的根節點
        printAST(function, symbol);
        return;
    }

    if ( !symbol.isAtom(root) && !symbol.isList(root) ) {
        // 若該 AST 不為 List 結構，則報錯
        cout << "ERROR (non-list) : ";
        printAST(function, symbol);
        return;
    }
    while ( !st.empty() ) {
        ASTNode **cur = st.top().first;
        ASTNode **parent = st.top().second;
        st.pop();
        if ( !parent ) {
            while ( (*cur)->left != NULL ) {
                st.push({&(*cur)->left, cur});
                ASTNode *temp = new ASTNode();
                symbol.copyTree(temp, *cur); // 將該結構複製，報錯時使用
                error_node.push(temp);
                cur = &(*cur)->left;
            }
        }
        else {
            try {
                if ( !*cur ) {
                    // 若該節點指令為空，則報錯
                    cout << "ERROR (no return value) : ";
                    root = error_node.top(); // 報錯時使用的 AST
                    break;
                }
                symbol.symbolCheck(cur, parent, 0);
                if ( *parent ) {
                    ASTNode *copy_temp = new ASTNode();
                    symbol.copyTree(copy_temp, *parent); // 將該自定義結構複製
                    *parent = copy_temp; // 給當前 AST 使用
                }
                
                while ( error_node.size() - 2 == st.size() ) 
                    error_node.pop();
            }
            catch ( const std::runtime_error &msg ) {
                cout << msg.what();
                cout << "\n> ";
                // 因錯誤而結束該S-expression，重新讀取下一筆 S-expression
                function.newAST();
                return;
            }
            catch ( const char *msg ) {
                throw msg;
            }
            catch ( ASTNode *temp ) {
                root = temp;
                symbol.isLambdaProcedure(root, NULL); // 檢查是否為 lambda 式
                *function.getRoot() = root; // 更新 AST 的根節點
                printAST(function, symbol);
                return;
            }
        }
    }
    if ( !root ) {
        cout << "ERROR (no return value) : ";
        root = root_temp; // 若無結果，則回傳原本的 AST
    }
    else symbol.isLambdaProcedure(root, NULL); // 檢查是否為 lambda 式

    *function.getRoot() = root; // 更新 AST 的根節點
    printAST(function, symbol);
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

void Error::checkSymbol(ASTNode *cur, bool non_function, stack<pair<unordered_map<string, ASTNode*>, bool>> local_map) {
    /* 檢查 Symbol 是否為 CONS, QUOTE, LIST, 或自定義變數等 */

    bool isLocal = false, localSystem = false;
    ASTNode *localVar = NULL;
    stack<pair<unordered_map<string, ASTNode*>, bool>> local_map_temp = local_map;
    while ( !local_map_temp.empty() ) {
        unordered_map<string, ASTNode*> local = local_map_temp.top().first;
        bool lambda_map = local_map_temp.top().second;
        if ( local.find(cur->value) != local.end() ) {
            // 區域變數
            if ( local[cur->value]->type == SYSTEM )
                localSystem = true;
            isLocal = true;
            localVar = local[cur->value];
            break;
        }
        else if ( lambda_map ) break; // 若為 lambda 式，則只需檢查最上層的區域變數
        local_map_temp.pop();
    }

    if ( !isLocal && symbol_set.find(cur->value) == symbol_set.end() 
            && primitive_predicate.find(cur->value) == primitive_predicate.end() 
            && basic_arithmetic.find(cur->value) == basic_arithmetic.end() 
            && user_map.find(cur->value) == user_map.end()
            && cur->value.find("<procedure " ) == std::string::npos ) {
        // 若該 SYMBOL 未定義，則拋出錯誤
        if ( cur->type == SYMBOL ) cout << "ERROR (unbound symbol) : ";
        else cout << "ERROR (attempt to apply non-function) : ";
        throw cur;
    }
    else if ( !non_function ) {
        // 若在函數位置卻為出現並未系統定義的 SYMBOL ，則拋出錯誤
        if ( cur->type == "QUOTE_DATA" ) {
            cout << "ERROR (attempt to apply non-function) : ";
            throw cur;
        }
        else if ( isLocal ) {
            // 區域變數
            if ( localSystem ) return; // 若為系統定義的函數，則不需拋出錯誤
            cout << "ERROR (attempt to apply non-function) : ";
            throw localVar;
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
    eval = false; // 是否為 eval 的 S-expression
    lambda_tree.clear();
    read = 0; // 設定 read 的數量
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
            ASTNode *temp = *parent;
            copyAndLink(&temp); // 複製一份，報錯時使用
            if ( root && (*root)->type == LEFT_PAREN ) {
                // 若為左括弧，則檢查其內容
                temp = *root;
                copyAndLink(&temp); // 複製一份，報錯時使用
                checkExpression(&(*root)->left, root); 
            }
            
            if ( !*root ) {
                // 若不存在命令或回覆，則拋出錯誤
                cout << "ERROR (no return value) : ";
                throw temp;
            }
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
        error.checkSymbol(*root, non_function, local_map); // 檢查是否為系統定義的函數
        procedure(root, non_function ? NULL : *parent); // 檢查是否為系統定義的函數
        if ( isCONS(*root, non_function ? NULL : *parent) ) return;
        else if ( isQUOTE(*root, non_function ? NULL : parent) ) return;
        else if ( isList(*root, non_function ? NULL : *parent) ) return;
        else if ( isUserDefine(*root, non_function ? NULL : *parent, deep) ) return;
        else if ( isCAR(root, non_function ? NULL : parent) ) return;
        else if ( isCDR(root, non_function ? NULL : parent) ) return;
        else if ( checkPrimitivePredicate(*root, non_function ? NULL : *parent) ) return;
        else if ( isArithmetic(*root, non_function ? NULL : parent) ) return;
        else if ( isEqual(*root, non_function ? NULL : *parent) ) return;
        else if ( isIf(*root, non_function ? NULL : parent) )return;
        else if ( isCond(*root, non_function ? NULL : parent) ) return;
        else if ( isBegin(*root, non_function ? NULL : parent) ) return;
        else if ( isCleanEnvironment(*root, non_function ? NULL : *parent, deep) ) return;
        else if ( isExit(*root, non_function ? NULL : *parent, deep) ) return; 
        else if ( isLet(*root, non_function ? NULL : parent) ) return;
        else if ( isLambda(*root, non_function ? NULL : parent) ) return;
        else if ( isLambdaArg(*root, non_function ? NULL : parent) ) return;
        else if ( isRead(*root, non_function ? NULL : parent) ) return;
        else if ( isWrite(*root, non_function ? NULL : parent) ) return;
        else if ( isChangeToString(*root, non_function ? NULL : parent) ) return;
        else if ( isEval(*root, non_function ? NULL : parent) ) return;
        else if ( isSet(*root, non_function ? NULL : parent) ) return;
        else userDefined(root, non_function ? NULL : parent);
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
        throw error_tree;
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
            ASTNode **fir_node = &parent->right->left, *fir = *fir_node;
            copyAndLink(fir_node); // 複製一份，報錯時使用
            checkExpression(&(*fir_node)->left, fir_node); // 檢查第一個參數
            ASTNode **sec_node = &parent->right->right->left, *sec = *sec_node;
            copyAndLink(sec_node); // 複製一份，報錯時使用
            checkExpression(&(*sec_node)->left, sec_node); // 檢查第二個參數
            if ( !*fir_node ) {
                // 若該 S-expression 內的資料不為 List ，則拋出錯誤
                cout << "ERROR (unbound parameter) : ";
                throw fir; 
            }
            else if ( !*sec_node ) {
                // 若該 S-expression 內的資料不為 List ，則拋出錯誤
                cout << "ERROR (unbound parameter) : ";
                throw sec; 
            }
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

bool Symbol::isQUOTE(ASTNode *root, ASTNode **parent) {
    /* 檢查 Symbol 是否為 QUOTE 並更新 AST */
    if ( root->value != "quote" ) return false;
    else if ( !parent || (*parent)->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 QUOTE function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        root->type = SYSTEM;
        return true;
    }

    int arg = countFunctionArg(*parent);   // 計算參數個數

    if ( arg != 1 ) 
        // 若參數個數不為1，則拋出錯誤
        throw std::runtime_error("ERROR (incorrect number of arguments) : quote\n");

    ASTNode *new_root = (*parent)->right->left;    // 將 QUOTE 後的 S-expression 提高一個階層
    if ( new_root->type == NIL ) {
        (*parent)->type = END;
        (*parent)->value = "nil";
        (*parent)->left = NULL;
        (*parent)->right = NULL;
        return true;
    }
    else if ( new_root->type != LEFT_PAREN ) {
        // 該 S-expression 內的資料為 ATOM ，則需額外定義
        (*parent)->type = new_root->type == ERROR ? ERROR : "QUOTE_DATA";
        if ( eval ) (*parent)->type = new_root->type; // 若為 eval 的 S-expression，則不需改變類型
        (*parent)->value = new_root->value;
        (*parent)->left = NULL;
        (*parent)->right = NULL;
    }
    else {
        (*parent)->right = new_root->right;
        (*parent)->left = new_root->left;
        stack<ASTNode*> st;
        st.push(*parent);
        while ( !st.empty() ) {
            ASTNode *cur = st.top();
            st.pop();
            if ( cur->left ) {
                // 若有左子樹，則將其類型設為 QUOTE_DATA
                if ( !eval && cur->left->type == SYMBOL ) cur->left->type = "QUOTE_DATA";
                st.push(cur->left);
            }
            if ( cur->right ) {
                // 若有右子樹，則將其類型設為 QUOTE_DATA
                if ( !eval && cur->right->type == SYMBOL ) cur->right->type = "QUOTE_DATA";
                st.push(cur->right);
            }
        }
    }
    
    try {
        if ( eval ) {
            eval = false; // 若為 eval 的 S-expression，則不再需要檢查
            if ( (*parent)->type == SYMBOL || (*parent)->left->type == SYMBOL )
                checkExpression(&(*parent)->left, parent); // 若為 eval 的 S-expression，則需檢查其內容
            else if ( (*parent)->left->type == LEFT_PAREN )
                checkExpression(&(*parent)->left->left, &(*parent)->left);
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
            ASTNode *error = temp->left;
            copyAndLink(&error); // 複製一份，報錯時使用
            checkExpression(&temp->left->left, &temp->left); // 檢查左子樹
            
            if ( !temp->left ) {
                // 若無結果，則拋出錯誤
                cout << "ERROR (unbound parameter) : ";
                throw error;
            }
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
    if ( arg < 2 ) {
        // 若參數個數不為2，則拋出錯誤
        cout << "ERROR (DEFINE format) : ";
        throw parent;
    }

    string key = parent->right->left->value; // 取得被定義的 SYMBOL
    try {
        if ( ( parent->right->left->type != SYMBOL && key != "(" )
            || symbol_set.find(key) != symbol_set.end() 
            || primitive_predicate.find(key) != primitive_predicate.end() 
            || basic_arithmetic.find(key) != basic_arithmetic.end() ) {
            // 若該 SYMBOL 為系統定義，則拋出錯誤
            cout << "ERROR (DEFINE format) : ";
            throw parent;
        }

        ASTNode *value = parent->right->right->left;
        if ( key == "(" ) {
            key = parent->right->left->left->value; // 取得被定義的 Function 的 SYMBOL
            if ( parent->right->left->left->type != SYMBOL 
                || symbol_set.find(key) != symbol_set.end() 
                || primitive_predicate.find(key) != primitive_predicate.end() 
                || basic_arithmetic.find(key) != basic_arithmetic.end() ) {
                // 若該 SYMBOL 為系統定義，則拋出錯誤
                cout << "ERROR (DEFINE format) : ";
                throw parent;
            }

            defineFunction(key, parent->right); // 若為 Function 定義，則進行定義
            value = new ASTNode(); // 將 value 指向新的 ASTNode
            value->type = SYSTEM; // 將其類型設為 SYSTEM
            value->value = "#<procedure " + key + ">"; // 將其值設為建構子
            user_map[key] = value;    // 將定義過的 SYMBOL 加入 map 中
        }
        else {
            if ( arg != 2 ) {
                // 若參數個數不為2，則拋出錯誤
                cout << "ERROR (DEFINE format) : ";
                throw parent;
            }
            ASTNode *temp = value;
            copyAndLink(&temp); // 複製一份，報錯時使用
            checkExpression(&value->left, &value); // 檢查被定義的 SYMBOL 內的內容
            if ( !value ) {
                // 若無結果，則拋出錯誤
                cout << "ERROR (no return value) : ";
                throw temp;
            }
            user_map[key] = value;    // 將定義過的 SYMBOL 加入 map 中
            if ( value->value.find("lambda") != value->value.std::string::npos && lambda_tree.find(value->value.substr(18, 1)) != lambda_tree.end() ) {
                // 若該 SYMBOL 為 lambda 定義，則將其加入 user_function_map 中
                user_function_map["lambda" + key] = lambda_tree[value->value.substr(18, 1)]; // 將定義過的 Function 加入 map 中
                user_map[key]->value = "#<procedure lambda" + key + ">"; // 將其值設為 LAMBDA 建構子
            }
        }
    }
    catch ( const std::runtime_error &msg ) {
        throw msg;
    }
    catch ( const char *msg ) {
        if ( msg == "ERROR (DEFINE format) : " ) {
            cout << msg;
            throw parent;
        }
        throw msg;
    }
    catch ( ASTNode *temp ) {
        throw temp;
    }

    cout << key << " defined" << endl;
    define = true; // 將 define 標記設為 true
    return true;
}

void Symbol::userDefined(ASTNode **root, ASTNode **parent) {
    /* 將 User Define 傳回並更新 AST */
    if ( (*root)->type == SYSTEM ) return; // 若該 SYMBOL 為系統定義過的，則不做任何處理
    // 若該 SYMBOL 為定義過的，則將其指向進當前 AST
    string key = (*root)->value;
    bool getDefine = false; // 標記是否已取得定義過的 SYMBOL

    try {
        stack<pair<unordered_map<string, ASTNode*>, bool>> local_map_temp = local_map; // 將當前的 local_map 複製一份
        while ( !local_map_temp.empty() ) {
            // 若該 SYMBOL 為當前區域定義過的，則將其指向進當前 AST
            unordered_map<string, ASTNode*> local = local_map_temp.top().first; // 取得當前區域的變數
            bool lambda_map = local_map_temp.top().second; // 取得當前區域是否為 Lambda 定義
            if ( local.find(key) != local.end() ) {
                // 區域變數
                *root = local[key]; // 將區域變數內容指向進當前 AST
                getDefine = true;
                break;
            }
            else if ( lambda_map ) break; // 若為 Lambda 定義，則不需繼續檢查
            local_map_temp.pop(); // 將當前區域的變數移除
        }
        
        if ( !getDefine )
            // 若該 SYMBOL 不為區域變數或 Lambda 定義
            // 則該 SYMBOL 為全域變數定義過的，將其指向進當前 AST
            *root = user_map[key]; // 將全域變數內容指向進當前 AST

        if ( parent && (*parent)->type == LEFT_PAREN ) 
            checkExpression(root, parent); // 檢查其內容
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

void Symbol::copyTree(ASTNode *root, ASTNode *defined_tree) {
    /* 從 defined_tree 複製 AST 至當前 root */

    root->type = defined_tree->type;
    root->value = defined_tree->value;
    if ( defined_tree->left ) {
        root->left = new ASTNode();
        copyTree(root->left, defined_tree->left);
    }
    else root->left = NULL; // 若無左子樹，則設為 NULL
    if ( defined_tree->right ) {
        root->right = new ASTNode();
        copyTree(root->right, defined_tree->right);
    }
    else root->right = NULL; // 若無右子樹，則設為 NULL
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
            ASTNode *check_node = (*parent)->right->left, *temp = check_node;  // 分析參數
            copyAndLink(&temp); // 複製一份，報錯時使用
            checkExpression(&check_node->left, &check_node); // 檢查 Expression 內的內容
            if ( !check_node ) {
                // 若參數不存在，則報錯
                cout << "ERROR (unbound parameter) : ";
                throw temp;
            }
            else if ( !check_node->left ) {
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
            ASTNode *check_node = (*parent)->right->left, *temp = check_node;  // 分析參數
            copyAndLink(&temp); // 複製一份，報錯時使用
            checkExpression(&check_node->left, &check_node); // 檢查 Expression 內的內容
            if ( !check_node ) {
                // 若參數不存在，則報錯
                cout << "ERROR (unbound parameter) : ";
                throw temp;
            }
            else if ( !check_node->right ) {
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
            ASTNode *check_node = parent->right->left, *temp = check_node;  // 該位置應該為需確認的 S-expression
            copyAndLink(&temp); // 複製一份，報錯時使用
            checkExpression(&check_node->left, &check_node); // 檢查 Expression 內的內容
            if ( !check_node ) {
                // 若參數為 ATOM ，則報錯
                cout << "ERROR (unbound parameter) : ";
                throw temp;
            }
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
            else if (root->value == "error-object?") 
                checkPrimitive = isErrorObject(check_node);
            
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

bool Symbol::isErrorObject(ASTNode *root) {
    /* 檢查該 AST 是否為 Error Object */
    if ( root->type == ERROR ) return true;
    return false;
}

bool Symbol::isArithmetic(ASTNode *root, ASTNode **parent) {
    /* 檢查該 Symbol 是否為基礎運算式或比較運算式 */
    
    if ( basic_arithmetic.find(root->value) == basic_arithmetic.end() ) return false;
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
                ASTNode *temp = count_node->left;
                copyAndLink(&temp); // 複製一份，報錯時使用
                checkExpression(&count_node->left->left, &count_node->left); // 檢查 Expression 內的內容
                if ( !count_node->left ) {
                    // 若不存在參數，則拋出錯誤
                    if ( root->value == "or" || root->value == "and" )
                        // 若為 or 或 and 運算式，則拋出錯誤
                        cout << "ERROR (unbound condition) : ";
                    else
                        // 若為其他運算式，則拋出錯誤
                        cout << "ERROR (unbound parameter) : ";
                    throw temp;
                }

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
            ASTNode *temp1 = first_element, *temp2 = second_element;  // 該位置應該為需比較的 S-expression
            copyAndLink(&temp1); // 複製一份，報錯時使用
            copyAndLink(&temp2); // 複製一份，報錯時使用
            checkExpression(&first_element->left, &first_element); // 檢查 Expression 內的內容
            checkExpression(&second_element->left, &second_element); // 檢查 Expression 內的內容
            if ( !first_element ) {
                // 若第一個參數不存在，則報錯
                cout << "ERROR (unbound parameter) : ";
                throw temp1;
            }
            else if ( !second_element ) {
                // 若第二個參數不存在，則報錯
                cout << "ERROR (unbound parameter) : ";
                throw temp2;
            }

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
            ASTNode *check_node = (*parent)->right;  // 該位置應該為需運行的 S-expression
            conditional(&check_node); // 運行判別式
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
                ASTNode *condition = check_node->left->left; // 該位置為該判對式的條件
                copyAndLink(&condition); // 將該判對式複製，報錯時使用
                checkExpression(&check_node->left->left->left, &check_node->left->left); // 檢查判斷式內的內容
                if ( !check_node->left->left ) {
                    // 結果不存在，則報錯
                    cout << "ERROR (unbound test-condition) : ";
                    throw condition;
                }
                if ( check_node->left->left && !isNull(check_node->left->left) ) {
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
        ASTNode *temp_root = check_node;
        copyAndLink(&temp_root); // 將該 S-expression 複製一份，報錯時使用
        checkExpression(&check_node->left, &check_node); // 檢查 Expression 內的內容
        bool conditional = true;
        if ( !check_node ) {
            cout << "ERROR (unbound test-condition) : ";
            throw temp_root;
        }
        else if ( check_node->value == "nil" || check_node->type == END )
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

        while ( check_node->type != END ) {
            // 檢查 S-expression 內的每個左子樹
            checkExpression(&check_node->left->left, &check_node->left); // 檢查 Expression 內的內容

            if ( check_node->right->type == END ) 
                // 若為最後一筆 S-expression，則將其設為結果
                *parent = check_node->left;

            check_node = check_node->right;  // 接續下一個 S-expression
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

    return true;
}

void Symbol::procedure(ASTNode **root, ASTNode *parent) {
    /* 若為自定義 Symbol 函數，且內部資料為 Procedure 系統函數，則需進行轉換 */
    
    ASTNode *procedure_node = NULL;
    stack<pair<unordered_map<string, ASTNode *>, bool>> local_map_temp = local_map; // 複製區域變數庫
    while ( !local_map_temp.empty() ) {
        unordered_map<string, ASTNode*> local = local_map_temp.top().first; // 取出區域變數庫的最上層
        bool lambda_map = local_map_temp.top().second; // 取出區域變數庫是否為 lambda 函數
        if ( local.find((*root)->value) != local.end() && local[(*root)->value]->type == SYSTEM ) {
            procedure_node = local[(*root)->value]; // 若該自定義函數為 Procedure 系統函數，則抓取其節點
            break;
        }
        else if ( local.find((*root)->value) != local.end() ) return; // 若該自定義函數為區域變數且不為 Procedure 系統函數，則不做任何處理
        else if ( lambda_map ) break; // 若為 lambda 函數，則只看最上層的區域變數庫
        local_map_temp.pop();
    }
    if ( !procedure_node && user_map.find((*root)->value) != user_map.end() && user_map[(*root)->value]->type == SYSTEM )
        // 若該自定義函數為 Procedure 系統函數，則抓取其節點
        procedure_node = user_map[(*root)->value];

    if ( procedure_node ) {
        // 若該自定義函數為 Procedure 系統函數，則將其轉換
        if ( parent && parent->type == LEFT_PAREN ) {
            // 若父節點為 LEFT_PAREN ，則運行 procedure function 功能
            if ( user_function_map.find(procedure_node->value.substr(12, procedure_node->value.size() - 13)) != user_function_map.end() ) {
                // 若該自定義函數為 lambda 函數，則將其設為 lambda 函數
                string key = procedure_node->value.substr(12, procedure_node->value.size() - 13); // 去除 <procedure 和 >
                lambda_tree[key] = user_function_map[key]; // 將其設為 lambda 函數
                copyAndLink(&lambda_tree[key]); // 將該自定義結構複製
                (*root)->value = "lambda" + key;
            }
            else 
                (*root)->value = procedure_node->value.substr(12, procedure_node->value.size() - 13); // 去除 <procedure 和 >
        }
        else {
            (*root)->value = procedure_node->value; // 若父節點不為 LEFT_PAREN ，則不做 procedure function 功能，只留回傳功能
            (*root)->type = SYSTEM; // 將其設為系統函數
        }
    }
    else if ( (*root)->type == SYSTEM ) {
        if ( parent && parent->type == LEFT_PAREN ) {
            // 若父節點為 LEFT_PAREN ，則運行 procedure function 功能
            ASTNode *temp = new ASTNode();
            copyTree(temp, (*root)); // 將該 S-expression 複製一份，避免導致記憶體覆蓋
            *root = temp;

            if ( user_function_map.find((*root)->value.substr(12, (*root)->value.size() - 13)) != user_function_map.end() ) {
                // 若該自定義函數為 lambda 函數，則將其設為 lambda 函數
                string key = (*root)->value.substr(12, (*root)->value.size() - 13); // 去除 <procedure 和 >
                lambda_tree[key] = user_function_map[key]; // 將其設為 lambda 函數
                copyAndLink(&lambda_tree[key]); // 將該自定義結構複製
                (*root)->value = "lambda" + key;
            }
            else
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
        localDefine((*parent)->right->left); // 將區域變數寫入區域變數庫

        ASTNode *work_node = (*parent)->right->right; // 該位置應該為需運行的 S-expression
        while ( work_node->type != END ) {
            // 檢查並執行每個 S-expression
            checkExpression(&work_node->left->left, &work_node->left); // 檢查 Expression 內的內容
            if ( work_node->right->type == END )
                *parent = work_node->left; // 若為最後一筆 S-expression，則將其設為結果
            
            work_node = work_node->right; // 接續下一個 S-expression
        }
        local_map.pop(); // 將當前的區域變數庫設為之前的區域變數庫
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
        unordered_map<string, ASTNode*> map; // 定義區域變數庫
        queue<pair<string, ASTNode*>> temp_map; // 暫存區域變數庫

        while ( root->type != END && root->value != "nil" ) {
            if ( root->type != LEFT_PAREN && root->type != LINK )
                // 確保為參數建構的儲存結構
                throw "ERROR (LET format) : ";

            ASTNode *define_node = root->left;
            if ( define_node->type != LEFT_PAREN || !isList(define_node) )
                // 若左子樹不為 S-expression 或該判斷式具備右節點，則拋出錯誤
                throw "ERROR (LET format) : ";
    
            string key = define_node->left->value; // 取得被定義的 SYMBOL
            if ( define_node->left->type != SYMBOL
                || symbol_set.find(key) != symbol_set.end() 
                || primitive_predicate.find(key) != primitive_predicate.end() 
                || basic_arithmetic.find(key) != basic_arithmetic.end() )
                // 若該 SYMBOL 為系統定義或不存在，則拋出錯誤
                throw "ERROR (LET format) : ";

            if ( define_node->right->type == END || define_node->right->right->type != END )
                // 若該定義資料不符合 Pair 格式，則拋出錯誤
                throw "ERROR (LET format) : ";

            ASTNode *value = define_node->right->left; // 取得被定義的 SYMBOL 的值
            temp_map.push({key, value}); // 將被定義的 SYMBOL 和其值暫存起來
            root = root->right; // 接續下一個被定義的 SYMBOL
        }
        while ( !temp_map.empty() ) {
            string key = temp_map.front().first; // 取得被定義的 SYMBOL
            ASTNode *value = temp_map.front().second, *temp = value; // 取得被定義的 SYMBOL 的值
            copyAndLink(&temp); // 將該被定義複製，報錯時使用

            checkExpression(&value->left, &value); // 檢查被定義的 SYMBOL 內的內容
            if ( !value ) {
                // 若被定義的 SYMBOL 不存在，則拋出錯誤
                cout << "ERROR (no return value) : ";
                throw temp;
            }
            map[key] = value;    // 將定義過的 SYMBOL 加入 map 中
            temp_map.pop();
        }
        local_map.push({map, false}); // 設定為當前的區域變數庫
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
        if ( arg < 2 ) {
            // 若參數個數小於 2，則拋出錯誤
            cout << "ERROR (LAMBDA format) : ";
            throw *parent;
        }
        else {
            // 創建 LAMBDA 樹，以供後續參數使用
            checkLambda((*parent)->right); // 檢查 LAMBDA 的格式
            (*parent)->value = "#<procedure " + root->value + to_string(lambda_tree.size() - 1) + ">";
            (*parent)->type = SYSTEM; // 將其設為系統函數
            (*parent)->left = NULL; // 將其左子樹設為 NULL
            (*parent)->right = NULL; // 將其右子樹設為 NULL
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

void Symbol::isLambdaProcedure(ASTNode *root, ASTNode **parent) {
    /* 檢查 Symbol 是否為 LAMBDA 並更新 AST */
    if ( root->value.size() < 18 || root->value.substr(0, 18) !=  "#<procedure lambda" ) return;
    else if ( !parent || (*parent)->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 LAMBDA function 功能，只留回傳功能
        root->value = "#<procedure lambda>";
        root->type = SYSTEM;
    }
}

void Symbol::checkLambda(ASTNode *root) {
    /* 檢查 LAMBDA 的格式 */
    ASTNode *define_node = root->left;
    if ( (define_node->type != LEFT_PAREN && define_node->value != "nil") || !isList(define_node) )
        // 若該定義參數位置不為該結構，則拋出錯誤
        throw "ERROR (LAMBDA format) : ";

    while ( define_node->type != END && define_node->value != "nil" ) {
        // 檢查 S-expression 內的每個左子樹
        string key = define_node->left->value; // 取得被定義的 SYMBOL
        if ( define_node->left->type != SYMBOL
            || symbol_set.find(key) != symbol_set.end() 
            || primitive_predicate.find(key) != primitive_predicate.end() 
            || basic_arithmetic.find(key) != basic_arithmetic.end() )
            // 若該 SYMBOL 為系統定義或不存在，則拋出錯誤
            throw "ERROR (LAMBDA format) : ";
        define_node = define_node->right; // 接續下一個被定義的 SYMBOL
    }
    lambda_tree[to_string(lambda_tree.size())] = root;
}

bool Symbol::isLambdaArg(ASTNode *root, ASTNode **parent) {
    /* 檢查 Symbol 是否為 LAMBDA 的參數 */
    if ( root->value.size() < 6 || root->value.substr(0, 6) != "lambda" ) return false;
    else if ( !parent || (*parent)->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 LAMBDA function 功能，只留回傳功能
        root->type = SYSTEM;
        return true;
    }

    try {
        string key = root->value.substr(6, root->value.size()); // 取出隱藏在 LAMBDA 標記後的參數名稱
        *parent = useLambda((*parent)->right, key); // 將 LAMBDA 的參數寫入 lambda_map 中，並取回
        local_map.pop(); // 將 user_define_function_arg 從區域變數庫中移除
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

ASTNode* Symbol::useLambda(ASTNode *root, string &key) {
    /* 依照參數回傳相應結果 */
    ASTNode *arg_node = root, *check_node = lambda_tree[key]->left;
    string error_name = atom.isINT(key) ? "lambda" : key;
    if ( error_name.substr(0, 6) == "lambda" ) error_name = error_name.substr(0, 6); // 去除 lambda 的標記
    copyAndLink(&check_node); // 將該自定義結構複製
    queue<pair<string, ASTNode*>> lambda_define;
    unordered_map<string, ASTNode*> temp_map; // 紀錄當前的 lambda_map

    try {
        while ( check_node->type != END && check_node->value != "nil" ) {
            // 檢查參數
            string key = check_node->left->value; // 取得被定義的 SYMBOL
            if ( arg_node->type == END )
                // 若該定輸入資料不足，則拋出錯誤
                throw std::runtime_error("ERROR (incorrect number of arguments) : " + error_name + "\n");

            ASTNode *value = arg_node->left; // 取得被定義的 SYMBOL 的值
            lambda_define.push({key, value}); // 將被定義的 SYMBOL 和其值暫存起來
    
            check_node = check_node->right; // 接續下一個被定義的 SYMBOL
            arg_node = arg_node->right; // 接續下一個被定義內容
        }
        if ( arg_node->type != END )
            // 若該定輸入資料過多，則拋出錯誤
            throw std::runtime_error("ERROR (incorrect number of arguments) : " + error_name + "\n");

        while ( !lambda_define.empty() ) {
            string key = lambda_define.front().first; // 取得被定義的 SYMBOL
            ASTNode *value = lambda_define.front().second, *temp = value; // 取得被定義的 SYMBOL 的值
            copyAndLink(&temp); // 將該被定義複製，報錯時使用

            checkExpression(&value->left, &value); // 檢查被定義的內容
            if ( !value ) {
                // 若被定義的 SYMBOL 不存在，則拋出錯誤
                cout << "ERROR (unbound parameter) : ";
                throw temp;
            }
            temp_map[key] = value;    // 將定義過的 SYMBOL 加入 map 中
            lambda_define.pop();
        }
        local_map.push({temp_map, true}); // 將 lambda_arg 寫入區域變數庫

        ASTNode *function_node = lambda_tree[key]->right; // 需運行的 S-expression
        copyAndLink(&function_node); // 將該自定義結構複製
        while ( function_node->type != END ) {
            // 檢查 S-expression 內的每個左子樹
            checkExpression(&function_node->left->left, &function_node->left); // 檢查 S-expression 內的內容
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

void Symbol::defineFunction(string &key, ASTNode *parent) {
    /* 將該 function 以 SYMBOL 定義 */ 
    ASTNode *function = parent;
    copyAndLink(&function); // 將該 S-expression 複製一份，避免覆蓋
    function->left = function->left->right; // 該位置為 function 的 Arguments

    ASTNode *check_arg = function->left; // 該位置為 Arguments
    while ( check_arg->type != END ) {
        // 檢查左子樹是否存在 S-expression
        if ( check_arg->left->type != SYMBOL )
            // 若 Argument 不為 SYMBOL，則拋出錯誤
            throw "ERROR (DEFINE format) : ";
        check_arg = check_arg->right; // 接續下一個 Argument
    }

    user_function_map[key] = function; // 將該 function 加入 user_function_map 中
}

bool Symbol::isRead(ASTNode *root, ASTNode **parent) {
    /* 檢查 Symbol 是否為 READ 並更新 AST */
    if ( root->value != "read" ) return false;
    else if ( !parent || (*parent)->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 READ function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        root->type = SYSTEM;
        return true;
    }

    int arg = countFunctionArg(*parent);   // 計算參數個數

    if ( arg > 0 ) throw std::runtime_error("ERROR (incorrect number of arguments) : read\n");
    
    if ( !read_queue.empty() ) {
        // 若讀入資料，則進行處理
        *parent = read_queue.front(); // 將讀入資料設為父節點
        read_queue.pop(); // 將讀入資料從佇列中移除
    }
    else {
        // 第一次進入讀入資料，則等待使用者輸入
        ++read; // 設定 read 加一，表示下一筆為讀入的資料
        throw "Read"; // 拋出 Read，並等待讀入資料
    }

    return true;
}

bool Symbol::isWrite(ASTNode *root, ASTNode **parent) {
    /* 檢查 Symbol 是否為 WRITE 並更新 AST */
    if ( root->value != "write" && root->value != "display-string" && root->value != "newline" ) return false;
    else if ( !parent || (*parent)->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 WRITE function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        root->type = SYSTEM;
        return true;
    }

    int arg = countFunctionArg(*parent);   // 計算參數個數

    if ( root->value != "newline" && arg != 1 ) throw std::runtime_error("ERROR (incorrect number of arguments) : " + root->value + "\n");
    else if ( root->value == "newline" && arg != 0 ) throw std::runtime_error("ERROR (incorrect number of arguments) : newline\n");

    if ( root->value == "newline" ) {
        // 若為 newline，則直接輸出換行
        cout << endl;
        ASTNode *new_root = new ASTNode;
        new_root->type = NIL; // 將其設為 nil
        new_root->value = "nil"; // 將其值設為 nil
        new_root->left = NULL; // 將其左子樹設為 NULL
        new_root->right = NULL; // 將其右子樹設為 NULL
        *parent = new_root;
        return true;
    }

    try {
        ASTNode *write_node = (*parent)->right->left, *temp = write_node;  // 分析參數
        copyAndLink(&temp); // 複製一份，報錯時使用
        checkExpression(&write_node->left, &write_node); // 檢查 Expression 內的內容
        if ( !write_node ) {
            // 若參數不存在，則報錯
            cout << "ERROR (unbound parameter) : ";
            throw temp;
        }

        ASTNode* display_string = new ASTNode;
        copyTree(display_string, write_node);
        if ( root->value == "display-string" ) {
            if ( write_node->type != STRING && write_node->type != ERROR ) {
                // 若參數不為 STRING 或 ERROR，則報錯
                cout << "ERROR (display-string with incorrect argument type) : ";
                throw write_node;
            }
            display_string->value = display_string->value.substr(1, display_string->value.size() - 2); // 去除 STRING 的引號
        }

        writeFunction(display_string); // 執行寫入功能
        *parent = write_node;  // 將結構內的第一筆資料作為回傳結果
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

void Symbol::writeFunction(ASTNode *root) {
    /* 執行寫入功能 */
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
            if ( direction != "right" )
                cout << "nil";
            else 
                cout << ")";

            if ( !st.empty() ) cout << endl;
        }
        else if ( !cur->left && !cur->right ) {
            if ( atom.isFLOAT(cur->value) ) atom.isFLOAT(cur->value, true); // 若為浮點數，則取小數點後三位
            cout << cur->value;
            if ( !st.empty() ) cout << endl;

            if ( direction == "right" ) {
                --left_paren;
                for ( int i = 0; i < left_paren; ++i ) cout << "  ";
                cout << ")";
                if ( !st.empty() ) cout << endl;
            }    
        }
    }
}

bool Symbol::isChangeToString(ASTNode *root, ASTNode **parent) {
    /* 檢查 Symbol 是否為轉換為 STRING 並更新 AST */
    if ( root->value != "symbol->string" && root->value != "number->string" ) return false;
    else if ( !parent || (*parent)->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做轉換為 STRING function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        root->type = SYSTEM;
        return true;
    }

    int arg = countFunctionArg(*parent);   // 計算參數個數

    if ( arg != 1 ) throw std::runtime_error("ERROR (incorrect number of arguments) : " + root->value + "\n");

    try {
        ASTNode *change_node = (*parent)->right->left, *temp = change_node; // 該位置應該為需轉換的 S-expression
        copyAndLink(&temp); // 將該 S-expression 複製一份，報錯時使用
        checkExpression(&change_node->left, &change_node); // 檢查 Expression 內的內容
        if ( !change_node ) {
            // 若參數不存在，則報錯
            cout << "ERROR (unbound parameter) : ";
            throw temp;
        }

        ASTNode *new_root = new ASTNode();
        new_root->type = STRING; // 將其設為 STRING
        if ( root->value == "symbol->string" && change_node->type == "QUOTE_DATA" )
            // 若參數為 QUOTE_DATA，則將其轉換為 STRING
            new_root->value = "\"" + change_node->value + "\""; // 將其值設為 STRING
        else if ( change_node->type == INT || change_node->type == FLOAT ) {
            // 若參數為 INT 或 FLOAT，則轉換為 STRING
            string value;
            if ( atom.isINT(change_node->value) )
                value = change_node->value; // 若為整數，則直接使用其值
            else {
                atom.isFLOAT(change_node->value, true); // 若為浮點數，則取小數點後三位
                value = change_node->value;
            }
            new_root->value = "\"" + value + "\""; // 將其值設為 STRING
        }
        else
            throw std::runtime_error("ERROR (" + root->value + " with incorrect argument type) : " + change_node->value + "\n");

        new_root->left = NULL; // 將其左子樹設為 NULL
        new_root->right = NULL; // 將其右子樹設為 NULL
        *parent = new_root; // 將其設為新的根節點
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

bool Symbol::isEval(ASTNode *root, ASTNode **parent) {
    /* 檢查 Symbol 是否為 EVAL 並更新 AST */
    if ( root->value != "eval" ) return false;
    else if ( !parent || (*parent)->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 EVAL function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        root->type = SYSTEM;
        return true;
    }

    int arg = countFunctionArg(*parent);   // 計算參數個數

    if ( arg != 1 ) throw std::runtime_error("ERROR (incorrect number of arguments) : eval\n");

    try {
        ASTNode *run_node = (*parent)->right->left, *temp = run_node; // 該位置應該為需運行的 S-expression
        eval = true; // 設定 eval 為 true，表示需要運行 S-expression
        copyAndLink(&temp); // 將該 S-expression 複製一份，報錯時使用
        checkExpression(&run_node->left, &run_node); // 檢查 Expression 內的內容
        if ( !run_node ) {
            // 若參數不存在，則報錯
            cout << "ERROR (unbound parameter) : ";
            throw temp;
        }
        eval = false; // 設定 eval 為 false，表示不需要運行 S-expression
        *parent = run_node; // 將其設為新的根節點
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

bool Symbol::isSet(ASTNode *root, ASTNode **parent) {
    /* 檢查 Symbol 是否為 SET 並更新 AST */
    if ( root->value != "set!" ) return false;
    else if ( !parent || (*parent)->type != LEFT_PAREN ) {
        // 若父節點不為 LEFT_PAREN ，則不做 SET function 功能，只留回傳功能
        root->value = "#<procedure " + root->value + ">";
        root->type = SYSTEM;
        return true;
    }

    int arg = countFunctionArg(*parent);   // 計算參數個數

    if ( arg != 2 ) {
        // 若參數個數不為2，則拋出錯誤
        cout << "ERROR (SET! format) : ";
        throw *parent;
    }

    string key = (*parent)->right->left->value; // 取得被定義的 SYMBOL
    try {
        if ( ( (*parent)->right->left->type != SYMBOL )
            || symbol_set.find(key) != symbol_set.end() 
            || primitive_predicate.find(key) != primitive_predicate.end() 
            || basic_arithmetic.find(key) != basic_arithmetic.end() ) {
            // 若該 SYMBOL 為系統定義，則拋出錯誤
            cout << "ERROR (SET! format) : ";
            throw *parent;
        }
        
        ASTNode *value = (*parent)->right->right->left, *temp = value; // 取得被定義的 SYMBOL 的值
        copyAndLink(&temp); // 複製一份，報錯時使用
        checkExpression(&value->left, &value); // 檢查被定義的 SYMBOL 內的內容
        if ( !value ) {
            // 若無結果，則拋出錯誤
            cout << "ERROR (no return value) : ";
            throw temp;
        }

        bool local_var = false, lambda = false; // 是否為區域變數
        queue<pair<unordered_map<string, ASTNode*>, bool>> local_queue_temp; // 將當前的 local_map 暫存
        while ( !local_map.empty() ) {
            // 若該 SYMBOL 為當前區域定義過的，則將其指向進當前 AST
            unordered_map<string, ASTNode*> local = local_map.top().first; // 取得當前區域的變數
            bool lambda_map = local_map.top().second; // 取得當前區域是否為 Lambda 定義
            if ( !lambda && !local_var && local.find(key) != local.end() ) {
                // 區域變數
                local[key] = value; // 將區域變數內容指向進當前 AST
                local_var = true; // 設定為區域變數
            }
            else if ( lambda_map ) lambda = true; // 若為 Lambda 定義，則不需繼續檢查
            local_queue_temp.push({local, lambda_map}); // 將當前區域的變數庫暫存
            local_map.pop(); // 將當前區域的變數移除
        }

        while ( !local_queue_temp.empty() ) {
            // 將當前的 local_map 恢復
            local_map.push(local_queue_temp.front());
            local_queue_temp.pop();
        }

        if ( !local_var )
            user_map[key] = value;    // 將定義過的 SYMBOL 加入 map 中
        *parent = value; // 將其設為新的根節點
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
