#pragma once

#include <Visitors/Visitor.h>
#include <Audio/AudioSceneData.h>

namespace chai::scene
{
    class AudioSceneVisitor : public Visitor
    {
    public:
        virtual void visit(GameObject* node) override;
        virtual void visit(Component* node) override;

        virtual void reset() override
        {
            audioData_ = audio::AudioSceneData();
        }

        [[nodiscard]] audio::AudioSceneData& getData()
        {
            return audioData_;
        }

    private:
        audio::AudioSceneData audioData_;
    };
}
