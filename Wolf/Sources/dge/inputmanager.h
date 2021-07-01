#pragma once
#include <algorithm>
#include "platform/platform.h"
namespace DGE {
    template <typename TRIGGERVALIDATOR, typename... Args>
    class InputSignal {
        GTech::Signal<Args...>sgnl{};
    public:
        using TriggerValidationList = std::vector<bool(TRIGGERVALIDATOR::*)()>; 
        enum class ValidationOperator {
            V_AND,
            V_OR
        };
    private:
        TRIGGERVALIDATOR* mTriggerValidatorInst{nullptr};
        TriggerValidationList mTriggerValidators{};
        ValidationOperator mOperator{ValidationOperator::V_AND};
    public:

        InputSignal(TRIGGERVALIDATOR*tinst) : mTriggerValidatorInst(tinst){}
        void VerifySignal() const{
            if (!mTriggerValidatorInst) return;
            if (mOperator == ValidationOperator::V_AND){
                if (std::all_of(
                    mTriggerValidators.begin(), 
                    mTriggerValidators.end(), 
                    [this](auto&mTriggerValidator){ 
                        return this->mTriggerValidatorInst->mTriggerValidator();
                    })){
                    sgnl.emit();
                }
            } else if (mOperator == ValidationOperator::V_OR){
                if (std::any_of(
                    mTriggerValidators.begin(),
                    mTriggerValidators.end(),
                    [this](auto&mTriggerValidator){
                        return this->mTriggerValidatorInst->mTriggerValidator();
                    })){
                    sgnl.emit();
                }
            }
        }
        inline void SetValidationOperator(ValidationOperator op) { mOperator = op; }
    };   
    class InputManager : public DGE::Singleton<InputManager> {
        friend class DGE::Singleton<InputManager>;
        DGE::IPlatform* mPlatform{nullptr};

    private:
        InputManager() {
            if (!mPlatform) {
                if (WindowManager::sWindowDriver == WindowManager::WindowDriver::DRIVER_GLFW3){
                    mPlatform = &PlatformGLFW::GetInstanceReference();
                }
                else {
                    std::exit(-1);
                }
            }
        }

    public:
        using Trigger = std::pair<Keys, EventType>;

        inline DGE::KeyState GetKeyState(DGE::IPlatform::WindowID wid, Keys t) {
            return mPlatform -> GetKeyState(wid,t);
        }
        inline void ConnectKey([[maybe_unused]] DGE::IPlatform::WindowID wid, [[maybe_unused]]Keys k){

        }
    };


}