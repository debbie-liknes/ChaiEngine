#pragma once
#include <glad/gl.h>
#include <ChaiEngine/PipelineState.h>
#include <ChaiEngine/Material.h>
#include <ChaiEngine/UniformBuffer.h>

namespace chai::brew
{
	void buildRasterizer(const RasterizerState& state)
	{
		//Cull Mode
		if(state.cullMode == RasterizerState::CullMode::None)
			glDisable(GL_CULL_FACE);
		else
		{
			glEnable(GL_CULL_FACE);
			if(state.cullMode == RasterizerState::CullMode::Front)
				glCullFace(GL_FRONT);
			else if(state.cullMode == RasterizerState::CullMode::Back)
				glCullFace(GL_BACK);
		}
	}

	void buildBlendState(const BlendState& state)
	{
		//Blending
		//if(state.blendEnabled)
		//	glEnable(GL_BLEND);
		//else
		//	glDisable(GL_BLEND);


  //      if(state.blendEnabled)
  //          glEnable(GL_DEPTH_TEST);
  //      else
		//	glDisable(GL_DEPTH_TEST);

  //      if(state.cullFaceEnabled)
  //          glEnable(GL_CULL_FACE);
		//else
		//	glDisable(GL_CULL_FACE);

        //glDepthFunc(GL_LESS);

        //glCullFace(GL_BACK);
        //glFrontFace(GL_CCW);

        //// Enable blending for transparency
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}