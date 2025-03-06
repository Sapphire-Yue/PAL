#include <iostream>
#include <queue>
#include <stack>

using namespace std;

struct ASTNode {
    string type, value;
    int line, column;
    ASTNode *left;
    ASTNode *right;
};

class Function {
    public:
        Function();
        ~Function();
        void checkTypeOfToken(string &token, string &type);
        bool isINT(string &token);
        bool isDOT(string &token);
        bool isSTRING(string &token);
        bool isFLOAT(string &token);
        bool isNIL(string &token);
        bool isT(string &token);
        bool isQUOTE(string &token);
        void newExpression(ASTNode **cur, vector<ASTNode*> &ast, bool &dot);
        void optimizeAST(ASTNode *root);
        void leafToRoot(ASTNode *root, int level);
        void printAST(ASTNode *root);
    private:
        string input;
        stack<pair<ASTNode*, int>> st;
};

class Reader {
    public:
        Reader();
        ~Reader();
        void read();
        void readNextLine(string &expr);
        bool readNextExpr(string &expr);
        void saperateExprToToken(string &expr, int &left_paren);
        void storeTokenInAST(string &token);
    private:
        Function function;
        ASTNode *root;
        ASTNode *cur;
        vector<ASTNode*> ast;
        bool dot;
};

int main() {
    //string num;
    //getline(cin, num);
    Reader reader;
    reader.read();
    return 0;
}

Reader::Reader() {
    root = new ASTNode();
    root->type = "BEGIN";
    root->value = "";
    function = Function();
    dot = false;
    cout << "Reader created" << endl;
}

Reader::~Reader() {
    cout << "Reader destroyed" << endl;
}

void Reader::read() {

    cout << "Welcome to OurScheme!" << endl;
    cout << "\n> ";
    char ch;    // 讀入字元
    string expr;
    cur = root;
    while ( true ) {
        // 重複讀取下一個S-expression
        try {
            if ( !readNextExpr(expr) ) break; // exit
        }
        catch ( const char *msg ) {
            cout << msg;
            break; // EOF
        }
    }
    cout << "\nThanks for using OurScheme!" << endl;
    cout.flush(); // 避免緩衝區輸出並未顯示
}

void Reader::readNextLine(string &expr) {
    // 讀取下一行
    do {
        if ( !getline(cin, expr) ) throw "ERROR (no more input) : END-OF-FILE encountered";
    } while ( expr.empty() ); // 該行並未輸入資料
    cout << "You entered: " << expr << endl;

}

bool Reader::readNextExpr(string &expr) {
    // 讀取下一個S-expression
    cout << "readNextExpr" << endl;
    int left_paren = 0; // 紀錄左括弧數量
    try {
        readNextLine(expr);
    }
    catch ( const char *msg ) {
        throw msg;
    }
    if ( expr == "(exit)" ) return false;
    saperateExprToToken(expr, left_paren);
    return true; 
}

void Reader::saperateExprToToken(string &expr, int &left_paren) {
   // cout << "saperateExprToToken" << endl; // debug
    int nil = 0; // 判斷是否為空括弧
    int line = 0, column = 0; // 紀錄目前字元位置
    bool str = false, backslash = false, annotation = false; // 判斷是否為字串與反斜線或註解
    string token;
    
    for ( auto &c : expr ) {
        if ( !str && c == '(' ) {
            ++left_paren;
            if ( !token.empty() ) {
                // 遇到左括弧時，還有資料未寫入AST
                storeTokenInAST(token);
                token.clear();
            }
            function.newExpression(&cur, ast, dot);
            nil = 0;
        }
        else if ( !str && c == ')' ) {
            --left_paren;
            if ( !nil && token.empty() ) {
                // 遇到空括弧，將nil存入AST
                cout << "nil" << endl;
                token = "nil";
                storeTokenInAST(token);
                ++nil;
                token.clear();

                cur = ast.back();
                ast.pop_back();
                continue; // 左右括弧只為單一Atom，該S-expression可能並未結束
            }
            else if ( !token.empty() ) {
                // 遇到右括弧時，還有資料未寫入AST
                storeTokenInAST(token);
            }
            token.clear();
            // cur->type = ")"; // 其中一個S-expression的結尾
            
            // 一個S-expression結束，且還有S-expression未結束
            if ( left_paren >= 1 ) {
                cur = ast.back();
                ast.pop_back();
            }
            
            if ( !left_paren ) {
                // 整個S-expression結束
                line = 0, column = 0; // 重新另一個S-expression
                function.optimizeAST(root); //debug
                //function.printAST(root);
                delete root;
                root = new ASTNode();
                cur = root;
                cur->value = "END";
                cout << "> ";
            }

        }
        else if ( !str && c == '\'' ) {
            // 遇到引號，將quote存入AST
            token = "'";
            storeTokenInAST(token);
            token.clear();
        }
        else if ( !str && c == ' ' ) {
            // 遇到空白，將token存入AST
            if ( token.empty() ) continue;
            storeTokenInAST(token);
            token.clear();
            ++nil; // 該括弧並不為空
        }
        else if ( !str && c == ';') {
            // 遇到註解，則該行後面的資料皆為註解
            return;
        }
        else if ( c == '\"' ) {
            // 遇到雙引號，將字串存入AST
            token += c; 
            ++column;
            if ( str && !backslash ) {
                // 若遇到反斜線則將雙引號公用消除，反之則該字串結束
                storeTokenInAST(token);
                token.clear();
                str = false;
            }
            else str = true;
        } 
        else {
            if ( c == '\\') backslash = true; // 遇到反斜線
            else backslash = false;
            token += c;
            ++column;
        }
    }

    if ( !token.empty() ) {
        storeTokenInAST(token);
        token.clear();
    }
    if ( str ) {
        string error = "ERROR (no closing quote) : END-OF-LINE encountered at Line " + to_string(line) + " Column " + to_string(column + 1);
        cout << error;
        delete root;
        root = new ASTNode();
        cur = root;
        root->type = "BEGIN";
        root->value = "";
        cout << "> ";
        return;
    }

    if ( left_paren ) return; // 未結束，繼續讀取下一行

    if ( root->value != "END" ) {
        // 整個S-expression結束
        function.optimizeAST(root);
        //function.printAST(root);
        delete root;
        root = new ASTNode();
        cur = root;
        cout << "> ";
    }

    root->type = "BEGIN";
    root->value = ""; // 內容為空
}

void Reader::storeTokenInAST(string &token) {
    // cout << "stroeTokenInAST" << endl; // debug
    // 確認Token型態並更新其內容
    string type = "SYMBOL";
    function.checkTypeOfToken(token, type);
    cout << "token: " << token << " " << type << endl; // debug

    if ( token == "." ) {
        dot = true;
        return;
    }
    else if ( token == "'") {
        cur->type = type;
        cur->value = "quote";
        cur->left = NULL;
        cur->right = new ASTNode();
        cur = cur->right;

        // 預設該位置為AST結尾節點
        cur->type = "END";
        return;
    }
    else if ( token == "nil" && cur->value == "(" ) {
        cur->type = type; // 左右括弧成立，則該位置為nil
        cur->value = "nil"; 

        cur->left = NULL;
        cur->right = NULL;
        return;
    }

    // 將1筆token存入新節點

    if ( dot ) {
        // 若前一個token為DOT，則將該token存入右節點
        cur->value = token;
        cur->type = type;
        cur->left = NULL;
        cur->right = new ASTNode();
        cur->right->type = "END"; // 預設該位置為AST結尾節點
        dot = false;
        return;
    }

    ASTNode *new_node = new ASTNode();
    new_node->value = token;
    new_node->type = type;
    new_node->left = NULL;
    new_node->right = NULL;

    if ( cur->type == "BEGIN" ) {
        // 若當前token為空，則將該token存入左節點
        cur->value = token;
        cur->type = type;
        cur->left = NULL;
        cur->right = NULL;
        return;
    }
    // 將新節點存入AST，並更新目前節點位置
    if ( cur->type == "END" ) 
        cur->type = "LINK"; // debug
    cur->left = new_node;
    cur->right = new ASTNode();
    cur = cur->right;

    // 預設該位置為AST結尾節點
    cur->type = "END";
}

Function::Function() {
    cout << "Function created" << endl;
}

Function::~Function() {
    cout << "Function destroyed" << endl;
}

void Function::checkTypeOfToken(string &token, string &type) {
    // 分辨Token的型態
    // cout << "checkTypeOfToken" << endl; // debug
    if ( isINT(token) ) type = "INT";
    else if ( isDOT(token) ) type = "DOT";
    else if ( isSTRING(token) ) type = "STRING";
    else if ( isFLOAT(token) ) type = "FLOAT";
    else if ( isNIL(token) ) type = "NIL";
    else if ( isT(token) ) type = "T";
    else if ( isQUOTE(token) ) type = "QUOTE";
    else type = "SYMBOL";
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

bool Function::isSTRING(string &token) {
    /*檢查Token是否為字串*/
    if ( token[0] != '\"' ) return false;
    string rebuild = "";
    bool backslash = false;
    for ( auto &c : token ) {
        if ( c == '\\' && !backslash ) backslash = true;
        else {
            if ( backslash ) {
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
            }
            else rebuild += c;
            backslash = false;
        }
    }
    token = rebuild;
    return true;
}

bool Function::isFLOAT(string &token) {
    /*檢查Token是否為浮點數*/
    int len = token.size(), decimal = 0;
    string num;
    bool dot = false, interger = false, round = false;

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
            if ( decimal >= 3 ) {
                // 小數點後最多3位
                if ( decimal == 3 && token[i] >= '5' ) round = true; // 四捨五入
                continue;
            } 
            num += token[i];
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
    token = num;
    return true;
}

bool Function::isNIL(string &token) {
    /*檢查Token是否為空*/
    if ( token == "nil" || token == "#f" ) {
        token = "nil";
        return true;
    }
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
    if ( token == "'" ) {
        token = "quote";
        return true;
    }
    return false;
}

void Function::newExpression(ASTNode **cur, vector<ASTNode*> &ast, bool &dot) {
    // cout << "newExpression" << endl; // debug
    // 遇見新的左括弧，即產生新的Exprssion

    if ( dot ) {
        // dot後面將接上另一個具有括弧的S-Expression
        (*cur)->value = ".(";
        ast.push_back((*cur)); 
        dot = false;
        return;
    }

    if ( (*cur)->type == "BEGIN" ) {
        // 前方並未存在S-expression
        (*cur)->type = "LEFT_PAREN";
        (*cur)->value = "(";
        ast.push_back((*cur)->right);
        return;
    }
    
    if ( (*cur)->type != "LEFT_PAREN" ) (*cur)->type = "LINK"; // debug
    (*cur)->left = new ASTNode();
    (*cur)->right = new ASTNode();
    (*cur)->right->type = "END"; // 預設該位置為AST結尾節點
    ast.push_back((*cur)->right); // 留存前一個S-expression目前的結尾節點
    
    *cur = (*cur)->left;   // 新的S-expression起點
    (*cur)->value = "(";
}

void Function::printAST(ASTNode *root) {
    // 印出AST
    cout << "printAST" << endl;
    stack<pair<ASTNode*, string>> st;
    st.push({root, ""});
    int left_paren = 0, temp = 0;
    bool beforeIsParen = false;
    while ( !st.empty() ) {
        ASTNode *cur = st.top().first;
        string direction = st.top().second;
        st.pop();
        if ( cur->type != "LINK" && cur->value != ".(" ) {
            if ( cur->type == "END" ) --left_paren; // 結束一個S-expression，則縮排
            if ( !beforeIsParen ) {
                if ( direction == "right" && cur->type != "END" && cur->value != "(" ) {
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
            else if ( cur->type == "END" ) cout << ")" << endl;
            else cout << cur->value << endl;
        }
        
        // 遍歷子樹
        if ( cur->right ) st.push({cur->right, "right"});
        if ( cur->left ) st.push({cur->left, "left"});
    }
    cout << endl;
}

void Function::optimizeAST(ASTNode *root) {
    // 整理AST
    cout << "optimizeAST" << endl;
    leafToRoot(root, 0);
    bool dot = false;
    /*while ( !st.empty() ) {
        ASTNode *cur = st.top().first;
        int level = st.top().second;
        cout << cur->type << " " << level << endl;
        st.pop();
        if ( cur->type == "DOT" && !dot ) {
            ASTNode *tmp = cur->right;
            cur->right = tmp->right;
            cur->left = tmp->left;
                
            tmp->right = NULL;
            tmp->left = NULL;
            delete tmp;
            dot = true;
        }
        else if ( cur->type == "DOT" ) {
            ASTNode *tmp = cur->right;
            cur->type = tmp->type;
            cur->right = tmp->right;
            cur->left = tmp->left;
                
            tmp->right = NULL;
            tmp->left = NULL;
            delete tmp;
        }
    }*/

    printAST(root);
}

void Function::leafToRoot(ASTNode *root, int level) {

    // 將AST由葉節點往根節點整理
    cout << root->type << " " << level << endl;
    st.push({root, level});
    if ( root->left ) leafToRoot(root->left, level + 1);
    if ( root->right ) leafToRoot(root->right, level + 1);
    
}