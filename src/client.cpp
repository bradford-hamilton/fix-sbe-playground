#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
  const char* server_ip = "127.0.0.1";
  const int server_port = 1234;

  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    std::cerr << "Failed to create socket" << std::endl;
    return 1;
  }

  struct sockaddr_in server_addr;
  socklen_t server_addr_len = sizeof(server_addr);

  memset(&server_addr, 0, server_addr_len);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(server_ip);
  server_addr.sin_port = htons(server_port);

  while (true) {
    // Mock and send data
    std::string mock_data = "Mock data: " + std::to_string(rand());

    if (sendto(sockfd, mock_data.c_str(), mock_data.length(), 0, (struct sockaddr*)&server_addr, server_addr_len) < 0) {
      std::cerr << "Failed to sendto\n";
      return 1;
    }

    std::cout << "Sent mock data: " << mock_data << std::endl;

    sleep(3);
  }

  close(sockfd);

  return 0;
}
