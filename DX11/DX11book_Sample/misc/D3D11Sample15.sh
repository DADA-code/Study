// �萔�o�b�t�@
matrix World[4];     // ���[���h�ϊ��s��
matrix View[6];      // �r���[�ϊ��s��
matrix Projection;   // �����ϊ��s��

float4 Diffuse;      // �f�B�t���[�Y�F
float4 Light[6];     // �������W(�r���[���W�n)

Texture2D   Tex2D   : register(t0); // �e�N�X�`��
TextureCube TexCube : register(t1); // �e�N�X�`��(���}�b�v)

static float3 ViewPos = float3(0.0f, 15.0, -15.0); // ���_(���[���h���W�n)

// �e�N�X�`���E�T���v��
SamplerState smpWrap : register(s0);

// **************************************************
// �����̂̃C���X�^���X�`��
// **************************************************

// ���_�V�F�[�_�̓��̓f�[�^��`
struct VS_INPUT_CUBE_INSTANCE {
    float3 Pos  : POSITION;      // ���_���W(���f�����W�n)
    float3 Norm : NORMAL;        // �@���x�N�g��(���f�����W�n)
    float2 Tex  : TEXTURE;       // �e�N�X�`�����W
    matrix Mat  : MATRIX;        // ���[���h�ϊ��s��
    uint InstID : SV_InstanceID; // �C���X�^���XID
};

// �s�N�Z�� �V�F�[�_�̓��̓f�[�^��`
struct PS_INPUT {
    float4 Pos  : SV_POSITION;  // ���_���W(�������W�n)
    float3 PosView  : POSVIEW;  // ���_���W(�r���[���W�n)
    float3 Norm : NORMAL;       // �@���x�N�g��(�r���[���W�n)
    float2 Tex  : TEXTURE;      // �e�N�X�`�����W
};

// ���_�V�F�[�_�̊֐�
PS_INPUT VS_CUBE_INSTANCE(VS_INPUT_CUBE_INSTANCE input) {
    PS_INPUT output;
    uint instNumber = input.InstID % 4;

    // ���_���W�@���f�����W�n���������W�n
    float4 pos4 = float4(input.Pos, 1.0);
    pos4 = mul(pos4, input.Mat);
    pos4 = mul(pos4, World[instNumber]);
    pos4 = mul(pos4, View[0]);
    output.PosView = pos4.xyz / pos4.w;
    output.Pos = mul(pos4, Projection);

    // �@���x�N�g���@���f�����W�n���r���[���W�n
    float3 Norm = mul(input.Norm, (float3x3)World[instNumber]);
    output.Norm = mul(Norm, (float3x3)View[0]);

    // �e�N�X�`�����W
    output.Tex = input.Tex;

    // �o��
    return output;
}

// ���C�e�B���O�v�Z
float lighting(float3 PosView, float3 Norm, float3 L)
{
    // �����x�N�g��
    float3 light = L - PosView;
    // ����
    float  leng = length(light);
    // ���邳
    return 30 * dot(normalize(light), normalize(Norm)) / pow(leng, 2);
}

// �s�N�Z�� �V�F�[�_�̊֐�
float4 PS_CUBE_INSTANCE(PS_INPUT input) : SV_TARGET
{
    // ���C�e�B���O�v�Z
    float bright = lighting(input.PosView, input.Norm, Light[0].xyz);

    // �e�N�X�`���擾
    float4 texCol = Tex2D.Sample(smpWrap, input.Tex);         // �e�N�Z���ǂݍ���

    // �F
    return saturate(bright * texCol * Diffuse);
}

// **************************************************
// ���̕`��
// **************************************************

// ���_�V�F�[�_�̓��̓f�[�^��`
struct VS_INPUT_SPHERE {
    float3 Pos  : POSITION;  // ���_���W(���f�����W�n)
    float3 Norm : NORMAL;    // �@���x�N�g��(���f�����W�n)
    float2 Tex  : TEXTURE;   // �e�N�X�`�����W
};

// �s�N�Z�� �V�F�[�_�̓��̓f�[�^��`
struct PS_INPUT_SPHERE {
    float4 Pos  : SV_POSITION;  // ���_���W(�������W�n)
    float3 PosView  : POSVIEW;  // ���_���W(�r���[���W�n)
    float3 Norm : NORMAL;       // �@���x�N�g��(�r���[���W�n)
    float2 Tex  : TEXTURE;      // �e�N�X�`�����W
    float3 ViewWorld : POSWORLD;  // ���_���W�x�N�g��(���[���h���W�n)
    float3 NormWorld : NORMWORLD; // �@���x�N�g��(���[���h���W�n)
};

// ���_�V�F�[�_�̊֐�(��)
PS_INPUT_SPHERE VS_SPHERE(VS_INPUT_SPHERE input) {
    PS_INPUT_SPHERE output;

    // ���_���W�@���f�����W�n���������W�n
    float4 pos4 = float4(input.Pos, 1.0);
    pos4 = mul(pos4, View[0]);
    output.PosView = pos4.xyz / pos4.w;
    output.Pos = mul(pos4, Projection);

    // �@���x�N�g���@���f�����W�n���r���[���W�n
    output.Norm = mul(input.Norm, (float3x3)View[0]);

    // �e�N�X�`�����W
    output.Tex = input.Tex;

    // �L���[�u�E�e�N�X�`���̃A�N�Z�X�p
    output.ViewWorld  = input.Pos - ViewPos;
    output.NormWorld = input.Norm;

    // �o��
    return output;
}

// �s�N�Z�� �V�F�[�_�̊֐�
float4 PS_SPHERE(PS_INPUT_SPHERE input) : SV_TARGET
{
    // ���C�e�B���O�v�Z
    float bright = lighting(input.PosView, input.Norm, Light[0].xyz);

    // �e�N�X�`���擾
    float4 texCol = Tex2D.Sample(smpWrap, input.Tex);         // �e�N�Z���ǂݍ���

    // ���}�b�v�v�Z
    float3 E = normalize(input.ViewWorld); // �����x�N�g��
    float3 N = normalize(input.NormWorld); // �@���x�N�g��
    float3 R = reflect(E, N);              // ���˃x�N�g��
    float4 envMap = TexCube.Sample(smpWrap, R); // �L���[�u�E�e�N�X�`���ɃA�N�Z�X

    // �F
    return saturate(envMap + 0.25 * bright * texCol * Diffuse);
}

// **************************************************
// ���}�b�v�̕`��
// **************************************************

// ���_�V�F�[�_�̓��̓f�[�^��`
// (�����̂̃C���X�^���X�`��Ɠ���)

// �W�I���g�� �V�F�[�_�̓��̓f�[�^��`
struct GS_INPUT_ENVMAP {
    float4 Pos  : SV_POSITION;  // ���_���W(���[���h���W�n)
    float3 Norm : NORMAL;       // �@���x�N�g��(���[���h���W�n)
    float2 Tex  : TEXTURE;      // �e�N�X�`�����W
};

// �s�N�Z�� �V�F�[�_�̓��̓f�[�^��`
struct PS_INPUT_ENVMAP {
    float4 Pos     : SV_POSITION;     // ���_���W(�ˉe���W�n)
    float3 PosView : POSVIEW;         // ���_���W(�r���[���W�n)
    float3 Norm    : NORMAL;          // �@���x�N�g��(�r���[���W�n)
    float2 Tex     : TEXTURE;         // �e�N�X�`�����W
    uint RTIndex   : SV_RenderTargetArrayIndex; // �`��^�[�Q�b�g
};

// ���_�V�F�[�_�̊֐�
GS_INPUT_ENVMAP VS_ENVMAP(VS_INPUT_CUBE_INSTANCE input) {
    GS_INPUT_ENVMAP output;
    uint instNumber = input.InstID % 4;

    // ���_���W�@���f�����W�n�����[���h���W�n
    float4 pos4 = float4(input.Pos, 1.0);
    pos4 = mul(pos4, input.Mat);
    output.Pos = mul(pos4, World[instNumber]);

    // �@���x�N�g���@���f�����W�n�����[���h���W�n
    float3 norm = mul(input.Norm, (float3x3)input.Mat);
    output.Norm = mul(norm, (float3x3)World[instNumber]);

    // �e�N�X�`�����W
    output.Tex = input.Tex;

    // �o��
    return output;
}

// �W�I���g�� �V�F�[�_�̊֐�
[maxvertexcount(18)]
void GS_ENVMAP(triangle GS_INPUT_ENVMAP In[3],
                inout TriangleStream<PS_INPUT_ENVMAP> EnvMapStream)
{
    // 6�ʂ��v�Z����
    for (int f = 0; f < 6; ++f)
    {
        PS_INPUT_ENVMAP Out;
        Out.RTIndex = f;  // ���̃v���~�e�B�u��`�悷��`��^�[�Q�b�g
        // �e�ʖ��ɒ��_���W���v�Z���ĕ`����ݒ�
        for (int v = 0; v < 3; v++)
        {
            // ���_���W�@���[���h���W�n���r���[���W�n
            Out.Pos = mul(In[v].Pos, View[f]);
            Out.PosView = Out.Pos;
            // ���_���W�@�r���[���W�n���ˉe���W�n
            Out.Pos = mul(Out.Pos, Projection);
            // �@���x�N�g���@���[���h���W�n���r���[���W�n
            Out.Norm = mul(In[v].Norm, (float3x3)View[f]);
            // �e�N�X�`�����W
            Out.Tex     = In[v].Tex;
            // ���_�o��
            EnvMapStream.Append(Out); // ���_�o��
        }
        // 1�X�g���b�v�I��
        EnvMapStream.RestartStrip();
    }
}

// �s�N�Z�� �V�F�[�_�̊֐�
float4 PS_ENVMAP(PS_INPUT_ENVMAP input) : SV_TARGET {
    // ���C�e�B���O�v�Z
    float bright = lighting(input.PosView, input.Norm, Light[input.RTIndex].xyz);

    // �e�N�X�`���擾
    float4 texCol = Tex2D.Sample(smpWrap, input.Tex);         // �e�N�Z���ǂݍ���

    // �F
    return saturate(bright * texCol * Diffuse);
}
