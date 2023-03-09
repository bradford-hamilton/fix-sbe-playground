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
    // Mock a TradeData message:
    char buffer[2048];
    TradeData td;

    td.wrapAndApplyHeader(buffer, 0, sizeof(buffer));
    td.quote()
      .market(Market::NYSE)
      .putSymbol("AAPL")
      .price(153.03)
      .currency(Currency::USD);
    td.volume(12871);

    // Mock and send data
    auto data = td.buffer();

    if (sendto(sockfd, data, sizeof(data), 0, (struct sockaddr*)&server_addr, server_addr_len) < 0) {
      std::cerr << "Failed to sendto\n";
      return 1;
    }

    sleep(3);
  }

  close(sockfd);

  return 0;
}
