#ifndef _FILE_READER_
#define _FILE_READER_
#include "response.h"
#include "csapp.h"  // pour rio_t dans fileput

// Q10 : offset = nombre de blocs déjà reçus (0 = téléchargement complet depuis le début)
response_t filereader(int connfd, char fichier[256], size_t offset);

// Q15 : envoie le contenu du repertoire serveur au client
response_t filels(int connfd);

// Q16 : supprime un fichier sur le serveur
response_t filerm(int connfd, char fichier[256]);

// Q16 : recoit un fichier du client et le sauvegarde sur le serveur
response_t fileput(int connfd, char fichier[256], rio_t *rio);

#endif