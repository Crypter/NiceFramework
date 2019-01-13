#include <Arduino.h>
#include <Print.h>
#include "MACAddress.h"

MACAddress::MACAddress() {
  _address.qword = 0;
}

MACAddress::MACAddress(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet, uint8_t fifth_octet, uint8_t sixth_octet) {
  _address.bytes[0] = first_octet;
  _address.bytes[1] = second_octet;
  _address.bytes[2] = third_octet;
  _address.bytes[3] = fourth_octet;
  _address.bytes[4] = fifth_octet;
  _address.bytes[5] = sixth_octet;
}

MACAddress::MACAddress(uint64_t address) {
  _address.qword = address;
}

MACAddress::MACAddress(const uint8_t *address) {
  memcpy(_address.bytes, address, sizeof(_address.bytes));
}

MACAddress& MACAddress::operator=(const uint8_t *address)
{
  memcpy(_address.bytes, address, sizeof(_address.bytes));
  return *this;
}

MACAddress& MACAddress::operator=(uint64_t address)
{
  _address.qword = address;
  return *this;
}

bool MACAddress::operator==(const uint8_t* addr) const
{
  return memcmp(addr, _address.bytes, sizeof(_address.bytes)) == 0;
}

// Printables overload
size_t MACAddress::printTo(Print& p) const
{
  size_t n = 0;
  for (int i = 0; i < 5; i++) {
    n += p.print(_address.bytes[i], HEX);
    n += p.print(':');
  }
  n += p.print(_address.bytes[5], HEX);
  return n;
}

String MACAddress::toString() const
{
  char szRet[18];
  sprintf(szRet, "%02x:%02x:%02x:%02x:%02x:%02x", _address.bytes[0], _address.bytes[1], _address.bytes[2], _address.bytes[3], _address.bytes[4], _address.bytes[5]);
  return String(szRet);
}

bool MACAddress::fromString(const char *address)
{

  uint16_t acc = 0; // Accumulator
  uint8_t dots = 0;

  while (*address)
  {
    char c = *address++;
    if (c >= '0' && c <= '9')
    {
      acc = acc * 16 + (c - '0');
      if (acc > 255) {
        // Value out of [0..255] range
        return false;
      }
    }
    else if (c >= 'a' && c <= 'f') {
      acc = acc * 16 + (c - 'a' + 10);
      if (acc > 255) {
        // Value out of [0..255] range
        return false;
      }
    }
    else if (c >= 'A' && c <= 'F') {
      acc = acc * 16 + (c - 'A' + 10);
      if (acc > 255) {
        // Value out of [0..255] range
        return false;
      }
    }
    else if (c == ':')
    {
      if (dots == 5) {
        // Too much dots (there must be 3 dots)
        return false;
      }
      _address.bytes[dots++] = acc;
      acc = 0;
    }
    else
    {
      // Invalid char
      return false;
    }
  }

  if (dots != 5) {
    // Too few dots (there must be 5 dots)
    return false;
  }
  _address.bytes[5] = acc;
  return true;
}
