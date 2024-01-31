#include <iostream>
#include <vector>
#include <list>

using namespace std;

struct leafNode;

struct internalNode
{
    int level;
    internalNode *parent, *leftChild, *rightChild;
    leafNode *leafReference;
};

struct leafNode
{
    int key;
    internalNode *highestReference;
};

class quakeHeap
{
public:
    vector<list<internalNode *>> rootsLevelWise;
    vector<int> nodesLevelWise;

    quakeHeap()
    {
        rootsLevelWise.push_back(list<internalNode *>());
        nodesLevelWise.push_back(0);
    }

    leafNode *insert(int value)
    {
        leafNode *leaf = new leafNode;
        internalNode *node = new internalNode;
        leaf->key = value;
        leaf->highestReference = node;
        node->leafReference = leaf;
        node->parent = nullptr;
        node->level = 0;
        node->leftChild = nullptr;
        node->rightChild = nullptr;
        rootsLevelWise[0].push_back(node);
        nodesLevelWise[0]++;
        delete node;
        return leaf;
    }
    leafNode *extractMin()
    {
        leafNode *minimumLeaf = findMin();
        deleteMin(minimumLeaf);
        consolidate();
        quake();
        return minimumLeaf;
    }
    void decreaseKey(leafNode *node, int newValue)
    {
        internalNode *highestReference = node->highestReference;
        if (highestReference->parent != nullptr)
            cut(highestReference);
        node->key = newValue;
    }
    leafNode *findMin()
    {
        leafNode *min = nullptr;
        for (list<internalNode *> roots : rootsLevelWise)
        {
            for (internalNode *root : roots)
            {
                leafNode *treeMinimum = root->leafReference;
                if (min == nullptr || treeMinimum->key < min->key)
                    min = treeMinimum;
            }
        }
        return min;
    }
    void deleteMin(leafNode *minLeaf)
    {
        int minimum = minLeaf->key;
        internalNode *currentNode = minLeaf->highestReference;
        int currentLevel = currentNode->level;
        while (currentLevel > 0)
        {
            rootsLevelWise[currentLevel].remove(currentNode);
            nodesLevelWise[currentLevel]--;
            currentLevel--;

            internalNode *leftChild = currentNode->leftChild;
            internalNode *rightChild = currentNode->rightChild;

            rootsLevelWise[currentLevel].push_back(leftChild);
            if (leftChild->leafReference->key == minimum)
                currentNode = leftChild;
            rootsLevelWise[currentLevel].push_back(rightChild);
            if (rightChild->leafReference->key == minimum)
                currentNode = rightChild;
        }
        if (currentNode->leafReference != nullptr)
        {
            rootsLevelWise[currentLevel].remove(currentNode);
            nodesLevelWise[currentLevel]--;
            delete currentNode;
        }
    }
    void consolidate()
    {
        for (int i = 0; i < rootsLevelWise.size(); i++)
        {
            list<internalNode *> roots = rootsLevelWise[i];
            while (roots.size() > 1)
            {
                internalNode *root1 = roots.front();
                roots.pop_front();
                internalNode *root2 = roots.front();
                roots.pop_front();
                link(root1, root2);
            }
        }
    }
    void quake()
    {
        bool doQuake = false;
        for (int i = 0; i < nodesLevelWise.size() - 1; i++)
        {
            if (doQuake)
                nodesLevelWise[i] = 0;
            else
            {
                if (nodesLevelWise[i + 1] > 3.0 / 4.0 * nodesLevelWise[i])
                    doQuake = true;
            }
        }
    }
    void cut(internalNode *node)
    {
        internalNode *parent = node->parent;
        if (parent != nullptr)
        {
            internalNode *leftChild = parent->leftChild;
            if (leftChild == node)
                leftChild = nullptr;
            internalNode *rightChild = parent->rightChild;
            if (rightChild == node)
                rightChild = nullptr;
            node->parent = nullptr;
            rootsLevelWise[node->level].push_back(node);
        }
    }
    void link(internalNode *root1, internalNode *root2)
    {
        int currentLevel = root1->level;
        internalNode *newRoot = new internalNode;
        newRoot->parent = nullptr;
        newRoot->level = currentLevel + 1;
        newRoot->leftChild = root1;
        newRoot->rightChild = root2;
        if (root1->leafReference->key < root2->leafReference->key)
        {
            newRoot->leafReference = root1->leafReference;
            root1->leafReference->highestReference = newRoot;
        }
        else
        {
            newRoot->leafReference = root2->leafReference;
            root2->leafReference->highestReference = newRoot;
        }
        root1->parent = newRoot;
        root2->parent = newRoot;
        if (rootsLevelWise.size() == currentLevel + 1)
            rootsLevelWise.push_back(list<internalNode *>());
        rootsLevelWise[currentLevel + 1].push_back(newRoot);
        if (nodesLevelWise.size() == currentLevel + 1)
            nodesLevelWise.push_back(0);
        nodesLevelWise[currentLevel + 1]++;
    }
};

int main()
{
    unordered_map<int, leafNode *> entries;
    entries.reserve(1000);
    quakeHeap heap;
    for (int i = 0; i < 1000; i++)
    {
        entries[i] = heap.insert(i);
    }
    for (int i = 0; i < 1000; i++)
    {
        cout << entries[i]->key << endl;
    }
    return 0;
}