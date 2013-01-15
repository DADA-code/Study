// �萔�o�b�t�@�̃f�[�^��`
cbuffer cbCBuffer : register(b0) { // ��ɃX���b�g�u0�v���g��
    matrix World[4];     // ���[���h�ϊ��s��
    matrix View;         // �r���[�ϊ��s��
    matrix Projection;   // �����ϊ��s��
    float4 Diffuse;      // �f�B�t���[�Y�F
    float3 Light;        // �������W(�r���[���W�n)
};

Texture2D Tex2D;     // �e�N�X�`��

// �e�N�X�`���E�T���v��
SamplerState smpWrap : register(s0);

// **************************************************
// �����̂̃C���X�^���X�`��
// **************************************************

// ���_�V�F�[�_�̓��̓f�[�^��`
struct VS_INPUT {
    float3 Pos  : POSITION;  // ���_���W(���f�����W�n)
    float3 Norm : NORMAL;    // �@���x�N�g��(���f�����W�n)
    float2 Tex  : TEXTURE;   // �e�N�X�`�����W
    matrix Mat  : MATRIX;    // ���[���h�ϊ��s��
    uint InstID : SV_InstanceID; // �C���X�^���XID
};

// �s�N�Z�� �V�F�[�_�̓��̓f�[�^��`
struct PS_INPUT {
    float4 Pos  : SV_POSITION; // ���_���W(�������W�n)
    float3 PosView  : POSVIEW; // ���_���W(�r���[���W�n)
    float3 Norm : NORMAL;      // �@���x�N�g��(�r���[���W�n)
    float2 Tex  : TEXTURE;     // �e�N�X�`�����W
};

// ���_�V�F�[�_�̊֐�
PS_INPUT VS(VS_INPUT input) {
    PS_INPUT output;
    uint instNumber = input.InstID % 4;

    // ���_���W�@���f�����W�n���������W�n
    float4 pos4 = float4(input.Pos, 1.0);
    pos4 = mul(pos4, input.Mat);
    pos4 = mul(pos4, World[instNumber]);
    pos4 = mul(pos4, View);
    output.PosView = pos4.xyz / pos4.w;
    output.Pos = mul(pos4, Projection);

    // �@���x�N�g���@���f�����W�n���r���[���W�n
    float3 Norm = mul(input.Norm, (float3x3)World[instNumber]);
    output.Norm = mul(Norm, (float3x3)View);

    // �e�N�X�`�����W
    output.Tex = input.Tex;

    // �o��
    return output;
}

// ���C�e�B���O�v�Z
float lighting(PS_INPUT input)
{
    // �����x�N�g��
    float3 light = Light - input.PosView;
    // ����
    float  leng = length(light);
    // ���邳
    return 30 * dot(normalize(light), normalize(input.Norm)) / pow(leng, 2);
}

// �s�N�Z�� �V�F�[�_�̊֐�
float4 PS(PS_INPUT input) : SV_TARGET
{
    // ���C�e�B���O�v�Z
    float bright = lighting(input);

    // �e�N�X�`���擾
    float4 texCol = Tex2D.Sample(smpWrap, input.Tex);         // �e�N�Z���ǂݍ���

    // �F
    return saturate(bright * texCol * Diffuse);
}
