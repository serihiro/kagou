#include "request_handler.h"
#include "util.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

static void test_content_type_from_filename() {
    assert(strcmp(content_type_from_filename("index.html"), "text/html") == 0);
    assert(strcmp(content_type_from_filename("style.css"), "text/css") == 0);
    assert(strcmp(content_type_from_filename("unknown.xyz"), "text/plain") == 0);
}

static void test_last_strtok() {
    char path1[] = "/usr/local/bin/test";
    char last[128];
    last_strtok(last, path1, "/");
    assert(strcmp(last, "test") == 0);

    char path2[] = "filename";
    last_strtok(last, path2, "/");
    assert(strcmp(last, "") == 0);
}

int main(void) {
    test_content_type_from_filename();
    test_last_strtok();
    printf("All tests passed\n");
    return 0;
}
