// 定数バッファ
matrix World[4];     // ワールド変換行列
matrix View[6];      // ビュー変換行列
matrix Projection;   // 透視変換行列

float4 Diffuse;      // ディフューズ色
float4 Light[6];     // 光源座標(ビュー座標系)

Texture2D   Tex2D   : register(t0); // テクスチャ
TextureCube TexCube : register(t1); // テクスチャ(環境マップ)

static float3 ViewPos = float3(0.0f, 15.0, -15.0); // 視点(ワールド座標系)

// テクスチャ・サンプラ
SamplerState smpWrap : register(s0);

// **************************************************
// 立方体のインスタンス描画
// **************************************************

// 頂点シェーダの入力データ定義
struct VS_INPUT_CUBE_INSTANCE {
    float3 Pos  : POSITION;      // 頂点座標(モデル座標系)
    float3 Norm : NORMAL;        // 法線ベクトル(モデル座標系)
    float2 Tex  : TEXTURE;       // テクスチャ座標
    matrix Mat  : MATRIX;        // ワールド変換行列
    uint InstID : SV_InstanceID; // インスタンスID
};

// ピクセル シェーダの入力データ定義
struct PS_INPUT {
    float4 Pos  : SV_POSITION;  // 頂点座標(透視座標系)
    float3 PosView  : POSVIEW;  // 頂点座標(ビュー座標系)
    float3 Norm : NORMAL;       // 法線ベクトル(ビュー座標系)
    float2 Tex  : TEXTURE;      // テクスチャ座標
};

// 頂点シェーダの関数
PS_INPUT VS_CUBE_INSTANCE(VS_INPUT_CUBE_INSTANCE input) {
    PS_INPUT output;
    uint instNumber = input.InstID % 4;

    // 頂点座標　モデル座標系→透視座標系
    float4 pos4 = float4(input.Pos, 1.0);
    pos4 = mul(pos4, input.Mat);
    pos4 = mul(pos4, World[instNumber]);
    pos4 = mul(pos4, View[0]);
    output.PosView = pos4.xyz / pos4.w;
    output.Pos = mul(pos4, Projection);

    // 法線ベクトル　モデル座標系→ビュー座標系
    float3 Norm = mul(input.Norm, (float3x3)World[instNumber]);
    output.Norm = mul(Norm, (float3x3)View[0]);

    // テクスチャ座標
    output.Tex = input.Tex;

    // 出力
    return output;
}

// ライティング計算
float lighting(float3 PosView, float3 Norm, float3 L)
{
    // 光源ベクトル
    float3 light = L - PosView;
    // 距離
    float  leng = length(light);
    // 明るさ
    return 30 * dot(normalize(light), normalize(Norm)) / pow(leng, 2);
}

// ピクセル シェーダの関数
float4 PS_CUBE_INSTANCE(PS_INPUT input) : SV_TARGET
{
    // ライティング計算
    float bright = lighting(input.PosView, input.Norm, Light[0].xyz);

    // テクスチャ取得
    float4 texCol = Tex2D.Sample(smpWrap, input.Tex);         // テクセル読み込み

    // 色
    return saturate(bright * texCol * Diffuse);
}

// **************************************************
// 球の描画
// **************************************************

// 頂点シェーダの入力データ定義
struct VS_INPUT_SPHERE {
    float3 Pos  : POSITION;  // 頂点座標(モデル座標系)
    float3 Norm : NORMAL;    // 法線ベクトル(モデル座標系)
    float2 Tex  : TEXTURE;   // テクスチャ座標
};

// ピクセル シェーダの入力データ定義
struct PS_INPUT_SPHERE {
    float4 Pos  : SV_POSITION;  // 頂点座標(透視座標系)
    float3 PosView  : POSVIEW;  // 頂点座標(ビュー座標系)
    float3 Norm : NORMAL;       // 法線ベクトル(ビュー座標系)
    float2 Tex  : TEXTURE;      // テクスチャ座標
    float3 ViewWorld : POSWORLD;  // 頂点座標ベクトル(ワールド座標系)
    float3 NormWorld : NORMWORLD; // 法線ベクトル(ワールド座標系)
};

// 頂点シェーダの関数(球)
PS_INPUT_SPHERE VS_SPHERE(VS_INPUT_SPHERE input) {
    PS_INPUT_SPHERE output;

    // 頂点座標　モデル座標系→透視座標系
    float4 pos4 = float4(input.Pos, 1.0);
    pos4 = mul(pos4, View[0]);
    output.PosView = pos4.xyz / pos4.w;
    output.Pos = mul(pos4, Projection);

    // 法線ベクトル　モデル座標系→ビュー座標系
    output.Norm = mul(input.Norm, (float3x3)View[0]);

    // テクスチャ座標
    output.Tex = input.Tex;

    // キューブ・テクスチャのアクセス用
    output.ViewWorld  = input.Pos - ViewPos;
    output.NormWorld = input.Norm;

    // 出力
    return output;
}

// ピクセル シェーダの関数
float4 PS_SPHERE(PS_INPUT_SPHERE input) : SV_TARGET
{
    // ライティング計算
    float bright = lighting(input.PosView, input.Norm, Light[0].xyz);

    // テクスチャ取得
    float4 texCol = Tex2D.Sample(smpWrap, input.Tex);         // テクセル読み込み

    // 環境マップ計算
    float3 E = normalize(input.ViewWorld); // 視線ベクトル
    float3 N = normalize(input.NormWorld); // 法線ベクトル
    float3 R = reflect(E, N);              // 反射ベクトル
    float4 envMap = TexCube.Sample(smpWrap, R); // キューブ・テクスチャにアクセス

    // 色
    return saturate(envMap + 0.25 * bright * texCol * Diffuse);
}

// **************************************************
// 環境マップの描画
// **************************************************

// 頂点シェーダの入力データ定義
// (立方体のインスタンス描画と同じ)

// ジオメトリ シェーダの入力データ定義
struct GS_INPUT_ENVMAP {
    float4 Pos  : SV_POSITION;  // 頂点座標(ワールド座標系)
    float3 Norm : NORMAL;       // 法線ベクトル(ワールド座標系)
    float2 Tex  : TEXTURE;      // テクスチャ座標
};

// ピクセル シェーダの入力データ定義
struct PS_INPUT_ENVMAP {
    float4 Pos     : SV_POSITION;     // 頂点座標(射影座標系)
    float3 PosView : POSVIEW;         // 頂点座標(ビュー座標系)
    float3 Norm    : NORMAL;          // 法線ベクトル(ビュー座標系)
    float2 Tex     : TEXTURE;         // テクスチャ座標
    uint RTIndex   : SV_RenderTargetArrayIndex; // 描画ターゲット
};

// 頂点シェーダの関数
GS_INPUT_ENVMAP VS_ENVMAP(VS_INPUT_CUBE_INSTANCE input) {
    GS_INPUT_ENVMAP output;
    uint instNumber = input.InstID % 4;

    // 頂点座標　モデル座標系→ワールド座標系
    float4 pos4 = float4(input.Pos, 1.0);
    pos4 = mul(pos4, input.Mat);
    output.Pos = mul(pos4, World[instNumber]);

    // 法線ベクトル　モデル座標系→ワールド座標系
    float3 norm = mul(input.Norm, (float3x3)input.Mat);
    output.Norm = mul(norm, (float3x3)World[instNumber]);

    // テクスチャ座標
    output.Tex = input.Tex;

    // 出力
    return output;
}

// ジオメトリ シェーダの関数
[maxvertexcount(18)]
void GS_ENVMAP(triangle GS_INPUT_ENVMAP In[3],
                inout TriangleStream<PS_INPUT_ENVMAP> EnvMapStream)
{
    // 6面を計算する
    for (int f = 0; f < 6; ++f)
    {
        PS_INPUT_ENVMAP Out;
        Out.RTIndex = f;  // このプリミティブを描画する描画ターゲット
        // 各面毎に頂点座標を計算して描画先を設定
        for (int v = 0; v < 3; v++)
        {
            // 頂点座標　ワールド座標系→ビュー座標系
            Out.Pos = mul(In[v].Pos, View[f]);
            Out.PosView = Out.Pos;
            // 頂点座標　ビュー座標系→射影座標系
            Out.Pos = mul(Out.Pos, Projection);
            // 法線ベクトル　ワールド座標系→ビュー座標系
            Out.Norm = mul(In[v].Norm, (float3x3)View[f]);
            // テクスチャ座標
            Out.Tex     = In[v].Tex;
            // 頂点出力
            EnvMapStream.Append(Out); // 頂点出力
        }
        // 1ストリップ終了
        EnvMapStream.RestartStrip();
    }
}

// ピクセル シェーダの関数
float4 PS_ENVMAP(PS_INPUT_ENVMAP input) : SV_TARGET {
    // ライティング計算
    float bright = lighting(input.PosView, input.Norm, Light[input.RTIndex].xyz);

    // テクスチャ取得
    float4 texCol = Tex2D.Sample(smpWrap, input.Tex);         // テクセル読み込み

    // 色
    return saturate(bright * texCol * Diffuse);
}
