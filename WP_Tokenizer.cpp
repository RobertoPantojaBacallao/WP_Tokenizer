#include <iostream>
#include <format>
#include <vector>
#include <cstring>
#include <algorithm>
#include <map>

#include "wordPieceTokenizer.h"

constexpr void print(const std::string_view str_fmt, auto&&... args) {
    fputs(std::vformat(str_fmt, std::make_format_args(args...)).c_str(), stdout);
}

using namespace std;

vector<string> corpus{ "This is the Hugging Face Course.",
                        "This chapter is about tokenization.",
                        "This section shows several tokenizer algorithms.",
                        "Hopefully, you will be able to understand how they are trained and generate tokens." };


int main()
{
    wordPieceTokenizer wp = wordPieceTokenizer(corpus);
    wp.train();

    vector<vector<string>> res = wp.tokenize(corpus);

    for (auto& a : res) {
        for (auto& b : a) {
            print("{} ", b);
        }
        print("\n");
    }
}
