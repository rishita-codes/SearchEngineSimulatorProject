#include<iostream>
#include<fstream>
#include<vector>
#include<filesystem>
#include<memory>
#include<sstream>
#include<string>
#include<algorithm>
#include<cctype>
#include<regex>

class Crawler{
public:
    struct FileInfo{
        std::string fname;
        int total_terms{0};
        double score;
    };
    struct TokenNode{
        std::string str;
        std::unique_ptr<TokenNode> next;
    };

    static void insert_front(std::unique_ptr<TokenNode>& head ,std::string str){
        auto k=std::make_unique<TokenNode>() ;
        if(head==nullptr)
            k->next=nullptr;
        else
            k->next=std::move(head);
        k->str=str;
        head=std::move(k);
    }

    static void display(std::unique_ptr<TokenNode>& head){
        auto p= head.get();
        while(p!=NULL){
            std::cout<< p->str << " ";
            p=(p->next).get();
        }
    }

    static std::vector<FileInfo> fetch_file_names(std::string dir_path="/home/pop/Desktop/R_folder/new_cpp_proj/database01"){
        std::vector<FileInfo> files;
        FileInfo f;
        for(const auto& entry: std::filesystem::directory_iterator(dir_path)){
            f.fname= dir_path+"/"+entry.path().filename().string();
            files.push_back(f);
        }
        return files;
    }

    static void clean_text(std::string &content){
        std::transform( content.begin(), content.end(), content.begin(), ::tolower);
        std::regex script_regex("<script\\b[^>]*>[\\s\\S]*?</script>");
        content= std::regex_replace(content, script_regex, "");
        std::regex style_regex("<style\\b[^>]*>[\\s\\S]*?</style>", std::regex_constants::icase);
        content= std::regex_replace(content, style_regex, "");
        std::regex tag_regex("<[^>]*>");
        content= std::regex_replace(content, tag_regex, " "); 
        std::regex special_char_regex("[^a-zA-Z0-9\\s]+");
        content = std::regex_replace(content, special_char_regex, "");
        std::regex space_regex("[\\s]+");
        content= std::regex_replace(content, space_regex, " "); 
    }

    static void tokenize(Crawler::FileInfo &cur_file, std::unique_ptr<TokenNode>& token_list){
        std::string filename=cur_file.fname;
        std::ifstream file(filename);
        if(!file.is_open())
        {
            std::cerr << "Error opening file " << filename << std::endl;
            exit(1);
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content= buffer.str();
        clean_text(content);
        std::stringstream con_stream(content);
        std::string word;
        std::vector<std::string> stop_words= {
        "a", "an", "the", "and", "but", "or", "because", 
        "as", "if", "of", "at", "by", "for", "with", 
        "is", "are", "was", "were", "be", "been", "to", 
        "in", "on", "from", "it", "its", "me", "my", 
        "you", "your", "i", "he", "she", "we", "they",
        "this", "that", "what", "where", "how", "am" };
        int f;
        while(con_stream >> word){
            f=0;
            for(std::string sword: stop_words)
            {
                if(word==sword)
                {
                    f=1;
                    break;
                }
            }
            if(f==0)
            {
                cur_file.total_terms++;
                insert_front(token_list, word);
            }
        }
    }    
};
