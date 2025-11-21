#include<cmath>
//#include"indexer.cpp"

class TfidfRanker{
public:
    static void rank_docs(std::vector<std::unique_ptr<HashTable::InvIdxNode>> &table,int n,  std::vector<Crawler::FileInfo> &files,std::string &query){
        Crawler::clean_text(query);
        std::stringstream query_stream(query);
        std::string term;
        int total_docs=files.size();
        double tf=0, idf=0, df=0;
        while(query_stream >> term){
            auto hstr=HashTable::fnv1a_hash(term)%n;
            std::vector<int> recs(total_docs, 0);
            HashTable::InvIdxNode *p=table[hstr].get();
            int f=0;
            while(p){
                if(p->tok==term){
                    f=1;
                    break;
                }
                p=p->next.get();
            }
            if(f==0) continue;
            int f2=0;
            HashTable::FileNode* fc=(p->files_occ).get();
            if(fc){
                df=0;
                while(fc){
                    recs[fc->doc_id]=fc->count;
                    df++;
                    fc=fc->next.get();
                }
            }
            else continue;
            idf=std::log(total_docs+1.0/(df+1.0));
            for(int i=0;i<total_docs;i++){
                tf=recs[i]/static_cast<double>(files[i].total_terms);
                files[i].score+=tf*idf;
            }
        }
    }
};

class Bm25Ranker {
    static double compute_bm25(double tf, double df, int N, int dl, double avgdl, double k1, double b){
        double idf = std::log((N - df + 0.5) / (df + 0.5) + 1.0);
        double num = tf * (k1 + 1);
        double den = tf + k1 * (1 - b + b * dl / avgdl);
        return idf * (num / den);
    }

public:
    static void rank_docs(std::vector<std::unique_ptr<HashTable::InvIdxNode>> &table,int n,std::vector<Crawler::FileInfo> &files,std::string &query){
        Crawler::clean_text(query);
        std::stringstream qs(query);
        std::string term;
        int N = files.size();
        double total_len = 0;
        for(auto &f : files) total_len += f.total_terms;
        double avgdl = total_len / N;
        double k1 = 1.5;
        double b = 0.75;

        while(qs >> term){
            auto h = HashTable::fnv1a_hash(term) % n;
            std::vector<int> recs(N,0);
            HashTable::InvIdxNode *p = table[h].get();
            int found = 0;
            while(p){
                if(p->tok == term){ found = 1; break; }
                p = p->next.get();
            }
            if(!found) continue;

            double df = 0;
            auto fc = p->files_occ.get();
            while(fc){
                recs[fc->doc_id] = fc->count;
                df++;
                fc = fc->next.get();
            }

            for(int i=0;i<N;i++){
                if(recs[i] == 0) continue;
                double tf = recs[i];
                double dl = files[i].total_terms;
                double score = compute_bm25(tf, df, N, dl, avgdl, k1, b);
                files[i].score += score;
            }
        }
    }
};
