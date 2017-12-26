#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <functional>

#define DEBUG false
#define FLUSH_ON_WRITE true

using namespace std::literals::string_literals;

void printHelp(const char* exec) {
    std::cout << "Usage: " << exec << " [input files]" << std::endl;
    return;
}

struct BFData {
    std::string buffer;
    std::unordered_map<unsigned, unsigned> fjm;
    std::unordered_map<unsigned, unsigned> bjm;
    unsigned buffer_length;
};

void interpret(struct BFData& data) {
    std::array<char, 100000> scratch{};
    unsigned scratch_index = 0;
    unsigned ip = 0; // instruction pointer
    std::vector<std::function<void(void)>> fn_map(256);
    fn_map['['] = [&](void){
        if(scratch[scratch_index] != 0) return;
        ip = data.fjm[ip]; // execute forwards jump
    };
    fn_map[']'] = [&](void){
        if(scratch[scratch_index] == 0) return;
        ip = data.bjm[ip]; // execute backwards jump
    };

    fn_map['>'] = [&](void){ scratch_index++; },
    fn_map['<'] = [&](void){
        if(scratch_index-- == 0)
            throw std::runtime_error("Attempted to set a negative data pointer offset");
    },
    fn_map['+'] = [&](void){ scratch[scratch_index]++; },
    fn_map['-'] = [&](void){ scratch[scratch_index]--; },
    fn_map['.'] = [&](void){
        putchar(scratch[scratch_index]);
        if(FLUSH_ON_WRITE)
            fflush(stdout);
    };
    fn_map[','] = [&](void){ scratch[scratch_index] = getchar(); };
    for(ip = 0; ip < data.buffer_length; ++ip) {
        // std::cout << "Executing " << data.buffer[ip] << ' ' << ip << ' ' << scratch_index << std::endl;
        fn_map[data.buffer[ip]]();
    }
    return;
}

void readFile(const char* filename) {
    std::ifstream inf{filename};
    std::string buffer;
    if(!inf) {
        throw std::runtime_error("Failed to open file: "s + filename);
    }

    char c;
    std::unordered_set<char> char_set {
        '[', ']', '>', '<', '+', '-', '.', ','
    };

    std::stack<unsigned> stack;

    std::unordered_map<unsigned, unsigned> forward_jump_map;
    std::unordered_map<unsigned, unsigned> backward_jump_map;

    unsigned pos = 0;
    while(inf >> c) {
        if(char_set.find(c) != char_set.end()) {
            buffer += c;
            if(c == '[') {
                stack.push(pos);
            } else if(c == ']') {
                if(stack.empty()) throw std::runtime_error("Unbalanced loops... exiting");
                unsigned jp = stack.top(); stack.pop();
                forward_jump_map[jp] = pos;
                backward_jump_map[pos] = jp;
            }
            pos++;
        }
    }
    if(!stack.empty()) {
        throw std::runtime_error("Unbalanced loops... exiting");
    }
    struct BFData data = {
        std::move(buffer),
        std::move(forward_jump_map),
        std::move(backward_jump_map),
        0
    };
    data.buffer_length = data.buffer.size();
    if(DEBUG) {
        std::cout << data.buffer;
        for (auto it : data.fjm)
            std::cout << "\t" << it.first << ":" << it.second << std::endl;
    }
    interpret(data);
}

int main(int argc, char** argv) {
    if(argc == 1) {
        printHelp(argv[0]);
        return 1;
    }
    for(int i = 1; i < argc; ++i) {
        try {
            readFile(argv[i]);
        } catch (std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }
    }
    return 0;
}
