/*
 * Copyright 2014, Vietor Liu <vietor.liu at gmail.com>
 * All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Redistribution source code with modification from
 * http://ftp.cc.uoc.gr/mirrors/OpenBSD/src/usr.sbin/nsd/rbtree.c
 * Copyright (c) 2001-2006, NLnet Labs. All rights reserved.
 * It's BSD-style license, See detail from
 * http://ftp.cc.uoc.gr/mirrors/OpenBSD/src/usr.sbin/nsd/LICENSE
 *
 */

#include "rbtree.h"

struct rbnode rbnode_null = {
	&rbnode_null,
	&rbnode_null,
	&rbnode_null,
	RBCOLOR_BLACK
};

static void rbtree_rotate_left(struct rbtree *rbtree, struct rbnode *node)
{
	struct rbnode *right = node->right;

	node->right = right->left;
	if (right->left != RBNODE_NULL)
		right->left->parent = node;

	right->parent = node->parent;

	if (node->parent != RBNODE_NULL) {
		if (node == node->parent->left) {
			node->parent->left = right;
		} else  {
			node->parent->right = right;
		}
	} else {
		rbtree->root = right;
	}
	right->left = node;
	node->parent = right;
}

static void rbtree_rotate_right(struct rbtree *rbtree, struct rbnode *node)
{
	struct rbnode *left = node->left;

	node->left = left->right;
	if (left->right != RBNODE_NULL)
		left->right->parent = node;

	left->parent = node->parent;

	if (node->parent != RBNODE_NULL) {
		if (node == node->parent->right) {
			node->parent->right = left;
		} else  {
			node->parent->left = left;
		}
	} else {
		rbtree->root = left;
	}
	left->right = node;
	node->parent = left;
}

static void rbtree_insert_balance(struct rbtree *rbtree, struct rbnode *node)
{
	struct rbnode *uncle;

	while (node != rbtree->root && node->parent->color == RBCOLOR_RED) {
		if (node->parent == node->parent->parent->left) {
			uncle = node->parent->parent->right;

			if (uncle->color == RBCOLOR_RED) {
				node->parent->color = RBCOLOR_BLACK;
				uncle->color = RBCOLOR_BLACK;
				node->parent->parent->color = RBCOLOR_RED;
				node = node->parent->parent;
			} else {
				if (node == node->parent->right) {
					node = node->parent;
					rbtree_rotate_left(rbtree, node);
				}
				node->parent->color = RBCOLOR_BLACK;
				node->parent->parent->color = RBCOLOR_RED;
				rbtree_rotate_right(rbtree, node->parent->parent);
			}
		} else {
			uncle = node->parent->parent->left;

			if (uncle->color == RBCOLOR_RED) {
				node->parent->color = RBCOLOR_BLACK;
				uncle->color = RBCOLOR_BLACK;
				node->parent->parent->color = RBCOLOR_RED;
				node = node->parent->parent;
			} else {
				if (node == node->parent->left) {
					node = node->parent;
					rbtree_rotate_right(rbtree, node);
				}
				node->parent->color = RBCOLOR_BLACK;
				node->parent->parent->color = RBCOLOR_RED;
				rbtree_rotate_left(rbtree, node->parent->parent);
			}
		}
	}
	rbtree->root->color = RBCOLOR_BLACK;
}

static inline void change_parent_ptr(struct rbtree* rbtree, struct rbnode* parent, struct rbnode* old, struct rbnode* new)
{
	if(parent == RBNODE_NULL)
	{
		if(rbtree->root == old) rbtree->root = new;
		return;
	}
	if(parent->left == old) parent->left = new;
	if(parent->right == old) parent->right = new;
}

struct rbnode *rbtree_insert3(struct rbtree *rbtree, struct rbnode *data, int flag)
{
	int r = 0;
	struct rbnode *node = rbtree->root;
	struct rbnode *parent = RBNODE_NULL;

	while (node != RBNODE_NULL) {
		parent = node;
		r = rbtree->compare(data, node);

		if (r < 0)
			node = node->left;
		else if(r > 0 || flag == 0)
			node = node->right;
		else if(flag == 1) {
			/* return exists node */
			rbnode_init(data);
			return node;
		}
		else {  /* flag == 2 */
			/* return and replace exists node */
			data->left = node->left;
			data->right = node->right;
			data->color = node->color;
			if(node->left != RBNODE_NULL)
				node->left->parent = data;
			if(node->right != RBNODE_NULL)
				node->right->parent = data;
			data->parent = node->parent;
			change_parent_ptr(rbtree, node->parent, node, data);
			rbnode_init(node);
			return node;
		}
	}

	data->parent = parent;
	data->left = data->right = RBNODE_NULL;
	data->color = RBCOLOR_RED;

	if (parent != RBNODE_NULL) {
		if (r < 0) {
			parent->left = data;
		} else {
			parent->right = data;
		}
	} else {
		rbtree->root = data;
	}
	rbtree_insert_balance(rbtree, data);
	return RBNODE_NULL;
}

static inline void swap_ul(unsigned long* x, unsigned long* y)
{
	unsigned long t = *x; *x = *y; *y = t;
}

static inline void swap_np(struct rbnode** x, struct rbnode** y)
{
	struct rbnode* t = *x; *x = *y; *y = t;
}

static inline void change_child_ptr(struct rbnode* child, struct rbnode* old, struct rbnode* new)
{
	if(child == RBNODE_NULL)
		return;
	if(child->parent == old)
		child->parent = new;
}

static void rbtree_delete_balance(struct rbtree* rbtree, struct rbnode* child, struct rbnode* child_parent)
{
	struct rbnode* sibling;
	int go_up = 1;

	if(child_parent->right == child)
		sibling = child_parent->left;
	else
		sibling = child_parent->right;

	while(go_up)
	{
		if(child_parent == RBNODE_NULL)
			return;

		if(sibling->color == RBCOLOR_RED)
		{
			child_parent->color = RBCOLOR_RED;
			sibling->color = RBCOLOR_BLACK;
			if(child_parent->right == child)
				rbtree_rotate_right(rbtree, child_parent);
			else
				rbtree_rotate_left(rbtree, child_parent);
			if(child_parent->right == child)
				sibling = child_parent->left;
			else
				sibling = child_parent->right;
		}

		if(child_parent->color == RBCOLOR_BLACK
			&& sibling->color == RBCOLOR_BLACK
			&& sibling->left->color == RBCOLOR_BLACK
			&& sibling->right->color == RBCOLOR_BLACK)
		{
			if(sibling != RBNODE_NULL)
				sibling->color = RBCOLOR_RED;
			child = child_parent;
			child_parent = child_parent->parent;
			if(child_parent->right == child)
				sibling = child_parent->left;
			else
				sibling = child_parent->right;
		}
		else
			go_up = 0;
	}

	if(child_parent->color == RBCOLOR_RED
		&& sibling->color == RBCOLOR_BLACK
		&& sibling->left->color == RBCOLOR_BLACK
		&& sibling->right->color == RBCOLOR_BLACK)
	{
		if(sibling != RBNODE_NULL)
			sibling->color = RBCOLOR_RED;
		child_parent->color = RBCOLOR_BLACK;
		return;
	}

	if(child_parent->right == child
		&& sibling->color == RBCOLOR_BLACK
		&& sibling->right->color == RBCOLOR_RED
		&& sibling->left->color == RBCOLOR_BLACK)
	{
		sibling->color = RBCOLOR_RED;
		sibling->right->color = RBCOLOR_BLACK;
		rbtree_rotate_left(rbtree, sibling);
		if(child_parent->right == child)
			sibling = child_parent->left;
		else
			sibling = child_parent->right;
	}
	else if(child_parent->left == child
		&& sibling->color == RBCOLOR_BLACK
		&& sibling->left->color == RBCOLOR_RED
		&& sibling->right->color == RBCOLOR_BLACK)
	{
		sibling->color = RBCOLOR_RED;
		sibling->left->color = RBCOLOR_BLACK;
		rbtree_rotate_right(rbtree, sibling);
		if(child_parent->right == child)
			sibling = child_parent->left;
		else
			sibling = child_parent->right;
	}

	sibling->color = child_parent->color;
	child_parent->color = RBCOLOR_BLACK;
	if(child_parent->right == child)
	{
		sibling->left->color = RBCOLOR_BLACK;
		rbtree_rotate_right(rbtree, child_parent);
	}
	else
	{
		sibling->right->color = RBCOLOR_BLACK;
		rbtree_rotate_left(rbtree, child_parent);
	}
}

void rbtree_delete(struct rbtree *rbtree, struct rbnode *to_delete)
{
	struct rbnode *child;

	if(rbnode_empty(to_delete))
		return;

	if(to_delete->left != RBNODE_NULL &&
		to_delete->right != RBNODE_NULL)
	{
		struct rbnode *smright = to_delete->right;

		while(smright->left != RBNODE_NULL)
			smright = smright->left;

		swap_ul(&to_delete->color, &smright->color);

		change_parent_ptr(rbtree, to_delete->parent, to_delete, smright);
		if(to_delete->right != smright)
			change_parent_ptr(rbtree, smright->parent, smright, to_delete);

		change_child_ptr(smright->left, smright, to_delete);
		change_child_ptr(smright->left, smright, to_delete);
		change_child_ptr(smright->right, smright, to_delete);
		change_child_ptr(smright->right, smright, to_delete);
		change_child_ptr(to_delete->left, to_delete, smright);
		if(to_delete->right != smright)
			change_child_ptr(to_delete->right, to_delete, smright);
		if(to_delete->right == smright)
		{
			to_delete->right = to_delete;
			smright->parent = smright;
		}

		swap_np(&to_delete->parent, &smright->parent);
		swap_np(&to_delete->left, &smright->left);
		swap_np(&to_delete->right, &smright->right);
	}

	if(to_delete->left != RBNODE_NULL)
		child = to_delete->left;
	else
		child = to_delete->right;

	change_parent_ptr(rbtree, to_delete->parent, to_delete, child);
	change_child_ptr(child, to_delete, to_delete->parent);

	if(to_delete->color == RBCOLOR_RED)
		;
	else if(child->color == RBCOLOR_RED) {
		if(child!=RBNODE_NULL)
			child->color = RBCOLOR_BLACK;
	}
	else
		rbtree_delete_balance(rbtree, child, to_delete->parent);

	rbnode_init(to_delete);
}

struct rbnode* rbtree_first (struct rbtree *rbtree)
{
	struct rbnode *node = rbtree->root;

	if (rbtree->root != RBNODE_NULL) {
		for (node = rbtree->root;
		     node->left != RBNODE_NULL;
		     node = node->left);
	}
	return node;
}

struct rbnode* rbtree_last (struct rbtree *rbtree)
{
	struct rbnode *node = rbtree->root;

	if (rbtree->root != RBNODE_NULL) {
		for (node = rbtree->root;
		     node->right != RBNODE_NULL;
		     node = node->right);
	}
	return node;
}

struct rbnode* rbtree_next (struct rbnode *node)
{
	struct rbnode *parent;

	if (node->right != RBNODE_NULL) {
		for (node = node->right;
		     node->left != RBNODE_NULL;
		     node = node->left);
	} else {
		parent = node->parent;
		while (parent != RBNODE_NULL && node == parent->right) {
			node = parent;
			parent = parent->parent;
		}
		node = parent;
	}
	return node;
}

struct rbnode* rbtree_previous(struct rbnode *node)
{
	struct rbnode *parent;

	if (node->left != RBNODE_NULL) {
		for (node = node->left;
		     node->right != RBNODE_NULL;
		     node = node->right);
	} else {
		parent = node->parent;
		while (parent != RBNODE_NULL && node == parent->left) {
			node = parent;
			parent = parent->parent;
		}
		node = parent;
	}
	return node;
}

struct rbnode *rbtree_search (struct rbtree *rbtree, void *context)
{
	int r = 0;
	struct rbnode *node = rbtree->root;

	while (node != RBNODE_NULL) {
		r = rbtree->search(context, node);
		if(r == 0)
			return node;
		else if (r < 0) {
			node = node->left;
		} else {
			node = node->right;
		}
	}
	return RBNODE_NULL;
}
