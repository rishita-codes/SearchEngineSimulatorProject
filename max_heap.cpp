
class MaxHeap {
    std::vector<std::pair<double, std::string>> heap;
    int get_parent(int i){ return (i-1)/2; }
    int get_left(int i){ return 2*i+1; }
    int get_right(int i){ return 2*i+2; }

    void heapify_up(int i){
        while(i!=0 && heap[get_parent(i)].first < heap[i].first){
            std::swap(heap[i], heap[get_parent(i)]);
            i = get_parent(i);
        }
    }
    void heapify_down(int i){
        int largest = i;
        int L = get_left(i), R = get_right(i);
        if(L < heap.size() && heap[L].first > heap[largest].first) largest = L;
        if(R < heap.size() && heap[R].first > heap[largest].first) largest = R;
        if(largest != i){
            std::swap(heap[i], heap[largest]);
            heapify_down(largest);
        }
    }

public:
    void insert(double score, const std::string& fname){
        heap.push_back({score, fname});
        heapify_up(heap.size()-1);
    }
    void clear(){ heap.clear(); }
    std::vector<std::pair<double,std::string>> get_top_k(int k){
        std::vector<std::pair<double,std::string>> top;
        int n = std::min(k, (int)heap.size());
        std::vector<std::pair<double,std::string>> copy = heap;
        for(int i=0;i<n;i++){
            top.push_back(copy[0]);
            copy[0] = copy.back(); copy.pop_back();
            int idx=0,l,r,largest;
            while(true){
                largest = idx; l = 2*idx+1; r = 2*idx+2;
                if(l<copy.size() && copy[l].first > copy[largest].first) largest=l;
                if(r<copy.size() && copy[r].first > copy[largest].first) largest=r;
                if(largest != idx){ std::swap(copy[idx], copy[largest]); idx=largest; } else break;
            }
        }
        return top;
    }
};
