// 定数バッファ
cbuffer CB : register(b0)
{
    unsigned int g_iCount;  // 演算するデータ数
};

// 入力バッファ(構造化バッファ。読み込み専用)
StructuredBuffer<float> Input : register(t0);
// 出力バッファ(構造化バッファ。読み書き可能)
RWStructuredBuffer<float> Result : register(u0);

// 共有メモリ
groupshared float shared_data[128];

// シェーダ関数
[numthreads(128, 1, 1)]   // スレッド グループのスレッド数
void CS( uint3 Gid : SV_GroupID,
        uint3 DTid : SV_DispatchThreadID,
        uint3 GTid : SV_GroupThreadID,
           uint GI : SV_GroupIndex)
{
    // データ読み込み
    if (DTid.x < g_iCount)
        shared_data[GI] = Input[DTid.x];
    else
        shared_data[GI] = 0;

    // 同期を取りながら、データを加算していく
    GroupMemoryBarrierWithGroupSync();
    if (GI < 64)
        shared_data[GI] += shared_data[GI + 64];
    GroupMemoryBarrierWithGroupSync();
    if (GI < 32)
        shared_data[GI] += shared_data[GI + 32];
    GroupMemoryBarrierWithGroupSync();
    if (GI < 16)
        shared_data[GI] += shared_data[GI + 16];
    GroupMemoryBarrierWithGroupSync();
    if (GI < 8)
        shared_data[GI] += shared_data[GI + 8];
    GroupMemoryBarrierWithGroupSync();
    if (GI < 4)
        shared_data[GI] += shared_data[GI + 4];
    GroupMemoryBarrierWithGroupSync();
    if (GI < 2)
        shared_data[GI] += shared_data[GI + 2];
    GroupMemoryBarrierWithGroupSync();
    if (GI < 1)
        shared_data[GI] += shared_data[GI + 1];

    // スレッド・グループの最初のスレッドで、結果を書き出す
    if (GI == 0)
        Result[Gid.x] = shared_data[0];
}
