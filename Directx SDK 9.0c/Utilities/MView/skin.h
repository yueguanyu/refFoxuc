#pragma once
class CBone
{
public:
    CBone* next;
    char* boneName;
    DWORD numWeights;
    D3DXMATRIX offsetMat;
    DWORD* vertIndices;
    float* weights;
};

typedef CBone *LPBONE;

LPBONE CreateBone(char* name, DWORD numWeights);

class CSkinMesh
{
public:
    LPD3DXMESH m_pMesh;
    LPBONE m_pBoneList;
    DWORD* m_faceMatId;
    float* m_vertWeight;

    void ProcessSkinData(LPDIRECT3DDEVICE9 pD3DDevice);
};
typedef CSkinMesh *LPSKINMESH;
