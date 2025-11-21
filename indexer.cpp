//#include<iostream>
//#include<vector>
//#include<string>
//#include<memory>
//#include"crawler.cpp"

class HashTable{
public:
    struct FileNode{
        int doc_id{-1}; // index of file in vector
        int count{0};
        std::unique_ptr<FileNode> next;
    };
    struct InvIdxNode{
        std::string tok{""};
        std::unique_ptr<FileNode> files_occ;
        std::unique_ptr<InvIdxNode> next;
    };
    static std::unique_ptr<FileNode> create_filenode(){
        auto k=std::make_unique<FileNode>() ;
        return k;
    }
    static std::unique_ptr<InvIdxNode> create_invidxnode(){
        auto k=std::make_unique<InvIdxNode>() ;
        return k;
    }
    static std::vector<std::unique_ptr<InvIdxNode>> create_hash_table(int n)
    {
        std::vector<std::unique_ptr<InvIdxNode>>inverted_index(n);
        return inverted_index;
    };
    static uint32_t fnv1a_hash(const std::string &str){
        const uint32_t fnv_prime= 16777619u;
        const uint32_t fnv_offset_basis= 2166136261u;
        uint32_t hash= fnv_offset_basis;
        for(char c: str){
            hash^=static_cast<uint8_t>(c);
            hash*=fnv_prime;
        }
        return hash; // remember to mod by range n
    }
    static void update_table(std::vector<std::unique_ptr<InvIdxNode>> &table,int n, std::string &str, int doc_id){
        auto hstr=fnv1a_hash(str)%n;
        std::cout<< "{"<<hstr<<"}\n";
        InvIdxNode *p=table[hstr].get(), *pre=nullptr;
        int f=0;
        if(p){
            while(p){
                if(p->tok==str){
                    f=1;
                    break;
                }
                pre=p;
                p=p->next.get();
            }
            if(f==1){
                int f2=0;
                FileNode* fc=(p->files_occ).get(), *fcpre=nullptr;
                if(fc){
                    while(fc){
                        if(fc->doc_id==doc_id){
                            fc->count++;
                            f2=1;
                            break;
                        }
                        fcpre=fc;
                        fc=fc->next.get();
                    }
                    if(f2==0){
                        fcpre->next=create_filenode();
                        fcpre->next->doc_id=doc_id;
                        fcpre->next->count++;
                    }
                }
                else{
                    p->files_occ=create_filenode();
                    fc=p->files_occ.get();
                    fc->doc_id=doc_id;
                    fc->count++;
                }
            }
            else{
                pre->next=create_invidxnode();  
                pre->next->tok=str;
                pre->next->files_occ=create_filenode();
                pre->next->files_occ->doc_id=doc_id;
                pre->next->files_occ->count++;
            }   
        }
        else{
            table[hstr]=create_invidxnode();
            p=table[hstr].get();
            p->tok=str;
            p->files_occ=create_filenode();
            p->files_occ->doc_id=doc_id;
            p->files_occ->count++;
        }
    }
    static void display_table(std::vector<std::unique_ptr<InvIdxNode>> &table,int n){
        int i=0;
        for(auto& bucket: table){
            auto p=bucket.get();
            while(p){
                std::cout<< i<< " "<< p->tok << " ";
                auto dp=p->files_occ.get();
                while(dp){
                    std::cout<< "[ DocId: "<< dp->doc_id<<";\t"<<"Count: "<<dp->count<<" ]\n";
                    dp=dp->next.get();
                }
                p=p->next.get();
            }
            i++;
        }
    }
};

class Indexer{
public:
    static void make_inverted_index(std::unique_ptr<Crawler::TokenNode>& token_list,std::vector<std::unique_ptr<HashTable::InvIdxNode>> &table,int n,int doc_id){
        Crawler::TokenNode *p=token_list.get();
        while(p){
            HashTable::update_table(table,n,p->str,doc_id);
            p=p->next.get();
        }
    }
};
