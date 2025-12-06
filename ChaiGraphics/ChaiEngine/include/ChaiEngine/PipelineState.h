#pragma once
#include <cstdint>

namespace chai
{
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

    struct DepthStencilState
    {
        bool depthTestEnable = true;
        bool depthMaskEnable = true;
        enum class CompareOp { Never, Less, Equal, LessEqual, Greater, NotEqual, GreaterEqual, Always };
        CompareOp depthCompareOp = CompareOp::Less;
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

    struct PipelineState
    {
        RasterizerState rasterState;
        DepthStencilState depthStencilState;
        BlendState blendState;
    };
}