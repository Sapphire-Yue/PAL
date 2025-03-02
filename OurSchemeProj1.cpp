#include<iostream>
#include <queue>
#include <stack>

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
        void newExpression(ASTNode **root, vector<ASTNode*> &ast);
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
    vector<ASTNode*> ast;
    string token;
    for ( auto &c : expr ) {
        if ( c == '(' ) {
            ++left_paren;
            if ( !token.empty() ) {
                // 遇到左括弧時，還有資料未寫入AST
                storeTokenInAST(token);
                token.clear();
            }
            function.newExpression(&cur, ast);
            cur->value = "("; // 其中一個S-expression的開頭
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
                token.clear();
                continue; // 左右括弧只為單一Atom，該S-expression可能並未結束
            }
            else if ( !token.empty() ) {
                // 遇到右括弧時，還有資料未寫入AST
                storeTokenInAST(token);
            }
            token.clear();
            cur->value = ")"; // 其中一個S-expression的結尾
            
            // 一個S-expression結束
            cur = ast.back();
            ast.pop_back();

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

    // 整個S-expression結束
    cur = root;
    function.optimizeAST(root);
    delete root;
    root = new ASTNode();
    cur = root;
}

void Reader::storeTokenInAST(string &token) {
    cout << "stroeTokenInAST" << endl;
    // 確認Token型態並更新其內容
    function.checkTypeOfToken(token);
    cout << "token: " << token << endl; // debug

    if ( token == "." ) {
        cur->value = "DOT";
        cur->left = NULL;
        cur->right = new ASTNode();
        cur = cur->right;

        // 預設該位置為AST結尾節點
        cur->value = "END";
        return;
    }
    else if ( token == "'") {
        cur->value = "QUOTE";
        cur->left = NULL;
        cur->right = new ASTNode();
        cur = cur->right;

        // 預設該位置為AST結尾節點
        cur->value = "END";
        return;
    }
    else if ( token == "nil" && cur->value == "(" ) {
        cur->value = "nil"; // 左右括弧成立，則該位置為nil

        cur->left = NULL;
        cur->right = new ASTNode();
        cur = cur->right;

        // 預設該位置為AST結尾節點
        cur->value = "END";
        return;
    }

    // 將1筆token存入新節點
    ASTNode *new_node = new ASTNode();
    new_node->value = token;
    new_node->left = NULL;
    new_node->right = NULL;

    // 將新節點存入AST，並更新目前節點位置
    if ( cur->value == "END" )
        cur->value = "LINK"; // debug
    cur->left = new_node;
    cur->right = new ASTNode();
    cur = cur->right;

    // 預設該位置為AST結尾節點
    cur->value = "END";
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
    if ( token == "'" ) {
        token = "quote";
        return true;
    }
    return false;
}

void Function::newExpression(ASTNode **root, vector<ASTNode*> &ast) {
    cout << "newExpression" << endl;
    ASTNode **cur = root;
    (*cur)->value = "LINK";
    (*cur)->left = new ASTNode();
    (*cur)->right = new ASTNode();
    ast.push_back((*cur)->right); // 留存前一個S-expression的結尾節點
    *root = (*cur)->left;   // 新的S-expression起點
}

void Function::optimizeAST(ASTNode *root) {
    // 整理AST
    cout << "optimizeAST" << endl;
    leafToRoot(root, 0);
    bool dot = false;
    while ( !st.empty() ) {
        ASTNode *cur = st.top().first;
        int level = st.top().second;
        cout << cur->value << " " << level << endl;
        st.pop();

        if ( !dot && cur->value == "DOT"  ) {
            if ( cur->right->value == "LINK" ) {
                ASTNode *tmp = cur->right;
                cur->right = tmp->right;
                cur->left = tmp->left;
                
            
                tmp->right = NULL;
                tmp->left = NULL;
                delete tmp;
                dot = true;
            }
        }
    }

    printAST(root);
}

void Function::leafToRoot(ASTNode *root, int level) {

    // 將AST由葉節點往根節點整理
    st.push({root, level});
    if ( root->left ) leafToRoot(root->left, level + 1);
    if ( root->right ) leafToRoot(root->right, level + 1);
}

void Function::printAST(ASTNode *root) {
    // 印出AST
    cout << "printAST" << endl;
    stack<ASTNode*> st;
    st.push(root);
    while ( !st.empty() ) {
        ASTNode *cur = st.top();
        st.pop();
        if ( cur->value == "END" || cur->value == ")" ) cout << ")";
        else cout << cur->value << " ";
        if ( cur->right ) st.push(cur->right);
        if ( cur->left ) st.push(cur->left);
    }
    cout << endl;
}