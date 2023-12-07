/*
* Poly1305
* (C) 2014 Jack Lloyd
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#ifndef BOTAN_MAC_POLY1305_H_
#define BOTAN_MAC_POLY1305_H_

#include <botan/mac.h>
#include <memory>

namespace Botan {

/**
* DJB's Poly1305
* Important note: each key can only be used once
*/
class Poly1305 final : public MessageAuthenticationCode {
   public:
      std::string name() const override { return "Poly1305"; }

      std::unique_ptr<MessageAuthenticationCode> new_object() const override { return std::make_unique<Poly1305>(); }

      void clear() override;

      size_t output_length() const override { return 16; }

      Key_Length_Specification key_spec() const override { return Key_Length_Specification(32); }

      bool fresh_key_required_per_message() const override { return true; }

      bool has_keying_material() const override;

   private:
      void add_data(Botan::span<const uint8_t>) override;
      void final_result(Botan::span<uint8_t>) override;
      void key_schedule(Botan::span<const uint8_t>) override;

      secure_vector<uint64_t> m_poly;
      secure_vector<uint8_t> m_buf;
      size_t m_buf_pos = 0;
};

}  // namespace Botan

#endif
