/*
 * magrathea
 * Copyright (C) 2014 Andrea Nardinocchi (andrea@nardinan.it)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "keys.h"
struct s_list *f_keys_initialize(struct s_list *supplied, const char *file, char separator) {
	struct s_list *result = supplied;
	struct s_keys_entry *entry;
	char buffer[d_string_buffer_size], *pointer;
	FILE *stream;
	if (!result)
		f_list_init(&result);
	if ((stream = fopen(file, "r"))) {
		while (!feof(stream)) {
			memset(buffer, 0, d_string_buffer_size);
			if (fgets(buffer, d_string_buffer_size, stream))
				if ((pointer = strchr(buffer, separator))) {
					*pointer = '\0';
					pointer++;
					f_string_trim(buffer);
					f_string_trim(pointer);
					if ((f_string_strlen(buffer) > 0) && (f_string_strlen(pointer) > 0))
						if ((entry = (struct s_keys_entry *) d_malloc(sizeof(struct s_keys_entry)))) {
							strncpy(entry->key, buffer, d_string_buffer_size);
							strncpy(entry->value, pointer, d_string_buffer_size);
							f_list_append(result, (struct s_list_node *)entry, e_list_insert_head);
						}
				}
		}
		fclose(stream);
	}
	return result;
}

void p_keys_float(struct s_list *list, const char *key, const char postfix, float *value) {
	struct s_keys_entry *entry;
	char buffer[d_string_buffer_size];
	snprintf(buffer, d_string_buffer_size, "%s_%c", key, postfix);
	d_foreach(list, entry, struct s_keys_entry)
		if (f_string_strcmp(entry->key, buffer) == 0) {
			*value = atof(entry->value);
			break;
		}
}

void p_keys_int(struct s_list *list, const char *key, const char postfix, int *value) {
	struct s_keys_entry *entry;
	char buffer[d_string_buffer_size];
	snprintf(buffer, d_string_buffer_size, "%s_%c", key, postfix);
	d_foreach(list, entry, struct s_keys_entry)
		if (f_string_strcmp(entry->key, buffer) == 0) {
			*value = atoi(entry->value);
			break;
		}
}

void f_keys_destroy(struct s_list **supplied) {
	struct s_list_node *entry;
	while ((entry = (*supplied)->head)) {
		f_list_delete(*supplied, (struct s_list_node *)entry);
		d_free(entry);
	}
	f_list_destroy(supplied);
}
