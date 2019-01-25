#ifndef MACAddress_h
#define MACAddress_h

#include <stdint.h>
#include <WString.h>
#include <Printable.h>

class MACAddress: public Printable {
  public:
    MACAddress();
    MACAddress(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet, uint8_t fifth_octet, uint8_t sixth_octet);
    MACAddress(uint64_t address);
    MACAddress(const uint8_t *address);
    virtual ~MACAddress() {}

    bool fromString(const char *address);
    bool fromString(const String &address) {
      return fromString(address.c_str());
    }

    // Overloaded cast operator to allow MACAddress objects to be used where a pointer
    // to a six-byte uint8_t array is expected
    operator uint64_t() const
    {
      return _address.qword;
    }
    bool operator==(const MACAddress& addr) const
    {
      return _address.qword == addr._address.qword;
    }
    bool operator==(const uint8_t* addr) const;

    // Overloaded index operator to allow getting and setting individual octets of the address
    uint8_t operator[](int index) const
    {
      return _address.bytes[index];
    }
    uint8_t& operator[](int index)
    {
      return _address.bytes[index];
    }

    // Overloaded copy operators to allow initialisation of MACAddress objects from other types
    MACAddress& operator=(const uint8_t *address);
    MACAddress& operator=(const uint64_t address);

    virtual size_t printTo(Print& p) const;
    String toString() const;

  private:
    union {
      uint8_t bytes[6];  // IPv4 address
      uint64_t qword;
    } _address;
};

#endif
