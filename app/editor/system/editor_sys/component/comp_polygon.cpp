#include "comp_polygon.h"
#include "../../raw_sys/raw.h"
#include "../../raw_sys/raw_sys.h"
#include "../../raw_sys/comp_transform.h"

#include "../editor_sys.h"
#include "comp_collapse_terrain.h"

CompPolygon::CompPolygon()
{
    mSegments.emplace_back(-50, -50);
    mSegments.emplace_back( 50, -50);

    mTrackPoints.emplace_back(-50, -50);
    mTrackPoints.emplace_back( 50, -50);

    AddState(StateEnum::kModifyTrackPoint, true);
    AddState(StateEnum::kInsertTrackPoint, true);
    AddState(StateEnum::kDeleteTrackPoint, true);
}

void CompPolygon::OnUpdate(UIObjectGLCanvas * canvas, float dt)
{
    if (ImGui::IsKeyPressed('A', true))
    {
        auto owner = GetOwner();
        auto parent = GetOwner()->GetParent();
        auto terrain = parent->QueryComponent<CompCollapseTerrain>();
        if (terrain && mLastCoord != GetOwner()->GetTransform()->GetPosition() && mTrackPoints.size() > 2)
        {
            mLastCoord = GetOwner()->GetTransform()->GetPosition();

            std::vector<glm::vec2> points;
            std::transform(mTrackPoints.begin(), mTrackPoints.end(), std::back_inserter(points), [&owner] (const auto & point)
                {
                    return owner->LocalToWorld(point);
                });
            terrain->Erase(points);

            auto root   = Global::Ref().mEditorSys->GetProject()->GetObject();
            auto polys  = root->GetObject("SceneCanvas")->GetObject("Polys");

            auto & objects = polys->GetObjects();
            while (objects.size() > terrain->GetAreas().size())
            {
                Global::Ref().mEditorSys->OptDeleteObject(objects.back());
            }

            for (auto i = 0; i != terrain->GetAreas().size(); ++i)
            {
                if (i == objects.size())
                {
                    auto name   = Global::Ref().mEditorSys->ObjectName(polys);
                    auto object = Global::Ref().mEditorSys->NewObject();
                    (void)object->SetName(name);

                    auto comp = std::create_ptr<CompPolygon>();
                    Global::Ref().mEditorSys->OptInsertObject(object, polys);
                    Global::Ref().mEditorSys->OptAppendComponent(object, comp);
                    object->GetTransform()->Position(0, 0);
                }
                auto object = objects.at(i);
                auto & area = terrain->GetAreas().at(i);

                std::vector<glm::vec2> points;
                for (auto & point : area)
                {
                    auto world = terrain->GetOwner()->LocalToWorld(point);
                    auto local = polys->WorldToLocal(world);
                    points.emplace_back(local);
                }
                object->QueryComponent<CompPolygon>()->ResetSegments(points);
            }
        }
    }
}

const std::string & CompPolygon::GetName()
{
    static const std::string name = "Polygon";
    return name;
}

void CompPolygon::EncodeBinary(std::ostream & os, Project * project)
{
    Component::EncodeBinary(os, project);
    tools::Serialize(os, mSegments);
    tools::Serialize(os, mTrackPoints);
}

void CompPolygon::DecodeBinary(std::istream & is, Project * project)
{
    Component::DecodeBinary(is, project);
    tools::Deserialize(is, mSegments);
    tools::Deserialize(is, mTrackPoints);

    //mSegments.clear();
    //mSegments.emplace_back(-20, -20);
    //mSegments.emplace_back( 20, -20);
    //mSegments.emplace_back( 20,  20);
    //mSegments.emplace_back(-20,  20);

    //mTrackPoints.clear();
    //mTrackPoints.emplace_back(-20, -20);
    //mTrackPoints.emplace_back(20, -20);
    //mTrackPoints.emplace_back(20, 20);
    //mTrackPoints.emplace_back(-20, 20);

    //const auto count = 10;
    //for (auto i = 0; i != count; ++i)
    //{
    //    auto a = glm::pi<float>() * 2.0f / count * i;
    //    mTrackPoints.emplace_back(
    //        std::cos(a) * 10,
    //        std::sin(a) * 10);
    //    mSegments.emplace_back(mTrackPoints.back());
    //}
}

bool CompPolygon::OnModifyProperty(const std::any & oldValue, const std::any & newValue, const std::string & title)
{
    return true;
}

void CompPolygon::ResetSegments(const std::vector<glm::vec2> & segments)
{
    mTrackPoints    = segments;
    mSegments       = segments;
}

std::vector<Component::Property> CompPolygon::CollectProperty()
{
    return Component::CollectProperty();
}

void CompPolygon::OnModifyTrackPoint(const size_t index, const glm::vec2 & point)
{
    AddState(StateEnum::kUpdate, true);
    mTrackPoints.at(index) = point;
    mSegments.at(index) = point;
}

void CompPolygon::OnInsertTrackPoint(const size_t index, const glm::vec2 & point)
{
    AddState(StateEnum::kUpdate, true);
    mSegments.insert(std::next(mSegments.begin(), index), point);
    mTrackPoints.insert(std::next(mTrackPoints.begin(), index), point);
}

void CompPolygon::OnDeleteTrackPoint(const size_t index, const glm::vec2 & point)
{
    AddState(StateEnum::kUpdate, true);
    mSegments.erase(std::next(mSegments.begin(), index));
    mTrackPoints.erase(std::next(mTrackPoints.begin(), index));
}

