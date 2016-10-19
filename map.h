#ifndef MAP_H
#define MAP_H

#define DIM 50

typedef struct
{
	int value;
	char key;
} EntrySet;

//Map<String, int>
typedef struct
{
	EntrySet* list;
	int size;
	int num_elem;
} Map;

void initialize(Map* map);
int insert(Map* map, char key);
int insertSet(Map* map, EntrySet set);
void destroy(Map* map);
void print(Map* map);
void merge(Map* map1, Map* map2);

#endif