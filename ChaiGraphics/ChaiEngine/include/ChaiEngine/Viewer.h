#pragma once
#include <ChaiGraphicsExport.h>

namespace CGraphics
{
	//tied to a camera in the scene, contains position, projection, and parameters
    //multiple viewports could be tied to a single viewer
    //a viewer more belongs to a scene, than anything
	class CHAIGRAPHICS_EXPORT Viewer
	{
    public:
        Viewer();
        ~Viewer() = default;

        // Camera controls
        /*void SetPosition(float x, float y, float z);
        void SetOrientation(float pitch, float yaw, float roll);
        void SetFieldOfView(float fov);
        void SetProjectionMode(bool perspective); // True = Perspective, False = Orthographic

        // Scene rendering
        virtual void RenderScene() = 0; // Render the scene from this viewer's perspective

        // Accessors
        void GetPosition(float& x, float& y, float& z) const;
        void GetOrientation(float& pitch, float& yaw, float& roll) const;
        */
	};
}