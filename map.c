#include "map.h"
#include <stdio.h>
#include <stdlib.h>

#define START_SIZE 2

void initialize(Map* map)
{
	map->list = ( EntrySet*) malloc(START_SIZE * sizeof(EntrySet));
	map->size = START_SIZE;
	map->num_elem = 0;
}

void destroy(Map* map)
{
	free(map->list);
	free(map);
}


void overflow(Map* map)
{
	map->size *= 2;
	EntrySet* tmp = (EntrySet*) malloc(sizeof(EntrySet) * map->size);
	int i;
	for(i = 0; i < map->size; i++)
		tmp[i] = map->list[i];
	free(map->list);
	map->list = tmp;
}

int binSearch( Map* map, char key, int start, int end)
{
	if(start == end)
		return end;
	int half = (start + end)/2;
	if(map->list[half].key == key)
		return half;
	if(map->list[half].key > key)
		return binSearch(map, key, start, half);
	if(map->list[half].key < key)
		return binSearch(map, key, half+1, end);
}

int checkChar(char c)
{
	if(c >= 'A' && c <= 'Z') return 1;
	if(c >= 'a' && c <= 'z') return 1;
	if(c >= '0' && c <= '9') return 1;
	if(c == '?' || c == '!'
		|| c == '.' || c == ','
		|| c == ';' || c == ':'
		|| c == '_' || c == '-'
		|| c == '(' || c == ')'
		|| c == '@' || c == '%'
		|| c == '&' || c == '$'
		|| c == '#') return 1;
	return 0;
}

int insert(Map* map, char key)
{
	if(!checkChar(key))
		return 0;
	
	int pos = binSearch(map, key, 0, map->num_elem);

	if (pos < map->num_elem && map->list[pos].key == key)
	{
		map->list[pos].value++;
		return 0;
	}
	else if( map->size == map->num_elem )
		overflow(map);

	int i;
	for (i = map->num_elem; i > pos-1; i--)
		map->list[i] = map->list[i-1];

	EntrySet entry;
	entry.key = key;
	entry.value = 1;

	map->list[pos] = entry;
	map->num_elem++;

	return 1;
}

int insertSet(Map* map, EntrySet set)
{
	if(!checkChar(set.key))
		return 0;
	
	int pos = binSearch(map, set.key, 0, map->num_elem);

	if (pos < map->num_elem && map->list[pos].key == set.key)
	{
		map->list[pos].value += set.value;
		return 0;
	}
	else if( map->size == map->num_elem )
		overflow(map);

	int i;
	for (i = map->num_elem; i > pos-1; i--)
		map->list[i] = map->list[i-1];

	EntrySet entry;
	entry.key = set.key;
	entry.value = set.value;

	map->list[pos] = entry;
	map->num_elem++;

	return 1;
}

void print(Map* map)
{
	int i;
	for(i = 0; i < map->num_elem; i++)
		printf("%c, %d\n", map->list[i].key, map->list[i].value);
	printf("\n");
}

void merge(Map* map1, Map* map2)
{
	int i;
	for (i = 0; i < map2->num_elem; i++)
		insertSet(map1, map2->list[i]);
	destroy(map2);
}