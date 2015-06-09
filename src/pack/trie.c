#include "trie.h"
#include <string.h>
#include <stdlib.h>

static int char_map(char c)
{
	switch (c)
	{
		case 'a':
			return CHAR_A;
			break;
		case 'b':
			return CHAR_B;
			break;
		case 'c':
			return CHAR_C;
			break;
		case 'd':
			return CHAR_D;
			break;
		case '2':
			return CHAR_2;
			break;
		case '4':
			return CHAR_4;
			break;
		case '6':
			return CHAR_6;
			break;
		case '8':
			return CHAR_8;
			break;
		case 'e':
			return CHAR_E;
			break;
		case 's':
			return CHAR_S;
			break;
		case 'h':
			return CHAR_H;
			break;
		default:
			return -1;
			break;
	}

	return -1;
}

static char enum_map(int c)
{
	char table[CHAR_Max] = {'2', '4', '6', '8', 'a', 'b', 'c', 'd', 'e', 's', 'h'};
	if (c >= CHAR_Max)
		return -1;

	return table[c];
}

static trie_node_t* AddNode(trie_node_t* pn, int ch)
{
	trie_node_t* pnew = (trie_node_t *)malloc(sizeof(trie_node_t));
	pnew->c = ch;
	pnew->end = 0;
	memset(pnew->childs, 0, sizeof(pnew->childs));

	pn->childs[ch] = pnew;
	return pn;
}

int InitTireTree(trie_tree_t* pttt, const char* str_table[], int size)
{
	if (!pttt)
	{
		return -1;
	}
	trie_node_t* root = &pttt->root;
	memset(root->childs, 0, sizeof(root->childs));

	int i;
	for (i = 0; i < size; ++i)
	{
		trie_node_t* pn = root;
		const char *one = str_table[i];
		int len = strlen(one);
		int j;
		for (j = 0; j < len; ++j)
		{
			int ch = char_map(one[j]);
			if (!pn->childs[ch])
			{
				(void)AddNode(pn, ch);
			}
			pn = pn->childs[ch];
		}
		pn->end = 1;
	}

	return 0;
}

char* TrieTreeFind(trie_tree_t* pttt, const char* str)
{
	if (!pttt)
	{
		return NULL;
	}

	trie_node_t* p = &pttt->root;
	int i, len = strlen(str);

	int idx = 0;
	char* ret = (char *)malloc((len + 1)*sizeof(char));
	for (i = 0; i < len; ++i)
	{
		int ch = char_map(str[i]);
		if (p->childs[ch])
		{
			p = p->childs[ch];
			ret[idx++] = str[i];
		}
	}
	if (p->end == 1)
	{
		ret[idx++] = '\0';
	}

	return ret;
}

void TrieTreeFini(trie_tree_t* pttt)
{
}
