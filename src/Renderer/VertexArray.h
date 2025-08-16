#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexArray{
private:
    unsigned int m_rendererID;
public:
    VertexArray();
    ~VertexArray();

    void bind() const;
    void unbind() const;
    void add_buffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
};

#endif //VERTEXARRAY_H
