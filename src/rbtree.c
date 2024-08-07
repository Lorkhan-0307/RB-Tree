#include "rbtree.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void printTree(rbtree* tree, node_t* parent) {
    if(parent != tree->nil) {
        printf("color: %d, key: %d\n", parent->color, parent->key);
        printTree(tree, parent->left);
        printTree(tree, parent->right);
    }
}

rbtree *new_rbtree(void) {
    rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
    // Setup nil
    node_t* nil = malloc(sizeof(node_t));
    nil->color = RBTREE_BLACK;
    nil->key = (int)NULL;
    nil->parent = NULL;
    p->nil = nil;
    p->root = nil;
    nil->parent = p->root;
    return p;
}

void delete_rbtree(rbtree *t) {
    node_t* cur_node = t->root;
    if(cur_node != t->nil) delete_all_node_by_recursion(t, cur_node);

    free(t->nil);
    free(t);
}

void delete_all_node_by_recursion(rbtree* t, node_t* node) {
    if(node->left != t->nil) delete_all_node_by_recursion(t, node->left);
    if(node->right != t->nil) delete_all_node_by_recursion(t, node->right);
    free(node);
}

void rbtree_insert_checker(rbtree *t, node_t *target) {
    if(target->parent->color == RBTREE_BLACK) return;
    node_t *parent_node = target->parent;
    node_t *gp_node = parent_node->parent;

    node_t *uncle_node = gp_node->left == parent_node ? gp_node->right : gp_node->left;

    bool is_target_left = parent_node->left == target ? true : false;

    if(uncle_node->color == RBTREE_RED) {
        // CASE 1
        parent_node->color = RBTREE_BLACK;
        uncle_node->color = RBTREE_BLACK;
        gp_node->color = RBTREE_RED;
        // Check from gp_node
        if(gp_node == t->root) {
            gp_node->color = RBTREE_BLACK;
            return;
        }
        rbtree_insert_checker(t, gp_node);
        return;
    }

    if(is_target_left) {
        // if case2, change it into case 3
        if(gp_node->right == parent_node) {
            circulation(t, parent_node, false);
            target = parent_node;
            parent_node = target->parent;

            // Do Case 3
            parent_node->color = RBTREE_BLACK;
            gp_node->color = RBTREE_RED;

            // Circulation to L or R(this time, L) from GP
            circulation(t, gp_node, true);
            return;
        }

        // Do Case 3
        parent_node->color = RBTREE_BLACK;
        gp_node->color = RBTREE_RED;

        // Circulation to L or R(this time, L) from GP
        circulation(t, gp_node, false);
        return;
    }

    // if case2, change it into case 3
    if(gp_node->left == parent_node) {
        circulation(t, parent_node, true);
        target = parent_node;
        parent_node = target->parent;

        parent_node->color = RBTREE_BLACK;
        gp_node->color = RBTREE_RED;

        // Circulation to L or R(this time, L) from GP
        circulation(t, gp_node, false);
        return;
    }

    /// Do Case 3
    parent_node->color = RBTREE_BLACK;
    gp_node->color = RBTREE_RED;

    // Circulation to L or R(this time, L) from GP
    circulation(t, gp_node, true);
    return;

}

node_t *rbtree_insert(rbtree *t, const key_t key) {
    // printf("================= inserting %d =================\n", key);
    // printTree(t, t->root);
    // implement insert
    // if root is NULL, new node goes in as root
    node_t * new_node = (node_t*)malloc(sizeof(node_t));
    // set new node's color as red
    new_node->color = RBTREE_RED;
    new_node->key = key;
    new_node->left = t->nil;
    new_node->parent = t->nil;
    new_node->right = t->nil;

    if(t->root == t->nil) {
        t->root = new_node;
        new_node->color = RBTREE_BLACK;
        return t->root;
    }

    // not root
    // if newnode's depth == 1
    if(t->root->left == t->nil && t->root->right == t->nil) {

        new_node->parent = t->root;
        if(t->root->key < key) {
            // go right
            t->root->right = new_node;
            return t->root;
        }

        // go left
        t->root->left = new_node;
        return t->root;
    }

    // Grandparent Node
    node_t* parent_node = t->root;

    while(1) {
        // same values should not go in to binary search tree.
        if(key > parent_node->key) {
            if(parent_node->right != t->nil) parent_node = parent_node->right;
            else {
                parent_node->right = new_node;
                new_node->parent = parent_node;
                // General insert
                if(parent_node->color == RBTREE_BLACK) return t->root;
                rbtree_insert_checker(t, new_node);
                return t->root;
            }
        }
        else{
            // parent_node->key <= key
            if(parent_node->left != t->nil) {
                parent_node = parent_node->left;
            }
            else {
                parent_node->left = new_node;
                new_node->parent = parent_node;
                if(parent_node->color == RBTREE_BLACK) return t->root;
                // General insert
                rbtree_insert_checker(t, new_node);
                return t->root;
            }
        }
    }
}

void circulation(rbtree* t, node_t* target, const bool dir) {
    // dir -> true -> L, false -> R
    node_t* target_parent_node = target->parent;
    node_t* target_child_left = target->left;
    node_t* target_child_right = target->right;

    const bool is_target_root = t->root == target? true : false;


    if(dir) {
        // Left circulation
        // right child becomes!

        // paretn->left == target
        if(target_parent_node->left == target) target_parent_node->left = target_child_right;
        // paretn->right == target
        else target_parent_node->right = target_child_right;
        target_child_right->parent = target_parent_node;
        target->right = target_child_right->left;
        if(target_child_right->left != t->nil) target_child_right->left->parent = target;
        target_child_right->left = target;
        target->parent = target_child_right;
        if(is_target_root) {
            t->root = target->parent;
            target->parent->color = RBTREE_BLACK;
        }
        return;
    }
    // Right circulation
    if(target_parent_node->left == target) target_parent_node->left = target_child_left;
    // paretn->right == target
    else target_parent_node->right = target_child_left;
    target_child_left->parent = target_parent_node;
    target->left = target_child_left->right;
    target_child_left->right->parent = target;
    target_child_left->right = target;
    target->parent = target_child_left;
    if(is_target_root) {
        t->root = target->parent;
        target->parent->color = RBTREE_BLACK;
    }
    return;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
    node_t* cur_node = t->root;

    if(cur_node == t->nil) return NULL;

    while(cur_node != t->nil && cur_node->key != key ) {
        if(key > cur_node->key) {
            cur_node = cur_node->right;
        }
        else {
            cur_node = cur_node->left;
        }
    }
    if(cur_node != t->nil) return cur_node;
    return NULL;
}

node_t *rbtree_min(const rbtree *t) {
    node_t* cur_node = t->root;
    if(cur_node == t->nil) return NULL;
    while(cur_node->left != t->nil) {
        cur_node = cur_node->left;
    }
    return cur_node;
}

node_t *rbtree_max(const rbtree *t) {
    node_t* cur_node = t->root;
    if(cur_node == t->nil) return NULL;
    while(cur_node->right != t->nil) {
        cur_node = cur_node->right;
    }
    return cur_node;
}

node_t * find_successor(const rbtree* t, node_t* start_node) {
    node_t *current = start_node->right;
    if (current == t->nil) // 오른쪽 자식이 없으면
    {
        current = start_node;
        while (1)
        {
            if (current->parent->right == current) // current가 오른쪽 자식인 경우
                current = current->parent;           // 부모 노드로 이동 후 이어서 탐색
            else
                return current->parent; // current가 왼쪽 자식인 경우 부모 리턴
        }
    }
    while (current->left != t->nil) // 왼쪽 자식이 있으면
        current = current->left;      // 왼쪽 끝으로 이동
    return current;
}


void erase_circulation(rbtree *t, node_t * target, const bool is_rotate_left) {

    node_t *parent = target->parent;
    node_t *grand_parent = parent->parent;
    node_t *target_child = is_rotate_left? target->left : target->right;

    if (parent == t->root)
        t->root = target;
    else
    { // 1-1) 노드의 부모를 grand_parent로 변경
        if (grand_parent->left == parent)
            grand_parent->left = target;
        else
            grand_parent->right = target;
    }
    if(is_rotate_left) {
        target->parent = grand_parent; // 1-2) 노드를 grand_parent의 자식으로 변경 (양방향 연결)
        parent->parent = target;       // 2-1) parent의 부모를 노드로 변경
        target->left = parent;         // 2-2) parent를 노드의 자식으로 변경 (양방향 연결)
        parent->right = target_child;   // 3-1) 노드의 자식의 부모를 parent로 변경
        target_child->parent = parent;  // 3-2) 노드의 자식을 부모의 자식으로 변경 (양방향 연결)
    }
    else {
        target->parent = grand_parent; // 1-2) 노드를 grand_parent의 자식으로 변경 (양방향 연결)
        parent->parent = target;       // 2-1) parent의 부모를 노드로 변경
        target->right = parent;        // 2-2) parent를 노드의 자식으로 변경 (양방향 연결)
        target_child->parent = parent; // 3-1) 노드의 자식의 부모를 parent로 변경
        parent->left = target_child;   // 3-2) 노드의 자식을 부모의 자식으로 변경 (양방향 연결)
    }
}

void exchange_color(node_t *a, node_t *b)
{
    int tmp = a->color;
    a->color = b->color;
    b->color = (tmp == RBTREE_BLACK) ? RBTREE_BLACK : RBTREE_RED;
}

// 키 값을 기준으로 다음 노드를 반환하는 함수
node_t *get_next_node(const rbtree *t, node_t *p)
{
    node_t *current = p->right;
    if (current == t->nil) // 오른쪽 자식이 없으면
    {
        current = p;
        while (1)
        {
            if (current->parent->right == current) // current가 오른쪽 자식인 경우
                current = current->parent;           // 부모 노드로 이동 후 이어서 탐색
            else
                return current->parent; // current가 왼쪽 자식인 경우 부모 리턴
        }
    }
    while (current->left != t->nil) // 왼쪽 자식이 있으면
        current = current->left;      // 왼쪽 끝으로 이동
    return current;
}

void rbtree_erase_checker(rbtree *t, node_t *parent, const bool is_left) {

    node_t *extra_black = is_left ? parent->left : parent->right;
    if (extra_black->color == RBTREE_RED)
    {
        extra_black->color = RBTREE_BLACK;
        return;
    }

    node_t *sibling = is_left ? parent->right : parent->left;
    node_t *sibling_left = sibling->left;
    node_t *sibling_right = sibling->right;

    if (sibling->color == RBTREE_RED)
    {
        if (is_left)
            circulation(t, parent, true);
        else
            circulation(t, parent, false);
        exchange_color(sibling, parent);
        rbtree_erase_checker(t, parent, is_left);
        return;
    }

    node_t *near = is_left ? sibling_left : sibling_right;
    node_t *distant = is_left ? sibling_right : sibling_left;

    if (is_left && near->color == RBTREE_RED && distant->color == RBTREE_BLACK)
    {
        circulation(t, near->parent, false);
        exchange_color(sibling, near);
        rbtree_erase_checker(t, parent, is_left);
        return;
    }

    if (is_left && distant->color == RBTREE_RED)
    {
        circulation(t, sibling->parent, true);
        exchange_color(sibling, parent);
        distant->color = RBTREE_BLACK;
        return;
    }

    if (near->color == RBTREE_RED && distant->color == RBTREE_BLACK)
    {
        circulation(t, near->parent, true);
        exchange_color(sibling, near);
        rbtree_erase_checker(t, parent, is_left);
        return;
    }

    if (distant->color == RBTREE_RED)
    {
        circulation(t, sibling->parent, true);
        exchange_color(sibling, parent);
        distant->color = RBTREE_BLACK;
        return;
    }

    sibling->color = RBTREE_RED;

    if (parent != t->root)
        rbtree_erase_checker(t, parent->parent, parent->parent->left == parent);
}

void rbtree_erase_fixup(rbtree *t, node_t *parent, const int is_left)
{
    node_t *extra_black = is_left ? parent->left : parent->right;
    if (extra_black->color == RBTREE_RED)
    {
        extra_black->color = RBTREE_BLACK;
        return;
    }

    node_t *sibling = is_left ? parent->right : parent->left;
    node_t *sibling_left = sibling->left;
    node_t *sibling_right = sibling->right;

    if (sibling->color == RBTREE_RED)
    {
        if (is_left)
            erase_circulation(t, sibling, true);
        else
            erase_circulation(t, sibling, false);
        exchange_color(sibling, parent);
        rbtree_erase_fixup(t, parent, is_left);
        return;
    }

    node_t *near = is_left ? sibling_left : sibling_right;    // 형제의 자식 중 extra_black으로부터 가까운 노드
    node_t *distant = is_left ? sibling_right : sibling_left; // 형제의 자식 중 extra_black으로부터 먼 노드

    if (is_left && near->color == RBTREE_RED && distant->color == RBTREE_BLACK)
    {
        erase_circulation(t, near, false);
        exchange_color(sibling, near);
        rbtree_erase_fixup(t, parent, is_left);
        return;
    }

    if (is_left && distant->color == RBTREE_RED)
    {
        erase_circulation(t, sibling, true);
        exchange_color(sibling, parent);
        distant->color = RBTREE_BLACK;
        return;
    }

    // CASE 2
    if (near->color == RBTREE_RED && distant->color == RBTREE_BLACK)
    {
        erase_circulation(t, near, true);
        exchange_color(sibling, near);
        rbtree_erase_fixup(t, parent, is_left);
        return;
    }

    // CASE 3
    if (distant->color == RBTREE_RED)
    {
        erase_circulation(t, sibling, false);
        exchange_color(sibling, parent);
        distant->color = RBTREE_BLACK;
        return;
    }

    // CASE 2
    sibling->color = RBTREE_RED;

    if (parent != t->root)
        rbtree_erase_fixup(t, parent->parent, parent->parent->left == parent);
}

int rbtree_erase(rbtree *t, node_t *p) {
    //printf("=============== erasing %d ===========\n", delete->key);
    //printTree(t, t->root);
    // TODO: implement erase

    node_t *successor_node;
    node_t *successor_node_replacer;

    if (p->left != t->nil && p->right != t->nil)
    {
        successor_node = get_next_node(t, p);
        successor_node_replacer = successor_node->right;
        p->key = successor_node->key;
    }
    else
    {
        successor_node = p;
        successor_node_replacer = (successor_node->right != t->nil) ? successor_node->right : successor_node->left;
    }

    node_t *successor_node_parent = successor_node->parent;

    if (successor_node == t->root)
    {
        t->root = successor_node_replacer;
        t->root->color = RBTREE_BLACK;
        free(successor_node);
        return 0;
    }

    int is_successor_black = successor_node->color;
    int is_successor_left = successor_node_parent->left == successor_node;

    if (is_successor_left)
        successor_node_parent->left = successor_node_replacer;
    else
        successor_node_parent->right = successor_node_replacer;

    successor_node_replacer->parent = successor_node_parent;
    free(successor_node);

    if (is_successor_black)
        rbtree_erase_fixup(t, successor_node_parent, is_successor_left);
    return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
    node_t *current = rbtree_min(t);
    arr[0] = current->key;
    for (int i = 1; i < n; i++)
    {
        if (current == t->nil)
            break;
        current = get_next_node(t, current);
        if (current == t->nil)
            break;
        arr[i] = current->key;
    }
    return 0;
}
