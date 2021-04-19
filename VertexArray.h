#pragma once

#include"VertexBuffer.h"
#include"VertexBufferLayout.h"


class VertexArray {
private:
	unsigned int m_RendererID;
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, VertexBufferLayout& layout);
	void Bind() const ;
	void Unbind() const ;

};
