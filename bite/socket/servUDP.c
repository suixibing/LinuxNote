#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main(int argc, char *argv[])
{
  if (argc != 3) {
    printf("argument like: ./cliUDP 172.17.223.241 9000\n");
    return 1;
  }

  int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sockfd < 0) {
    perror("socket error");
    return 1;
  }

  struct sockaddr_in addr;
  socklen_t len = sizeof(struct sockaddr_in);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(atoi(argv[2]));
  inet_pton(AF_INET, argv[1], &addr.sin_addr.s_addr);
  int ret = bind(sockfd, (struct sockaddr*)&addr, len);
  if (ret < 0) {
    perror("bind error");
    return 1;
  }

  while (1) {
    char buf[4096] = { 0 };
    struct sockaddr_in cliaddr;
    len = sizeof(struct sockaddr_in);
    ret = recvfrom(sockfd, buf, 4096, 0, (struct sockaddr*)&cliaddr, &len);
    if (ret < 0) {
      perror("recvfrom error");
      close(sockfd);
      return 1;
    }

    printf("client say: %s\n", buf);
    strcat(buf, ", server reply.");

    len = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&cliaddr, len);
    if (ret < 0) {
      perror("sendto error");
      close(sockfd);
      return 1;
    }
  }
  close(sockfd);
  return 0;
}
