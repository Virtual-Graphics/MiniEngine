#pragma once
#include <glad/glad.h>
#include <functional>

namespace mini::gfx{
class Object{
protected:
    GLuint id_ = 0;
    std::function<void(GLuint)> deleter_;
public:
    explicit Object(std::function<void(GLuint)> deleter) : deleter_(std::move(deleter)) {}
    virtual ~Object(){
        if(id_ && deleter_){
            deleter_(id_);
        }
    }

    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;

    Object(Object&& o) noexcept : id_(o.id_),deleter_(std::move(o.deleter_)){ o.id_ = 0; }
    Object& operator=(Object&& o) noexcept {
        if (this != &o) {
            if (id_ && deleter_) deleter_(id_);
            id_ = o.id_;
            deleter_ = std::move(o.deleter_);
            o.id_ = 0;
        }
        return *this;
    }

    GLuint id() const{
        return id_;
    }

    bool valid() const {
        return id_ != 0;
    }

    void reset(){
        if (id_ && deleter_){
            deleter_(id_);
            id_ = 0;
        }
    }
};
}
