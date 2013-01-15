matrix View;         // ビュー変換行列
matrix Projection;   // 透視変換行列

float2        ParticleSize;  // パーティクルの大きさ
float         Time;          // 経過時間(秒)
static float3 Acceleration = float3(0.0, -9.8, 0.0);  // 加速度(m/s^2)

Texture2D     Tex2D;         // テクスチャ

// テクスチャ・サンプラ
SamplerState smpWrap : register(s0);

// **************************************************
// パーティクル座標の計算処理
// **************************************************

// ジオメトリ・シェーダの入力データ定義
struct GS_INPUT_PARTICLE {
    float3 Position      : POSITION;     // 頂点座標(ワールド座標系)
    float3 Velocity      : NORMAL;       // 現在の速度ベクトル(ワールド座標系)
    float3 VelocityFirst : TEXCOORD0;    // 最初の速度ベクトル(ワールド座標系)
};

// 頂点シェーダ
GS_INPUT_PARTICLE VS_PARTICLE_VERTEX(GS_INPUT_PARTICLE In)
{
    return In;
}

// ジオメトリ・シェーダの関数
[maxvertexcount(1)]
void GS_PARTICLE_VERTEX(point GS_INPUT_PARTICLE In[1],
                inout PointStream<GS_INPUT_PARTICLE> ParticleStream)
{
    GS_INPUT_PARTICLE Out;
    // 最初の速度は変化しない
    Out.VelocityFirst = In[0].VelocityFirst;
    // Time秒後の速度
    Out.Velocity = Acceleration * Time + In[0].Velocity;
    // Time秒後の位置
    Out.Position = 0.5 * Acceleration * Time * Time + In[0].Velocity * Time + In[0].Position;
    if (Out.Position.y < 0) {
        // パーティクルが床下に落ちたので、新しく始める
        Out.Position = float3(0.0, 0.1, 0.0);
        Out.Velocity = In[0].VelocityFirst;
    }
    // 計算を出力
    ParticleStream.Append(Out);
}

// **************************************************
// パーティクルの描画処理
// **************************************************

// ピクセル・シェーダの入力データ定義
struct PS_INPUT {
    float4 Pos  : SV_Position;     // 頂点座標(射影座標系)
    float2 Tex  : TEXTURE;         // テクスチャ座標
};

// 頂点シェーダの関数
GS_INPUT_PARTICLE VS_PARTICLE(GS_INPUT_PARTICLE In)
{
    return In;
}

// ジオメトリ・シェーダの関数
[maxvertexcount(4)]
void GS_PARTICLE(point GS_INPUT_PARTICLE In[1],
                inout TriangleStream<PS_INPUT> ParticleStream)
{
    // 座標変換　ワールド座標系　→　ビュー座標系
    float4 pos = mul(float4(In[0].Position, 1.0), View);

    // 点を面にする
    float4 posLT = pos + float4(-ParticleSize.x,  ParticleSize.y, 0.0, 0.0) * pos.w;
    float4 posLB = pos + float4(-ParticleSize.x, -ParticleSize.y, 0.0, 0.0) * pos.w;
    float4 posRT = pos + float4( ParticleSize.x,  ParticleSize.y, 0.0, 0.0) * pos.w;
    float4 posRB = pos + float4( ParticleSize.x, -ParticleSize.y, 0.0, 0.0) * pos.w;

    // 左上の点の位置(射影座標系)を計算して出力
    PS_INPUT Out;
    Out.Pos = mul(posLT, Projection);
    Out.Tex = float2(0.0, 0.0);
    ParticleStream.Append(Out);

    // 右上の点の位置(射影座標系)を計算して出力
    Out.Pos = mul(posRT, Projection);
    Out.Tex = float2(1.0, 0.0);
    ParticleStream.Append(Out);

    // 左下の点の位置(射影座標系)を計算して出力
    PS_INPUT Out1;
    Out1.Pos = mul(posLB, Projection);
    Out1.Tex = float2(0.0, 1.0);
    ParticleStream.Append(Out1);

    // 右下の点の位置(射影座標系)を計算して出力
    Out.Pos = mul(posRB, Projection);
    Out.Tex = float2(1.0, 1.0);
    ParticleStream.Append(Out);

    ParticleStream.RestartStrip();
}

// ピクセル・シェーダの関数
float4 PS_PARTICLE(PS_INPUT input) : SV_Target
{
    return Tex2D.Sample(smpWrap, input.Tex);
}

// **************************************************
// 床の描画処理
// **************************************************

// 頂点シェーダの関数
PS_INPUT VS_FLOOR(uint vid : SV_VertexID)
{
    PS_INPUT output = (PS_INPUT)0;
    // 床の頂点座標を生成
    switch(vid) {
    case 0:
    case 5:
        // 左下
        output.Pos = float4(-2, 0, -2, 1); output.Tex = float2(0, 1); break;
    case 1:
        // 左上
        output.Pos = float4(-2, 0,  2, 1); output.Tex = float2(0, 0); break;
    case 2:
    case 3:
        // 右上
        output.Pos = float4( 2, 0,  2, 1); output.Tex = float2(1, 0); break;
    case 4:
        // 右下
        output.Pos = float4( 2, 0, -2, 1); output.Tex = float2(1, 1); break;
    }
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    return output;
}

// ピクセル・シェーダの関数
float4 PS_FLOOR(PS_INPUT input) : SV_Target
{
    return Tex2D.Sample(smpWrap, input.Tex) * 0.5;
}
