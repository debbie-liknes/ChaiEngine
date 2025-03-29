#include <Scene/RenderFrameBuilder.h>
#include <Scene/Entity.h>

namespace chai::cup
{
	//this is built every frame
	brew::RenderFrame RenderFrameBuilder::build(const Scene& scene, Viewport* vp)
	{
        brew::RenderFrame frame;

        brew::GPUCamera gpuCam;
        gpuCam.view = scene.m_cam.GetViewMatrix();

        int x, y, width, height;
        vp->getDimensions(x, y, width, height);

        gpuCam.proj = scene.m_cam.GetProjectionMatrix((float)width / (float)height);
        gpuCam.position = scene.m_cam.getPosition();

        frame.camera = gpuCam;
        frame.viewport = vp;

        for (auto& light : scene.m_lights) {
            brew::GPULight gpuLight;
            gpuLight.position = light.position;
            gpuLight.color = light.color;
            gpuLight.intensity = light.intensity;

            frame.lights.push_back(gpuLight);
        }

        for (auto& entity : scene.m_entities) {
            frame.renderables.insert(frame.renderables.end(), entity.m_testRenderables.begin(), entity.m_testRenderables.end());
        }

        return frame;
	}
}