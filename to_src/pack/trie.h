#ifndef _TRIE_H_
#define _TRIE_H_

enum CHAR_ENUM
{
	CHAR_2 = 0,
	CHAR_4,
	CHAR_6,
	CHAR_8,
	CHAR_A,
	CHAR_B,
	CHAR_C,
	CHAR_D,
	CHAR_E,
	CHAR_S,
	CHAR_H,
	CHAR_Max
};

typedef struct trie_node_s
{
	char c;
	int end;
	struct trie_node_s* childs[CHAR_Max];
}trie_node_t;

typedef struct trie_tree_s
{
	trie_node_t root;
}trie_tree_t;

int InitTireTree(trie_tree_t* pttt, const char* str_table[], int size);
char* TrieTreeFind(trie_tree_t* pttt, const char* str);
void TrieTreeFini(trie_tree_t* pttt);

#endif
