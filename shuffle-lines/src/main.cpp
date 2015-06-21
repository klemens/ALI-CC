#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <iterator>

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cerr << "usage: " << argv[0] << " file [numeric seed]" << std::endl;
        return 1;
    }

    std::ifstream fileStream(argv[1]);
    if(!fileStream.good()) {
        std::cerr << "could not read file" << std::endl;
        return 2;
    }

    std::vector<std::string> lines;
    std::string line;
    while(std::getline(fileStream, line)) {
        if(!line.empty()) {
            lines.push_back(line);
        }
    }

    uint32_t seed = std::time(nullptr);
    if(argc > 2) {
        try {
            seed = std::stoul(argv[2]);
        } catch(...) {
            std::cerr << "invalid seed" << std::endl;
            return 3;
        }
    }

    std::cerr << "seed: " << seed << std::endl;

    std::mt19937 twister(seed);
    std::shuffle(lines.begin(), lines.end(), twister);

    std::copy(lines.begin(), lines.end(), std::ostream_iterator<std::string>(std::cout, "\n"));

    return 0;
}
