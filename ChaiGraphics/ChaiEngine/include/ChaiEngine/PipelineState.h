#pragma once

namespace chai
{
    struct DepthStencilState
    {
        bool depthTestEnable = true;
        bool depthWriteEnable = true;
        enum class CompareOp { Never, Less, Equal, LessEqual, Greater, NotEqual, GreaterEqual, Always };
        CompareOp depthCompareOp = CompareOp::Less;

        //Stencil
        bool stencilEnable = false;
        uint8_t stencilReadMask = 0xFF;
        uint8_t stencilWriteMask = 0xFF;
    };

    struct RasterizerState
    {
        enum class CullMode { None, Front, Back };
        enum class FrontFace { CounterClockwise, Clockwise };
        enum class FillMode { Solid, Wireframe, Point };

        CullMode cullMode = CullMode::Back;
        FrontFace frontFace = FrontFace::CounterClockwise;
        FillMode fillMode = FillMode::Solid;

        //Depth bias
        float depthBiasConstant = 0.0f;
        float depthBiasSlope = 0.0f;
        bool depthBiasEnable = false;

        //Extras
        bool depthClampEnable = false;
        float lineWidth = 1.0f;
    };

    struct BlendState
    {
        enum class BlendFactor 
        {
            Zero, One,
            SrcColor, OneMinusSrcColor,
            DstColor, OneMinusDstColor,
            SrcAlpha, OneMinusSrcAlpha,
            DstAlpha, OneMinusDstAlpha,
            ConstantColor, OneMinusConstantColor
        };

        enum class BlendOp { Add, Subtract, ReverseSubtract, Min, Max };

        enum ColorWriteMask 
        {
            None = 0,
            Red = 1 << 0,
            Green = 1 << 1,
            Blue = 1 << 2,
            Alpha = 1 << 3,
            RGB = Red | Green | Blue,
            All = Red | Green | Blue | Alpha
        };

        bool blendEnable = false;

        BlendFactor srcColorBlendFactor = BlendFactor::One;
        BlendFactor dstColorBlendFactor = BlendFactor::Zero;
        BlendOp colorBlendOp = BlendOp::Add;

        BlendFactor srcAlphaBlendFactor = BlendFactor::One;
        BlendFactor dstAlphaBlendFactor = BlendFactor::Zero;
        BlendOp alphaBlendOp = BlendOp::Add;

        ColorWriteMask colorWriteMask = ColorWriteMask::All;

        static BlendState opaque() 
        {
            return BlendState{};
        }

        static BlendState alphaBlend() 
        {
            BlendState state;
            state.blendEnable = true;
            state.srcColorBlendFactor = BlendFactor::SrcAlpha;
            state.dstColorBlendFactor = BlendFactor::OneMinusSrcAlpha;
            state.srcAlphaBlendFactor = BlendFactor::One;
            state.dstAlphaBlendFactor = BlendFactor::OneMinusSrcAlpha;
            return state;
        }

        static BlendState additive() 
        {
            BlendState state;
            state.blendEnable = true;
            state.srcColorBlendFactor = BlendFactor::One;
            state.dstColorBlendFactor = BlendFactor::One;
            return state;
        }
    };

    struct ShaderStage
    {
        enum class Type { Vertex, Fragment, Geometry, Compute, TessControl, TessEval };

        Type type;
        std::string path;  //File path to shader
        std::string sourceCode;  //inline source
        std::string entryPoint = "main";
        std::vector<std::string> defines;

        bool isValid() const 
        {
            return !path.empty() || !sourceCode.empty();
        }
    };

    struct VertexInputBinding
    {
        enum class InputRate { PerVertex, PerInstance };

        uint32_t binding;
        uint32_t stride;
        InputRate inputRate = InputRate::PerVertex;
    };

    struct VertexInputAttribute
    {
        enum class Format 
        {
            Float, Float2, Float3, Float4,
            Int, Int2, Int3, Int4,
            UInt, UInt2, UInt3, UInt4
        };

        uint32_t location;
        uint32_t binding;
        Format format;
        uint32_t offset;
    };

    struct VertexLayout
    {
        std::vector<VertexInputBinding> bindings;
        std::vector<VertexInputAttribute> attributes;

        // Helper for common layouts
        static VertexLayout positionNormalUV() 
        {
            VertexLayout layout;
            layout.bindings = { {0, sizeof(float) * 8, VertexInputBinding::InputRate::PerVertex} };
            layout.attributes = 
            {
                {0, 0, VertexInputAttribute::Format::Float3, 0},   // position
                {1, 0, VertexInputAttribute::Format::Float3, 12},  // normal
                {2, 0, VertexInputAttribute::Format::Float2, 24}   // uv
            };
            return layout;
        }
    };

    struct PipelineDesc
    {
        enum class PrimitiveTopology 
        {
            PointList,
            LineList, LineStrip,
            TriangleList, TriangleStrip, TriangleFan
        };

        // Shader stages
        std::vector<ShaderStage> shaderStages;
        std::vector<std::string> globalDefines;  // Applied to all stages

        // Pipeline state
        VertexLayout vertexLayout;
        PrimitiveTopology topology = PrimitiveTopology::TriangleList;
        RasterizerState rasterizer;
        DepthStencilState depthStencil;
        BlendState blend;

        // Render pass compatibility (important for Vulkan!)
        struct RenderPassInfo 
        {
            uint32_t colorAttachmentCount = 1;
            bool hasDepthAttachment = true;
            // Could add formats here for full validation
        } renderPassInfo;

        // Generate a hash for pipeline caching
        size_t hash() const 
        {
            // Hash all state - critical for deduplication
            size_t h = 0;
            // ... hash implementation
            return h;
        }
    };

    class PipelineState
    {
    public:
        uint32_t getPipelineId() const { return m_pipelineId; }

    private:
        uint32_t m_pipelineId;
        PipelineDesc m_desc;

        friend class PipelineCache;
    };
}