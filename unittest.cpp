#include <filesystem>
#include <FlexLexer.h> 
#include "y.tab.h"
#include "logic/logic.h"
#include "./ast/ast.h"

#define DEBUG 0

using namespace robotlogic;
using namespace ast;

extern FILE *yyin;
extern void yyrestart(FILE*);
extern void reset_variables();

int engage_test(std::string filename) {
    reset_variables();
    FILE* customInput;
    int code;
    customInput = fopen(filename.c_str(), "r");
    if (!customInput) {
        std::cerr << "Error opening input file\n";
        return -1;
    }
    yyin = customInput;
    yyrestart(yyin);
    std::cout << "[Test " << filename << " executed with following output]\n\n";
    try{
        code = yyparse();
    }
    catch(LogicException &e){
        std::cerr << "Terminated with a LogicException with message:\n" << e.what() << std::endl;
        code = 2;
    }
    return code;
}

int main(){
    std::vector<std::string> filenames;
    for (auto& p : std::filesystem::directory_iterator("./units")) {
        if (p.is_regular_file()) {
            if (p.path().extension().string() == ".txt") {
                filenames.push_back(p.path().string());
            }
        }
    }
    std::cout << "List of tests:" << std::endl;
    std::map<std::string, int> testcode;
    for(const auto& p : filenames){
        std::cout << p << std::endl;
        testcode.insert(std::make_pair(p, 0));
    }
    yydebug = DEBUG;
    int code;
    for(const auto& p : filenames){
        code = engage_test(p);
        testcode.find(p)->second = code;
        std::cout << "\n[Exit code: " << code << "]\n\n";
    }
    std::cout << "Results:\n";
    for(const auto&p : testcode)
        std::cout << "Test: [" << p.first << "] Code: " << p.second << (p.second != 0 ? " !" : "") << "\n";
}