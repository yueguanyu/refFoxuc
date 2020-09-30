//
// Standard texture effect
// Copyright {c} 2000 Microsoft Corporation. All rights reserved.
//

vector vClr;   // color 

vector vScl;

matrix mWld;    // World
matrix mTot;     // Total 

matrix mWd1;     
matrix mWd2;     
matrix mWd3;     
matrix mWd4;     
matrix mWd5;     
matrix mWd6;     
matrix mWd7;     
matrix mWd8;     
matrix mWd9;     
matrix mWd10;     
matrix mWd11;     
matrix mWd12;     
matrix mWd13;     
matrix mWd14;     
matrix mWd15;     
matrix mWd16;     
matrix mWd17;     
matrix mWd18;     
matrix mWd19;     
matrix mWd20;     
matrix mWd21;     
matrix mWd22;     
matrix mWd23;     
matrix mWd24;     
matrix mWd25;     
matrix mWd26;     
matrix mWd27;     
matrix mWd28;    

float4x3 mWorlds[28];

Vertexshader vDif = 
    decl
    {
                stream 0;

                float v0[3];
                float v1[4];

                float v2[3];
                float v3[4];

                float v4[3];
                float v5[4];

                float v6[3];
    }
    asm
    {
    vs.1.1

    dcl_position v0;
    dcl_blendweight v1;

    dcl_texcoord0 v2;
    dcl_texcoord1 v3;
    dcl_texcoord2 v4;
    dcl_texcoord3 v5;
    dcl_texcoord4 v6;

    //skin the point in v0 using v1 weights
    m4x3 r0.xyz,v0,c0
    m4x3 r1.xyz,v0,c4
    m4x3 r2.xyz,v0,c8
    m4x3 r3.xyz,v0,c12

    mul r0.xyz,r0.xyz,v1.xxxx
    mad r0.xyz,r1.xyz,v1.yyyy,r0
    mad r0.xyz,r2.xyz,v1.zzzz,r0
    mad r0.xyz,r3.xyz,v1.wwww,r0

    // move into avg register
    mul r8.xyz, r0.xyz, v6.xxxx

    //skin the point in v2 using v3 weights
    m4x3 r0.xyz,v2,c0
    m4x3 r1.xyz,v2,c4
    m4x3 r2.xyz,v2,c8
    m4x3 r3.xyz,v2,c12

    mul r0.xyz,r0.xyz,v3.xxxx
    mad r0.xyz,r1.xyz,v3.yyyy,r0
    mad r0.xyz,r2.xyz,v3.zzzz,r0
    mad r0.xyz,r3.xyz,v3.wwww,r0

    // add into avg register
    mad r8.xyz, r0.xyz,v6.yyyy,r8

    //skin the point in v4 using v5 weights
    m4x3 r0.xyz,v4,c0
    m4x3 r1.xyz,v4,c4
    m4x3 r2.xyz,v4,c8
    m4x3 r3.xyz,v4,c12

    mul r0.xyz,r0.xyz,v5.xxxx
    mad r0.xyz,r1.xyz,v5.yyyy,r0
    mad r0.xyz,r2.xyz,v5.zzzz,r0
    mad r0.xyz,r3.xyz,v5.wwww,r0

    // add into avg register
    mad r8.xyz, r0.xyz,v6.zzzz,r8

    mov r8.w,c20.x
    m4x4 oPos,r8,c16

    mov oD0,c22
    };


struct VS_INPUT_NONINDEXED
{
    float4 Position1 : POSITION;
    float4 Weights1 : BLENDWEIGHT;
    float4 Position2 : TEXCOORD0;
    float4 Weights2 : TEXCOORD1;
    float4 Position3 : TEXCOORD2;
    float4 Weights3 : TEXCOORD3;
    float3 Barycentric : TEXCOORD4;
};

struct VS_INPUT_INDEXED
{
    float4 Position1 : POSITION;
    float4 Weights1 : BLENDWEIGHT;
    int4 BlendIndices1 : BLENDINDICES;
    float4 Position2 : TEXCOORD0;
    float4 Weights2 : TEXCOORD1;
    int4 BlendIndices2 : TEXCOORD2;
    float4 Position3 : TEXCOORD3;
    float4 Weights3 : TEXCOORD4;
    int4 BlendIndices3 : TEXCOORD5;
    float3 Barycentric : TEXCOORD6;
};

struct VS_OUTPUT
{
    float4 Position : POSITION;
    float4 Color : COLOR;
};



VS_OUTPUT NonIndexed(VS_INPUT_NONINDEXED Input)
{
    float3 Position;
    float3 Position1;
    float3 Position2;
    float3 Position3;
    VS_OUTPUT Output;


    Position1 = mul(Input.Position1, (float4x3)mWd1) * Input.Weights1.xxx;
    Position1 += mul(Input.Position1, (float4x3)mWd2) * Input.Weights1.yyy;
    Position1 += mul(Input.Position1, (float4x3)mWd3) * Input.Weights1.zzz;
    Position1 += mul(Input.Position1, (float4x3)mWd4) * Input.Weights1.www;

    Position2 = mul(Input.Position2, (float4x3)mWd1) * Input.Weights2.xxx;
    Position2 += mul(Input.Position2, (float4x3)mWd2) * Input.Weights2.yyy;
    Position2 += mul(Input.Position2, (float4x3)mWd3) * Input.Weights2.zzz;
    Position2 += mul(Input.Position2, (float4x3)mWd4) * Input.Weights2.www;

    Position3 = mul(Input.Position3, (float4x3)mWd1) * Input.Weights3.xxx;
    Position3 += mul(Input.Position3, (float4x3)mWd2) * Input.Weights3.yyy;
    Position3 += mul(Input.Position3, (float4x3)mWd3) * Input.Weights3.zzz;
    Position3 += mul(Input.Position3, (float4x3)mWd4) * Input.Weights3.www;

    Position = Position1 * Input.Barycentric.x + Position2 * Input.Barycentric.y + Position3 * Input.Barycentric.z;
    
    Output.Color = vClr;
    Output.Position = mul(float4(Position, 1.0), mTot);

    return Output;    
}

VS_OUTPUT Indexed(VS_INPUT_INDEXED Input)
{
    float3 Position;
    float3 Position1;
    float3 Position2;
    float3 Position3;
    VS_OUTPUT Output;
    float4 BlendIndices;

    BlendIndices = D3DCOLORtoUBYTE4(Input.BlendIndices1);

    Position1 = mul(Input.Position1, (float4x3)mWorlds[BlendIndices.x]) * Input.Weights1.xxx;
    Position1 += mul(Input.Position1, (float4x3)mWorlds[BlendIndices.y]) * Input.Weights1.yyy;
    Position1 += mul(Input.Position1, (float4x3)mWorlds[BlendIndices.z]) * Input.Weights1.zzz;
    Position1 += mul(Input.Position1, (float4x3)mWorlds[BlendIndices.w]) * Input.Weights1.www;

    BlendIndices = D3DCOLORtoUBYTE4(Input.BlendIndices2);

    Position2 = mul(Input.Position2, (float4x3)mWorlds[BlendIndices.x]) * Input.Weights2.xxx;
    Position2 += mul(Input.Position2, (float4x3)mWorlds[BlendIndices.y]) * Input.Weights2.yyy;
    Position2 += mul(Input.Position2, (float4x3)mWorlds[BlendIndices.z]) * Input.Weights2.zzz;
    Position2 += mul(Input.Position2, (float4x3)mWorlds[BlendIndices.w]) * Input.Weights2.www;

    BlendIndices = D3DCOLORtoUBYTE4(Input.BlendIndices3);

    Position3 = mul(Input.Position3, (float4x3)mWorlds[BlendIndices.x]) * Input.Weights3.xxx;
    Position3 += mul(Input.Position3, (float4x3)mWorlds[BlendIndices.y]) * Input.Weights3.yyy;
    Position3 += mul(Input.Position3, (float4x3)mWorlds[BlendIndices.z]) * Input.Weights3.zzz;
    Position3 += mul(Input.Position3, (float4x3)mWorlds[BlendIndices.w]) * Input.Weights3.www;

    Position = Position1 * Input.Barycentric.x + Position2 * Input.Barycentric.y + Position3 * Input.Barycentric.z;
        
    Output.Color = vClr;
    Output.Position = mul(float4(Position, 1.0), mTot);

    return Output;    
}


technique T111
{
    pass P0
    {
        ColorVertex  = FALSE;

        ColorOp[0]   = Disable;
        AlphaOp[0]   = Disable;

        VertexShader = compile vs_1_1 NonIndexed();
#if 0
        VertexShader = <vDif>;

        VertexShaderConstant[0] = <mWd1>;
        VertexShaderConstant[4] = <mWd2>;
        VertexShaderConstant[8] = <mWd3>;
        VertexShaderConstant[12] = <mWd4>;
        VertexShaderConstant[16] = <mTot>;
        VertexShaderConstant[20] = {1.0f, 0.0f, 0.0f, 0.0f};
        VertexShaderConstant[21] = <vScl>;
        VertexShaderConstant[22] = <vClr>;
#endif
    }
}

Vertexshader vIndexed = 
    decl
    {
                stream 0;

                float v0[3];
                float v1[4];
                d3dcolor v2[1];

                float v3[3];
                float v4[4];
                d3dcolor v5[1];

                float v6[3];
                float v7[4];
                d3dcolor v8[1];

                float v9[3];
    }
    asm
    {
    vs.1.1

    dcl_position v0;
    dcl_blendweight v1;
    dcl_blendindices v2;

    dcl_texcoord0 v3;
    dcl_texcoord1 v4;
    dcl_texcoord2 v5;
    dcl_texcoord3 v6;
    dcl_texcoord4 v7
    dcl_texcoord5 v8;
    dcl_texcoord6 v9;

	// need to add dcl! here!!! UNDONE UNDONE

    // Compensate for lack of UBYTE4 on Geforce3
    mul r8,v2.zyxw,c4.wwww
    
    mov a0.x,r8.x
    m4x3 r0.xyz,v0,c[a0.x + 9]
    mov a0.x,r8.y
    m4x3 r1.xyz,v0,c[a0.x + 9]
    mov a0.x,r8.z
    m4x3 r2.xyz,v0,c[a0.x + 9]
    mov a0.x,r8.w
    m4x3 r3.xyz,v0,c[a0.x + 9]

    mul r0.xyz,r0.xyz,v1.xxxx
    mad r0.xyz,r1.xyz,v1.yyyy,r0
    mad r0.xyz,r2.xyz,v1.zzzz,r0
    mad r0.xyz,r3.xyz,v1.wwww,r0

    // move into avg register
    mul r7.xyz, r0.xyz, v9.xxxx

    // Compensate for lack of UBYTE4 on Geforce3
    mul r8,v5.zyxw,c4.wwww
    
    mov a0.x,r8.x
    m4x3 r0.xyz,v3,c[a0.x + 9]
    mov a0.x,r8.y
    m4x3 r1.xyz,v3,c[a0.x + 9]
    mov a0.x,r8.z
    m4x3 r2.xyz,v3,c[a0.x + 9]
    mov a0.x,r8.w
    m4x3 r3.xyz,v3,c[a0.x + 9]

    mul r0.xyz,r0.xyz,v4.xxxx
    mad r0.xyz,r1.xyz,v4.yyyy,r0
    mad r0.xyz,r2.xyz,v4.zzzz,r0
    mad r0.xyz,r3.xyz,v4.wwww,r0

    // add into avg register
    mad r7.xyz, r0.xyz,v9.yyyy,r7

    // Compensate for lack of UBYTE4 on Geforce3
    mul r8,v8.zyxw,c4.wwww
    
    mov a0.x,r8.x
    m4x3 r0.xyz,v6,c[a0.x + 9]
    mov a0.x,r8.y
    m4x3 r1.xyz,v6,c[a0.x + 9]
    mov a0.x,r8.z
    m4x3 r2.xyz,v6,c[a0.x + 9]
    mov a0.x,r8.w
    m4x3 r3.xyz,v6,c[a0.x + 9]

    mul r0.xyz,r0.xyz,v7.xxxx
    mad r0.xyz,r1.xyz,v7.yyyy,r0
    mad r0.xyz,r2.xyz,v7.zzzz,r0
    mad r0.xyz,r3.xyz,v7.wwww,r0

    // add into avg register
    mad r7.xyz, r0.xyz,v9.zzzz,r7

    mov r7.w,c4.x
    m4x4 oPos,r7,c0

    mov oD0,c5
    };

technique Indexed
{
    pass P0
    {
        ColorVertex = FALSE;

        ColorOp[0]   = Disable;
        AlphaOp[0]   = Disable;
#if 0
        VertexShader = <vIndexed>;

        VertexShaderConstant[0] = <mTot>;
        VertexShaderConstant[4] = { 1.0f, 0.0f, 0.0f, 765.01f };
        VertexShaderConstant[5] = <vClr>;
        VertexShaderConstant[6] = <vScl>;

        VertexShaderConstant3[9] = <mWd1>;
        VertexShaderConstant3[12] = <mWd2>;
        VertexShaderConstant3[15] = <mWd3>;
        VertexShaderConstant3[18] = <mWd4>;
        VertexShaderConstant3[21] = <mWd5>;
        VertexShaderConstant3[24] = <mWd6>;
        VertexShaderConstant3[27] = <mWd7>;
        VertexShaderConstant3[30] = <mWd8>;
        VertexShaderConstant3[33] = <mWd9>;
        VertexShaderConstant3[36] = <mWd10>;
        VertexShaderConstant3[39] = <mWd11>;
        VertexShaderConstant3[42] = <mWd12>;
        VertexShaderConstant3[45] = <mWd13>;
        VertexShaderConstant3[48] = <mWd14>;
        VertexShaderConstant3[51] = <mWd15>;
        VertexShaderConstant3[54] = <mWd16>;
        VertexShaderConstant3[57] = <mWd17>;
        VertexShaderConstant3[60] = <mWd18>;
        VertexShaderConstant3[63] = <mWd19>;
        VertexShaderConstant3[66] = <mWd20>;
        VertexShaderConstant3[69] = <mWd21>;
        VertexShaderConstant3[72] = <mWd22>;
        VertexShaderConstant3[75] = <mWd23>;
        VertexShaderConstant3[78] = <mWd24>;
        VertexShaderConstant3[81] = <mWd25>;
        VertexShaderConstant3[84] = <mWd26>;
        VertexShaderConstant3[87] = <mWd27>;
        VertexShaderConstant3[90] = <mWd28>;
#else
        VertexShader = compile vs_1_1 Indexed();
#endif
    }
}

