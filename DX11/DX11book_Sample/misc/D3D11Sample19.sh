// �萔�o�b�t�@
cbuffer CB : register(b0)
{
    unsigned int g_iCount;  // ���Z����f�[�^��
};

// ���̓o�b�t�@(�\�����o�b�t�@�B�ǂݍ��ݐ�p)
StructuredBuffer<float> Input : register(t0);
// �o�̓o�b�t�@(�\�����o�b�t�@�B�ǂݏ����\)
RWStructuredBuffer<float> Result : register(u0);

// ���L������
groupshared float shared_data[128];

// �V�F�[�_�֐�
[numthreads(128, 1, 1)]   // �X���b�h �O���[�v�̃X���b�h��
void CS( uint3 Gid : SV_GroupID,
        uint3 DTid : SV_DispatchThreadID,
        uint3 GTid : SV_GroupThreadID,
           uint GI : SV_GroupIndex)
{
    // �f�[�^�ǂݍ���
    if (DTid.x < g_iCount)
        shared_data[GI] = Input[DTid.x];
    else
        shared_data[GI] = 0;

    // ���������Ȃ���A�f�[�^�����Z���Ă���
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

    // �X���b�h�E�O���[�v�̍ŏ��̃X���b�h�ŁA���ʂ������o��
    if (GI == 0)
        Result[Gid.x] = shared_data[0];
}
