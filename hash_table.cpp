#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

struct invdocs {
    std::string doc_id;
    int count = 0;
};

struct dnode {
    std::string d;
    std::vector<invdocs> docs;
    dnode* next = nullptr;
};

class index {
public:
    static uint32_t fnv1a_hash(const std::string &s) {
        uint32_t fnv_prime = 21212333u;
        uint32_t hash = 28768887u;
        for (char c : s) {
            hash ^= static_cast<uint8_t>(c);
            hash *= fnv_prime;
        }
        return hash;
    }

    static void inc_doc_count(dnode* k, const std::string &doc_id) {
        for (auto &doc : k->docs) {
            if (doc.doc_id == doc_id) {
                doc.count++;
                return;
            }
        }
        k->docs.push_back({doc_id, 1});
    }

    static dnode* insert_end_dnode(dnode* head, const std::string &word) {
        dnode* k = new dnode();
        k->d = word;
        k->next = nullptr;

        if (!head) return k;

        dnode* p = head;
        while (p->next != nullptr)
            p = p->next;
        p->next = k;
        return head;
    }

    static void update_index(const std::string &word, const std::string &doc_id, int n, std::vector<dnode> &table) {
        uint32_t hash = fnv1a_hash(word) % n;

        if (table[hash].d.empty()) {
            table[hash].d = word;
            inc_doc_count(&table[hash], doc_id);
        } else {
            dnode* p = &table[hash];
            while (true) {
                if (p->d == word) {
                    inc_doc_count(p, doc_id);
                    break;
                }
                if (p->next == nullptr) {
                    p->next = new dnode();
                    p->next->d = word;
                    inc_doc_count(p->next, doc_id);
                    break;
                }
                p = p->next;
            }
        }
    }
    static void showInvertedIndex(std::vector<dnode> &table) 
    {
        std::cout << "\n---- Inverted Index ----\n";
        for (size_t i = 0; i < table.size(); i++) {
            dnode* p = &table[i];
            if (p->d.empty()) continue; 

            while (p) {
                if (!p->d.empty()) {
                    std::cout << "Word: " << p->d << "\n";
                    for (size_t j = 0; j < p->docs.size(); j++) {
                        std::cout << "   Doc: " << p->docs[j].doc_id
                                << "  Count: " << p->docs[j].count << "\n";
                    }
                }
                p = p->next;
            }
        }
    }
};



/*int main() {
    int n = 10; // table size
    std::vector<dnode> table(n); 

    index::update_index("hello", "doc1", n, table);
    index::update_index("hello", "doc2", n, table);
    index::update_index("world", "doc1", n, table);

    for (int i = 0; i < n; i++) {
        dnode* p = &table[i];
        while (p) {
            if (!p->d.empty()) {
                std::cout << "Word: " << p->d << "\n";
                for (auto &doc : p->docs)
                    std::cout << "  Doc: " << doc.doc_id << ", Count: " << doc.count << "\n";
            }
            p = p->next;
        }
    }

    return 0;
}*/
