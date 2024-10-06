#ifndef AST
#define AST
#include "../logic/logic.h"
using namespace robotlogic;

namespace ast {
    
    struct Node
    {
        typedef std::map<std::string, Node*> FunctionContainer;
        enum types{ARG, INITVAR, ASGNEL, CONCAT, GO, PICK, DROP, LOOK, PRNT, EXPR, COND, RVAR, LOOP, INDEX, ASGNVAR, ESC, FUNCTION};
        types type;
        std::string arg;
        std::vector<Node*> ch; 
        VarContainer& vars;
        Robot& robo;
        FunctionContainer& funcs;
        Node(VarContainer& varbs, Robot& r, FunctionContainer& f, types t,
        std::string s = std::string(), std::vector<Node*> args = std::vector<Node*>()) : 
        vars(varbs), ch(args), robo(r), arg(s), type(t), funcs(f) {}
        void add_ch(Node *n) { if(n) ch.push_back(n); }
        void execute();
        Node(const Node& o) :
        vars(o.vars), ch(std::vector<Node*>()), robo(o.robo), arg(o.arg), type(o.type), funcs(o.funcs) {}
        static Node* copy(Node* root);
        Node& operator=(const Node& o) {
            type = o.type;
            arg = o.arg;
            ch = o.ch; 
            vars = o.vars;
            robo = o.robo;
            funcs = o.funcs;
            return *this;
        }
    };

    class NodeFactory{
        private:
        VarContainer& vars;
        Robot& robo;
        Node::FunctionContainer& funcs;
        public:
        NodeFactory(VarContainer& v, Robot& r, Node::FunctionContainer& f) : vars(v), robo(r), funcs(f) {}
        Node* make_node(Node::types t, std::string s = std::string(), std::vector<Node*> args = std::vector<Node*>()){
            return new Node(vars, robo, funcs, t, s, args);
        }
    };

    class Ast
    {
    private:
        Node* root;
        void copy(const Ast& other);
    public:
        Ast() : root(nullptr) {};
        Ast(Ast&& o){
            root = o.root;
            o.root = nullptr;
        }
        
        Ast(const Ast& o){
            copy(o);
        }
        
        Ast& operator=(const Ast &o){
            copy(o);
            return *this;
        }
        
        ~Ast() {clear();}

        static void clear_it(Node *n){
            if(!n) return;
            for(auto it : n->ch)
                clear_it(it);
            delete n;
        }

        void clear() {
            clear_it(root);
            root = nullptr;
        }

        void insert(Node*n) {
            if(!root){
                root = n;
                return;
            } 
            if(root->type == Node::CONCAT){
                root->ch.push_back(n);
                if(n->type == Node::INITVAR){
                    root->arg += (" " + n->ch[0]->arg);
                }
                return;
            }
            n->add_ch(root);
            root = n;
        }

        void insert(Ast&& o){
            insert(o.root);
            o.root = nullptr;
        }

        void execute() { 
            if(!root) return;
            root->execute();
        }

        Node* decapitate() {
            Node *r = root;
            root = nullptr;
            return r;
        }
        static std::string copy_and_execute(Node* root);
    };
}

#endif