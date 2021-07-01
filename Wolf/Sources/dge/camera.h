#pragma once
namespace DGE{


    class Camera {

        const glm::vec3 referenceUp{0.0f, 1.0f, 0.0f};
        glm::mat4 mTransform {1.0f};

        DGE::fLocation mFront{-mPositionReference};
        glm::vec3 mRight{};
        glm::vec3 mUp{referenceUp};

        DGE::fLocation& mPositionReference;
        DGE::fLocation& mLookAtReference;
    public:

        glm::mat4& GetViewMatrix()
        {
            return mTransform;
        }

        void Update()
        {
            mFront = mLookAtReference - mPositionReference;
            auto camZAxis = -mFront;
            camZAxis = glm::normalize(camZAxis);
            mRight = glm::cross(referenceUp, camZAxis);
            mUp = glm::cross(camZAxis, mRight);
            mTransform = glm::lookAt(mPositionReference, mPositionReference + mFront, mUp);
        }


        Camera(DGE::fLocation& position, DGE::fLocation& lookat) : mPositionReference(position), mLookAtReference(lookat){
            Update();
        }

    };


}