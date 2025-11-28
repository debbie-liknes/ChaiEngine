#pragma once
#include <ChaiEngine/PipelineState.h>
#include <OpenGLRenderer/GLHelpers.h>

namespace chai::brew
{
    class GLPipelineState
    {
    public:
        GLPipelineState() = default;

        void init()
        {
            // Disable blending by default
            glDisable(GL_BLEND);

            m_currentRasterState.cullMode = RasterizerState::CullMode::Back;
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            m_currentRasterState.frontFace = RasterizerState::FrontFace::CounterClockwise;
            glFrontFace(GL_CCW);

            m_currentDepthStencilState.depthTestEnable = true;
            glEnable(GL_DEPTH_TEST);
            m_currentDepthStencilState.depthCompareOp = DepthStencilState::CompareOp::Less;
            glDepthFunc(GL_LESS);
        }

        void updateRasterizerState(const RasterizerState& newState)
        {
            if (newState.cullMode != m_currentRasterState.cullMode) {
                if (newState.cullMode == RasterizerState::CullMode::None) {
                    glDisable(GL_CULL_FACE);
                }
                else if (m_currentRasterState.cullMode == RasterizerState::CullMode::None) {
                    glEnable(GL_CULL_FACE);
                }

                if (newState.cullMode != RasterizerState::CullMode::None)
                    glCullFace(convertCullMode(newState.cullMode));
            }

            if (newState.frontFace != m_currentRasterState.frontFace) {
                glFrontFace(convertFrontFace(newState.frontFace));
            }

            //do the others

            m_currentRasterState = newState;
        }

        void updateDepthState(const DepthStencilState& newState)
        {
            if (newState.depthCompareOp != m_currentDepthStencilState.depthCompareOp) {
                glDepthFunc(convertDepthOp(newState.depthCompareOp));
            }
        }

        RasterizerState& getRasterizerState() { return m_currentRasterState; }
        DepthStencilState& getDepthStencilState() { return m_currentDepthStencilState; }

    private:
        RasterizerState m_currentRasterState;
        DepthStencilState m_currentDepthStencilState;
    };
}