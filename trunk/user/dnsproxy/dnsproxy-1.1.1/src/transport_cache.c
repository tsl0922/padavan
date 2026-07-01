/*
 * Copyright 2014, Vietor Liu <vietor.liu at gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or any later version. For full terms that can be
 * found in the LICENSE file.
 */

#include "dnsproxy.h"

static struct {
	unsigned int count;
	unsigned short index;
	unsigned short timeout;
	struct rbtree rb_new;
	struct list_head list;
} g_cache;

static int new_search(const void* k, const struct rbnode* r)
{
	TRANSPORT_CACHE *right;
	right = rbtree_entry(r, TRANSPORT_CACHE, rb_new);
	return (int)*(unsigned short*)k - right->new_id;
}

static int new_compare(const struct rbnode* l, const struct rbnode* r)
{
	TRANSPORT_CACHE *left, *right;
	left = rbtree_entry(l, TRANSPORT_CACHE, rb_new);
	right = rbtree_entry(r, TRANSPORT_CACHE, rb_new);
	return (int)left->new_id - right->new_id;
}

void transport_cache_init(unsigned short timeout)
{
	g_cache.count = 0;
	g_cache.timeout = timeout;
	g_cache.index = (unsigned short)rand();
	rbtree_init(&g_cache.rb_new, new_search, new_compare);
	list_init(&g_cache.list);
}

TRANSPORT_CACHE* transport_cache_search(unsigned short new_id)
{
	struct rbnode *node = rbtree_search(&g_cache.rb_new, &new_id);
	if(node == RBNODE_NULL)
		return NULL;
	return rbtree_entry(node, TRANSPORT_CACHE, rb_new);
}

TRANSPORT_CACHE* transport_cache_insert(unsigned short old_id, struct sockaddr_in *address, void *context)
{
	TRANSPORT_CACHE *cache = (TRANSPORT_CACHE*)calloc(1, sizeof(TRANSPORT_CACHE));
	if(cache == NULL)
		return NULL;
	cache->context = context;
	cache->new_id = ++g_cache.index;
	cache->expire = time(NULL) + g_cache.timeout;
	cache->old_id = old_id;
	memcpy(&cache->source, address, sizeof(struct sockaddr_in));
	++g_cache.count;
	list_insert(&g_cache.list, &cache->list);
	rbtree_insert(&g_cache.rb_new, &cache->rb_new);
	return cache;
}

void transport_cache_delete(TRANSPORT_CACHE *cache)
{
	--g_cache.count;
	list_remove(&cache->list);
	rbtree_delete(&g_cache.rb_new, &cache->rb_new);
	free(cache);
}

void transport_cache_clean(time_t current)
{
	TRANSPORT_CACHE *cache, *tmp;

	if(list_empty(&g_cache.list))
		return;
	list_for_each_safe(cache, tmp, &g_cache.list, TRANSPORT_CACHE, list) {
		if(cache->expire > current)
			break;
		transport_cache_delete(cache);
	}
}
