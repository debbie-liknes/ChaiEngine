#include <ChaiEngine/RenderCommandCollector.h>

namespace chai::brew
{
	void RenderCommandCollector::submit(RenderCommand& cmd)
	{
		cmd.viewMatrix = camera->getViewMatrix();
		cmd.projectionMatrix = camera->getProjectionMatrix();
		commands.push_back(cmd);
	}
	const std::vector<RenderCommand>& RenderCommandCollector::getCommands() const
	{
		return commands;
	}
	void RenderCommandCollector::setCamera(ICamera* cam)
	{
		camera = cam;
	}
	void RenderCommandCollector::setViewport(IViewport* vp)
	{
		viewport = vp;
	}
}