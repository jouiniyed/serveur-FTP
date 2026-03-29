#include "csapp.h"
#include "request.h"

#define MAX_NAME_LEN 256
#define NB_PROC 10       // nb de processus dans le pool
#define PORT_ESCLAVE 2122 // port d'ecoute de l'esclave (different du maitre sur 2121)

static pid_t workers[NB_PROC]; // pids des fils

void child_handler(int sig){
    while (Waitpid(-1, NULL, WNOHANG) > 0);
}

void parent_stop_handler(int sig){
    printf("\nEsclave arrete...\n");
    for (int i = 0; i < NB_PROC; i++) {
        kill(workers[i], SIGTERM);
    }
    while(wait(NULL) > 0);
    exit(0);
}

int main(int argc, char **argv)
{
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;

    // on peut passer le port en argument, sinon on utilise PORT_ESCLAVE par defaut
    // ex : ./ftpslave 2122 localhost 2123  (port propre + adresses des autres esclaves)
    int port = (argc >= 2) ? atoi(argv[1]) : PORT_ESCLAVE;

    // Q16 : recuperer les adresses des autres esclaves pour la propagation rm/put
    // les args suivants sont des paires "ip port" pour les autres esclaves
    char autres_ips[10][64];
    int  autres_ports[10];
    int  j = 0;
    for (int i = 2; i + 1 < argc && j < 10; i += 2, j++) {
        strncpy(autres_ips[j], argv[i], 63);
        autres_ips[j][63] = '\0';
        autres_ports[j] = atoi(argv[i + 1]);
        printf("Propagation vers : %s:%d\n", autres_ips[j], autres_ports[j]);
    }
    set_slaves(autres_ips, autres_ports, j);

    Signal(SIGCHLD, child_handler);
    Signal(SIGINT, parent_stop_handler);

    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(port);
    printf("Serveur esclave demarre sur le port %d\n", port);

    for (int i = 0; i < NB_PROC; i++) {

        pid_t p = Fork();

        if (p < 0) exit(-3);

        if (p == 0) {
            // Q10 : on ignore SIGPIPE sinon le fils meurt si le client crashe
            signal(SIGPIPE, SIG_IGN);

            while (1) {
                // on attend qu'un client se connecte
                connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
                printf("esclave : client connecte\n");

                // traitement de la requete (meme logique que ftpserveri)
                response_t res = requestHandler(connfd);

                afficherResponse(res);
                Close(connfd);
            }

            exit(0);
        }

        workers[i] = p;
    }

    while (1) {
        pause();
    }

    exit(0);
}
