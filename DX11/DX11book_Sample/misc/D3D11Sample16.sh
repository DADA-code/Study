matrix View;         // �r���[�ϊ��s��
matrix Projection;   // �����ϊ��s��

float2        ParticleSize;  // �p�[�e�B�N���̑傫��
float         Time;          // �o�ߎ���(�b)
static float3 Acceleration = float3(0.0, -9.8, 0.0);  // �����x(m/s^2)

Texture2D     Tex2D;         // �e�N�X�`��

// �e�N�X�`���E�T���v��
SamplerState smpWrap : register(s0);

// **************************************************
// �p�[�e�B�N�����W�̌v�Z����
// **************************************************

// �W�I���g���E�V�F�[�_�̓��̓f�[�^��`
struct GS_INPUT_PARTICLE {
    float3 Position      : POSITION;     // ���_���W(���[���h���W�n)
    float3 Velocity      : NORMAL;       // ���݂̑��x�x�N�g��(���[���h���W�n)
    float3 VelocityFirst : TEXCOORD0;    // �ŏ��̑��x�x�N�g��(���[���h���W�n)
};

// ���_�V�F�[�_
GS_INPUT_PARTICLE VS_PARTICLE_VERTEX(GS_INPUT_PARTICLE In)
{
    return In;
}

// �W�I���g���E�V�F�[�_�̊֐�
[maxvertexcount(1)]
void GS_PARTICLE_VERTEX(point GS_INPUT_PARTICLE In[1],
                inout PointStream<GS_INPUT_PARTICLE> ParticleStream)
{
    GS_INPUT_PARTICLE Out;
    // �ŏ��̑��x�͕ω����Ȃ�
    Out.VelocityFirst = In[0].VelocityFirst;
    // Time�b��̑��x
    Out.Velocity = Acceleration * Time + In[0].Velocity;
    // Time�b��̈ʒu
    Out.Position = 0.5 * Acceleration * Time * Time + In[0].Velocity * Time + In[0].Position;
    if (Out.Position.y < 0) {
        // �p�[�e�B�N���������ɗ������̂ŁA�V�����n�߂�
        Out.Position = float3(0.0, 0.1, 0.0);
        Out.Velocity = In[0].VelocityFirst;
    }
    // �v�Z���o��
    ParticleStream.Append(Out);
}

// **************************************************
// �p�[�e�B�N���̕`�揈��
// **************************************************

// �s�N�Z���E�V�F�[�_�̓��̓f�[�^��`
struct PS_INPUT {
    float4 Pos  : SV_Position;     // ���_���W(�ˉe���W�n)
    float2 Tex  : TEXTURE;         // �e�N�X�`�����W
};

// ���_�V�F�[�_�̊֐�
GS_INPUT_PARTICLE VS_PARTICLE(GS_INPUT_PARTICLE In)
{
    return In;
}

// �W�I���g���E�V�F�[�_�̊֐�
[maxvertexcount(4)]
void GS_PARTICLE(point GS_INPUT_PARTICLE In[1],
                inout TriangleStream<PS_INPUT> ParticleStream)
{
    // ���W�ϊ��@���[���h���W�n�@���@�r���[���W�n
    float4 pos = mul(float4(In[0].Position, 1.0), View);

    // �_��ʂɂ���
    float4 posLT = pos + float4(-ParticleSize.x,  ParticleSize.y, 0.0, 0.0) * pos.w;
    float4 posLB = pos + float4(-ParticleSize.x, -ParticleSize.y, 0.0, 0.0) * pos.w;
    float4 posRT = pos + float4( ParticleSize.x,  ParticleSize.y, 0.0, 0.0) * pos.w;
    float4 posRB = pos + float4( ParticleSize.x, -ParticleSize.y, 0.0, 0.0) * pos.w;

    // ����̓_�̈ʒu(�ˉe���W�n)���v�Z���ďo��
    PS_INPUT Out;
    Out.Pos = mul(posLT, Projection);
    Out.Tex = float2(0.0, 0.0);
    ParticleStream.Append(Out);

    // �E��̓_�̈ʒu(�ˉe���W�n)���v�Z���ďo��
    Out.Pos = mul(posRT, Projection);
    Out.Tex = float2(1.0, 0.0);
    ParticleStream.Append(Out);

    // �����̓_�̈ʒu(�ˉe���W�n)���v�Z���ďo��
    PS_INPUT Out1;
    Out1.Pos = mul(posLB, Projection);
    Out1.Tex = float2(0.0, 1.0);
    ParticleStream.Append(Out1);

    // �E���̓_�̈ʒu(�ˉe���W�n)���v�Z���ďo��
    Out.Pos = mul(posRB, Projection);
    Out.Tex = float2(1.0, 1.0);
    ParticleStream.Append(Out);

    ParticleStream.RestartStrip();
}

// �s�N�Z���E�V�F�[�_�̊֐�
float4 PS_PARTICLE(PS_INPUT input) : SV_Target
{
    return Tex2D.Sample(smpWrap, input.Tex);
}

// **************************************************
// ���̕`�揈��
// **************************************************

// ���_�V�F�[�_�̊֐�
PS_INPUT VS_FLOOR(uint vid : SV_VertexID)
{
    PS_INPUT output = (PS_INPUT)0;
    // ���̒��_���W�𐶐�
    switch(vid) {
    case 0:
    case 5:
        // ����
        output.Pos = float4(-2, 0, -2, 1); output.Tex = float2(0, 1); break;
    case 1:
        // ����
        output.Pos = float4(-2, 0,  2, 1); output.Tex = float2(0, 0); break;
    case 2:
    case 3:
        // �E��
        output.Pos = float4( 2, 0,  2, 1); output.Tex = float2(1, 0); break;
    case 4:
        // �E��
        output.Pos = float4( 2, 0, -2, 1); output.Tex = float2(1, 1); break;
    }
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    return output;
}

// �s�N�Z���E�V�F�[�_�̊֐�
float4 PS_FLOOR(PS_INPUT input) : SV_Target
{
    return Tex2D.Sample(smpWrap, input.Tex) * 0.5;
}
