#pragma once


//struct MESH_ELEMENT_HEADER {
//	char elementTag[4];
//	unsigned short numElement;
//	unsigned short stride;
//};


struct MESH_HEADER {
	unsigned int headerVersion; // バージョン
	unsigned int numSubmesh;    // サブメッシュの数
};

#define MESH_HEADER_VERSION 0x0001


struct SUBMESH_HEADER {
	unsigned int headerVersion; // バージョン
	unsigned int numIndex;      // 使ってない
	unsigned int numVertex;     // 頂点数
	unsigned int reserved;      // 使ってない
};

#define SUBMESH_HEADER_VERSION 0x0001


#define MESH_ELEMENT_TAG_INDEX "IDX"
#define MESH_ELEMENT_TAG_VERTEX "VTX"
#define MESH_ELEMENT_TAG_UV "UV"
#define MESH_ELEMENT_TAG_NORMAL "NML"


struct VERTEX_MESH {
	struct _VX {
		float x, y, z;
	} pos;

	struct {
		float r, g, b, a;
	} color;

	struct _UV {
		float u, v;
	} uv;
};
