#ifndef ID3_READER_H
#define ID3_READER_H

#include "id3_utils.h"
#include "type.h"

/**
TODO: Add documention as sample given
 */ 
TagData* read_id3_tags(const char *filename);

/**
TODO: Add documention as sample given 
 */
unsigned int swap(unsigned int x);

void display_metadata(const TagData *data);

/**
TODO: Add documention as sample given
 */
void view_tags(const char *filename);

Status checkfilename(const char *filename);

#endif // ID3_READER_H
