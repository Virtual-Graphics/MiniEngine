#pragma once
#include "buffer.h"
#include <glad/glad.h>
#include <vector>

namespace mini::gl{
class IndexBuffer : public Buffer{
public:
    enum class Type{
        Uint8 = GL_UNSIGNED_BYTE,
        Uint16 = GL_UNSIGNED_SHORT,
        Uint32 = GL_UNSIGNED_INT
    };

    IndexBuffer() = default;
    ~IndexBuffer() = default;
    
    template<typename T>
    void set_data(const std::vector<T>& data, Type type, Usage usage = Usage::Static){
        count_ = data.size();
        type_ = type;
        Buffer::set_data(data.data(), data.size() * type_size(type), usage);
    }
    
    
    void set_data(const void* data, std::size_t count, Type type, Usage usage = Usage::Static){
        count_ = count; 
        type_ = type;
        Buffer::set_data(data, count * type_size(type), usage);
    }
private:
    Type type_ = Type::Uint32;
    std::size_t count_ = 0;

    static std::size_t type_size(Type t) {
        switch (t) {
            case Type::Uint8:  return 1;
            case Type::Uint16: return 2;
            case Type::Uint32: return 4;
        }
        return 4;
    }
};
}
