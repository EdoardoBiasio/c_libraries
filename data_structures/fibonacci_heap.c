#include "ds.h"
#include <string.h>
#include <stdlib.h>

int fibonacci_heap_init(fibonacci_heap_t* h, size_t type_size, int (*cmp)(void*, void*), int heap_type) {
    if (!h || !cmp)
	return -1;

    h->type_size = type_size;
    h->list_size = 0;
    h->size = 0;
    h->cmp = cmp;
    h->root = h->tree_list = NULL;
    return 0;
}

void fibonacci_heap_close_node(fibonacci_heap_node_t* node) {
    if (node->left) {
	node->left->right = NULL;
	fibonacci_heap_close_node(node->left);
    }
    if (node->right) {
	node->right->left = NULL;
	fibonacci_heap_close_node(node->right);
    }
    
    if (node->child)
	fibonacci_heap_close_node(node->child);
    free(node);
}

int fibonacci_heap_close(fibonacci_heap_t* h) {
    if (!h)
	return -1;

    if (h->tree_list)
	fibonacci_heap_close_node(h->tree_list);
    h->size = 0;
    h->list_size = 0;
    h->root = h->tree_list = NULL;
    h->cmp = NULL;
    h->type_size = 0;
    return 0;
}


void fibonacci_heap_list_append(fibonacci_heap_node_t* list, fibonacci_heap_node_t* node) {
    fibonacci_heap_node_t *temp;
    node->parent = list->parent;
    temp = list->left;
    list->left = node;
    node->right = list;
    temp->right = node;
    node->left = temp;    
}

#define node_get_el(node) ((node) + sizeof(fibonacci_heap_node_t))
fibonacci_heap_node_t* fibonacci_heap_insert(fibonacci_heap_t* h, void* el, int heap_type) {
    fibonacci_heap_node_t *node;
    if (!h || !el)
	return NULL;

    node = (fibonacci_heap_node_t*)malloc(sizeof(fibonacci_heap_node_t) + h->type_size);
    node->marked = 0;
    node->degree = 0;
    node->child = NULL;
    memcpy(node + (sizeof(fibonacci_heap_node_t)), el, h->type_size);

    h->size++;

    node->parent = NULL;
    
    if (!h->root) {
	h->root = h->tree_list = node;
	h->list_size = 1;
	node->left = node->right = node;
    } else {
	if (heap_type == min_heap_type) {
	    if (h->cmp(el, node_get_el(h->root)) < 0) {
		h->root = node;
	    }
	} else {
	    if (h->cmp(el, node_get_el(h->root)) > 0) {
		h->root = node;
	    }
	}
	fibonacci_heap_list_append(h->tree_list, node);
	h->list_size++;
    }
    return 0;
}

void fibonacci_heap_consolidate(fibonacci_heap_t* h, int heap_type) {
    fibonacci_heap_node_t **R;
    int i, first, g;
    fibonacci_heap_node_t *x, *y, *temp, *start;
    size_t maxdeg;
    maxdeg = 0;
    x = h->tree_list;
    for (i = 0; i < h->list_size; i++) {
	if (x->degree > maxdeg)
	    maxdeg = x->degree;
	x = x->right;
    }

    R = calloc(sizeof(fibonacci_heap_node_t*), maxdeg);
    x = h->tree_list;
    start = x;
    while (h->list_size > 0) {

	g = x->degree;
	while (R[g] != NULL) {
	    y = R[g];
	    if (heap_type == min_heap_type && h->cmp(node_get_el(x), node_get_el(y)) > 0) {
		if (first)
		    start = y;
		temp = x;
		x = y;
		y = temp;
	    } else if (heap_type == max_heap_type && h->cmp(node_get_el(x), node_get_el(y)) < 0) {
		if (first)
		    start = y;
		temp = x;
		x = y;
		y = temp;
	    }
	    y->left->right = y->right;
	    y->right->left = y->left;
	    h->list_size--;
	    y->marked = 0;
	    if (!x->child) {
		x->child = y;
		y->left = y->right = y;
		x->degree = 1;
	    } else {
		fibonacci_heap_list_append(x->child, y);
		x->degree++;
	    }
	    R[g] = NULL;
	    g++;
	}

	R[g] = x;
	x = x->right;
	R[g]->left->right = R[g]->right;
	R[g]->right->left = R[g]->left;
	h->list_size--;
	if (h->list_size)
	    h->tree_list = NULL;
    }

    for (i = 0; i < maxdeg; i++) {
	if (R[i]) {
	    if (!h->tree_list) {
		h->tree_list = R[i];
		h->tree_list->left = h->tree_list->right = h->tree_list;
		h->list_size++;
	    } else
		fibonacci_heap_list_append(h->tree_list, R[i]);
	}
    }
    x = temp = h->tree_list;
    for (i = 0; i < h->list_size; i++) {
	x = x->right;
	if (heap_type == min_heap_type && h->cmp(node_get_el(x), node_get_el(temp)) < 0)
	    temp = x;
	else if (heap_type == max_heap_type && h->cmp(node_get_el(x), node_get_el(temp)) > 0)
	    temp = x;
    }

    h->root = temp;

    free(R);
}

int fibonacci_heap_pop_root(fibonacci_heap_t* h, void* out, int heap_type) {
    fibonacci_heap_node_t* outn, *node, *next;
   
    
    int i = 0;
    if (!h)
	return -1;
    if (h->size == 0)
	return 0;
    outn = h->root;
    h->size--;
    node = outn->child;
    for (i = 0; i < outn->degree; i++) {
	next = node->right;
	fibonacci_heap_list_append(h->tree_list, node);
	node = next;
	h->list_size++;
    }
    outn->child = NULL;
    if (h->size == 0) {
	h->root = NULL;
	h->tree_list = NULL;
	h->list_size--;
    } else {
	h->root = h->root->right;
	fibonacci_heap_consolidate(h, heap_type);
    }
    if (out)
	memcpy(out, outn + sizeof(fibonacci_heap_node_t), h->type_size);
    return 0;
}

int fibonacci_heap_peek_root(fibonacci_heap_t* h, void* out) {
    if (!h || !out)
	return -1;
    memcpy(out, h->root + sizeof(fibonacci_heap_node_t), h->type_size);
    return 0;
}

void* fibonacci_heap_peek_root_ref(fibonacci_heap_t* h) {
    if (!h)
	return NULL;
    return h->root + sizeof(fibonacci_heap_node_t);
}

int fibonacci_heap_peek_root_node(fibonacci_heap_t* h, fibonacci_heap_t* out) {
    if (!h || !out)
	return -1;
    memcpy(out, h->root, sizeof(fibonacci_heap_node_t) + h->type_size);
    return 0;
}

fibonacci_heap_node_t* fibonacci_heap_peek_root_node_ref(fibonacci_heap_t* h) {
    if (!h)
	return NULL;
    return h->root;
}

void fibonacci_heap_union(fibonacci_heap_t* lhs, fibonacci_heap_t* rhs, int heap_type) {

    fibonacci_heap_node_t* temp, *best;
    int i = 0;
    if (!lhs || !rhs || lhs->type_size != rhs->type_size)
	return;

    for (i = 0; i < rhs->list_size; i++) {
	lhs->list_size++;
	temp = rhs->tree_list;
	rhs->tree_list->left->right = rhs->tree_list->right;
	rhs->tree_list->right->left = rhs->tree_list->left;
	rhs->tree_list = rhs->tree_list->right;
	fibonacci_heap_list_append(lhs->tree_list, temp);

	if (heap_type == min_heap_type && lhs->cmp(node_get_el(lhs->root), node_get_el(temp)) > 0)
	    lhs->root = temp;
	else if (heap_type == max_heap_type && lhs->cmp(node_get_el(lhs->root), node_get_el(temp)) < 0)
	    lhs->root = temp;
    }
}

void fibonacci_heap_update_value(fibonacci_heap_t* h, fibonacci_heap_node_t* node, void* newval, int heap_type) {
    fibonacci_heap_node_t* par, *temp;
    if (!h || !node || !newval)
	return;

    if (heap_type == min_heap_type && h->cmp(newval, node_get_el(node)) >= 0) {
	memcpy(node + sizeof(fibonacci_heap_t), newval, h->type_size);
	return;
    } else if (heap_type == max_heap_type && h->cmp(newval, node_get_el(node)) <= 0) {
	memcpy(node + sizeof(fibonacci_heap_t), newval, h->type_size);
	return;	
    }

    node->left->right = node->right;
    node->right->left = node->left;
    par = node->parent;
    if (par)
	par->degree--;
    memcpy(node + sizeof(fibonacci_heap_t), newval, h->type_size);
    node->parent->child = NULL;
    node->parent = NULL;
    node->marked = 0;

    h->list_size++;
    fibonacci_heap_list_append(h->tree_list, node);
    if (heap_type == min_heap_type && h->cmp(node_get_el(h->root), node_get_el(node)) > 0)
	h->root = node;
    else if (heap_type == max_heap_type && h->cmp(node_get_el(h->root), node_get_el(node)) < 0)
	h->root = node;

    while (par && par->marked == 1) {
	par->marked = 0;
	par->left->right = par->right;
	par->right->left = par->left;
	temp = par->parent;
	if (par->parent) {
	    par->parent->child = NULL;
	    par->parent->degree--;
	}
	par->parent = NULL;
       

	h->list_size++;
	fibonacci_heap_list_append(h->tree_list, par);
	if (heap_type == min_heap_type && h->cmp(node_get_el(h->root), node_get_el(par)) > 0)
	    h->root = par;
	else if (heap_type == max_heap_type && h->cmp(node_get_el(h->root), node_get_el(par)) < 0)
	    h->root = par;
	par = temp;
    }
    if (par && par->parent != NULL)
	par->marked = 1;    
}

