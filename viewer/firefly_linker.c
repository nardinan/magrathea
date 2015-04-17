#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#define d_linker_string_size 512
#define d_linker_location_size 8
#define d_linker_configuration "firefly_linker.ini"
#define d_linker_magrathea_configuration "../App/magrathea.cfg"
#define d_linker_ladders 24
#define d_linker_temperatures 48
#define d_linker_trbs 8
#define d_linker_directory_test "test"
#define d_linker_directory_draft "draft"
#define d_linker_calibration_ladder_prefix "ladder"
#define d_linker_calibration_trb_prefix "TRB"
#define d_linker_time_format "%d %b %Y %H:%M:%S"
#define d_true 1
#define d_false 0
char v_working_directory[d_linker_string_size], v_ladder_name[d_linker_ladders][d_linker_ladders], v_test_kind = 0x00, v_test_location[d_linker_location_size],
     v_source_bin[d_linker_string_size];
void p_linker_load_temperature(int trb_id, float *values) {
	/* todo */
}

void p_linker_move_cal(const char *source, unsigned short int ladder, const char *destination, float temperature_left, float temperature_right) {
	char buffer_input[d_linker_string_size], buffer_time[d_linker_string_size];
	FILE *source_stream, *destination_stream;
	time_t timestamp = time(NULL);
	if ((source_stream = fopen(source, "r"))) {
		if ((destination_stream = fopen(destination, "w"))) {
			printf("[>] ladder %s (%d) [%s -> %s]\n", v_ladder_name[ladder], ladder, source, destination);
			strftime(buffer_time, d_linker_string_size, d_linker_time_format, localtime(&(timestamp)));
			fprintf(destination_stream, "name=%s\nTRB_position=%02d\nTRB_file=%s\nstarting_time=%s\ntemp_left=%.03f\ntemp_right=%.03f\n",
					v_ladder_name[ladder], ladder, v_source_bin, buffer_time, temperature_right, temperature_left);
			while (fgets(buffer_input, d_linker_string_size, source_stream) > 0)
				fprintf(destination_stream, buffer_input);
			fclose(destination_stream);
		}
		fclose(source_stream);
	} else
		fprintf(stderr, "[!] Error - file %s not found!\n", source);
}

void f_linker_move(const char *source, unsigned short int ladder, float temperature_left, float temperature_right) {
	char ladder_directory[d_linker_string_size], result_directory[d_linker_string_size], clean_name[d_linker_string_size],
	     result_name[d_linker_string_size], result_path[d_linker_string_size], buffer_command[d_linker_string_size], buffer_input[d_linker_string_size];
	int index, founded;
	FILE *stream;
	if (strlen(v_ladder_name[ladder]) > 0) {
		snprintf(ladder_directory, d_linker_string_size, "%s/%s", v_working_directory, v_ladder_name[ladder]);
		mkdir(ladder_directory, 0777);
		if (v_test_kind != 0x00) {
			snprintf(result_directory, d_linker_string_size, "%s/%s", ladder_directory, d_linker_directory_test);
			snprintf(clean_name, d_linker_string_size, "%s_%s_%c", v_ladder_name[ladder], v_test_location, v_test_kind);
		} else {
			snprintf(result_directory, d_linker_string_size, "%s/%s", ladder_directory, d_linker_directory_draft);
			snprintf(clean_name, d_linker_string_size, "%s_%s", v_ladder_name[ladder], v_test_location);
		}
		mkdir(result_directory, 0777);
		for (index = 0, founded = d_false; (!founded); ++index) {
			snprintf(buffer_command, d_linker_string_size, "ls \"%s/%s_%03d\"* 2>/dev/null", result_directory, clean_name, index);
			if ((stream = popen(buffer_command, "r")) != NULL) {
				if (fgets(buffer_input, d_linker_string_size, stream) == NULL) {
					snprintf(result_name, d_linker_string_size, "%s_%03d", clean_name, index);
					founded = d_true;
				}
				pclose(stream);
			}
		}
		snprintf(result_path, d_linker_string_size, "%s/%s.cal", result_directory, result_name);
		p_linker_move_cal(source, ladder, result_path, temperature_left, temperature_right);
	} /* else skip */
}

void f_linker(const char *source) {
	char result_source[d_linker_string_size];
	int index;
	unsigned short int trb_id = 0;
	FILE *stream;
	for (index = 0; index < d_linker_ladders; ++index) {
		for (; trb_id < d_linker_trbs; ++trb_id) {
			snprintf(result_source, d_linker_string_size, "%s/%s%02d_%s_%02d.cal", source, d_linker_calibration_trb_prefix, trb_id,
					d_linker_calibration_ladder_prefix, index);
			if ((stream = fopen(result_source, "r"))) {
				fclose(stream);
				break;
			}
		}
		f_linker_move(result_source, index, 0.0, 0.0);
	}
}

void p_linker_load_value(const char *key, const char *value) {
	char *pointer;
	int ladder;
	if (strcmp(key, "working_directory") == 0)
		strcpy(v_working_directory, value);
	else if (strcmp(key, "location") == 0)
		strcpy(v_test_location, value);
	else if (strcmp(key, "test") == 0)
		v_test_kind = value[0];
	else if (strncmp(key, "ladder", (strlen("ladder")-1)) == 0) {
		if ((pointer = strchr(key, '_'))) {
			ladder = atoi(pointer+1);
			if ((ladder >= 0) && (ladder < d_linker_ladders))
				strcpy(v_ladder_name[ladder], value);
		}
	} else
		fprintf(stderr, "[!] Error - unknown key: %s = %s\n", key, value);
}

void f_linker_load(const char *configuration) {
	FILE *stream;
	char buffer_input[d_linker_string_size], key[d_linker_string_size], value[d_linker_string_size], *pointer;
	if ((stream = fopen(configuration, "r"))) {
		while (fgets(buffer_input, d_linker_string_size, stream) > 0)
			if ((pointer = strchr(buffer_input, '='))) {
				*pointer = '\0';
				strcpy(key, buffer_input);
				strcpy(value, (pointer+1));
				pointer = (value+strlen(value));
				while ((pointer > value) && ((*pointer == '\0') || (*pointer == '\n'))) {
					*pointer = '\0';
					pointer--;
				}
				if ((strlen(key) > 0) && (strlen(value) > 0))
					p_linker_load_value(key, value);
			}
		fclose(stream);
	} else
		fprintf(stderr, "[!] Error - file %s not found\n", configuration);
}

int main (int argc, char *argv[]) {
	/* prototype argv[0] <calibration directory> */
	if (argc == 3) {
		/* load configuration file */
		strncpy(v_source_bin, argv[2], d_linker_string_size);
		f_linker_load("./firefly_linker.cfg");
		f_linker(argv[1]);
		mkdir(v_working_directory, 0777);
	} else
		fprintf(stderr, "usage:\n%s <calibration directory> <source BIN>\n", argv[0]);
	return 0;
}
