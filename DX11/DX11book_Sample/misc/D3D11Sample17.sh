matrix World;        // ���[���h�ϊ��s��
matrix View;         // �r���[�ϊ��s��
matrix Projection;   // �����ϊ��s��
float4 Diffuse;      // �f�B�t���[�Y�F
float3 Light;        // �������W(�r���[���W�n)

Texture2D Tex2D;     // �e�N�X�`��

// �e�N�X�`���E�T���v��
SamplerState smpWrap : register(s0);

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

// **************************************************
// �V���h�E�E�{�����[���̕`��
// **************************************************

struct GS_INPUT_SV {
    float3 Pos : SV_POSITION;
};

struct PS_INPUT_SV {
    float4 Pos : SV_POSITION;
};

// ���_�V�F�[�_�̊֐�
float3 VS_SV(VS_INPUT input) : POSITION {
    // ���_���W�@���f�����W�n���r���[���W�n
    float4 Pos = float4(input.Pos, 1.0);
    Pos = mul(Pos, World);
    Pos = mul(Pos, View);
    return Pos.xyz / Pos.w;
}

// �W�I���g���E�V�F�[�_�̊֐��@
[maxvertexcount(12)]
void GS_SV_1(triangleadj float3 input[6] : POSITION,
        inout TriangleStream<PS_INPUT_SV> TriStream) {
    PS_INPUT_SV output = (PS_INPUT_SV)0;

    // �@���x�N�g���̌v�Z
    float3 faceNorm  = normalize(cross(input[2] - input[0], input[4] - input[0]));

    // �����x�N�g���̌v�Z
    float3 lightVec = Light - (input[0] + input[2] + input[4]) / 3.0;

    // �����������Ă���H
    if (dot(faceNorm, lightVec) >= 0.0)
        return;

/* *********************************************
    // �ȉ��̃R�[�h�͂Ȃ������������삵�Ȃ�(GeForce 8800 GTX)�B[2007/6/21]
    // �e�ӂ̔���
    for (uint iA=0; iA<6; iA+=2) {
        uint iB = iA+1, iC = (iA+2) % 6;
        // �����x�N�g���̌v�Z
        lightVec = Light - (input[iA] + input[iB] + input[iC]) / 3.0;
        // �אږʂ̖@���x�N�g���̌v�Z
        faceNorm = cross(input[iB] - input[iA], input[iC] - input[iA]);
        // ���E����
        if (dot(faceNorm, lightVec) >= 0.0) {
            // �V���h�E�E�{�����[���𐶐�
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
    // �ȏ�̃R�[�h�����������삵�Ȃ��̂ŁA�ȉ���for���[�v��W�J����B
    uint iA = 0; {
        uint iB = iA+1, iC = (iA+2) % 6;
        // �����x�N�g���̌v�Z
        lightVec = Light - (input[iA] + input[iB] + input[iC]) / 3.0;
        // �אږʂ̖@���x�N�g���̌v�Z
        faceNorm = cross(input[iB] - input[iA], input[iC] - input[iA]);
        // ���E����
        if (dot(faceNorm, lightVec) >= 0.0) {
            // �V���h�E�E�{�����[���𐶐�
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
        // �����x�N�g���̌v�Z
        lightVec = Light - (input[iA] + input[iB] + input[iC]) / 3.0;
        // �אږʂ̖@���x�N�g���̌v�Z
        faceNorm = cross(input[iB] - input[iA], input[iC] - input[iA]);
        // ���E����
        if (dot(faceNorm, lightVec) >= 0.0) {
            // �V���h�E�E�{�����[���𐶐�
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
        // �����x�N�g���̌v�Z
        lightVec = Light - (input[iA] + input[iB] + input[iC]) / 3.0;
        // �אږʂ̖@���x�N�g���̌v�Z
        faceNorm = cross(input[iB] - input[iA], input[iC] - input[iA]);
        // ���E����
        if (dot(faceNorm, lightVec) >= 0.0) {
            // �V���h�E�E�{�����[���𐶐�
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

// �W�I���g���E�V�F�[�_�̊֐��A
[maxvertexcount(12)]
void GS_SV_2(triangle float3 input[3] : POSITION,
        inout TriangleStream<PS_INPUT_SV> TriStream) {
    PS_INPUT_SV output;

    // �@���x�N�g���̌v�Z
    float3 faceNorm  = cross(input[1] - input[0], input[2] - input[0]);

    // �����x�N�g���̌v�Z
    float3 lightVec = Light - (input[0] + input[1] + input[2]) / 3.0;

    // �����������Ă���H
    if (dot(faceNorm, lightVec) > 0.0)
        return;

    // �e�ӂ��������΂�
    for (uint i1=0; i1<3; ++i1) {
        uint i2 = (i1+1)%3, i3 = (i1+2)%3;
        // �V���h�E�E�{�����[���𐶐�
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
// �V���h�E�̕`��
// **************************************************

// ���_�V�F�[�_�̊֐�
float4 VS_S(uint id : SV_VertexID) : SV_POSITION{
    switch(id) {
    case 0: return float4(-1,  1, 1, 1);
    case 1: return float4( 1,  1, 1, 1);
    case 2: return float4(-1, -1, 1, 1);
    default:
    case 3: return float4( 1, -1, 1, 1);
    }
}

// �s�N�Z���E�V�F�[�_�̊֐�
float4 PS_S(float4 pos : SV_POSITION) : SV_TARGET {
    return float4(0.0, 0.0, 0.0, 0.5);
}
