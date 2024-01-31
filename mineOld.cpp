#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

struct Internal_Node;

struct Leaf_Node
{
    int value;
    Internal_Node *highest_reference; // leaf_highest_reference
};

struct Internal_Node
{
    int height;
    Internal_Node *parent, *left_child, *right_child;
    Leaf_Node *leaf_reference; // leaf_node_reference
};

unordered_map<int, vector<Internal_Node *>> forest;
unordered_map<int, Leaf_Node *> map;
unordered_map<int, int> count_level;

Internal_Node *root = nullptr;

void insertElement(int val)
{
    Internal_Node *i_node = new Internal_Node;
    Leaf_Node *l_node = new Leaf_Node;
    l_node->value = val;
    l_node->highest_reference = i_node;
    i_node->leaf_reference = l_node;
    i_node->parent = i_node;
    i_node->left_child = nullptr;
    i_node->right_child = nullptr;
    i_node->height = 0;
    map[val] = l_node;
    count_level[val] += 1;
    forest[i_node->height].push_back(i_node);
    if (root == nullptr || val < root->leaf_reference->value)
        root = i_node;
}

void link(Internal_Node *node1, Internal_Node *node2)
{
    Internal_Node *i_node = new Internal_Node;
    i_node->height = node1->height + 1;
    i_node->left_child = node1;
    i_node->right_child = node2;
    i_node->parent = i_node;
    if (node1->leaf_reference->value < node2->leaf_reference->value)
    {
        i_node->leaf_reference = node1->leaf_reference;
        node1->parent = i_node;
        node1->leaf_reference->highest_reference = i_node;
    }
    else
    {
        i_node->leaf_reference = node2->leaf_reference;
        node2->parent = i_node;
        node2->leaf_reference->highest_reference = i_node;
    }
    forest[i_node->height].push_back(i_node);
    count_level[i_node->height] += 1;
}

void trim(Internal_Node *node)
{
    Internal_Node *left, *right;
    left = node->left_child;
    right = node->right_child;
    node->left_child = nullptr;
    node->right_child = nullptr;
    left->parent = left;
    right->parent = right;
    left->leaf_reference->highest_reference = left;
    right->leaf_reference->highest_reference = right;
    forest[left->height].push_back(left);
    forest[right->height].push_back(right);
    count_level[node->height] -= 1;
    node->height = 0;
    delete node;
}

void cut(Internal_Node *node)
{
    Internal_Node *parent = node->parent;
    if (parent->left_child == node)
        parent->left_child = nullptr;
    else
        parent->right_child = nullptr;
    node->parent = node;
    node->leaf_reference->highest_reference = node;
    forest[node->height].push_back(node);
}

void decrease_key(int val, int decrease)
{
    Leaf_Node *l_node = map[val];
    map.erase(val);
    Internal_Node *i_node = l_node->highest_reference;
    l_node->value -= decrease;
    if (i_node->parent == i_node)
        return;
    Internal_Node *parent = i_node->parent;
    cut(i_node);
    map[l_node->value] = l_node;
}

int main()
{
    // ... (your main function here)
    return 0;
}