#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <algorithm>
#include "crawler.cpp"
#include "indexer.cpp"
#include "ranker.cpp"
#include "max_heap.cpp"
#include <cstdlib> 

int main() {
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(1130, 567, "Search Engine Simulator", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::StyleColorsDark();
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.25f, 0.25f, 0.28f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.35f, 0.35f, 0.4f, 1.0f);
    style.Colors[ImGuiCol_Text] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f,0.35f,0.35f,1.0f);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    char query[256] = "";
    bool use_bm25 = true, use_tfidf = false;
    std::vector<Crawler::FileInfo> files;
    std::vector<std::vector<std::string>> preview_tokens;
    std::vector<Crawler::FileInfo> ranked_files;
    MaxHeap top_heap;
    int selected_db_index = 0;
    const char* databases[] = { "database01", "database02", "database03" };
    bool data_loaded = false;
    auto table = HashTable::create_hash_table(100);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowSize(ImVec2(1120, 550), ImGuiCond_Once);
        ImGui::Begin("Search Engine Simulator", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);

        ImGui::Text("Select Database:");
        ImGui::SameLine();
        if(ImGui::Combo("##db_select", &selected_db_index, databases, IM_ARRAYSIZE(databases)))
            data_loaded = false;

        if(!data_loaded){
            files = Crawler::fetch_file_names(std::string(databases[selected_db_index]));
            preview_tokens.clear(); preview_tokens.resize(files.size());
            table = HashTable::create_hash_table(100);

            int doc_id = 0;
            for(auto &f : files){
                std::unique_ptr<Crawler::TokenNode> tl = nullptr;
                Crawler::tokenize(f, tl);
                Indexer::make_inverted_index(tl, table, 100, doc_id);

                int count = 0;
                Crawler::TokenNode* p = tl.get();
                while(p && count < 20){
                    preview_tokens[doc_id].push_back(p->str);
                    p = p->next.get();
                    count++;
                }
                tl = nullptr;
                doc_id++;
            }
            data_loaded = true;
            ranked_files.clear();
            for(auto &f : files) f.score = 0;
        }

        ImGui::Separator();

        ImGui::InputText("Enter Query", query, sizeof(query));
        if(ImGui::Checkbox("Use BM25", &use_bm25)) use_tfidf = !use_bm25;
        if(ImGui::Checkbox("Use TF-IDF", &use_tfidf)) use_bm25 = !use_tfidf;

        if(ImGui::Button("Search")){
            std::string query_str(query);
            ranked_files = files;
            for(auto &f : ranked_files) f.score = 0;
            top_heap.clear();

            if(use_bm25)
                Bm25Ranker::rank_docs(table, 100, ranked_files, query_str);
            else
                TfidfRanker::rank_docs(table, 100, ranked_files, query_str);

            std::sort(ranked_files.begin(), ranked_files.end(),
                      [](const Crawler::FileInfo &a, const Crawler::FileInfo &b){ return a.score > b.score; });

            for(auto &f : ranked_files){
                if(f.score > 0) top_heap.insert(f.score, f.fname);
            }
        }
        ImGui::SameLine();
        if(ImGui::Button("Clear Query")){
            query[0] = '\0';
            ranked_files.clear();
            for(auto &f : files) f.score = 0;
            top_heap.clear();
        }

        ImGui::Separator();

        float total_height = ImGui::GetContentRegionAvail().y;
        float panel_top3 = total_height * 0.15f;
        float panel_ranked = total_height * 0.20f;
        float panel_tokens = total_height * 0.33f;
        float panel_index = total_height * 0.32f;

        ImGui::Text("Top 3 Documents :");
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.18f,0.18f,0.18f,1.0f));
        ImGui::BeginChild("TopHeap", ImVec2(0, panel_top3), true);
        auto top3 = top_heap.get_top_k(3);
        if(top3.empty()) ImGui::TextColored(ImVec4(1,0,0,1), "No results found!");
        else {
            for(auto &p : top3){
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f,0.7f,1.0f,1.0f)); 
                if(ImGui::Selectable(p.second.c_str())) {
                    std::string command = "xdg-open \"" + p.second + "\"";
                    system(command.c_str());
                }
                ImGui::PopStyleColor();
                ImGui::SameLine();
                ImGui::Text("Score: %.4f", p.first);
            }
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::Text("Ranked Documents:");
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.18f,0.18f,0.18f,1.0f));
        ImGui::BeginChild("Results", ImVec2(0, panel_ranked), true, ImGuiWindowFlags_HorizontalScrollbar);
        bool has_results = false;
        for(auto &f : ranked_files){
            if(f.score > 0){
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f,0.7f,1.0f,1.0f));
                if(ImGui::Selectable(f.fname.c_str())) {
                    std::string command = "xdg-open \"" + f.fname + "\"";
                    system(command.c_str());
                }
                ImGui::PopStyleColor();
                ImGui::SameLine();
                ImGui::Text("Score: %.4f", f.score);
                has_results = true;
            }
        }
        if(!has_results) ImGui::TextColored(ImVec4(1,0,0,1), "No results found for the query!");
        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::Text("Preview Tokens (first 20 per file):");
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.18f,0.18f,0.18f,1.0f));
        ImGui::BeginChild("Tokens", ImVec2(0, panel_tokens), true, ImGuiWindowFlags_HorizontalScrollbar);
        for(size_t i=0;i<preview_tokens.size();i++){
            ImGui::Text("File %d: ", int(i));
            for(auto &tok : preview_tokens[i]){
                ImGui::SameLine(); ImGui::Text("%s", tok.c_str());
            }
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::Text("Inverted Index Preview (first 20 entries):");
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.18f,0.18f,0.18f,1.0f));
        ImGui::BeginChild("Index", ImVec2(0, panel_index), true, ImGuiWindowFlags_HorizontalScrollbar);
        int shown = 0;
        for(int i=0;i<100 && shown < 20;i++){
            HashTable::InvIdxNode* p = table[i].get();
            while(p && shown < 20){
                ImGui::Text("Bucket %d: %s", i, p->tok.c_str());
                HashTable::FileNode* fc = p->files_occ.get();
                while(fc){
                    ImGui::Text(" DocID: %d Count: %d", fc->doc_id, fc->count);
                    fc = fc->next.get();
                }
                p = p->next.get();
                shown++;
            }
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0,0,display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
