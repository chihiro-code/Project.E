#pragma once


//struct MESH_ELEMENT_HEADER {
//	char elementTag[4];
//	unsigned short numElement;
//	unsigned short stride;
//};


struct MESH_HEADER {
	unsigned int headerVersion; // �o�[�W����
	unsigned int numSubmesh;    // �T�u���b�V���̐�
};

#define MESH_HEADER_VERSION 0x0001


struct SUBMESH_HEADER {
	unsigned int headerVersion; // �o�[�W����
	unsigned int numIndex;      // �g���ĂȂ�
	unsigned int numVertex;     // ���_��
	unsigned int reserved;      // �g���ĂȂ�
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
