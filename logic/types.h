#ifndef ROBOTTYPES
#define ROBOTTYPES
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <stack>

namespace types{
    class LogicException : public std::exception{
    private:
        std::string message;
    public:
        LogicException(const char *m) : message(m) {}
        const char * what () const noexcept override {
            return message.c_str();
        }
    };

    enum cell{EMPTY, WALL, BOX, EXIT, UNDEF};    
    enum vartype{INT, BOOL, CELL};
    enum dir{LEFT, RIGHT, UP, DOWN};

    class Variable {
    private:
        std::vector<int> data;
        std::vector<int> dimensions;
        vartype type;
    public:
        Variable(std::vector<int> dims, vartype t) {
            int totalSize = 1;
            for (int dim : dims) {
                totalSize *= dim;
                dimensions.push_back(dim);
            }
            data = std::vector<int>(totalSize);
            type = t;
        }
        int& operator[](std::vector<int> indices) {
            if(dimensions.size() != indices.size()) throw LogicException("Invalid index dimencity");
            int index = 0;
            int stride = 1;
            for (int i = 0; i < indices.size(); ++i) {
                index += indices[i] * stride;
                stride *= dimensions[i];
            }
            return data[index];
        }
        const int& operator[](std::vector<int> indices) const {
            if(dimensions.size() != indices.size()) throw LogicException("Invalid index dimencity");
            int index = 0;
            int stride = 1;
            for (int i = 0; i < indices.size(); ++i) {
                index += indices[i] * stride;
                stride *= dimensions[i];
            }
            return data[index];
        }
        Variable& operator=(std::vector<cell> cv){
            data.clear();
            for(auto it : cv){
                data.push_back((int)it);
            }
            int size = data.size();
            dimensions = {size};
            return *this;
        }
        vartype gettype() const {return type;}
        const std::vector<int>& get_dims() {return dimensions;}
        int in(int f) const{
            for(const auto& it : data){
                if(it == f) return 1;
            }
            return 0;
        }
        void print() const{
            for(const auto&it : data) std::cout << it << " ";
            std::cout << std::endl;
        }
        void change_dims(std::vector<int> newdims){
            if(dimensions == newdims) return;
            int totalSize = 1;
            dimensions.clear();
            for (int dim : newdims) {
                totalSize *= dim;
                dimensions.push_back(dim);
            }
            data.resize(totalSize);
        }
        void change_type(vartype t){
            if(type == t) return;
            if(t == BOOL)
                for(auto &it : data)
                    it = (it != 0 ? 1 : 0);
            if(t == CELL)
                for(auto &it : data)
                    it = it % 5;
        }
        Variable& operator=(Variable& other){
            change_dims(other.dimensions);
            data = other.data;
            return *this;
        }
    };

    class Conv{
    public:
        static bool itob(int i){
            return i != 0;
        }

        static int btoi(bool b){
            return b ? 1 : 0;
        }

        static bool ctob(cell c){
            return c != UNDEF;
        }

        static cell btoc(bool b){
            if(b) throw LogicException("Conversation from bool with \'true\' value to cell is undefined.");
            return UNDEF;
        }

        static int ctoi(cell c){
            return ctob(c) ? 1 : 0;
        }

        static int stoi(std::string s){
            try{
                int i = std::stoi(s); 
            }catch(...){
                std::cerr << "String: " << s << std::endl;
                throw;
            }
            return std::stoi(s);
        }

        static bool stob(std::string s){
            return s == "true";
        }

        static cell stoc(std::string s){  
            if(s == "EMPTY") return EMPTY;
            if(s == "WALL") return WALL;
            if(s == "BOX") return BOX;
            if(s == "EXIT") return EXIT;
            return UNDEF;
        }

        static cell itoc(int i){
            return btoc(itob(i));
        }
        
        static std::string itos(int i){
            return std::to_string(i);
        }
        
        static vartype stot(std::string s){
            //std::cout << s << std::endl;
            if(s == "INT") return INT;
            if(s == "BOOL") return BOOL;
            return CELL;
        }
        
        static std::string dirtos(dir d){
            std::map<dir, std::string> converter = {
                {LEFT, "LEFT"}, {RIGHT, "RIGHT"}, {UP, "UP"}, {DOWN, "DOWN"}
            };
            return converter[d];
        }

        static dir stodir(std::string s){
            std::map<std::string, dir> converter = {
                {"LEFT", LEFT}, {"RIGHT", RIGHT}, {"UP", UP}, {"DOWN", DOWN}
            };
            return converter[s];
        }

        static std::vector<int> stod(std::string ind){
            std::vector<int> res;
            std::string num;
            for(const auto& it : ind){
                if(it == '[') continue;
                if(it == ']') continue;
                if(it == ','){
                    //std::cout << std::endl << num << std::endl;
                    res.push_back(std::stoi(num));
                    num = "";
                    continue;
                }
                num += it;
            }
            //std::cout << std::endl << num << std::endl;
            res.push_back(std::stoi(num));
            num = "";
            return res;
        }
    };

    class VarContainer {
    private:
        typedef std::map<std::string, Variable*> container_type;
        container_type vars;
    public:
        VarContainer() : vars() {
            vars.insert(std::make_pair("lres", new Variable({}, CELL)));
            vars.insert(std::make_pair("lsize", new Variable({1}, CELL)));
        };
        ~VarContainer(){
            for(auto&it : vars)
                delete it.second;
        }
        container_type& get() {return vars;}
        void insert(std::pair<std::string, Variable*> p){
            //std::string name = p.first;
            //std::cout << "Created " << name << " ";
            //for(auto it : p.second->get_dims()) std::cout << it << ", ";
            //std::cout << std::endl;
            vars.insert(p);
        }
        void emplace(std::string name, std::vector<int> dims, vartype t) {
            auto f = vars.find(name);
            if(f != vars.end()){
                f->second->change_type(t);
                f->second->change_dims(dims);
                return;
            }
            vars.insert(std::make_pair(name, new Variable(dims, t)));
        }
        Variable* operator[](std::string name) {
            auto it = vars.find(name);
            std::string msg = "Variable \"" + name + "\" is not found";
            if(it == vars.end()) throw LogicException(msg.c_str());
            return it->second;
        }
        void erase(std::string k) {
            auto f = vars.find(k);
            if(f != vars.end()){
                delete f->second;
                vars.erase(f);
            }
        }
        void clear() {
            for(auto &it : vars){
                delete it.second;
            }
            vars.clear();
        }
        bool contains(std::string k) {
            auto f = vars.find(k);
            return f != vars.end();
        }
    };

}
#endif