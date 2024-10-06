%{
    #include "logic/logic.h"
    #include <FlexLexer.h>
    #include "ast/ast.h"
    using namespace robotlogic;
    using namespace ast;

    #define FILENAME "input/map.txt"

    extern FILE* yyin;
    extern int yylex();

    void yyerror(const char*);
    void reset_variables();

    int sx, sy;
    std::vector<std::vector<cell>> map; // = get_map(FILENAME, sx, sy);
    Robot robo; //(sx, sy, 2, map);
    VarContainer vars;
    std::string type;
    std::string name, lname;
    Ast tree;
    bool accumulate;
    std::string funcname;
    std::map<std::string, Node*> funcs;
    NodeFactory nf(vars, robo, funcs);
    int line = 0;
%}



%union {
  int i;
  char* s;
  unsigned long p;
}

%token VALUE VALINT VALBOOL VALCELL

%token VARIABLENAME
%token TYPENAME
%token UNEXP
%token DIMIND
%token ENDL
%token ASOPERATOR
%token COMMAND
%token DIR
%token PRINT
%token IF DO FROM TO STEP
%token FUNCKW 
%token FUNCEND
%token OUTPUT
%token COMMENT
%token ESCAPE

%token ALL SOME IN LESS

%%

program:
        program ENDL {line++;}
        | program initVar ENDL { line++; if(!accumulate) {tree.execute(); tree.clear();} }
        | program assignment ENDL { line++; if(!accumulate) {tree.execute(); tree.clear();} }
        | program expr ENDL { line++;
                tree.insert(reinterpret_cast<Node*>($<p>2));
                if(!accumulate){
                    tree.execute();
                    if(!(reinterpret_cast<Node*>($<p>2))->arg.empty()) 
                        std::cout << "value: " <<  ((reinterpret_cast<Node*>($<p>2))->arg) << std::endl; 
                    tree.clear();
                }
        }
        | program '.' expr ENDL { line++;
                tree.insert(reinterpret_cast<Node*>($<p>3));
                if(!accumulate){
                    tree.execute();
                    tree.clear();
                }
        }
        | program function ENDL { line++; }
        | program end ENDL { line++; }
        | program do ENDL { line++;
            tree.insert(reinterpret_cast<Node*>($<p>2));
            if(!accumulate) { tree.execute(); tree.clear();}
        }
        | program if ENDL { line++; if(!accumulate) {tree.execute(); tree.clear();} }
        | program for ENDL { line++; if(!accumulate) {tree.execute(); tree.clear();} }
        | program output ENDL { line++; if(!accumulate) {tree.execute(); tree.clear();} }
        | program COMMENT ENDL { line++; }
        | {}
        ;
dimind:
    expr {
        Node *n = nf.make_node(Node::INDEX);
        n->add_ch(reinterpret_cast<Node*>($<p>1));
        $<p>$ = reinterpret_cast<unsigned long>(n);
        }
    | dimind ',' expr {
        Node *n = reinterpret_cast<Node*>($<p>1);
        n->add_ch(reinterpret_cast<Node*>($<p>3));
        $<p>$ = reinterpret_cast<unsigned long>(n);
        }
;
initVar:
        TYPENAME {type = std::string($<s>1);}
        VARIABLENAME {lname = std::string($<s>3);}
        '[' dimind ']' {
            Node* typen = nf.make_node(Node::ARG, type);
            Node* namen = nf.make_node(Node::ARG, lname);
            Node* indn = reinterpret_cast<Node*>($<p>6);
            Node* n = nf.make_node(Node::INITVAR, "", {namen, typen, indn});
            tree.insert(n);
        }
        ;
assignment:
        lvalue ASOPERATOR expr {
            std::string arg;
            arg.push_back(*($<s>2));
            Node* varn = reinterpret_cast<Node*>($<p>1);
            varn->arg = "l";
            Node* valn = reinterpret_cast<Node*>($<p>3);
            Node* n = nf.make_node(Node::ASGNEL, arg, {varn, valn});
            tree.insert(n);
        }
        | VARIABLENAME ASOPERATOR VARIABLENAME {
            std::string varname;
            for(auto it : std::string($<s>1)){
                if(it == '+' || it == '-') yyerror("Invalid operator for variable - variable assignment");
                if(it == ' ' || it == '<')
                    break;
                varname.push_back(it);
            }
            Node *var1 = nf.make_node(Node::ARG, varname);
            Node *var2 = nf.make_node(Node::ARG, $<s>3);
            Node* n = nf.make_node(Node::ASGNVAR, "", {var1, var2});
            tree.insert(n);
        }
        ;
lvalue:
    VARIABLENAME '[' dimind ']' {
        name = std::string($<s>1);
        size_t found = name.find('[');
        if (found != std::string::npos) {
            name = name.substr(0, found);
        }
        Node *n = nf.make_node(Node::RVAR);
        n->add_ch(nf.make_node(Node::ARG, name));
        n->add_ch(reinterpret_cast<Node*>($<p>3));
        $<p>$ = reinterpret_cast<unsigned long>(n);
    }
;
rvalue:
    VALUE   {
        Node *n = nf.make_node(Node::ARG);
        n->arg = Conv::itos($<i>$);
        $<p>$ = reinterpret_cast<unsigned long>(n);   
    }
    | lvalue {
        $<p>$ = $<p>1;  
        }
    | ESCAPE {
        Node *n = nf.make_node(Node::ESC);
        $<p>$ = reinterpret_cast<unsigned long>(n);
    }
;
exprhp:
    rvalue { $<p>$ = $<p>1; }
    | '{' do '}' { $<p>$ = $<p>2; }
    | command { $<p>$ = $<p>1; }
    | '-' exprhp { 
        Node *n = nf.make_node(Node::EXPR, "-");
        n->ch.push_back(reinterpret_cast<Node*>($<p>2));
        $<p>$ = reinterpret_cast<unsigned long>(n);
    }
    | '!' exprhp { 
        Node *n = nf.make_node(Node::EXPR, "!");
        n->ch.push_back(reinterpret_cast<Node*>($<p>2));
        $<p>$ = reinterpret_cast<unsigned long>(n);
    }
;
exprmp:
    exprhp {$<p>$ = $<p>1;}
    | exprmp '*' exprhp { 
        Node *n = nf.make_node(Node::EXPR, "*");
        n->ch.push_back(reinterpret_cast<Node*>($<p>1));
        n->ch.push_back(reinterpret_cast<Node*>($<p>3));
        $<p>$ = reinterpret_cast<unsigned long>(n);
    }
    | exprmp '/' exprhp { 
        Node *n = nf.make_node(Node::EXPR, "/");
        n->ch.push_back(reinterpret_cast<Node*>($<p>1));
        n->ch.push_back(reinterpret_cast<Node*>($<p>3));
        $<p>$ = reinterpret_cast<unsigned long>(n);
    }
    | exprmp '&' exprhp { 
        Node *n = nf.make_node(Node::EXPR, "&");
        n->ch.push_back(reinterpret_cast<Node*>($<p>1));
        n->ch.push_back(reinterpret_cast<Node*>($<p>3));
        $<p>$ = reinterpret_cast<unsigned long>(n);
    }
;
expr:
    exprmp {$<p>$ = $<p>1;}
    | expr '+' exprmp { 
        Node *n = nf.make_node(Node::EXPR, "+");
        n->ch.push_back(reinterpret_cast<Node*>($<p>1));
        n->ch.push_back(reinterpret_cast<Node*>($<p>3));
        $<p>$ = reinterpret_cast<unsigned long>(n);
    }
    | expr '-' exprmp { 
        Node *n = nf.make_node(Node::EXPR, "+");
        n->ch.push_back(reinterpret_cast<Node*>($<p>1));
        n->ch.push_back(nf.make_node(Node::EXPR, "-"));
        n->ch[1]->ch.push_back(reinterpret_cast<Node*>($<p>3));
        $<p>$ = reinterpret_cast<unsigned long>(n);
    }
    | expr '|' exprmp { 
        Node *n = nf.make_node(Node::EXPR, "|");
        n->ch.push_back(reinterpret_cast<Node*>($<p>1));
        n->ch.push_back(reinterpret_cast<Node*>($<p>3));
        $<p>$ = reinterpret_cast<unsigned long>(n);
    }
    | expr IN VARIABLENAME {        
        Node *n = nf.make_node(Node::EXPR, "I");
        n->ch.push_back(reinterpret_cast<Node*>($<p>1));
        n->ch.push_back(nf.make_node(Node::ARG, $<s>3));
        $<p>$ = reinterpret_cast<unsigned long>(n);
    }
    | expr LESS exprmp { 
        Node *n = nf.make_node(Node::EXPR, "L");
        n->ch.push_back(reinterpret_cast<Node*>($<p>1));
        n->ch.push_back(reinterpret_cast<Node*>($<p>3));
        $<p>$ = reinterpret_cast<unsigned long>(n);
    }
    | expr '=' exprmp { 
        Node *n = nf.make_node(Node::EXPR, "=");
        n->ch.push_back(reinterpret_cast<Node*>($<p>1));
        n->ch.push_back(reinterpret_cast<Node*>($<p>3));
        $<p>$ = reinterpret_cast<unsigned long>(n);
    }
;
command:
    COMMAND DIR {
        dir direct;
        Node *n = nullptr;
        switch($<i>2){
            case 1:
                direct = LEFT;
                break;
            case 2:
                direct = RIGHT;
                break;
            case 3:
                direct = UP;
                break;
            case 4:
                direct = DOWN;
                break;
            default:
                break;
        }
        switch($<i>1){
            case 0:
                n = nf.make_node(Node::GO, Conv::dirtos(direct));
                break;
            case 1:
                n = nf.make_node(Node::PICK, Conv::dirtos(direct));
                break;
            case 2:
                n = nf.make_node(Node::LOOK, Conv::dirtos(direct));
                break;
            case 3:
                n = nf.make_node(Node::DROP, Conv::dirtos(direct));
                break;
            default:
                break;
        }
        $<p>$ = reinterpret_cast<unsigned long>(n);
    }
    | PRINT {
        Node* n = nf.make_node(Node::PRNT, "");
        $<p>$ = reinterpret_cast<unsigned long>(n);
    }
;

function:
    FUNCKW VARIABLENAME { 
        accumulate = true;
        funcname = $<s>2;
        tree.insert(nf.make_node(Node::CONCAT, ""));
        }
;

end:
    FUNCEND {
        accumulate = false;
        Ast exectree;
        if(funcname == "main") {
            exectree = tree;
            exectree.execute();
            exectree.clear();
        }
        funcs.insert(std::make_pair(funcname, tree.decapitate()));
    }
;

do:
    DO VARIABLENAME {
        /*auto f = funcs.find($<s>2);
        if(f == funcs.end()){
            std::string msg = std::string("Function \"") + std::string($<s>2) + std::string("\" is not found");
            yyerror(msg.c_str());
        }
        $<p>$ = reinterpret_cast<unsigned long>(Ast::copy(f->second));*/
        Node*n = nf.make_node(Node::FUNCTION);
        n->add_ch(nf.make_node(Node::ARG, $<s>2));
        $<p>$ = reinterpret_cast<unsigned long>(n);
        
    }
;

if:
    IF expr do {
        Node *n = nf.make_node(Node::COND);
        n->add_ch(reinterpret_cast<Node*>($<p>2));
        n->add_ch(reinterpret_cast<Node*>($<p>3));
        tree.insert(n);
    }
;

for:
    FROM expr TO expr STEP expr do {
        Node* start = reinterpret_cast<Node*>($<p>2);
        Node* end = reinterpret_cast<Node*>($<p>4);
        Node* step = reinterpret_cast<Node*>($<p>6);
        Node* f = reinterpret_cast<Node*>($<p>7);
        Node* p = nf.make_node(Node::LOOP);
        p->add_ch(start);
        p->add_ch(end);
        p->add_ch(step);
        p->add_ch(f);
        tree.insert(p); 
    }
;

output:
    OUTPUT VARIABLENAME {
        vars[$<s>2]->print();
    }
;

%%
void yyerror(const char* msg){
    std::cerr << msg;
    std::cerr << " at line: " << line--;
    std::cerr << std::endl;
}
void reset_variables(){
    std::vector<std::vector<cell>> map = get_map(FILENAME, sx, sy);
    robo = Robot(sx, sy, 2, map);
    vars.clear();
    type, funcname, name, lname = std::string();
    tree = Ast();
    accumulate = false;
    funcs = std::map<std::string, Node*>();
    line = 0;
}
