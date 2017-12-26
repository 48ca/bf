#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <functional>

#define BUFFER_SIZE 10000
#define F(func) std::bind(&Buffer::func, this)

class Buffer {
  private:
    std::array<char, BUFFER_SIZE> buffer{};
    unsigned buffer_index = 0;
    std::string output;
  public:
    void shift_left(void) {
        output += '<';
        buffer_index -= 1;
    }
    void shift_right(void) {
        output += '>';
        buffer_index += 1;
    }
    void write(void) {
        output += '.';
    }
    void read(void) { // should not be used
        output += ',';
    }
    void increment(void) {
        output += '+';
        buffer[buffer_index]++;
    }
    void decrement(void) {
        output += '-';
        buffer[buffer_index]--;
    }
    void multiple(const std::function<void(void)>& fn, int num) {
        for(int i = 0; i < num; ++i)
            fn();
    }
    /* important methods */
    bool optimized_placement(char c) {
        int diff = c - buffer[buffer_index];
        if(abs(diff) < 20) {
            if(diff > 0) {
                multiple(F(increment), diff);
            } else if(diff < 0) {
                multiple(F(decrement), -diff);
            }
            write();
            return true;
        }
        return false;
    }
    void print(void) {
        std::cout << output << std::endl;
    }
};

#undef F
#define F(func) std::bind(&Buffer::func, &buffer)

int main(void) {
    std::string input;
    for(std::string line; std::getline(std::cin, line);) {
        input += line + "\n";
    }
    unsigned length = input.length();

    Buffer buffer;

    for(unsigned i = 0; i < length; ++i) {
        char c = input[i];

        if(buffer.optimized_placement(c))
            continue;

        buffer.shift_right();
        buffer.multiple(F(increment), input[i]);
        buffer.write();
    }
    buffer.print();
    return 0;
}
