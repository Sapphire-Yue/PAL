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

class Function {
    public:
        Function();
        ~Function();
        ASTNode *getRoot() { return root; };
        bool getDot() { return dot; };
        bool getQuote() { return quote; };
        void newAST();
        void checkTypeOfAtom(string &atom, string &type);
        void storeTokenInAST(string &token, int &line, int &column);
        void newExpression();
        void backExpression();
        void quoteExpression();
        bool checkExit();
        void optimizeAST();
        void leafToRoot(ASTNode *root, int level);
        bool printAST();
    private:
        ASTNode *root;
        ASTNode *cur;
        vector<ASTNode*> ast;
        bool dot;
        int quote;
        Atom atom;
        string input;
        stack<pair<ASTNode*, int>> st;
};

class Separator {
    public:
        Separator(Function &function);
        ~Separator();
        void leftParen(string &token, int &line, int &column);
        void rightParen(string &token, int &nil, int &left_paren, int &line, int &column);
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
    left_paren = 0; // 預設左括弧為0
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
    // 用來讀取下一行輸入的資料
    // 若遇到EOF則拋出例外

    if ( !getline(cin, expr) ) throw "ERROR (no more input) : END-OF-FILE encountered"; // 讀到EOF
    ++line;    // 紀錄行數

    cout << "You entered: " << expr << " " << endl;    // debug
    try {
        saperateExprToToken(expr, line, nil);  // 將輸入的S-expression分割成Token
    }
    catch ( const std::runtime_error &msg ) {
        cerr << msg.what();
        cout << "\n> ";
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
    // cout << "saperateExprToToken" << endl; // debug
    int column = 0; // 紀錄目前字元位置
    bool str = false, backslash = false, annotation = false, lineAreNotEmpty = false; // 判斷是否為字串與反斜線或註解、該行行數是否需要被算入下一筆Token
    string token;
    if ( expr[0] == ';' ) {
        return; // 該行為註解
        --line;
    }
    for ( auto &c : expr ) {
        ++column;
        if ( !line ) ++line;
        if ( c != ' ' ) lineAreNotEmpty = true;
        if ( !str && c == '(' ) {
            ++left_paren;
            separtor.leftParen(token, line, column);
            nil = 0;
        }
        else if ( !str && c == ')' ) {
            --left_paren;
            try {
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
                if ( !token.empty() ) function.storeTokenInAST(token, line, column);
                token = "'";
                function.storeTokenInAST(token, line, column);
            }
            catch ( const std::runtime_error &msg ) {
                throw msg;
            }
            token.clear();
        }
        else if ( !str && c == ' ' ) {
            // 遇到空白，將token存入AST
            if ( token.empty() ) continue;
            try {
                function.storeTokenInAST(token, line, column);
            }
            catch ( const std::runtime_error &msg ) {
                throw msg;
            }
            token.clear();
            ++nil; // 該括弧並不為空
        }
        else if ( !str && c == ';') {
            // 遇到註解，則該行後面的資料皆為註解
            break;
        }
        else if ( c == '\"' ) {
            // 遇到雙引號，將字串存入AST
            if ( !str && !token.empty() ) {
                try {
                    function.storeTokenInAST(token, line, column);
                }
                catch ( const std::runtime_error &msg ) {
                    throw msg;
                }
                token.clear();
            }
            token += c; 
            if ( str && !backslash ) {
                // 若遇到反斜線則將雙引號公用消除，反之則該字串結束
                try {
                    function.storeTokenInAST(token, line, column);
                }
                catch ( const std::runtime_error &msg ) {
                    throw msg;
                }
                token.clear();
                str = false;
            }
            else {
                str = true;
                backslash = false;
            }
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
        cout << error;
        left_paren = 0;
        function.newAST();
        cout << "\n> ";
        return;
    }
    else if ( !token.empty() ) {
        try {
            function.storeTokenInAST(token, line, column);
        }
        catch ( const std::runtime_error &msg ) {
            throw msg;
        }
        token.clear();
    }
    

    if ( left_paren || function.getQuote() ) return; // 該S-expression未結束，繼續讀取下一行
    else if ( lineAreNotEmpty ) line = 0; // 重新另一個S-expression

    if ( function.printAST() ) {
        line = 0;
        cout << " line "<< line << endl;
    }
    

}

void Function::storeTokenInAST(string &token, int &line, int &column) {
    // cout << "stroeTokenInAST" << endl; // debug
    // 確認Token型態並更新其內容
    string type = SYMBOL;
    checkTypeOfAtom(token, type);
    cout << "token: " << token << " " << type << " " << column << endl; // debug
    cout << "cur: " << cur->value << " " << cur->type << endl; // debug
    // cout << "dot: " << dot << endl; // debug
    
    if ( cur->type != LEFT_PAREN && cur->type != TEMP && cur->type != BEGIN && cur->type != "QUOTE_TEMP" ) {
        string error = "ERROR (unexpected token) : ')' expected when token at Line " + to_string(line) + " Column " + to_string(column - token.size() + 1) + " is >>" + token + "<<\n";
        throw std::runtime_error(error); 
    }
    if ( type == DOT ) {
        // 若該token為DOT，則下一個token為另一個S-expression
        if ( cur->type == BEGIN || cur->type == LEFT_PAREN || dot ) {
            // 前方並未存在S-expression
            string error = "ERROR (unexpected token) : atom or '(' expected when token at Line " + to_string(line) + " Column " + to_string(column - 1) + " is >>" + token + "<<\n";
            throw std::runtime_error(error);
        }
        dot = true;
        return;
    }
    else if ( type == QUOTE ) {
        quoteExpression();
        ++quote;
        // Quote後面將接上另一個S-Expression
    }
    else if ( (token == "nil" || token == ")")  && ( cur->value == "(" || cur->value == ".(" || dot ) ) {
        if ( token == ")" && cur->value == "(" ) {
            cur->type = NIL; // 左右括弧成立，則該位置為nil
            cur->value = "nil"; 

            cur->left = NULL;
            cur->right = NULL;
        }
        else if ( cur->value == ".(" || dot ) {
            // 將nil存在右子樹，則不需儲存
            cur->value = "nil";
            cur->type = END;
            dot = false;
        }
        else {
            if ( cur->type == TEMP || cur->type == "QUOTE_TEMP" ) 
                cur->type = LINK; // debug

            ASTNode *new_node = new ASTNode();
            new_node->value = token;
            new_node->type = type;
            new_node->left = NULL;
            new_node->right = NULL;

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
    
    if ( type != QUOTE && quote > 0 && !ast.empty() && ast.back()->type == "QUOTE_TEMP" ) {
        // 若前一個S-expression結束，且上一個節點為END，則再回到上一個S-expression
        cur->type = END;
        backExpression();
        if ( ast.empty() ) printAST();
    }
}

Function::Function() {
    // cout << "Function created" << endl;  // debug
    atom = Atom();
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
    cur = root;
    root->type = BEGIN;
    root->value = "";
    ast.clear();
    dot = false;
    quote = false;
}

bool Function::checkExit() {
    // cout << "checkExit" << endl; // debug
    // 檢查是否為exit
    if ( root->left->value == "exit" && root->right->type == END ) return true;
    return false;
}

void Function::checkTypeOfAtom(string &atom, string &type) {
    /*檢查Token型態，並將設定其型態*/
    // cout << "checkTypeOfToken" << endl; // debug
    if ( this->atom.isINT(atom) ) type = INT;
    else if ( this->atom.isSTRING(atom) ) type = STRING;
    else if ( this->atom.isDOT(atom) ) type = DOT;
    else if ( this->atom.isFLOAT(atom) ) type = FLOAT;
    else if ( this->atom.isNIL(atom) ) type = NIL;
    else if ( this->atom.isT(atom) ) type = T;
    else if ( this->atom.isQUOTE(atom) ) type = QUOTE;
    else type = SYMBOL;
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
    // cout << "leftParen" << endl; // debug
    // 遇見新的左括弧，即產生新的Exprssion
    if ( !token.empty() ) {
        // 遇到左括弧時，還有資料未寫入AST
        function.storeTokenInAST(token, line, column);
        token.clear();
    }
    function.newExpression();
}

void Separator::rightParen(string &token, int &nil, int &left_paren, int &line, int &column) {
    // cout << "rightParen" << endl;    // debug
    // 遇見右括弧，即結束該S-expression
    if ( (function.getDot() && token.empty()) || left_paren < 0 ) {
        // 若左括弧為0，則該右括弧為錯誤
        string error = "ERROR (unexpected token) : atom or '(' expected when token at Line " + to_string(line) + " Column " + to_string(column) + " is >>)<<\n";
        throw std::runtime_error(error);
    }
    if ( !nil && token.empty() ) {
        // 遇到空括弧，將nil存入AST
        cout << "nil" << endl;
        token = ")";
        function.storeTokenInAST(token, line, column);
        ++nil;
        token.clear();

        if ( !left_paren ) {
            // 整個S-expression結束
            line = 0, column = 0; // 重新另一個S-expression
            function.optimizeAST(); //debug
            //function.printAST();
            return;
        }

        function.backExpression();
        return; // 左右括弧只為單一Atom，該S-expression可能並未結束
    }
    else if ( !token.empty() ) {
        // 遇到右括弧時，還有資料未寫入AST
        function.storeTokenInAST(token, line, column);
    }
    token.clear();
    // cur->type = ")"; // 其中一個S-expression的結尾
    
    // 該S-expression結束，回到上一個S-expression
    function.backExpression();
    
    if ( !left_paren ) {
        // 整個S-expression結束
        if ( function.checkExit() ) throw "Exit"; // 若為exit，則結束
        line = 0, column = 0; // 重新另一個S-expression
        function.optimizeAST(); //debug
        //function.printAST();
    }
}

void Function::newExpression() {
    // cout << "newExpression" << endl; // debug
    // 遇見新的左括弧，即產生新的Exprssion

    if ( dot ) {
        // dot後面將接上另一個具有括弧的S-Expression
        cur->value = ".(";
        ast.push_back(cur); 
        dot = false;
        return;
    }

    if ( cur->type == BEGIN ) {
        // 前方並未存在S-expression
        cur->type = LEFT_PAREN;
        cur->value = "(";
        cur->right = new ASTNode();
        cur->right->type = TEMP;
        ast.push_back(cur->right);
        return;
    }
    
    cur->left = new ASTNode();
    cur->right = new ASTNode();
    cur->right->type = TEMP; // 預設該位置為AST結尾節點
    if ( cur->type != LEFT_PAREN ) cur->type = TEMP; // debug
    ast.push_back(cur->right); // 留存前一個S-expression目前的結尾節點
    
    cur = cur->left;   // 新的S-expression起點
    cur->type = "LEFT_PAREN";
    cur->value = "(";
}

void Function::backExpression() {
    // cout << "backExpression" << endl; // debug
    // 跳出外圈S-expression的位置，因該內部S-expression以結束
    // 須回到上層AST結尾節點
    do {
        if ( cur->type == TEMP || cur->type == "QUOTE_TEMP" ) cur->type = END; // debug
        if ( cur->type == "QUOTE_TEMP" ) --quote;
        cur = ast.back();
        ast.pop_back();
    } while ( !ast.empty() && ast.back()->type == "QUOTE_TEMP" );
}

void Function::quoteExpression() {

    if ( cur->type == BEGIN ) {
        // 前方並未存在S-expression
        cur->type = LEFT_PAREN;
        cur->value = "(";
        cur->right = new ASTNode();
        cur->right->type = "QUOTE_TEMP";
        ast.push_back(cur->right);
        return;
    }

    cur->left = new ASTNode();
    cur->right = new ASTNode();
    cur->right->type = "QUOTE_TEMP";
    ast.push_back(cur->right); // 留存前一個S-expression目前的結尾節點

    cur = cur->left;   // 新的S-expression起點
    cur->type = "LEFT_PAREN";
    cur->value = "(";
}

bool Function::printAST() {
    // 印出AST
    if ( root->type == BEGIN ) return false;
    cout << "printAST" << endl;
    while ( !ast.empty() ) backExpression();
    stack<pair<ASTNode*, string>> st;
    st.push({root, ""});
    int left_paren = 0, temp = 0;
    bool beforeIsParen = false;
    while ( !st.empty() ) {
        ASTNode *cur = st.top().first;
        string direction = st.top().second;
        st.pop();
        if ( cur->type != LINK && cur->type != TEMP && cur->type != "QUOTE_TEMP" && cur->value != ".(" ) {
            if ( cur->type == END ) --left_paren; // 結束一個S-expression，則縮排
            if ( !beforeIsParen ) {
                if ( direction == "right" && cur->type != END && cur->value != "(" ) {
                    for ( int i = 0; i < left_paren; ++i ) cout << "  ";
                    cout << "." << endl;
                }
                for ( int i = 0; i < left_paren; ++i ) cout << "  ";
            }
                
            else beforeIsParen = false;

            if ( cur->value == "(" ) {
                ++left_paren;
                cout << cur->value << " ";
                beforeIsParen = true;
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