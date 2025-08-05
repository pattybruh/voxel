//
// Created by Patrick Li on 3/5/25.
//
#include "Renderer.h"

Renderer::Renderer(){}
Renderer::~Renderer(){}
void Renderer::draw(const VertexArray& va, const IndexBuffer& ib) const{
    va.bind();
    ib.bind();

    glDrawElements(GL_TRIANGLES, ib.get_count(), GL_UNSIGNED_INT, 0);
}
