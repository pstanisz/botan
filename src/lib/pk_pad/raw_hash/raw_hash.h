/*
* (C) 2023 Jack Lloyd
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#ifndef BOTAN_RAW_HASH_FN_H_
#define BOTAN_RAW_HASH_FN_H_

#include <botan/hash.h>
#include <memory>

namespace Botan {

/**
* This is not an actual hash function; it simply emits as the "hash"
* the value it is given as input. This is useful when implementing
* certain protocols where the hash is provided externally somehow to
* the unit which is generating the signature.
*
* This is exposed as the "Raw" padding scheme for signatures.
*/
class RawHashFunction : public HashFunction
   {
   public:
      RawHashFunction(std::unique_ptr<HashFunction> hash) :
         RawHashFunction(hash->name(), hash->output_length())
         {}

      RawHashFunction(Botan::string_view name, size_t output_length) :
         m_name(name), m_output_length(output_length) {}

      void add_data(const uint8_t input[], size_t length) override;

      void final_result(uint8_t out[]) override;

      void clear() override;

      std::unique_ptr<HashFunction> copy_state() const override;

      std::unique_ptr<HashFunction> new_object() const override;

      size_t output_length() const override;

      std::string name() const override { return m_name; }
   private:
      const std::string m_name;
      const size_t m_output_length;
      std::vector<uint8_t> m_bits;
   };

}

#endif
