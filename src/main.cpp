#include <iostream>
#include "fix_sbe_playground/MessageHeader.h"
#include "fix_sbe_playground/TradeData.h"
#include "fix_sbe_playground/Currency.h"
#include "fix_sbe_playground/Decimal.h"
#include "fix_sbe_playground/Market.h"
#include "fix_sbe_playground/Quote.h"

using namespace fix::sbe::playground;

const int message_header_version = 0;

size_t encode_header(
  MessageHeader &hdr,
  char *buffer,
  uint64_t offset,
  uint64_t buffer_length
) {
  // Encode the header
  hdr.wrap(buffer, offset, message_header_version, buffer_length)
    .blockLength(TradeData::sbeBlockLength())
    .templateId(TradeData::sbeTemplateId())
    .schemaId(TradeData::sbeSchemaId())
    .version(TradeData::sbeSchemaVersion());

  return hdr.encodedLength();
}

size_t decode_header(
  MessageHeader &hdr,
  char *buffer,
  uint16_t offset,
  uint16_t buffer_length)
{
  hdr.wrap(buffer, offset, message_header_version, buffer_length);

  // Decode the header
  std::cout << "messageHeader.blockLength=" << hdr.blockLength() << std::endl;
  std::cout << "messageHeader.templateId=" << hdr.templateId() << std::endl;
  std::cout << "messageHeader.schemaId=" << hdr.schemaId() << std::endl;
  std::cout << "messageHeader.schemaVersion=" << hdr.version() << std::endl;
  std::cout << "messageHeader.encodedLength=" << hdr.encodedLength() << std::endl;

  return hdr.encodedLength();
}

size_t encode_trade_data(
  TradeData &td,
  char *buffer,
  uint16_t offset,
  uint16_t buffer_length)
{
  td.wrapForEncode(buffer, offset, buffer_length);
  
  td.quote()
    .market(Market::NYSE)
    .putSymbol("AAPL")
    .price(153.03)
    .currency(Currency::USD);
  
  td.volume(12871);

  return td.encodedLength();
}

size_t decode_trade_data(
  TradeData &td,
  char *buffer,
  uint64_t offset,
  uint64_t acting_block_length,
  uint64_t acting_version,
  uint64_t buffer_length)
{
  td.wrapForDecode(buffer, offset, acting_block_length, acting_version, buffer_length);

  std::cout.setf(std::ios::fixed);

  std::cout << "\ntd.quote.market=" << td.quote().market();
  std::cout << "\ntd.quote.symbol=" << td.quote().symbol();
  std::cout << "\ntd.quote.price=" << td.quote().price();
  std::cout << "\ntd.quote.currency=" << td.quote().currency();

  return td.encodedLength();
}

template <typename T>
void print_json(T wrapped_for_decode) {
  std::cout << wrapped_for_decode << std::endl;
}

int main() {
  char buffer[2048];
  MessageHeader hdr;
  TradeData td;

  size_t encode_hdr_length = encode_header(hdr, buffer, 0, sizeof(buffer));
  size_t encode_msg_length = encode_trade_data(td, buffer, hdr.encodedLength(), sizeof(buffer));
  size_t predicted_length = TradeData::computeLength();

  std::cout << "Encoded Lengths are " << encode_hdr_length << " + " << encode_msg_length << " (" << predicted_length << ")" << std::endl;

  print_json(hdr);

  size_t decode_hdr_length = decode_header(hdr, buffer, 0, sizeof(buffer));
  size_t decode_msg_length = decode_trade_data(td, buffer, hdr.encodedLength(), hdr.blockLength(), hdr.version(), sizeof(buffer));

  std::cout << "Decoded Lengths are " << decode_hdr_length << " + " << decode_msg_length << std::endl;

  if (encode_hdr_length != decode_hdr_length) {
    std::cerr << "Encode/Decode header lengths do not match" << std::endl;
    return EXIT_FAILURE;
  }

  if (encode_msg_length != decode_msg_length) {
    std::cerr << "Encode/Decode message lengths do not match" << std::endl;
    return EXIT_FAILURE;
  }

  td.wrapForDecode(buffer, hdr.encodedLength(), hdr.blockLength(), hdr.version(), sizeof(buffer));

  print_json(td);

  return 0;
}
