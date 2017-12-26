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

using namespace std::literals::string_literals;

unsigned DATA_SIZE = 100000; // 100K
bool FLUSH_ON_WRITE = false;
const bool DEBUG = false;

static std::string callee{"brainfuck"};

void printHelp() {
    std::cout << "Usage: " << callee << " [options] <input file>" << std::endl;
    std::cerr << "Options:"         << '\n'
              << "\t-h, --help\t"   << "Show this help message\n"
              << "\t-d <bytes>\t"   << "Set size of data array for the programs (default: 100 KB)\n"
              << "\t-f\t\t"         << "Flush stdout every time a character is written\n";
    return;
}

struct BFData {
    std::string buffer;
    std::unordered_map<unsigned, unsigned> fjm;
    std::unordered_map<unsigned, unsigned> bjm;
    unsigned buffer_length;
};

void interpret(struct BFData& data) {
    std::vector<char> scratch(DATA_SIZE);
    unsigned scratch_index = 0;
    unsigned ip = 0; // instruction pointer

    /* define functions in vector to get rid of hashing overhead */
    std::vector<std::function<void(void)>> fn_map(256);

    fn_map['['] = [&](void){
        if(scratch[scratch_index] != 0) return;
        ip = data.fjm[ip]; // execute forwards jump
    };
    fn_map[']'] = [&](void){
        if(scratch[scratch_index] == 0) return;
        ip = data.bjm[ip]; // execute backwards jump
    };
    fn_map['>'] = [&](void){
        if(scratch_index++ >= DATA_SIZE)
            throw std::runtime_error("Buffer overflow datected");
    },
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

    /* start execution */
    for(ip = 0; ip < data.buffer_length; ++ip) {
        // std::cout << "Executing " << data.buffer[ip] << ' ' << ip << ' ' << scratch_index << std::endl;
        fn_map[data.buffer[ip]]();
    }
    return;
}

void read(std::istream& instream) {
    std::string buffer;

    char c;
    std::unordered_set<char> char_set {
        '[', ']', '>', '<', '+', '-', '.', ','
    };

    std::stack<unsigned> stack;

    std::unordered_map<unsigned, unsigned> forward_jump_map;
    std::unordered_map<unsigned, unsigned> backward_jump_map;

    const std::runtime_error loop_error("Unbalanced loops detected");

    unsigned pos = 0;
    while(instream >> c) {
        if(char_set.find(c) != char_set.end()) {
            buffer += c;
            if(c == '[') {
                stack.push(pos);
            } else if(c == ']') {
                if(stack.empty()) throw loop_error;
                unsigned jp = stack.top(); stack.pop();
                forward_jump_map[jp] = pos;
                backward_jump_map[pos] = jp;
            }
            pos++;
        }
    }
    if(!stack.empty()) {
        throw loop_error;
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

int parseArguments(int argc, char** argv) {
    /*
    std::string filename;
    std::ifstream inf{filename};
    if(!inf) {
        throw std::runtime_error("Failed to open file '"s + filename + '\'');
    }
    */

    bool readFromCin = false;
    bool istreamSet = false;

    int i = 1;

    std::string filename;

    std::unordered_map<std::string, std::function<void(void)>> flag_map {
        {"-h", [&]() { printHelp(); exit(1); } },
        {"--help", [&]() { printHelp(); exit(1); } },
        {"-",  [&]() {
                         if(istreamSet)
                             throw std::runtime_error("Attempted to read from stdin after setting filename");
                         readFromCin = true; istreamSet = true;
                     }
        },
        {"-f", [&]() { FLUSH_ON_WRITE = true; } },
        {"-d", [&]() { if(++i < argc) DATA_SIZE = atoi(argv[i]); else throw std::runtime_error("Flag '-d' missing argument"); } }
    };

    auto parseFlag = [&flag_map](std::string flag) {
        auto got = flag_map.find(flag);
        if(got == flag_map.end())
            throw std::runtime_error("Failed to parse argument " + flag);
        return flag_map[flag]();
    };

    for(; i < argc; ++i) {
        if(argv[i][0] == '-') {
            parseFlag(argv[i]);
        } else {
            if(istreamSet)
                throw std::runtime_error("Attempted to set input twice");
            filename = argv[i];
            istreamSet = true;
        }
    }
    if(!istreamSet) {
        printHelp();
        return 1;
    }
    if(readFromCin) {
        read(std::cin);
    } else {
        std::ifstream inf{filename};
        read(inf);
    }
    return 0;
}

int main(int argc, char** argv) {
    callee = argv[0];
    if(argc == 1) {
        printHelp();
        return 1;
    }
    try {
        if(parseArguments(argc, argv))
            return 1;
    } catch (std::runtime_error& e) {
        std::cerr << "FATAL: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
