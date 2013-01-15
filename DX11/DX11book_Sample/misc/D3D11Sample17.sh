matrix World;        // ワールド変換行列
matrix View;         // ビュー変換行列
matrix Projection;   // 透視変換行列
float4 Diffuse;      // ディフューズ色
float3 Light;        // 光源座標(ビュー座標系)

Texture2D Tex2D;     // テクスチャ

// テクスチャ・サンプラ
SamplerState smpWrap : register(s0);

// **************************************************
// 3Dオブジェクトの描画
// **************************************************

// 頂点シェーダの入力データ定義
struct VS_INPUT {
    float3 Pos  : POSITION;  // 頂点座標(モデル座標系)
    float3 Norm : NORMAL;    // 法線ベクトル(モデル座標系)
    float2 Tex  : TEXTURE;   // テクスチャ座標
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

    // 頂点座標　モデル座標系→透視座標系
    float4 pos4 = float4(input.Pos, 1.0);
    pos4 = mul(pos4, World);
    pos4 = mul(pos4, View);
    output.PosView = pos4.xyz / pos4.w;
    output.Pos = mul(pos4, Projection);

    // 法線ベクトル　モデル座標系→ビュー座標系
    float3 Norm = mul(input.Norm, (float3x3)World);
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

// **************************************************
// シャドウ・ボリュームの描画
// **************************************************

struct GS_INPUT_SV {
    float3 Pos : SV_POSITION;
};

struct PS_INPUT_SV {
    float4 Pos : SV_POSITION;
};

// 頂点シェーダの関数
float3 VS_SV(VS_INPUT input) : POSITION {
    // 頂点座標　モデル座標系→ビュー座標系
    float4 Pos = float4(input.Pos, 1.0);
    Pos = mul(Pos, World);
    Pos = mul(Pos, View);
    return Pos.xyz / Pos.w;
}

// ジオメトリ・シェーダの関数①
[maxvertexcount(12)]
void GS_SV_1(triangleadj float3 input[6] : POSITION,
        inout TriangleStream<PS_INPUT_SV> TriStream) {
    PS_INPUT_SV output = (PS_INPUT_SV)0;

    // 法線ベクトルの計算
    float3 faceNorm  = normalize(cross(input[2] - input[0], input[4] - input[0]));

    // 光源ベクトルの計算
    float3 lightVec = Light - (input[0] + input[2] + input[4]) / 3.0;

    // 光源を向いている？
    if (dot(faceNorm, lightVec) >= 0.0)
        return;

/* *********************************************
    // 以下のコードはなぜか正しく動作しない(GeForce 8800 GTX)。[2007/6/21]
    // 各辺の判定
    for (uint iA=0; iA<6; iA+=2) {
        uint iB = iA+1, iC = (iA+2) % 6;
        // 光源ベクトルの計算
        lightVec = Light - (input[iA] + input[iB] + input[iC]) / 3.0;
        // 隣接面の法線ベクトルの計算
        faceNorm = cross(input[iB] - input[iA], input[iC] - input[iA]);
        // 境界判定
        if (dot(faceNorm, lightVec) >= 0.0) {
            // シャドウ・ボリュームを生成
            output.Pos = mul(float4(input[iA], 1.0), Projection);
            TriStream.Append(output);
            output.Pos = mul(float4(input[iC], 1.0), Projection);
            TriStream.Append(output);
            output.Pos = mul(float4(input[iA] + (input[iA] - Light) * 1000, 1.0), Projection);
            TriStream.Append(output);
            output.Pos = mul(float4(input[iC] + (input[iC] - Light) * 1000, 1.0), Projection);
            TriStream.Append(output);

            TriStream.RestartStrip();
        }
    }
********************************************* */
    // 以上のコードが正しく動作しないので、以下にforループを展開する。
    uint iA = 0; {
        uint iB = iA+1, iC = (iA+2) % 6;
        // 光源ベクトルの計算
        lightVec = Light - (input[iA] + input[iB] + input[iC]) / 3.0;
        // 隣接面の法線ベクトルの計算
        faceNorm = cross(input[iB] - input[iA], input[iC] - input[iA]);
        // 境界判定
        if (dot(faceNorm, lightVec) >= 0.0) {
            // シャドウ・ボリュームを生成
            output.Pos = mul(float4(input[iA], 1.0), Projection);
            TriStream.Append(output);
            output.Pos = mul(float4(input[iC], 1.0), Projection);
            TriStream.Append(output);
            output.Pos = mul(float4(input[iA] + (input[iA] - Light) * 1000, 1.0), Projection);
            TriStream.Append(output);
            output.Pos = mul(float4(input[iC] + (input[iC] - Light) * 1000, 1.0), Projection);
            TriStream.Append(output);

            TriStream.RestartStrip();
        }
    }

    iA = 2; {
        uint iB = iA+1, iC = (iA+2) % 6;
        // 光源ベクトルの計算
        lightVec = Light - (input[iA] + input[iB] + input[iC]) / 3.0;
        // 隣接面の法線ベクトルの計算
        faceNorm = cross(input[iB] - input[iA], input[iC] - input[iA]);
        // 境界判定
        if (dot(faceNorm, lightVec) >= 0.0) {
            // シャドウ・ボリュームを生成
            output.Pos = mul(float4(input[iA], 1.0), Projection);
            TriStream.Append(output);
            output.Pos = mul(float4(input[iC], 1.0), Projection);
            TriStream.Append(output);
            output.Pos = mul(float4(input[iA] + (input[iA] - Light) * 1000, 1.0), Projection);
            TriStream.Append(output);
            output.Pos = mul(float4(input[iC] + (input[iC] - Light) * 1000, 1.0), Projection);
            TriStream.Append(output);

            TriStream.RestartStrip();
        }
    }

    iA = 4; {
        uint iB = iA+1, iC = (iA+2) % 6;
        // 光源ベクトルの計算
        lightVec = Light - (input[iA] + input[iB] + input[iC]) / 3.0;
        // 隣接面の法線ベクトルの計算
        faceNorm = cross(input[iB] - input[iA], input[iC] - input[iA]);
        // 境界判定
        if (dot(faceNorm, lightVec) >= 0.0) {
            // シャドウ・ボリュームを生成
            output.Pos = mul(float4(input[iA], 1.0), Projection);
            TriStream.Append(output);
            output.Pos = mul(float4(input[iC], 1.0), Projection);
            TriStream.Append(output);
            output.Pos = mul(float4(input[iA] + (input[iA] - Light) * 1000, 1.0), Projection);
            TriStream.Append(output);
            output.Pos = mul(float4(input[iC] + (input[iC] - Light) * 1000, 1.0), Projection);
            TriStream.Append(output);

            TriStream.RestartStrip();
        }
    }
}

// ジオメトリ・シェーダの関数②
[maxvertexcount(12)]
void GS_SV_2(triangle float3 input[3] : POSITION,
        inout TriangleStream<PS_INPUT_SV> TriStream) {
    PS_INPUT_SV output;

    // 法線ベクトルの計算
    float3 faceNorm  = cross(input[1] - input[0], input[2] - input[0]);

    // 光源ベクトルの計算
    float3 lightVec = Light - (input[0] + input[1] + input[2]) / 3.0;

    // 光源を向いている？
    if (dot(faceNorm, lightVec) > 0.0)
        return;

    // 各辺を引き延ばす
    for (uint i1=0; i1<3; ++i1) {
        uint i2 = (i1+1)%3, i3 = (i1+2)%3;
        // シャドウ・ボリュームを生成
        output.Pos = mul(float4(input[i1], 1.0), Projection);
        TriStream.Append(output);
        output.Pos = mul(float4(input[i3], 1.0), Projection);
        TriStream.Append(output);
        output.Pos = mul(float4(input[i1] + (input[i1] - Light) * 100, 1.0), Projection);
        TriStream.Append(output);
        output.Pos = mul(float4(input[i3] + (input[i3] - Light) * 100, 1.0), Projection);
        TriStream.Append(output);
        TriStream.RestartStrip();
    }
}

// **************************************************
// シャドウの描画
// **************************************************

// 頂点シェーダの関数
float4 VS_S(uint id : SV_VertexID) : SV_POSITION{
    switch(id) {
    case 0: return float4(-1,  1, 1, 1);
    case 1: return float4( 1,  1, 1, 1);
    case 2: return float4(-1, -1, 1, 1);
    default:
    case 3: return float4( 1, -1, 1, 1);
    }
}

// ピクセル・シェーダの関数
float4 PS_S(float4 pos : SV_POSITION) : SV_TARGET {
    return float4(0.0, 0.0, 0.0, 0.5);
}
