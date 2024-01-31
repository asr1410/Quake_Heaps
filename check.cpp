#include <utility>
#include <string>
#include <memory>
#include <list>
#include <vector>
#include <iterator>
#include <cmath>
#include <iostream>
#include <unordered_map>
#include <algorithm>

using namespace std;

struct leafNode;
struct internalNode;

struct leafNode
{
    int key;
    shared_ptr<internalNode> highestReference;
};

struct internalNode
{
    unsigned int height;
    shared_ptr<leafNode> leafReference;
    shared_ptr<internalNode> leftChild, rightChild, parent;
    list<shared_ptr<internalNode>>::iterator pointerAccess;
};

class QuakeHeap
{
public:
    list<shared_ptr<internalNode>> forest;
    list<shared_ptr<internalNode>>::iterator minimumPointer = forest.end();
    unsigned long length = 0;
    float alpha = 0.5;
    void updateMin()
    {
        if (minimumPointer == forest.end())
            minimumPointer--;
        else if (forest.back()->leafReference->key < (*minimumPointer)->leafReference->key)
            minimumPointer = --forest.end();
    }
    void insert(shared_ptr<leafNode> &entry)
    {
        shared_ptr<internalNode> node = make_shared<internalNode>(entry);
        entry->highestReference = node;
        node->leftChild = node->rightChild = node->parent = nullptr;
        node->height = 0;
        node->leafReference = entry;
        forest.push_back(node);
        updateMin();
        node->pointerAccess = --forest.end();
        length++;
    }
    void decreaseKey(shared_ptr<leafNode> &entry, int newKey)
    {
        if (entry->key < newKey)
        {
            cout << "New key is greater than current key!" << endl;
            return;
        }
        entry->key = newKey;
        shared_ptr<internalNode> node = entry->highestReference;
        if (node->parent == nullptr)
            forest.erase(node->pointerAccess);
        else if (node->parent->leftChild == node)
            node->parent->leftChild = nullptr;
        else
            node->parent->rightChild = nullptr;
        forest.push_back(node);
        updateMin();
    }
    void link(shared_ptr<internalNode> &linked, shared_ptr<internalNode> &a, shared_ptr<internalNode> &b)
    {
        linked = make_shared<internalNode>();
        linked->leftChild = a;
        linked->rightChild = b;
        linked->parent = nullptr;

        if (a != nullptr && b != nullptr)
            linked->leafReference = (a->leafReference->key < b->leafReference->key) ? a->leafReference : b->leafReference;
        else if (a != nullptr)
            linked->leafReference = a->leafReference;
        else
            linked->leafReference = b->leafReference;

        linked->height = max((a != nullptr ? a->height : 0), (b != nullptr ? b->height : 0)) + 1;
        a->parent = linked;
        b->parent = linked;
    }
    void shake(shared_ptr<internalNode> root, bool isUpdateMin = false)
    {
        shared_ptr<leafNode> entry = root->leafReference;
        while (root != nullptr)
        {
            if (root->leftChild && root->leftChild->leafReference == entry)
            {
                if (root->rightChild)
                {
                    root->rightChild->parent = nullptr;
                    forest.push_back(root->rightChild);
                    if (isUpdateMin)
                        updateMin();
                }
                root = root->leftChild;
            }
            else if (root->rightChild && root->rightChild->leafReference == entry)
            {
                if (root->leftChild)
                {
                    root->leftChild->parent = nullptr;
                    forest.push_back(root->leftChild);
                    if (isUpdateMin)
                        updateMin();
                }
                root = root->rightChild;
            }
            else
                break;
        }
    }
    shared_ptr<leafNode> deleteMin()
    {
        list<shared_ptr<internalNode>>::iterator oldMinimum = minimumPointer;
        shared_ptr<leafNode> minEntry = (*minimumPointer)->leafReference;
        shake(*minimumPointer);
        forest.erase(oldMinimum);

        vector<list<shared_ptr<internalNode>>> buckets(1 + ceil(log2(length)));
        vector<int> heights(1 + ceil(log2(length)));

        while (forest.size() > 0)
        {
            shared_ptr<internalNode> node = forest.front();
            forest.pop_front();
            buckets[node->height].push_back(node);
        }
        for (int i = 0; i < buckets.size(); i++)
        {
            list<shared_ptr<internalNode>> *node_list = &buckets[i];
            if (node_list->size() > 1)
            {
                heights[i] = 1;
            }
            while (node_list->size() > 1)
            {
                shared_ptr<internalNode> a = node_list->back();
                node_list->pop_back();
                shared_ptr<internalNode> b = node_list->back();
                node_list->pop_back();
                shared_ptr<internalNode> linked;
                link(linked, a, b);
                buckets[i + 1].push_back(linked);
            }
        }
        int min_i = -1;
        for (int i = 1; i < heights.size() && min_i != -1; ++i)
        {
            if (heights[i] > heights[i - 1] * alpha)
            {
                min_i = i;
            }
        }

        minimumPointer = forest.end();

        for (int i = 0; i < buckets.size(); ++i)
            if (buckets[i].size() > 0)
                if (min_i != -1 && i > min_i)
                    shake(buckets[i].front(), true);
                else
                {
                    forest.push_back(buckets[i].front());
                    updateMin();
                }
        length--;
        return minEntry;
    }
};

int main()
{
    const unsigned int N = 1000;
    QuakeHeap heap;
    unordered_map<int, shared_ptr<leafNode>> entries;
    entries.reserve(N);
    vector<int> ins, del;
    for (int i = 0; i < N; ++i)
    {
        int key = rand() % 1000;
        shared_ptr<leafNode> node = make_shared<leafNode>(key);
        ins.push_back(key);
        entries[key] = node;
        heap.insert(node);
    }

    cout << "Size of entries: " << entries.size() << endl;

    while (heap.length)
    {
        int temp = heap.deleteMin()->key;
        del.push_back(temp);
        entries.erase(temp);
    }

    cout << "Size of entries: " << entries.size() << endl;

    sort(ins.begin(), ins.end());
    sort(del.begin(), del.end());

    if (ins == del)
    {
        cout << "Inserted and Deleted keys are equal!" << endl;
    }

    return 0;
}