#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    std::cerr << "Failed to create socket\n";
    return 1;
  }

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(1234);

  int b = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if (b < 0) {
    std::cerr << "Failed to bind socket\n";
    return 1;
  }

  char buffer[1024];
  ssize_t num_bytes;
  socklen_t server_addr_len = sizeof(server_addr);

  while (1) {
    num_bytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, &server_addr_len);
    if (num_bytes < 0) {
        std::cerr << "Failed to receive data" << std::endl;
        return 1;
    }

    buffer[num_bytes] = '\0';
    std::cout << "Received data: " << buffer << std::endl;
  }

  close(sockfd);

  return 0;
}
