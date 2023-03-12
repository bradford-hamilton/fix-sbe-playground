#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <random>
#include <cmath>
#include "fix_sbe_playground/MessageHeader.h"
#include "fix_sbe_playground/TradeData.h"

using namespace fix::sbe::playground;

TradeData mock_trade_data(char* buffer, uint64_t buffer_length);

float aapl_price();

int main() {
  const char* server_ip = "127.0.0.1";
  const int server_port = 1234;
  const int sendrecv_flags = 0;

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
    // Mock a TradeData message and send to server
    char buffer[1024];
    auto td = mock_trade_data(buffer, sizeof(buffer));

    auto s = sendto(
      sockfd,
      td.buffer(),
      td.sbeBlockAndHeaderLength(),
      sendrecv_flags,
      (struct sockaddr*)&server_addr,
      server_addr_len
    );
    if (s < 0) {
      std::cerr << "Failed to sendto\n";
      return 1;
    }

    sleep(3);
  }

  close(sockfd);

  return 0;
}

TradeData mock_trade_data(char* buffer, uint64_t buffer_length)
{
  TradeData td;

  td.wrapAndApplyHeader(buffer, 0, buffer_length);
  td.quote()
    .market(Market::NYSE)
    .putSymbol("AAPL")
    .price(aapl_price())
    .currency(Currency::USD);
  td.volume(12871);

  return td;
};

// Silly function to give me a different AAPL price between 125-150 for each message.
float aapl_price()
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(125.0, 175.0);

  return std::round(dis(gen) * 100.0) / 100.0;
}
