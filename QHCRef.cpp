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

class QuakeHeap
{
private:
    struct leafNode;
    struct internalNode;
    struct leafNode
    {
        leafNode() {}
        leafNode(int k);
        int key() { return value; }

    private:
        friend class QuakeHeap;
        int value;
        shared_ptr<internalNode> highestReference;
    };

    struct internalNode
    {
        internalNode(shared_ptr<leafNode> e = nullptr);
        internalNode(shared_ptr<internalNode> left, shared_ptr<internalNode> right);
        shared_ptr<internalNode> leftChild, rightChild, parent;
        shared_ptr<leafNode> leafReference;
        list<shared_ptr<internalNode>>::iterator pointerAccess;
        unsigned int height;
    };

    struct TException
    {
        TException(string msg) : _msg(msg) {}
        inline string what() { return _msg; }
        string _msg;
    };

    inline void updateMin();
    void shake(shared_ptr<internalNode> root, bool isUpdateMin = false);

    list<shared_ptr<internalNode>>::iterator minimumPointer;
    list<shared_ptr<internalNode>> forest;
    unsigned long length;
    float alpha;

public:
    using Entry = shared_ptr<leafNode>;
    using Exception = TException;

    QuakeHeap(float alpha = 0.5);
    virtual ~QuakeHeap();
    void insert(shared_ptr<leafNode> &entry);
    void decreaseKey(shared_ptr<leafNode> &entry, int newKey);
    shared_ptr<leafNode> getMin();
    shared_ptr<leafNode> deleteMin();
    bool empty();
    unsigned long size();
    static shared_ptr<leafNode> makeEntry(int key);
};

QuakeHeap::leafNode::leafNode(int k) : value(k), highestReference(nullptr) {}

QuakeHeap::internalNode::internalNode(shared_ptr<leafNode> e) : leftChild(nullptr), rightChild(nullptr), parent(nullptr), height(0), leafReference(nullptr)
{
    leafReference = shared_ptr<leafNode>(e);
}

QuakeHeap::internalNode::internalNode(shared_ptr<internalNode> left, shared_ptr<internalNode> right) : leftChild(left), rightChild(right), parent(nullptr), leafReference(nullptr)
{
    if (left != nullptr && right != nullptr)
    {
        leafReference = (left->leafReference->value < right->leafReference->value) ? left->leafReference : right->leafReference;
    }
    else if (left != nullptr)
    {
        leafReference = left->leafReference;
    }
    else if (right != nullptr)
    {
        leafReference = right->leafReference;
    }
    else
    {
        throw TException("Tournament Node: Cannot Link two empty Nodes!");
    }
    height = max(left->height, right->height) + 1;
}

QuakeHeap::QuakeHeap(float alpha) : length(0), alpha(alpha)
{
    minimumPointer = forest.end();
}

QuakeHeap::~QuakeHeap() {}

shared_ptr<QuakeHeap::leafNode> QuakeHeap::makeEntry(int key)
{
    return make_shared<leafNode>(key);
}

void QuakeHeap::updateMin()
{
    if (minimumPointer == forest.end())
    {
        --minimumPointer;
    }
    else if (forest.back()->leafReference->value < (*minimumPointer)->leafReference->value)
    {
        minimumPointer = forest.end();
        --minimumPointer;
    }
}

void QuakeHeap::shake(shared_ptr<internalNode> root, bool isUpdateMin)
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

bool QuakeHeap::empty()
{
    return length == 0;
}

unsigned long QuakeHeap::size()
{
    return length;
}

void QuakeHeap::insert(shared_ptr<leafNode> &entry)
{
    shared_ptr<internalNode> node = make_shared<internalNode>(entry);
    entry->highestReference = node;
    forest.push_back(node);
    updateMin();
    // (*forest.rbegin())->pointerAccess = forest.end();
    node->pointerAccess = --forest.end();
    // --((*forest.rbegin())->pointerAccess);
    ++length;
}

void QuakeHeap::decreaseKey(shared_ptr<leafNode> &entry, int newKey)
{
    if (entry->value < newKey)
        throw TException("Decrease Key: New key " + to_string(newKey) +
                         " must be less than old key " + to_string(entry->value));
    entry->value = newKey;
    shared_ptr<internalNode> x = entry->highestReference;
    if (x->parent == nullptr)
    {
        forest.erase(x->pointerAccess);
    }
    else if (x->parent->leftChild == x)
    {
        x->parent->leftChild = nullptr;
    }
    else if (x->parent->rightChild == x)
    {
        x->parent->rightChild = nullptr;
    }
    forest.push_back(x);
    updateMin();
}

shared_ptr<QuakeHeap::leafNode> QuakeHeap::getMin()
{
    return (*minimumPointer)->leafReference;
}

shared_ptr<QuakeHeap::leafNode> QuakeHeap::deleteMin()
{
    list<shared_ptr<internalNode>>::iterator old_minimum = minimumPointer;
    shared_ptr<leafNode> min_entry = (*minimumPointer)->leafReference;
    shake(*minimumPointer);
    forest.erase(old_minimum);

    vector<list<shared_ptr<internalNode>>> buckets(1 + ceil(log2(length)));
    vector<int> heights(1 + ceil(log2(length)));

    while (forest.size() > 0)
    {
        shared_ptr<internalNode> node = forest.front();
        forest.pop_front();
        buckets[node->height].push_back(node);
    }

    for (int i = 0; i < buckets.size(); ++i)
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
            shared_ptr<internalNode> linked = make_shared<internalNode>(a, b);
            a->parent = linked;
            b->parent = linked;
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
    {
        if (buckets[i].size() > 0)
        {
            if (min_i != -1 && i > min_i)
            {
                shake(buckets[i].front(), true);
            }
            else
            {
                forest.push_back(buckets[i].front());
                updateMin();
            }
        }
    }

    --length;
    return min_entry;
}

int main()
{
    const unsigned int N = 1000;
    QuakeHeap pq;
    using EntryIterator = unordered_map<int, QuakeHeap::Entry>::iterator;
    unordered_map<int, QuakeHeap::Entry> entries;
    entries.reserve(N);
    vector<int> ins;
    vector<int> del;

    for (int i = 0; i < N; ++i)
    {
        int key = rand() % 1000;
        ins.push_back(key);
        entries[key] = QuakeHeap::makeEntry(key);
        pq.insert(entries[key]);
    }

    cout << "Size of entries: " << entries.size() << endl;

    while (!pq.empty())
    {
        int temp = pq.deleteMin()->key();
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