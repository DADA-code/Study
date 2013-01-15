// �萔�o�b�t�@�̃f�[�^��`
cbuffer cbCBuffer : register(b0) { // ��ɃX���b�g�u0�v���g��
    matrix World;        // ���[���h�ϊ��s��
    matrix View;         // �r���[�ϊ��s��
    matrix Projection;   // �����ϊ��s��
    float3 Light;        // �������W(�r���[���W�n)
};

Texture2D myTex2D;   // �e�N�X�`��

// �T���v��
SamplerState texSampler : register(s0);

// **************************************************
// �����̂̕`��
// **************************************************

// �s�N�Z�� �V�F�[�_�̓��̓f�[�^��`
struct PS_INPUT {
    float4 Pos  : SV_POSITION; // ���_���W(�������W�n)
    float3 PosView  : POSVIEW; // ���_���W(�r���[���W�n)
    float3 Norm : NORMAL;      // �@���x�N�g��(���[���h���W�n)
    float2 Tex  : TEXTURE;     // �e�N�X�`�����W
};

// ���_�V�F�[�_�̊֐�
float4 VS(uint vID : SV_VertexID) : SV_POSITION {
    float4 pos;
    // ���_���W(���f�����W�n)�̐���
    static const uint pID[36] = { 0,1,3, 1,2,3, 1,5,2, 5,6,2, 5,4,6, 4,7,6,
                            4,5,0, 5,1,0, 4,0,7, 0,3,7, 3,2,7, 2,6,7 };
    switch (pID[vID]) {
    case 0: pos = float4(-1.0,  1.0, -1.0, 1.0); break;
    case 1: pos = float4( 1.0,  1.0, -1.0, 1.0); break;
    case 2: pos = float4( 1.0, -1.0, -1.0, 1.0); break;
    case 3: pos = float4(-1.0, -1.0, -1.0, 1.0); break;
    case 4: pos = float4(-1.0,  1.0,  1.0, 1.0); break;
    case 5: pos = float4( 1.0,  1.0,  1.0, 1.0); break;
    case 6: pos = float4( 1.0, -1.0,  1.0, 1.0); break;
    case 7: pos = float4(-1.0, -1.0,  1.0, 1.0); break;
    }

    // ���_���W�@���f�����W�n���r���[���W�n
    pos = mul(pos, World);
    pos = mul(pos, View);

    // �o��
    return pos;
}

// �W�I���g�� �V�F�[�_�̊֐�
[maxvertexcount(3)]
void GS(triangle float4 input[3] : SV_POSITION, uint pID : SV_PrimitiveID,
        inout TriangleStream<PS_INPUT> TriStream) {
    PS_INPUT output;

    // �e�N�X�`�����W�̌v�Z
    static const float2 tID[6] = {{-2.0, -2.0}, {3.0, -2.0}, {-2.0, 3.0},
                                  { 3.0, -2.0}, {3.0,  3.0}, {-2.0, 3.0}};
    uint tIdx = (pID & 0x01) ? 3 : 0;

    // �@���x�N�g���̌v�Z
    float3 faceEdge  = input[0].xyz / input[0].w;
    float3 faceEdgeA = (input[1].xyz / input[1].w) - faceEdge;
    float3 faceEdgeB = (input[2].xyz / input[2].w) - faceEdge;
    output.Norm = normalize(cross(faceEdgeA, faceEdgeB));

    // �e���_�̌v�Z
    for (int i=0; i<3; ++i) {
        // ���_���W�@�r���[���W�n
        output.PosView = input[i] / input[i].w;
        // ���_���W�@�r���[���W�n���ˉe���W�n
        output.Pos = mul(input[i], Projection);
        // �e�N�X�`�����W
        output.Tex = tID[tIdx + i];
        // �o��
        TriStream.Append(output);
    }
    TriStream.RestartStrip();
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

// �s�N�Z�� �V�F�[�_�̊֐�(Load�֐�)
float4 PS_Load(PS_INPUT input) : SV_TARGET
{
    // ���C�e�B���O�v�Z
    float bright = lighting(input);

    // �e�N�X�`���擾
    uint width, height;
    myTex2D.GetDimensions(width, height);           // ���A����
    float3 texCoord = float3(input.Tex.x * width, input.Tex.y * height, 0); // �~�b�v�}�b�v�E���x���u0�v
    float4 texCol = myTex2D.Load(texCoord);         // �e�N�Z���ǂݍ���

    // �F
    return saturate(bright * texCol);
}

// �s�N�Z���E�V�F�[�_�̊֐�(Sample�֐�)
float4 PS_Sample(PS_INPUT input) : SV_TARGET
{
    // ���C�e�B���O�v�Z
    float bright = lighting(input);

    // �e�N�X�`���擾
    float4 texCol = myTex2D.Sample(texSampler, input.Tex);         // �e�N�Z���ǂݍ���

    // �F
    return saturate(bright * texCol);
}
