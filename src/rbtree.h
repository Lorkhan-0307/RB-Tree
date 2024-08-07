#ifndef _RBTREE_H_
#define _RBTREE_H_

#include <stdbool.h>
#include <stddef.h>

typedef enum { RBTREE_RED, RBTREE_BLACK } color_t;

typedef int key_t;

typedef struct node_t {
    color_t color;
    key_t key;
    struct node_t *parent, *left, *right;
} node_t;

typedef struct {
    node_t *root;
    node_t *nil;  // for sentinel
} rbtree;

rbtree *new_rbtree(void);
void delete_rbtree(rbtree *);
void delete_all_node_by_recursion(rbtree *, node_t *);
void printTree(rbtree *, node_t *);
void circulation(rbtree *,node_t*, const bool);
void rbtree_insert_checker(rbtree *, node_t *);

node_t *rbtree_insert(rbtree *, const key_t);
node_t *rbtree_find(const rbtree *, const key_t);
node_t *rbtree_min(const rbtree *);
node_t *rbtree_max(const rbtree *);
int rbtree_erase(rbtree *, node_t *p);
node_t *find_successor(const rbtree *, node_t* );

int rbtree_to_array(const rbtree *, key_t *, const size_t);

#endif  // _RBTREE_H_
