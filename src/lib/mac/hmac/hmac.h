/*
* HMAC
* (C) 1999-2007,2014 Jack Lloyd
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#ifndef BOTAN_HMAC_H_
#define BOTAN_HMAC_H_

#include <botan/mac.h>
#include <botan/hash.h>

namespace Botan {

/**
* HMAC
*/
class HMAC final : public MessageAuthenticationCode
   {
   public:
      void clear() override;
      std::string name() const override;
      std::unique_ptr<MessageAuthenticationCode> new_object() const override;

      size_t output_length() const override;

      Key_Length_Specification key_spec() const override;

      bool has_keying_material() const override;

      /**
      * @param hash the hash to use for HMACing
      */
      explicit HMAC(std::unique_ptr<HashFunction> hash);

   // TODO: pstanisz - why clang thinks that above is copy ctor??
   public:
      HMAC(const HMAC&);
      HMAC& operator=(const HMAC&);

   private:
      void add_data(const uint8_t[], size_t) override;
      void final_result(uint8_t[]) override;
      void key_schedule(const uint8_t[], size_t) override;

      std::unique_ptr<HashFunction> m_hash;
      secure_vector<uint8_t> m_ikey, m_okey;
      size_t m_hash_output_length;
      size_t m_hash_block_size;
   };

}

#endif
