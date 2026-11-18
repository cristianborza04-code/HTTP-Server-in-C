#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

const char *CRLF = "\r\n";
const char *SP = " ";

/* Request-Line = Method SP Request-URI SP HTTP-Version CRLF */
typedef struct {
    char *method;
    char *uri;
    char *version;
} http_req_line;

typedef enum {
    HTTP_RES_OK,
    HTTP_RES_INTERNAL_SERVER_ERR,
} http_result;

typedef struct {
    char *start;
    char *end;
} string_view;

typedef struct {
    string_view *splits;
    size_t count;
} string_splits;

static string_splits split_string(const char *str, size_t len, char split_by) {}

static void free_splits(string_splits *splits) {
    if (splits) {
        free(splits->splits);
        splits->splits = NULL;
    }
}

http_req_line http_req_line_init(void) {
    http_req_line line;
    line.method = NULL;
    line.uri = NULL;
    line.version = NULL;
    return line;
}

http_result parse_req_line(const char *buff, size_t len,
                           http_req_line *req_line) {
    if (!buff || !req_line) {
        return HTTP_RES_INTERNAL_SERVER_ERR;
    }
    return HTTP_RES_OK;
}

int handle_client(int client_socket) {
    ssize_t n = 0;
    char buff[1024];
    const char *hello = "HTTP/1.0 200 OK\r\n\r\n<h1>Hello,World!</h1>";

    printf("\n---\n");
    for (;;) {
        memset(buff, 0, sizeof(buff));

        n = read(client_socket, buff, sizeof(buff) - 1);
        if (n < 0) {
            perror("read(client)");
            return -1;
        }
        if (n == 0) {
            printf("connection closed gracefully!\n");
            break;
        }

        printf("REQUEST:\n%s", buff);
        (void)write(client_socket, hello, strlen(hello));
        close(client_socket);
        break;
    }
    printf("\n---\n");

    return 0;
}

int main(void) {
    /* declare */
    int return_code = 0;
    struct sockaddr_in bind_addr;
    int tcp_socket = 0;
    int ret = 0;
    int client_socket = 0;
    int enabled = true;

    /* initialize */
    memset(&bind_addr, 0, sizeof(bind_addr));
    tcp_socket = socket(AF_INET,     /* IPv4 */
                        SOCK_STREAM, /* TCP */
                        0            /* Protocol */
    );

    if (tcp_socket == -1) {
        perror("socket");
        ret = 1;
        goto exit;
    }
    printf("socket creation succeeded\n");

    /* we dont care if this fails */
    (void)setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &enabled,
                     sizeof(bind_addr));

    bind_addr.sin_port = htons(6969);
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = INADDR_ANY;

    return_code = bind(tcp_socket, (const struct sockaddr *)&bind_addr,
                       sizeof(bind_addr));
    if (return_code < 0) {
        perror("bind()");
        ret = 1;
        goto exit;
    }
    printf("bind succeeded\n");

    return_code = listen(tcp_socket, SOMAXCONN);
    if (return_code < 0) {
        perror("listen()");
        ret = 1;
        goto exit;
    }
    printf("listen succeeded\n");

    for (;;) {
        printf("Waiting for connections ...\n");
        client_socket = accept(tcp_socket, NULL, NULL);

        printf("got a connection!\n");
        return_code = handle_client(client_socket);
        /* ignore errors, dont care for now */
    }

exit:
    close(tcp_socket);
    return ret;
}
