#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include "indexer.cpp"  

class HTMLParser {
public:
    static std::string stripHTML(const std::string &html) {
        std::string text;
        bool inTag = false;
        for (char c : html) {
            if (c == '<') inTag = true;
            else if (c == '>') inTag = false;
            else if (!inTag) text += c;
        }
        return text;
    }

    static std::string cleanWord(const std::string &word) {
        std::string result;
        for (char c : word) {
            if (std::isalnum(static_cast<unsigned char>(c))) {
                result += c;
            }
        }
        return result;
    }

    static node* parseFile(const std::string &filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Cannot open file: " << filename << "\n";
            return nullptr;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string htmlContent = buffer.str();
        file.close();

        std::string cleanText = stripHTML(htmlContent);

        node* head = nullptr;
        std::string word;
        std::stringstream ss(cleanText);

        while (ss >> word) {
            word = cleanWord(word);
            if (!word.empty())
                linked_list::insert_end(&head, word);
        }

        token::tokenization(&head);

        return head;
    }
};
