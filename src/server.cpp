#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "fix_sbe_playground/MessageHeader.h"
#include "fix_sbe_playground/TradeData.h"

using namespace fix::sbe::playground;

int main() {
  const int message_header_version = 0;
  const int sendrecv_flags = 0;

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

  socklen_t server_addr_len = sizeof(server_addr);

  while (true) {
    char buffer[1024];

    ssize_t num_bytes = recvfrom(
      sockfd,
      buffer,
      sizeof(buffer),
      sendrecv_flags,
      (struct sockaddr*)&server_addr,
      &server_addr_len
    );
    if (num_bytes < 0) {
        std::cerr << "Failed to receive data" << std::endl;
        return 1;
    }

    TradeData td;
    MessageHeader hdr;

    hdr.wrap(buffer, 0, message_header_version, sizeof(buffer));
    td.wrapForDecode(
      buffer,
      hdr.encodedLength(),
      hdr.blockLength(),
      hdr.version(),
      sizeof(buffer)
    );

    std::cout << "Received MessageHeader: " << hdr << std::endl;
    std::cout << "Received TradeData: " << td << std::endl;
  }

  close(sockfd);

  return 0;
}
