#include "ast.h"
#include <sstream>

namespace ast {

    void Node::execute()
    {
        //std::cout << "TYPE: " << type << std::endl;
        if(type == ARG) return;
        if(type == COND) {
            ch[0]->execute();
            if(!Conv::itob(Conv::stoi(ch[0]->arg))) return;
            ch[1]->execute();
            return;
        }
        if(type == LOOP) {
            Node *start = ch[0], *end = ch[1], *step = ch[2], *func = ch[3];
            int l = Conv::stoi(Ast::copy_and_execute(start)), r = Conv::stoi(Ast::copy_and_execute(end));
            //std::cout << l << " to " << r << std::endl;
            while(l <= r){
                Ast::copy_and_execute(func);
                l += Conv::stoi(Ast::copy_and_execute(step));
                //std::cout << l << std::endl;
                r = Conv::stoi(Ast::copy_and_execute(end));
            }
            return;
        }
        if(type == FUNCTION){
            auto f = funcs.find(ch[0]->arg);
            if(f == funcs.end()){
                std::string msg = std::string("Function \"") + std::string(ch[0]->arg) + std::string("\" is not found");
                throw LogicException(msg.c_str());
            }
            *this = *copy(f->second);
        }
        for(auto &it : ch) it->execute();
        int a1, a2, intarg;
        std::istringstream iss(arg);
        std::string s;
        std::stack<std::string> localvars;
        Variable* v;
        std::vector<int> dims;
        switch (type)
        {
        case RVAR:
            if(arg == "l") break;
            for(auto it : ch[1]->ch) dims.push_back(Conv::stoi(it->arg));
            type = ARG;
            arg = Conv::itos((*(vars[ch[0]->arg]))[dims]);
            break;
        case INITVAR:
            for(auto it : ch[2]->ch) dims.push_back(Conv::stoi(it->arg));
            vars.emplace(ch[0]->arg, dims, Conv::stot(ch[1]->arg));
            break;
        case ASGNEL:
            //std::cout << Conv::stoi(ch[2]->arg) << std::endl;
            for(auto it : ch[0]->ch[1]->ch) dims.push_back(Conv::stoi(it->arg));
            if(arg == "<") (*(vars[ch[0]->ch[0]->arg]))[dims] = Conv::stoi(ch[1]->arg);
            if(arg == "+") (*(vars[ch[0]->ch[0]->arg]))[dims] += Conv::stoi(ch[1]->arg);
            if(arg == "-") (*(vars[ch[0]->ch[0]->arg]))[dims] -= Conv::stoi(ch[1]->arg);
            //std::cout << "Assigned " << ch[0]->ch[0]->arg << " " << (*(vars[ch[0]->ch[0]->arg]))[dims] << std::endl;
            break;
        case ASGNVAR:
            *(vars[ch[0]->arg]) = *(vars[ch[1]->arg]);
            break;
        case GO:
            arg = Conv::itos(Conv::btoi(robo.go(Conv::stodir(arg)))); 
            //std::cout << "Moved " << arg << std::endl;
            break;
        case PICK:
            arg = Conv::itos(Conv::btoi(robo.pick(Conv::stodir(arg))));
            break;
        case DROP:
            arg = Conv::itos(Conv::btoi(robo.drop(Conv::stodir(arg))));
            break;
        case LOOK:
            if(!vars.contains("lres")) vars.insert(std::make_pair("lres", new Variable({robo.get_rad_of_view()}, CELL)));
            if(!vars.contains("lsize")) vars.insert(std::make_pair("lsize", new Variable({1}, CELL)));
            *(vars["lres"]) = robo.look(Conv::stodir(arg));
            (*(vars["lsize"]))[{0}] = (*(vars["lres"])).get_dims()[0];
            arg = "1";
            break;
        case PRNT:
            robo.print_room();
            arg = "1";
            break;
        case EXPR:
            type = ARG;
            switch (arg[0])
            {
                case '-':
                    intarg = - Conv::stoi(ch[0]->arg);
                    break;
                case '!':
                    intarg = Conv::btoi(!Conv::itob(Conv::stoi(ch[0]->arg)));
                    break;
                case '+':
                    intarg = Conv::stoi(ch[0]->arg) + (Conv::stoi(ch[1]->arg));
                    break;
                case '&':
                    intarg = Conv::btoi(Conv::itob(Conv::stoi(ch[0]->arg)) & Conv::itob(Conv::stoi(ch[1]->arg)));
                    break;
                case '|':
                    intarg = Conv::btoi(Conv::itob(Conv::stoi(ch[0]->arg)) | Conv::itob(Conv::stoi(ch[1]->arg)));
                    break;
                case '=':
                    intarg = Conv::stoi(ch[0]->arg) == Conv::stoi(ch[1]->arg) ? 1 : 0;
                    break;
                case 'L':
                    intarg = Conv::stoi(ch[0]->arg) < Conv::stoi(ch[1]->arg) ? 1 : 0;
                    break;
                case 'I':
                    intarg = vars[ch[1]->arg]->in(Conv::stoi(ch[0]->arg));
                    break;
                case '*':
                    intarg = Conv::stoi(ch[0]->arg) * (Conv::stoi(ch[1]->arg));
                    break;
                case '/':
                    intarg = Conv::stoi(ch[0]->arg) / (Conv::stoi(ch[1]->arg));
                    break;
                default:
                    break;
            }
            arg = Conv::itos(intarg);
            break;
        case CONCAT:
            while (std::getline(iss, s, ' ')) {
                localvars.push(s);
            }
            while(!localvars.empty()) {
                vars.erase(localvars.top());
                localvars.pop();
            }
            type = ARG;
            if(ch[ch.size()-1]->type == ARG){
                arg = ch[ch.size()-1]->arg;
            } else
                arg = "0";
            break;
        case ESC: 
            type = ARG;
            arg = Conv::itos(Conv::btoi(robo.is_exit()));
            break;
        default:
            break;
        }
    }

    Node *Node::copy(Node *root)
    {
        if(!root) return nullptr;
        Node *n = new Node(*root);
        for(auto &it : root->ch){
            n->add_ch(copy(it));
        }
        return n;
    }

    void Ast::copy(const Ast &other)
    {
        root = Node::copy(other.root);
    }
    
    std::string Ast::copy_and_execute(Node *root)
    {
        auto res = std::string();
        Node* p = Node::copy(root);
        p->execute();
        res = p->arg;
        return res;
    }
}