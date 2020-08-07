#include <mysql.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <json-c/json.h>

#define BUF_LEN 256
#define CHUNK 200

char *concat(const char *s1, const char *s2) {
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

char *ReadConf() {
    char *buffer = NULL;
    int string_size, read_size;
    FILE *handler = fopen("config.json", "r");
    if (handler) {
        fseek(handler, 0, SEEK_END);
        string_size = ftell(handler);
        rewind(handler);
        buffer = (char *) malloc(sizeof(char) * (string_size + 1));
        read_size = fread(buffer, sizeof(char), string_size, handler);
        buffer[string_size] = '\0';

        if (string_size != read_size) {
            free(buffer);
            buffer = NULL;
        }
        fclose(handler);
    } else {
        printf("Error print for log");
    }
    return buffer;
}

int main() {
    char *fp = ReadConf();
    char confbuffer[1024];
    char Today[BUF_LEN], Time[BUF_LEN], Cmd[BUF_LEN] = {0};
    time_t rawtime = time(NULL);
    struct tm *ptm = localtime(&rawtime);

    strftime(Today, BUF_LEN, "%Y-%m-%d/", ptm);
    strftime(Time, BUF_LEN, "%T/", ptm);

    struct stat st;
    struct json_object *pars,*dbhost, *dbport, *dbuser, *dbpass, *sslsup;

    char *MainDir, *MainSubDir, *Dir, *DirDet;

    pars = json_tokener_parse(fp);
    json_object_object_get_ex(pars, "dbhost", &dbhost);
    json_object_object_get_ex(pars, "dbport", &dbport);
    json_object_object_get_ex(pars, "dbuser", &dbuser);
    json_object_object_get_ex(pars, "dbpass", &dbpass);
    json_object_object_get_ex(pars, "sslsup", &sslsup);

    MainDir = "/Backups/";
    MainSubDir = "/Backups/DB/";
    Dir = concat(MainSubDir, Today);
    DirDet = concat(Dir, Time);

    (stat(MainDir, &st) == -1 ? mkdir(MainDir, 0755) : printf("%s Exist\n", MainDir));
    (stat(MainSubDir, &st) == -1 ? mkdir(MainSubDir, 0755) : printf("%s Exist\n", MainSubDir));
    (stat(Dir, &st) == -1 ? mkdir(Dir, 0755) : printf("%s Exist\n", Dir));
    (stat(DirDet, &st) == -1 ? mkdir(DirDet, 0755) : printf("%s Exist\n", DirDet));

    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    conn = mysql_init(NULL);

    if (!mysql_real_connect(conn, json_object_get_string(dbhost), json_object_get_string(dbuser), json_object_get_string(dbpass), NULL, json_object_get_int(dbport), NULL, 0)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    if (mysql_query(conn, "SELECT `SCHEMA_NAME` FROM `information_schema`.`SCHEMATA` WHERE `SCHEMA_NAME` NOT IN ('information_schema','performance_schema','mysql','#mysql50#.pki','postfix','oauthv2','roundcube')")) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    res = mysql_use_result(conn);

    while ((row = mysql_fetch_row(res)) != NULL) {
        sprintf(Cmd, "mysqldump --extended-insert --events --no-create-db --no-create-info --no-set-names --skip-add-locks --skip-comments --skip-triggers --skip-tz-utc --net_buffer_length=25000 %s | gzip > %s%s.sql.gz", row[0], DirDet, row[0]);
        system(Cmd);
    }

    mysql_free_result(res);
    mysql_close(conn);

    free(Dir);
    //free(concat);
    memset(Today, 0, BUF_LEN);
    memset(Time, 0, BUF_LEN);
    memset(Cmd, 0, BUF_LEN);
    return 1;
}
