matrix World;           // ���[���h�ϊ��s��
matrix View;            // �r���[�ϊ��s��
matrix Projection;      // �����ϊ��s��
matrix SMWorldViewProj; // ���[���h�~�r���[�~�����ϊ��s��(�V���h�E�}�b�v�p)
float4 Diffuse;         // �f�B�t���[�Y�F
float3 Light;           // �������W(�r���[���W�n)


// �e�N�X�`��
Texture2D Tex2D : register(t0);
Texture2D ShadowMap : register(t1); // �V���h�E�}�b�v

// �e�N�X�`���E�T���v��
SamplerState smpWrap   : register(s0);
SamplerState smpBorder : register(s1);

// **************************************************
// 3D�I�u�W�F�N�g�̕`��
// **************************************************

// ���_�V�F�[�_�̓��̓f�[�^��`
struct VS_INPUT {
    float3 Pos  : POSITION;  // ���_���W(���f�����W�n)
    float3 Norm : NORMAL;    // �@���x�N�g��(���f�����W�n)
    float2 Tex  : TEXTURE;   // �e�N�X�`�����W
};

// �s�N�Z�� �V�F�[�_�̓��̓f�[�^��`
struct PS_INPUT {
    float4 Pos  : SV_POSITION; // ���_���W(�������W�n)
    float3 PosView  : POSVIEW; // ���_���W(�r���[���W�n)
    float3 Norm : NORMAL;      // �@���x�N�g��(�r���[���W�n)
    float2 Tex  : TEXTURE;     // �e�N�X�`�����W
    float3 PosSM: POSITION_SM; // ���_���W(�V���h�E�}�b�v�̓������W�n)
};

// ���_�V�F�[�_�̊֐�
PS_INPUT VS(VS_INPUT input) {
    PS_INPUT output;

    // ���_���W�@���f�����W�n���������W�n
    float4 pos4 = float4(input.Pos, 1.0);
    pos4 = mul(pos4, World);
    pos4 = mul(pos4, View);
    output.PosView = pos4.xyz / pos4.w;
    output.Pos = mul(pos4, Projection);

    // �@���x�N�g���@���f�����W�n���r���[���W�n
    float3 Norm = mul(input.Norm, (float3x3)World);
    output.Norm = mul(Norm, (float3x3)View);

    // �e�N�X�`�����W
    output.Tex = input.Tex;

    // ���_���W�@���f�����W�n���������W�n(�V���h�E�}�b�v)
    pos4 = mul(float4(input.Pos, 1.0), SMWorldViewProj);
    pos4.xyz = pos4.xyz / pos4.w;
    output.PosSM.x = (pos4.x + 1.0) / 2.0;
    output.PosSM.y = (-pos4.y + 1.0) / 2.0;
    output.PosSM.z = pos4.z;

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

    // �V���h�E�}�b�v
    float sm = ShadowMap.Sample(smpBorder, input.PosSM.xy);
    float sma = (input.PosSM.z < sm) ? 1.0 : 0.5;

    // �F
    return saturate(bright * texCol * Diffuse * sma);
}

// **************************************************
// �V���h�E�}�b�v���g��Ȃ�3D�I�u�W�F�N�g�̕`��
// **************************************************

// �s�N�Z�� �V�F�[�_�̊֐�
float4 PS_NOSM(PS_INPUT input) : SV_TARGET
{
    // ���C�e�B���O�v�Z
    float bright = lighting(input);

    // �e�N�X�`���擾
    float4 texCol = Tex2D.Sample(smpWrap, input.Tex);         // �e�N�Z���ǂݍ���

    // �F
    return saturate(bright * texCol * Diffuse);
}

// **************************************************
// �V���h�E�}�b�v�̕`��
// **************************************************

// ���_�V�F�[�_�̊֐�
float4 VS_SM(VS_INPUT input) : SV_POSITION {
    // ���_���W�@���f�����W�n���������W�n
    float4 pos4 = float4(input.Pos, 1.0);
    pos4 = mul(pos4, World);
    pos4 = mul(pos4, View);
    return mul(pos4, Projection);
}

