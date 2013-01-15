// 定数バッファのデータ定義
cbuffer cbCBuffer : register(b0) { // 常にスロット「0」を使う
    matrix World[4];     // ワールド変換行列
    matrix View;         // ビュー変換行列
    matrix Projection;   // 透視変換行列
    float4 Diffuse;      // ディフューズ色
    float3 Light;        // 光源座標(ビュー座標系)
};

Texture2D Tex2D;     // テクスチャ

// テクスチャ・サンプラ
SamplerState smpWrap : register(s0);

// **************************************************
// 立方体のインスタンス描画
// **************************************************

// 頂点シェーダの入力データ定義
struct VS_INPUT {
    float3 Pos  : POSITION;  // 頂点座標(モデル座標系)
    float3 Norm : NORMAL;    // 法線ベクトル(モデル座標系)
    float2 Tex  : TEXTURE;   // テクスチャ座標
    matrix Mat  : MATRIX;    // ワールド変換行列
    uint InstID : SV_InstanceID; // インスタンスID
};

// ピクセル シェーダの入力データ定義
struct PS_INPUT {
    float4 Pos  : SV_POSITION; // 頂点座標(透視座標系)
    float3 PosView  : POSVIEW; // 頂点座標(ビュー座標系)
    float3 Norm : NORMAL;      // 法線ベクトル(ビュー座標系)
    float2 Tex  : TEXTURE;     // テクスチャ座標
};

// 頂点シェーダの関数
PS_INPUT VS(VS_INPUT input) {
    PS_INPUT output;
    uint instNumber = input.InstID % 4;

    // 頂点座標　モデル座標系→透視座標系
    float4 pos4 = float4(input.Pos, 1.0);
    pos4 = mul(pos4, input.Mat);
    pos4 = mul(pos4, World[instNumber]);
    pos4 = mul(pos4, View);
    output.PosView = pos4.xyz / pos4.w;
    output.Pos = mul(pos4, Projection);

    // 法線ベクトル　モデル座標系→ビュー座標系
    float3 Norm = mul(input.Norm, (float3x3)World[instNumber]);
    output.Norm = mul(Norm, (float3x3)View);

    // テクスチャ座標
    output.Tex = input.Tex;

    // 出力
    return output;
}

// ライティング計算
float lighting(PS_INPUT input)
{
    // 光源ベクトル
    float3 light = Light - input.PosView;
    // 距離
    float  leng = length(light);
    // 明るさ
    return 30 * dot(normalize(light), normalize(input.Norm)) / pow(leng, 2);
}

// ピクセル シェーダの関数
float4 PS(PS_INPUT input) : SV_TARGET
{
    // ライティング計算
    float bright = lighting(input);

    // テクスチャ取得
    float4 texCol = Tex2D.Sample(smpWrap, input.Tex);         // テクセル読み込み

    // 色
    return saturate(bright * texCol * Diffuse);
}
