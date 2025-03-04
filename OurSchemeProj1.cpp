#include <iostream>
#include <queue>
#include <stack>

using namespace std;

struct ASTNode {
    string type;
    int line, column;
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
        void newExpression(ASTNode **cur, vector<ASTNode*> &ast);
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
};

int main() {
    Reader reader;
    reader.read();
    return 0;
}

Reader::Reader() {
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
    while ( true ) {
        // 重複讀取下一個S-expression
        try {
            if ( !readNextExpr(expr) ) break; // exit
        }
        catch ( const char *msg ) {
            cout << msg << endl;
            break; // EOF
        }
    }
    cout << "Thanks for using OurScheme!" << endl;
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
    bool str = false; // 判斷是否為字串
    string token;
    
    for ( auto &c : expr ) {
        if ( !str && c == '(' ) {
            ++left_paren;
            if ( !token.empty() ) {
                // 遇到左括弧時，還有資料未寫入AST
                storeTokenInAST(token);
                token.clear();
            }
            function.newExpression(&cur, ast);
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
                function.optimizeAST(root);
                delete root;
                root = new ASTNode();
                cur = root;
                cur->type = "END";
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
        else if ( c == '\"' ) {
            // 遇到雙引號，將字串存入AST
            token += c;
            if ( str ) {
                storeTokenInAST(token);
                token.clear();
                str = false;
            }
            else str = true;
        } 
        else token += c;
    }

    if ( !token.empty() ) {
        storeTokenInAST(token);
        token.clear();
    }

    if ( left_paren ) return; // 未結束，繼續讀取下一行

    if ( root->type != "END" ) {
        // 整個S-expression結束
        //function.optimizeAST(root);
        function.printAST(root);
        delete root;
        root = new ASTNode();
        cur = root;
        cout << "> ";
    }

    root->type = "END"; // 內容為空，即起點為終點
}

void Reader::storeTokenInAST(string &token) {
    // cout << "stroeTokenInAST" << endl; // debug
    // 確認Token型態並更新其內容
    function.checkTypeOfToken(token);
    cout << "token: " << token << endl; // debug

    if ( token == "." ) {
        cur->type = "DOT";
        cur->left = NULL;
        cur->right = new ASTNode();
        cur = cur->right;

        // 預設該位置為AST結尾節點
        cur->type = "END";
        return;
    }
    else if ( token == "'") {
        cur->type = "QUOTE";
        cur->left = NULL;
        cur->right = new ASTNode();
        cur = cur->right;

        // 預設該位置為AST結尾節點
        cur->type = "END";
        return;
    }
    else if ( token == "nil" && cur->type == "(" ) {
        cur->type = "nil"; // 左右括弧成立，則該位置為nil

        cur->left = NULL;
        cur->right = new ASTNode();
        cur = cur->right;

        // 預設該位置為AST結尾節點
        cur->type = "END";
        return;
    }

    // 將1筆token存入新節點
    ASTNode *new_node = new ASTNode();
    new_node->type = token;
    new_node->left = NULL;
    new_node->right = NULL;

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

void Function::checkTypeOfToken(string &token) {
    // 分辨Token的型態
    // cout << "checkTypeOfToken" << endl; // debug
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
    if ( token == "'" ) {
        token = "quote";
        return true;
    }
    return false;
}

void Function::newExpression(ASTNode **cur, vector<ASTNode*> &ast) {
    // cout << "newExpression" << endl; // debug
    // 遇見新的左括弧，即產生新的Exprssion
    
    if ( (*cur)->type == "" ) {
        // 前方並未存在S-expression
        (*cur)->type = "(";
        return;
    }
    (*cur)->type = "LINK"; // debug
    (*cur)->left = new ASTNode();
    (*cur)->right = new ASTNode();
    (*cur)->right->type = "END"; // 預設該位置為AST結尾節點
    ast.push_back((*cur)->right); // 留存前一個S-expression目前的結尾節點
    *cur = (*cur)->left;   // 新的S-expression起點
    (*cur)->type = "(";
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

void Function::printAST(ASTNode *root) {
    // 印出AST
    cout << "printAST" << endl;
    stack<ASTNode*> st;
    st.push(root);
    int left_paren = 0, temp = 0;
    bool beforeIsParen = false;
    while ( !st.empty() ) {
        ASTNode *cur = st.top();
        st.pop();
        if ( cur->type != "LINK" ) {
            if ( cur->type == "END" ) --left_paren; // 結束一個S-expression，則縮排
            if ( !beforeIsParen )
                for ( int i = 0; i < left_paren; ++i ) cout << "  ";
            else beforeIsParen = false;

            if ( cur->type == "(" ) {
                ++left_paren;
                cout << cur->type << " ";
                beforeIsParen = true;
            }
            else if ( cur->type == "END" ) cout << ")" << endl;
            else cout << cur->type << endl;
        }
        
        // 遍歷子樹
        if ( cur->right ) st.push(cur->right);
        if ( cur->left ) st.push(cur->left);
    }
    cout << endl;
}