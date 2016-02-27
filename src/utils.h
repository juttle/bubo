#pragma once

#include <vector>
#include <string>

#include "bubo-types.h"

/* helper struct for sorting entry tokens */
struct EntryToken {
    const char* tag_;
    const char* val_;
    uint32_t tag_seq_no_;
    uint32_t val_seq_no_;
    ~EntryToken() {
        tag_ = NULL; val_ = NULL;
    }
};


namespace bubo_utils {

void initialize(std::vector<std::string> ignoredAttrs);

void hex_out(const BYTE* data, int len, const char* hint=NULL);

// Using Google's protobuffer encoding (https://github.com/google/protobuf)
inline void encode_packed(uint32_t val, BYTE* out, int* outlen) {
    int length = 1;
    while (val >= 0x80) {
        *out = static_cast<uint8_t>(val | 0x80);
        val >>= 7;
        out++;
        length++;
    }
    *out = static_cast<uint8_t>(val);
    *outlen = length;
}

inline uint32_t decode_packed(const BYTE* in) {
    uint32_t result = *in;
    if (result < 0x80) {
        return result;
    }
    uint32_t b;
    result -= 0x80;
    in++;
    b = *(in++); result += b <<  7; if (!(b & 0x80)) return result;
    result -= 0x80 << 7;
    b = *(in++); result += b << 14; if (!(b & 0x80)) return result;
    result -= 0x80 << 14;
    b = *(in++); result += b << 21; if (!(b & 0x80)) return result;
    result -= 0x80 << 21;
    b = *(in++); result += b << 28;

    return result;
}

inline bool cmp(const v8::Local<v8::String>& lhs, const v8::Local<v8::String>& rhs) {
    const v8::String::Utf8Value lval(lhs);
    const v8::String::Utf8Value rval(rhs);
    int ret = memcmp(*lval, *rval, lval.length());
    return ret < 0;
}


inline bool cmp_entry_token(const EntryToken* lhs, const EntryToken* rhs) {
    return strcmp(lhs->tag_, rhs->tag_) < 0;
}


inline int get_entry_len(const BYTE* b) {
    if (!b) {
        return 0;
    }
    uint64_t num_tuples = decode_packed(b);
    if (num_tuples == 0) {
        return 1;
    }

    int len = 0;
    for (uint64_t i = 0; i < 2* num_tuples + 1; i++) {
        do {
            len ++;
        } while (*b++ & 0x80);
    }
    return len;
}


inline uint32_t hash_byte_sequence(const BYTE* data, uint32_t len) {
    /* http://en.wikipedia.org/wiki/Jenkins_hash_function */
    uint32_t hash, i;
    for(hash = i = 0; i < len; ++i) {
        hash += data[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}


}
