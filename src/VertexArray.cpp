#include "VertexArray.h"

VertexArray::VertexArray(){
    glGenVertexArrays(1, &m_rendererID);
}

VertexArray::~VertexArray(){
    glDeleteVertexArrays(1, &m_rendererID);
}

void VertexArray::bind() const{
    glBindVertexArray(m_rendererID);
}

void VertexArray::unbind() const{
    glBindVertexArray(0);
}

void VertexArray::addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout){
    bind();
    vb.bind();
    unsigned int off = 0;
    const auto& elements = layout.getElements();

    for(unsigned int i=0; i<elements.size(); i++){
        glEnableVertexAttribArray(i);
        const VertexBufferElement& e = elements[i];
        glVertexAttribPointer(i, e.count, e.type, e.norm, layout.getStride(), (const void*)off);
        off+=e.count * VertexBufferElement::getSizeof(e.type);
    }
}
