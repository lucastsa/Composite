#ifndef SQLITE_SERVER_H
#define SQLITE_SERVER_H 1

#define SQLITE_DONE 101
#define SQLITE_ROW 100

#define SQLITE_INTEGER  1
#define SQLITE_FLOAT    2
#define SQLITE_BLOB     4
#define SQLITE_NULL     5
#ifdef SQLITE_TEXT
# undef SQLITE_TEXT
#else
# define SQLITE_TEXT     3
#endif
#define SQLITE3_TEXT     3

typedef struct cos_sqlite3_result cos_sqlite3_result;
struct cos_sqlite3_result {
    int result_code;
    int column_count;
    char ** column_names;
    int * column_types;
    void ** column_values;
};

/**
    Deserializes buffer received from SQLite component
*/
cos_sqlite3_result * cos_sqlite3_build_result(char * bufdata) {
    cos_sqlite3_result * ret = malloc(sizeof(cos_sqlite3_result));

    /**
    [RESULT_CODE:1] [COLUMN_COUNT:4+1] ( [COLUMN_TYPE:4+1][COLUMN_NAME_LENGTH:4+1][COLUMN_NAME:???+1][COLUMN_VALUE_LENGTH:4+1]?[COLUMN_VALUE:???+1] ...)
    */
    char * cur_column_name = NULL;
    int cur_column_name_len;
    int idx=0, i=0;

    // loading result code
    ret->result_code = bufdata[idx++];

    if (ret->result_code != SQLITE_ROW) return ret;

    // loading column count
    sscanf(&bufdata[idx], "%d", &ret->column_count);
    idx += sizeof(ret->column_count) + 1;

    // allocating memory for arrays
    ret->column_types = (int *)malloc(sizeof(int) * ret->column_count);
    ret->column_names = (char **)malloc(sizeof(char *) * ret->column_count);
    ret->column_values = (void **)malloc(sizeof(void *) * ret->column_count);

    for (i=0; i < ret->column_count; i++) {
        // loading column types
        sscanf(&bufdata[idx], "%d", &ret->column_types[i]);
        idx += sizeof(int) + 1;

        /*********************/
        // column name length
        sscanf(&bufdata[idx], "%d", &cur_column_name_len);
        idx += sizeof(cur_column_name_len) + 1;

        cur_column_name = malloc(cur_column_name_len + 1);

        // column name value
        cur_column_name = &bufdata[idx];
        idx += cur_column_name_len + 1;

        ret->column_names[i] = cur_column_name;
        // printc("#SQLITE: column name %d: %s\n", i, cur_column_name);

        int * value_int;

        char * value_text ;
        int value_text_len;
        char * value_blob;
        int value_blob_len;

        switch (ret->column_types[i]) {
            case SQLITE_INTEGER:
                value_int = malloc(sizeof(int));
                sscanf(&bufdata[idx], "%d", value_int);
                idx += sizeof(int) + 1;

                ret->column_values[i] = value_int;
                break;

            case SQLITE_FLOAT:
                // TODO: CONVERT CHAR ARRAY TO DOUBLE (assume DOUBLE 64-bit)
                // value_double = malloc(sizeof(double));

                break;

            case SQLITE_BLOB:
                sscanf(&bufdata[idx], "%d", &value_blob_len);
                idx += sizeof(int) + 1;

                value_blob = malloc(value_blob_len);

                memcpy(value_blob, &bufdata[idx], value_blob_len);
                idx += value_blob_len;

                ret->column_values[i] = value_blob;
                // free(value_blob);
                break;

            case SQLITE_NULL:
                break;

            case SQLITE_TEXT:
                sscanf(&bufdata[idx], "%d", &value_text_len);
                idx += sizeof(int) + 1;

                value_text = malloc(value_text_len);

                strcpy(value_text, &bufdata[idx]);
                idx += value_text_len + 1;

                ret->column_values[i] = value_text;
                // free(value_text);
        }
    }

    return ret;
}

#endif
