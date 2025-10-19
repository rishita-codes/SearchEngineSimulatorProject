#pragma once
#include <iostream>
#include <cmath>
#include <cstring>
#include <map>
#include <sstream>
#include "indexer.cpp"
#include "priority_queue.cpp"

struct DocScore{
    std::string doc_id;
    double score = 0.0;
};

class Ranker{
public:
    static void computeTFIDF(dnode* table, int n, const char* query) 
    {
        node* queryHead = nullptr;
        std::stringstream ss(query);
        std::string word;
        while (ss >> word){
            linked_list::insert_end(&queryHead, word);
        }
        token::tokenization(&queryHead); 

        if(!queryHead) 
        {
            std::cout << "No valid query words after stopword removal.\n";
            return;
        }

        std::map<std::string, double> docScores;
        for (int i=0; i<n; i++) {
            dnode* p = &table[i];
            while (p) {
                if (!p->d.empty())
                {
                    for (size_t j=0; j<p->docs.size(); j++) 
                    {
                        docScores[p->docs[j].doc_id] = 0.0; 
                    }
                }
                p = p->next;
            }
        }

        int totalDocs = docScores.size();

        node* qnode = queryHead;
        while (qnode) {
            uint32_t hash = index::fnv1a_hash(qnode->d) % n;
            dnode* bucket = &table[hash];
            dnode* wordNode = nullptr;

            if (bucket->d == qnode->d) 
                wordNode = bucket;
            else {
                dnode* p = bucket->next;
                while(p)
                {
                    if (p->d == qnode->d) {
                        wordNode = p;
                        break;
                    }
                    p = p->next;
                }
            }

            int df = wordNode ? wordNode->docs.size() : 0;
            double idf = log10(static_cast<double>(totalDocs) / (1 + df));

            if (wordNode) 
            {
                for (size_t i = 0; i < wordNode->docs.size(); i++) {
                    const std::string &docid = wordNode->docs[i].doc_id;
                    int count = wordNode->docs[i].count;
                    double tf = 1 + log10(static_cast<double>(count));
                    double tfidf = tf * idf;
                    docScores[docid] += tfidf; 
                }
            }
            qnode = qnode->next;
        }

        PriorityQueue pq;
        for (auto &pair : docScores) {
            pq.push(pair.first.c_str(), pair.second);
        }

        std::cout << "\nRanking results for query: \"" << query << "\"\n";
        int rank = 1;
        while (!pq.empty()) {
            PQItem item = pq.pop();
            std::cout << rank++ << ". " << item.doc_id
                      << "  -->   Score: " << item.score << "\n";
        }

        qnode = queryHead;
        while (qnode) {
            node* temp=qnode;
            qnode=qnode->next;
            free(temp); 
        }
    }
};
