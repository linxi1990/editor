#pragma once

#include "../../raw_sys/component.h"

class CompSegment : public Component {
public:
    enum UpdateEnum {
        kSegment = 1,
        kMesh    = 2,
    };

public:
    CompSegment();
    virtual void OnUpdate(UIObjectGLCanvas * canvas, float dt) override;

    virtual const std::string & GetName() override;
    virtual void EncodeBinary(std::ostream & os, Project * project) override;
    virtual void DecodeBinary(std::istream & is, Project * project) override;
    virtual bool OnModifyProperty(const std::any & oldValue, 
                                  const std::any & newValue, 
                                  const std::string & title) override;

protected:
    virtual std::vector<Property> CollectProperty() override;

private:
    void Update();
    void GenSegm();
    void GenMesh();
    void DrawBezier(
        const glm::vec2 & a,
        const glm::vec2 & b,
        const glm::vec2 & c);
    void DrawBezier(
        const glm::vec2 & a, 
        const glm::vec2 & b, 
        const glm::vec2 & c,
        const glm::vec2 & d);
    virtual void OnModifyTrackPoint(const size_t index, const glm::vec2 & point) override;
    virtual void OnInsertTrackPoint(const size_t index, const glm::vec2 & point) override;
    virtual void OnDeleteTrackPoint(const size_t index, const glm::vec2 & point) override;

private:
    glm::vec4           mColor;
    float               mWidth;     //  �߿�
    float               mSmooth;
    uint                mUpdate;

    SharePtr<RawMesh>    mMesh;
    SharePtr<RawProgram> mProgram;
    std::vector<glm::vec2> mSegments;
};