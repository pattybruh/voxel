#ifndef VERTEXBUFFERLAYOUT_H
#define VERTEXBUFFERLAYOUT_H

#include <glad/glad.h>
#include <vector>
#include <cassert>

struct VertexBufferElement{
    unsigned int type;
    unsigned int count;
    unsigned char norm;
    static unsigned int getSizeof(unsigned int type){
        switch(type){
            case GL_FLOAT: return 4;
            case GL_UNSIGNED_INT: return 4;
            case GL_UNSIGNED_BYTE: return 1;
        }
        assert(false);
        return 0;
    }
};

class VertexBufferLayout{
private:
    unsigned int m_stride;
    std::vector<VertexBufferElement> m_elements;
public:
    VertexBufferLayout()
        : m_stride(0){}

    template<typename T>
    void push(unsigned int count){
        static_assert(sizeof(T)==0, "invalid type for VertexBufferLayout::push<T>()");
    }

    inline unsigned int get_stride() const{
        return m_stride;
    }
    inline const std::vector<VertexBufferElement>& get_elements() const{
        return m_elements;
    }
};

template<>
inline void VertexBufferLayout::push<float>(unsigned int count){
    m_elements.push_back({GL_FLOAT, count, GL_FALSE});
    m_stride += count*VertexBufferElement::getSizeof(GL_FLOAT);
}
template<>
inline void VertexBufferLayout::push<unsigned int>(unsigned int count){
    m_elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE});
    m_stride += count*VertexBufferElement::getSizeof(GL_UNSIGNED_INT);
}
template<>
inline void VertexBufferLayout::push<unsigned char>(unsigned int count){
    m_elements.push_back({GL_UNSIGNED_BYTE, count, GL_TRUE});
    m_stride += count*VertexBufferElement::getSizeof(GL_UNSIGNED_BYTE);
}

#endif //VERTEXBUFFERLAYOUT_H
