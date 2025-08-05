//
// Created by Patrick Li on 3/5/25.
//

#ifndef RENDERER_H
#define RENDERER_H
#include <glad/glad.h>
#include "VertexArray.h"
#include "IndexBuffer.h"

class Renderer{
public:
    Renderer();
    ~Renderer();
    void draw(const VertexArray& va, const IndexBuffer& ib) const;
};

#endif //RENDERER_H
