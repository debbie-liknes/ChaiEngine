#include <Scene/RenderFrameBuilder.h>
#include <Scene/Entity.h>

namespace chai::cup
{
	//this is built every frame
	brew::RenderFrame RenderFrameBuilder::build(const Scene& scene, Viewport* vp)
	{
        brew::RenderFrame frame;

        brew::GPUCamera gpuCam;
        gpuCam.forward = scene.m_cam.getDirection();
        gpuCam.up = scene.m_cam.getUp();
        gpuCam.position = scene.m_cam.getPosition();

        //gpuCam.view = scene.m_cam.GetViewMatrix();
        gpuCam.view = scene.m_cam.getCoordinateSpace().GetViewMatrix(gpuCam);

        int x, y, width, height;
        vp->getDimensions(x, y, width, height);

        gpuCam.proj = scene.m_cam.GetProjectionMatrix((float)width / (float)height);
        

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
            //make a temp copy, since im doing coordinate transforms here
            for (auto& ro : entity.m_testRenderables)
            {
                brew::Renderable temp = *ro;
                temp.setPosition(scene.m_cam.getCoordinateSpace().ToWorld(ro->getPosition()));
                frame.renderables.push_back(std::make_shared<brew::Renderable>(temp));
            }
            //frame.renderables.insert(frame.renderables.end(), entity.m_testRenderables.begin(), entity.m_testRenderables.end());
        }

        return frame;
	}
}