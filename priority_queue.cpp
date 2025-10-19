#pragma once
#include <iostream>
#include <cstring>

#define MAX_HEAP_SIZE 200

struct PQItem {
    char doc_id[50];
    double score;
};

class PriorityQueue {
private:
    PQItem heap[MAX_HEAP_SIZE];
    int size;

    void swap(PQItem &a, PQItem &b) {
        PQItem temp = a;
        a = b;
        b = temp;
    }

    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heap[index].score > heap[parent].score) {
                swap(heap[index], heap[parent]);
                index = parent;
            } else break;
        }
    }

    void heapifyDown(int index) {
        int left, right, largest;
        while (true) {
            left = 2 * index + 1;
            right = 2 * index + 2;
            largest = index;

            if (left < size && heap[left].score > heap[largest].score)
                largest = left;
            if (right < size && heap[right].score > heap[largest].score)
                largest = right;

            if (largest != index) {
                swap(heap[index], heap[largest]);
                index = largest;
            } else break;
        }
    }

public:
    PriorityQueue() { size = 0; }

    bool empty() const { return size == 0; }

    void push(const char* id, double score) {
        if (size >= MAX_HEAP_SIZE) {
            std::cerr << "Heap overflow!\n";
            return;
        }
        strcpy(heap[size].doc_id, id);
        heap[size].score = score;
        heapifyUp(size);
        size++;
    }

    PQItem pop() {
        if (size == 0) {
            std::cerr << "Heap underflow!\n";
            PQItem empty = {"", 0.0};
            return empty;
        }
        PQItem top = heap[0];
        heap[0] = heap[size - 1];
        size--;
        heapifyDown(0);
        return top;
    }
};
