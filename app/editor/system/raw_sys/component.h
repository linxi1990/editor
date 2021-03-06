#pragma once

#include "raw.h"
#include "gl_object.h"
#include "serializer.h"
#include "../raw_sys/res.h"
#include "../interface/render.h"
#include "../ui_sys/ui_parse/ui_parse.h"
#include "../ui_sys/ui_object/ui_object.h"

class Component
    : public Serializer 
    , public std::enable_shared_from_this<Component>
{
public:
    enum class StateEnum {
        kActive = 1 << 0,             //  激活
        kUpdate = 1 << 1,             //  刷新
        kInsertTrackPoint = 1 << 2,   //  支持插入控制点
        kDeleteTrackPoint = 1 << 3,   //  支持删除控制点
        kModifyTrackPoint = 1 << 4,   //  支持修改控制点
    };

    struct Property {
        UIParser::StringValueTypeEnum mType;
        std::string                   mName;
        void *                        mMember;
        std::any                      mExtend;

        Property()
            : mType(UIParser::StringValueTypeEnum::kErr)
            , mMember(nullptr)
        { }

        Property(
            UIParser::StringValueTypeEnum type, 
            const std::string & name, void * member, 
            const std::any extend = std::any())
            : mType(type)
            , mName(name)
            , mMember(member)
            , mExtend(extend)
        { }
    };

public:
    //  创建组件
    static SharePtr<Component> Create(const std::string & name);

	Component()
        : mOwner(nullptr)
        , mState((uint)StateEnum::kActive | 
                 (uint)StateEnum::kUpdate) { }
	virtual ~Component() { }
    virtual void OnAppend();
    virtual void OnDelete();
    virtual void OnStart(UIObjectGLCanvas * canvas);
    virtual void OnLeave(UIObjectGLCanvas * canvas);
    virtual void OnUpdate(UIObjectGLCanvas * canvas, float dt);

    void AddState(StateEnum state, bool add)
    {
        if (add) mState |=  (uint)state;
        else     mState &= ~(uint)state;
    }

    bool HasState(StateEnum state)
    {
        return mState & (uint)state;
    }

    SharePtr<GLObject> GetOwner() { return mOwner->shared_from_this(); }
	void SetOwner(GLObject * owner) { mOwner = owner; }

    //  组件名字
    virtual const std::string & GetName() = 0;
    virtual void EncodeBinary(std::ostream & os, Project * project) override;
    virtual void DecodeBinary(std::istream & is, Project * project) override;
    //  Property修改时被调用
    virtual bool OnModifyProperty(const std::any & oldValue, 
                                  const std::any & newValue,
                                  const std::string & title) = 0;
    std::vector<SharePtr<UIObject>> CreateUIPropertys();

    const std::vector<glm::vec2> & GetTrackPoints();
    void ModifyTrackPoint(const size_t index, const glm::vec2 & point) { if (mState & (uint)StateEnum::kModifyTrackPoint) { OnModifyTrackPoint(index, point); } }
    void InsertTrackPoint(const size_t index, const glm::vec2 & point) { if (mState & (uint)StateEnum::kInsertTrackPoint) { OnInsertTrackPoint(index, point); } }
    void DeleteTrackPoint(const size_t index, const glm::vec2 & point) { if (mState & (uint)StateEnum::kDeleteTrackPoint) { OnDeleteTrackPoint(index, point); } }

protected:
    virtual std::vector<Property> CollectProperty();
    virtual void OnModifyTrackPoint(const size_t index, const glm::vec2 & point) {};
    virtual void OnInsertTrackPoint(const size_t index, const glm::vec2 & point) {};
    virtual void OnDeleteTrackPoint(const size_t index, const glm::vec2 & point) {};

protected:
    size_t              mState;
    GLObject *          mOwner;
    std::vector<glm::vec2> mTrackPoints;
};