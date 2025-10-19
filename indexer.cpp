#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <unordered_set>

struct node {
    std::string d;
    node* next = nullptr;
};

class linked_list {
public:
    static void insert_end(node** head, const std::string& d) {
        node* k = new node();
        k->d = d;
        k->next = nullptr;

        if (!*head) {
            *head = k;
            return;
        }

        node* p = *head;
        while (p->next)
            p = p->next;
        p->next = k;
    }

    static void traverse(node** head) {
        if (!*head) {
            std::cout << "list is empty\n";
            return;
        }

        node* p = *head;
        while (p) {
            std::cout << p->d << ' ';
            p = p->next;
        }
        std::cout << '\n';
    }
};

class token {
public:
    static std::string lowercase(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return s;
    }

    static bool is_stopword(const std::string& s) {
        static const std::unordered_set<std::string> stopwords = {
            "the", "is", "in", "on", "and", "a", "an", "to", "for", "of", "by", "with"
        };
        return stopwords.count(s) > 0;
    }

    static void tokenization(node** head) {
        if (!*head) {
            std::cout << "list is empty\n";
            return;
        }

        node* p = *head;
        node* prev = nullptr;

        while (p) {
            p->d = lowercase(p->d);

            if (is_stopword(p->d)) {
                node* temp = p;
                if (prev)
                    prev->next = p->next;
                else
                    *head = p->next;

                p = p->next;
                delete temp;
            } else {
                prev = p;
                p = p->next;
            }
        }
    }
};
