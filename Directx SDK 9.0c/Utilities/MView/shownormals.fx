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

Vertexshader vNonIndexed = 
    decl
    {
                stream 0;

                float v0[3];
                float v1[4];
                float v3[3];
    }
    asm
    {
    vs.1.1
    
    dcl_position v0;
    dcl_blendweight v1;
    dcl_normal v3;

    m4x3 r0.xyz,v0,c0
    m4x3 r1.xyz,v0,c4
    m4x3 r2.xyz,v0,c8
    m4x3 r3.xyz,v0,c12

    mul r0.xyz,r0.xyz,v1.xxxx
    mad r0.xyz,r1.xyz,v1.yyyy,r0
    mad r0.xyz,r2.xyz,v1.zzzz,r0
    mad r0.xyz,r3.xyz,v1.wwww,r0

    m3x3 r4.xyz,v3,c0
    m3x3 r5.xyz,v3,c4
    m3x3 r6.xyz,v3,c8
    m3x3 r7.xyz,v3,c12

    mul r4.xyz,r4.xyz,v1.xxxx
    mad r4.xyz,r5.xyz,v1.yyyy,r4
    mad r4.xyz,r6.xyz,v1.zzzz,r4
    mad r4.xyz,r7.xyz,v1.wwww,r4

    mad r0.xyz,r4.xyz,c21.xxxx,r0

    mov r0.w,c20.x
    m4x4 oPos,r0,c16

    mov oD0,c22
    };

struct VS_INPUT_NONINDEXED
{
    float4 Position : POSITION;
    float4 Weights : BLENDWEIGHT;
    float3 Normal: NORMAL;
};

struct VS_INPUT_INDEXED
{
    float4 Position : POSITION;
    float4 Weights : BLENDWEIGHT;
    float4 BlendIndices : BLENDINDICES;
    float3 Normal: NORMAL;
};

struct VS_OUTPUT
{
    float4 Position : POSITION;
    float4 Color : COLOR;
};



VS_OUTPUT NonIndexed(VS_INPUT_NONINDEXED Input)
{
    float3 Position;
    float3 Normal;
    VS_OUTPUT Output;


    Position = mul(Input.Position, (float4x3)mWd1) * Input.Weights.xxx;
    Normal = mul(Input.Normal, (float3x3)mWd1) * Input.Weights.xxx;

    Position += mul(Input.Position, (float4x3)mWd2) * Input.Weights.yyy;
    Normal += mul(Input.Normal, (float3x3)mWd2) * Input.Weights.yyy;

    Position += mul(Input.Position, (float4x3)mWd3) * Input.Weights.zzz;
    Normal += mul(Input.Normal, (float3x3)mWd3) * Input.Weights.zzz;

    Position += mul(Input.Position, (float4x3)mWd4) * Input.Weights.www;
    Normal += mul(Input.Normal, (float3x3)mWd4) * Input.Weights.www;

    Position += Normal * vScl.xxx;

    Output.Color = vClr;
    Output.Position = mul(float4(Position, 1.0), mTot);

    return Output;    
}

#if 1
VS_OUTPUT Indexed(VS_INPUT_INDEXED Input)
{
    float3 Position;
    float3 Normal;
    VS_OUTPUT Output;
    float4 BlendIndices;

    BlendIndices = D3DCOLORtoUBYTE4(Input.BlendIndices);

    Position = mul(Input.Position, (float4x3)mWorlds[BlendIndices.x]) * Input.Weights.xxx;
    Position += mul(Input.Position, (float4x3)mWorlds[BlendIndices.y]) * Input.Weights.yyy;
    Position += mul(Input.Position, (float4x3)mWorlds[BlendIndices.z]) * Input.Weights.zzz;
    Position += mul(Input.Position, (float4x3)mWorlds[BlendIndices.w]) * Input.Weights.www;

    Normal = mul(Input.Normal, (float3x3)mWorlds[BlendIndices.x]) * Input.Weights.xxx;
    Normal += mul(Input.Normal, (float3x3)mWorlds[BlendIndices.y]) * Input.Weights.yyy;
    Normal += mul(Input.Normal, (float3x3)mWorlds[BlendIndices.z]) * Input.Weights.zzz;
    Normal += mul(Input.Normal, (float3x3)mWorlds[BlendIndices.w]) * Input.Weights.www;

    Position += Normal * vScl.xxx;

    Output.Color = vClr;
    Output.Position = mul(float4(Position, 1.0), mTot);

    return Output;    
}
#endif

Vertexshader vIndexed = 
    decl
    {
                stream 0;

                float v0[3];
                float v1[4];
                d3dcolor v2[1];
                float v3[3];
    }
    asm
    {
    vs.1.1

    dcl_position v0;
    dcl_blendweight v1;
    dcl_blendindices v2;
    dcl_normal v3;

    // Compensate for lack of UBYTE4 on Geforce3
    mul r8,v2.zyxw,c4.wwww
    
    mov a0.x,r8.x
    m4x3 r0.xyz,v0,c[a0.x + 9]
    m3x3 r4.xyz,v3,c[a0.x + 9]
    mov a0.x,r8.y
    m4x3 r1.xyz,v0,c[a0.x + 9]
    m3x3 r5.xyz,v3,c[a0.x + 9]
    mov a0.x,r8.z
    m4x3 r2.xyz,v0,c[a0.x + 9]
    m3x3 r6.xyz,v3,c[a0.x + 9]
    mov a0.x,r8.w
    m4x3 r3.xyz,v0,c[a0.x + 9]
    m3x3 r7.xyz,v3,c[a0.x + 9]

    mul r0.xyz,r0.xyz,v1.xxxx
    mad r0.xyz,r1.xyz,v1.yyyy,r0
    mad r0.xyz,r2.xyz,v1.zzzz,r0
    mad r0.xyz,r3.xyz,v1.wwww,r0

    mul r4.xyz,r4.xyz,v1.xxxx
    mad r4.xyz,r5.xyz,v1.yyyy,r4
    mad r4.xyz,r6.xyz,v1.zzzz,r4
    mad r4.xyz,r7.xyz,v1.wwww,r4

    mad r0.xyz,r4.xyz,c6.xxxx,r0

    mov r0.w,c4.x
    m4x4 oPos,r0,c0

    mov oD0,c5
    };

technique NonIndexed
{
    pass P0
    {
        ColorVertex = FALSE;

        ColorOp[0]   = Disable;
        AlphaOp[0]   = Disable;

        //VertexShader = <vNonIndexed>;
        VertexShader = compile vs_1_1 NonIndexed();
#if 0
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