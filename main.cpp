#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include "crawler.cpp"
#include "hash_table.cpp"
#include "ranking.cpp"

namespace fs = std::filesystem;

int main() {
    std::string folderPath;
    std::string query;

    std::cout<< "**************************SEARCH ENGINE SIMULATOR**************************\n\n";
    std::cout << "Enter the directory path containing HTML files: ";
    std::getline(std::cin, folderPath);

    std::cout << "Enter your search query: ";
    std::getline(std::cin, query);

    int tableSize = 1000;  
    std::vector<dnode> table(tableSize);

    for (const auto &entry : fs::directory_iterator(folderPath)) {
        if (!entry.is_regular_file()) continue;

        std::string filename = entry.path().string();

        node* head = HTMLParser::parseFile(filename);
        if (!head) continue;

        node* p = head;
        while (p) {
            index::update_index(p->d, filename, tableSize, table);
            p = p->next;
        }

        p = head;
        while (p) {
            node* temp = p;
            p = p->next;
            free(temp);  
        }
    }

    std::cout << "Indexing complete.\n";

    Ranker::computeTFIDF(table.data(), tableSize, query.c_str());

    return 0;
}
