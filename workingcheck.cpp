#include <utility>
#include <string>
#include <memory>
#include <list>
#include <vector>
#include <iterator>
#include <cmath>
#include <iostream>
#include <algorithm>

using namespace std;

struct leafNode;
struct internalNode;

using internalPointer = shared_ptr<internalNode>;
using leafPointer = shared_ptr<leafNode>;
using listInternalIterator = list<internalPointer>::iterator;

struct leafNode
{
    int key;
    int value;
    internalPointer highestReference;
};

struct internalNode
{
    unsigned int height;
    leafPointer leafReference;
    internalPointer leftChild, rightChild, parent;
    listInternalIterator pointerAccess;
};

class QuakeHeap
{
public:
    list<internalPointer> forest;
    listInternalIterator minimumPointer = forest.end();
    unsigned long length = 0;
    float alpha = 0.75;
    void updateMin()
    {
        if (minimumPointer == forest.end())
            minimumPointer--;
        else if (forest.back()->leafReference->value < (*minimumPointer)->leafReference->value)
            minimumPointer = --forest.end();
    }
    void insert(leafPointer &entry)
    {
        internalPointer node = make_shared<internalNode>();
        node->height = 0;
        node->leafReference = entry;
        node->leftChild = node->rightChild = node->parent = nullptr;
        entry->highestReference = node;
        forest.push_back(node);
        updateMin();
        node->pointerAccess = --forest.end();
        length++;
    }
    void decreaseKey(leafPointer &entry, int newValue)
    {
        if (entry->value < newValue)
        {
            cout << "New value is greater than the Current value!" << endl;
            return;
        }
        entry->value = newValue;
        internalPointer node = entry->highestReference;
        if (node->parent == nullptr)
            forest.erase(node->pointerAccess);
        else if (node->parent->leftChild == node)
            node->parent->leftChild = nullptr;
        else
            node->parent->rightChild = nullptr;
        forest.push_back(node);
        updateMin();
    }
    void link(internalPointer &linked, internalPointer &a, internalPointer &b)
    {
        linked = make_shared<internalNode>();
        linked->leftChild = a;
        linked->rightChild = b;
        linked->parent = nullptr;

        if (a != nullptr && b != nullptr)
            linked->leafReference = (a->leafReference->value < b->leafReference->value) ? a->leafReference : b->leafReference;
        else if (a != nullptr)
            linked->leafReference = a->leafReference;
        else
            linked->leafReference = b->leafReference;

        linked->height = max((a != nullptr ? a->height : 0), (b != nullptr ? b->height : 0)) + 1;
        a->parent = linked;
        b->parent = linked;
    }
    void cut(internalPointer root, bool isUpdateMin = false)
    {
        leafPointer entry = root->leafReference;
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
    leafPointer deleteMin()
    {
        listInternalIterator oldMinimum = minimumPointer;
        leafPointer minEntry = (*minimumPointer)->leafReference;
        cut(*minimumPointer);
        forest.erase(oldMinimum);

        vector<list<internalPointer>> roots(1 + ceil(log2(length)));
        vector<int> heights(1 + ceil(log2(length)));

        while (forest.size() > 0)
        {
            internalPointer node = forest.front();
            forest.pop_front();
            roots[node->height].push_back(node);
        }
        for (int i = 0; i < roots.size(); i++)
        {
            list<internalPointer> *node_list = &roots[i];
            if (node_list->size() > 1)
            {
                heights[i] = 1;
            }
            while (node_list->size() > 1)
            {
                internalPointer a = node_list->back();
                node_list->pop_back();
                internalPointer b = node_list->back();
                node_list->pop_back();
                internalPointer linked;
                link(linked, a, b);
                roots[i + 1].push_back(linked);
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

        for (int i = 0; i < roots.size(); ++i)
            if (roots[i].size() > 0)
            {
                if (min_i != -1 && i > min_i)
                    cut(roots[i].front(), true);
                else
                {
                    forest.push_back(roots[i].front());
                    updateMin();
                }
            }
        length--;
        return minEntry;
    }
    // print forest
    void printForest()
    {
        if (length == 0)
            cout << "Empty Forest";
        else
            for (auto node : forest)
                cout << node->leafReference->key << " " << node->leafReference->value << " | ";
        cout << endl;
    }
};

int main()
{
    const unsigned int N = 10;
    QuakeHeap heap;
    vector<leafPointer> entries(N);
    heap.printForest();
    // insert all
    for (int i = 0; i < N; i++)
    {
        entries[i] = make_shared<leafNode>();
        entries[i]->key = i;
        entries[i]->value = rand() % 100;
        heap.insert(entries[i]);
    }
    cout << "Insertion Done" << endl;
    heap.printForest();
    // decrease all value by 1
    for (int i = 0; i < N; i++)
    {
        heap.decreaseKey(entries[i], entries[i]->value - 1);
    }
    cout << heap.forest.size() << endl;
    cout << heap.length << endl;
    heap.printForest();
    cout << "Decrease Done" << endl;
    // delete all
    for (int i = 0; i < N; i++)
    {
        auto minNode = heap.deleteMin();
        cout << minNode->key << " " << minNode->value << endl;
        entries[minNode->key] = nullptr;
        heap.printForest();
    }
    cout << "Delete Done" << endl;
    return 0;
}
