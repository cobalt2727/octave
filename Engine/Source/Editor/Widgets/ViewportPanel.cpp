#if EDITOR

#include "Widgets/ViewportPanel.h"
#include "Widgets/Console.h"
#include "Widgets/StatsOverlay.h"
#include "InputDevices.h"
#include "EditorState.h"
#include "World.h"
#include "Engine.h"
#include "Grid.h"
#include "Widgets/Quad.h"
#include "Widgets/Text.h"
#include "Widgets/Button.h"
#include "Widgets/ActionList.h"
#include "Log.h"
#include "Renderer.h"
#include "AssetManager.h"
#include "World.h"
#include "Maths.h"
#include "StaticMeshActor.h"
#include "ActionManager.h"
#include "Assets/SkeletalMesh.h"
#include "Constants.h"
#include "EditorConstants.h"
#include "EditorUtils.h"
#include "PanelManager.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/TransformComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/ParticleComponent.h"
#include "Components/AudioComponent.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

static float sDefaultFocalDistance = 10.0f;



void ViewportPanel::HandleFilePressed(Button* button)
{
    std::string buttonText = button->GetTextString();
    ActionManager* am = ActionManager::Get();
    ActionList* actionList = GetActionList();
    bool hideActionList = true;

    if (buttonText == "File")
    {
        std::vector<std::string> actions;
        actions.push_back("Open Project");
        actions.push_back("New Project");
        actions.push_back("Save Level");
        actions.push_back("Package Project");
        actions.push_back("Resave All Levels");
        actions.push_back("Resave All Assets");
        actions.push_back("Import Scene");
        actionList->SetActions(actions, HandleFilePressed);
        hideActionList = false;
    }
    else if (buttonText == "Open Project")
    {
        am->OpenProject();
    }
    else if (buttonText == "New Project")
    {
        am->CreateNewProject();
    }
    else if (buttonText == "Save Level")
    {
        am->SaveLevel(true);
    }
    else if (buttonText == "Package Project")
    {
        am->ShowBuildDataPrompt();
        hideActionList = false;
    }
    else if (buttonText == "Resave All Levels")
    {
        am->RecaptureAndSaveAllLevels();
    }
    else if (buttonText == "Resave All Assets")
    {
        am->ResaveAllAssets();
    }
    else if (buttonText == "Import Scene")
    {
        am->ImportScene();
    }

    if (hideActionList)
    {
        actionList->Hide();
    }
}

void ViewportPanel::HandleViewPressed(Button* button)
{
    std::string buttonText = button->GetTextString();
    ActionManager* am = ActionManager::Get();
    Renderer* renderer = Renderer::Get();
    ActionList* actionList = GetActionList();
    bool hideActionList = true;

    if (buttonText == "View")
    {
        std::vector<std::string> actions;
        actions.push_back("Wireframe");
        actions.push_back("Collision");
        actions.push_back("Proxy");
        actions.push_back("Bounds");
        actions.push_back("Grid");
        actions.push_back("Stats");
        actionList->SetActions(actions, HandleViewPressed);
        hideActionList = false;
    }
    else if (buttonText == "Wireframe")
    {
        if (renderer->GetDebugMode() == DEBUG_WIREFRAME)
        {
            renderer->SetDebugMode(DEBUG_NONE);
        }
        else
        {
            renderer->SetDebugMode(DEBUG_WIREFRAME);
        }
    }
    else if (buttonText == "Collision")
    {
        if (renderer->GetDebugMode() == DEBUG_COLLISION)
        {
            renderer->SetDebugMode(DEBUG_NONE);
        }
        else
        {
            renderer->SetDebugMode(DEBUG_COLLISION);
        }
    }
    else if (buttonText == "Proxy")
    {
        bool enabled = renderer->IsProxyRenderingEnabled();
        renderer->EnableProxyRendering(!enabled);
    }
    else if (buttonText == "Bounds")
    {
        uint32_t newMode = (uint32_t(renderer->GetBoundsDebugMode()) + 1) % uint32_t(BoundsDebugMode::Count);
        renderer->SetBoundsDebugMode((BoundsDebugMode)newMode);
    }
    else if (buttonText == "Grid")
    {
        ToggleGrid();
    }
    else if (buttonText == "Stats")
    {
        bool enabled = renderer->IsStatsOverlayEnabled();
        renderer->EnableStatsOverlay(!enabled);
    }

    if (hideActionList)
    {
        actionList->Hide();
    }
}

void ViewportPanel::HandleWorldPressed(Button* button)
{
    std::string buttonText = button->GetTextString();
    ActionManager* am = ActionManager::Get();
    ActionList* actionList = GetActionList();
    ViewportPanel* vp = PanelManager::Get()->GetViewportPanel();
    bool hideActionList = true;

    if (buttonText == "World")
    {
        std::vector<std::string> actions;
        actions.push_back("Spawn Actor");
        actions.push_back("Spawn Basic");
        actions.push_back("Toggle Transform Mode");
        actionList->SetActions(actions, HandleWorldPressed);
        hideActionList = false;
    }
    else if (buttonText == "Spawn Actor")
    {
        ShowSpawnActorPrompt(false);
        hideActionList = false;
    }
    else if (buttonText == "Spawn Basic")
    {
        ShowSpawnActorPrompt(true);
        hideActionList = false;
    }
    else if (buttonText == "Toggle Transform Mode")
    {
        vp->ToggleTransformMode();
    }

    if (hideActionList)
    {
        actionList->Hide();
    }
}

void HandleSpawnActorPressed(Button* button)
{
    const std::string& actorTypeName = button->GetTextString();

    // Find actorid we need to spawn.
    bool spawnedActor = false;
    const std::vector<Factory*>& factories =  Actor::GetFactoryList();
    for (uint32_t i = 0; i < factories.size(); ++i)
    {
        if (strncmp(factories[i]->GetClassName(), actorTypeName.c_str(), MAX_PATH_SIZE) == 0)
        {
            TypeId actorType = factories[i]->GetType();
            CameraComponent* camera = GetWorld()->GetActiveCamera();
            float focalDistance = PanelManager::Get()->GetViewportPanel()->GetFocalDistance();
            glm::vec3 spawnPos = camera->GetAbsolutePosition() + focalDistance * camera->GetForwardVector();

            ActionManager::Get()->SpawnActor(actorType, spawnPos);

            spawnedActor = true;
            break;
        }
    }
    
    if (!spawnedActor)
    {
        LogError("Failed to spawn actor of type %s", actorTypeName.c_str());
    }

    // Hide the modal spawn list
    Renderer::Get()->SetModalWidget(nullptr);
}

void HandleSpawnBasicPressed(Button* button)
{
    glm::vec3 spawnPos = EditorGetFocusPosition();

    const std::string& basicName = button->GetTextString();
    
    ActionManager::Get()->SpawnBasicActor(basicName, spawnPos);

    Renderer::Get()->SetModalWidget(nullptr);
}

ViewportPanel::ViewportPanel() :
    mFirstPersonMoveSpeed(10.0f),
    mFirstPersonRotationSpeed(10.0f),
    mFocalDistance(sDefaultFocalDistance)
{
    SetName("Viewport");
    SetAnchorMode(AnchorMode::FullStretch);
    SetMargins(sDefaultWidth, 0.0f, sDefaultWidth, 0.0f);

    mFileButton = new Button();
    mFileButton->SetTextString("File");
    mFileButton->SetPressedHandler(HandleFilePressed);
    mFileButton->SetPosition(5.0f, 4.0f);
    mFileButton->SetDimensions(34.0f, 24.0f);
    AddChild(mFileButton);

    mViewButton = new Button();
    mViewButton->SetTextString("View");
    mViewButton->SetPressedHandler(HandleViewPressed);
    mViewButton->SetPosition(45.0f, 4.0f);
    mViewButton->SetDimensions(40.0f, 24.0f);
    AddChild(mViewButton);

    mWorldButton = new Button();
    mWorldButton->SetTextString("World");
    mWorldButton->SetPressedHandler(HandleWorldPressed);
    mWorldButton->SetPosition(90.0f, 4.0f);
    mWorldButton->SetDimensions(50.0f, 24.0f);
    AddChild(mWorldButton);

#if CONSOLE_ENABLED
    // Move the console into  viewport region
    Renderer::Get()->GetConsoleWidget()->SetRect(sDefaultWidth + 5.0f, 30, 1280 - sDefaultWidth, 720);
#endif

    StatsOverlay* statsWidget = Renderer::Get()->GetStatsWidget();
    statsWidget->SetPosition(
        -statsWidget->GetWidth() - sDefaultWidth,
        statsWidget->GetY());
    statsWidget->SetVisible(true);

    mHeaderText->SetVisible(false);
    mHeaderQuad->SetVisible(false);
    mBodyQuad->SetVisible(false);
}

ViewportPanel::~ViewportPanel()
{

}

void ViewportPanel::Update()
{
    Panel::Update();

    switch (GetControlMode())
    {
    case ControlMode::Pilot: mHeaderText->SetText("Pilot"); break;
    case ControlMode::Translate: mHeaderText->SetText("Translate"); break;
    case ControlMode::Rotate: mHeaderText->SetText("Rotate"); break;
    case ControlMode::Scale: mHeaderText->SetText("Scale"); break;
    case ControlMode::Pan: mHeaderText->SetText("Pan"); break;
    case ControlMode::Orbit: mHeaderText->SetText("Orbit"); break;
    default: mHeaderText->SetText(""); break;
    }
}

void ViewportPanel::HandleInput()
{
    Panel::HandleInput();

    if (ShouldHandleInput())
    {
        if (GetEditorState()->mMouseNeedsRecenter)
        {
            EditorCenterCursor();
            GetEditorState()->mMouseNeedsRecenter = false;
        }

        ControlMode controlMode = GetControlMode();

        switch (controlMode)
        {
        case ControlMode::Default: HandleDefaultControls(); break;
        case ControlMode::Pilot: HandlePilotControls(); break;
        case ControlMode::Translate: HandleTransformControls(); break;
        case ControlMode::Rotate: HandleTransformControls(); break;
        case ControlMode::Scale: HandleTransformControls(); break;
        case ControlMode::Pan: HandlePanControls(); break;
        case ControlMode::Orbit: HandleOrbitControls(); break;
        }
    }

    INP_GetMousePosition(mPrevMouseX, mPrevMouseY);
}

float ViewportPanel::GetFocalDistance() const
{
    return mFocalDistance;
}

void ViewportPanel::HandleDefaultControls()
{
    Renderer* renderer = Renderer::Get();
    CameraComponent* camera = GetWorld()->GetActiveCamera();
    glm::vec3 focus = camera->GetAbsolutePosition() + camera->GetForwardVector() * mFocalDistance;

    if (IsMouseInsidePanel())
    {
        const int32_t scrollDelta = GetScrollWheelDelta();
        const bool controlDown = IsControlDown();
        const bool shiftDown = IsShiftDown();

        if (IsMouseButtonJustDown(MOUSE_RIGHT))
        {
            SetControlMode(ControlMode::Pilot);
        }

        if (IsMouseButtonJustDown(MOUSE_LEFT) &&
            !mFileButton->ContainsMouse() &&
            !mViewButton->ContainsMouse() &&
            !mWorldButton->ContainsMouse())
        {
            int32_t mouseX = 0;
            int32_t mouseY = 0;
            GetMousePosition(mouseX, mouseY);
            Actor* selectActor = Renderer::Get()->ProcessHitCheck(GetWorld(), mouseX, mouseY);

            if (shiftDown)
            {
                if (selectActor != nullptr)
                {
                    AddSelectedActor(selectActor, false);
                }
            }
            else
            {
                SetSelectedActor(selectActor);
            }
        }

        if (GetSelectedComponent() != nullptr &&
            GetSelectedComponent()->IsTransformComponent())
        {
            if (!controlDown && IsKeyJustDown(KEY_G))
            {
                SetControlMode(ControlMode::Translate);
                SavePreTransformVector(ControlMode::Translate);
            }

            if (!controlDown && IsKeyJustDown(KEY_R))
            {
                SetControlMode(ControlMode::Rotate);
                SavePreTransformVector(ControlMode::Rotate);
            }

            if (!controlDown && IsKeyJustDown(KEY_S))
            {
                SetControlMode(ControlMode::Scale);
                SavePreTransformVector(ControlMode::Scale);
            }
        }

        if (IsKeyJustDown(KEY_Z))
        {
            renderer->SetDebugMode((renderer->GetDebugMode() != DEBUG_WIREFRAME) ? DEBUG_WIREFRAME : DEBUG_NONE);
        }

        if (IsKeyJustDown(KEY_K))
        {
            renderer->SetDebugMode((renderer->GetDebugMode() != DEBUG_COLLISION) ? DEBUG_COLLISION : DEBUG_NONE);
        }

        if (!controlDown && IsKeyJustDown(KEY_P))
        {
            renderer->EnableProxyRendering(!renderer->IsProxyRenderingEnabled());
        }

        if (!controlDown && IsKeyJustDown(KEY_B))
        {
            uint32_t boundsMode = (uint32_t) renderer->GetBoundsDebugMode();
            boundsMode = (boundsMode + 1) % uint32_t(BoundsDebugMode::Count);
            renderer->SetBoundsDebugMode(BoundsDebugMode(boundsMode));
        }

        if (controlDown && IsKeyJustDown(KEY_G))
        {
            ToggleGrid();
        }

        if (!controlDown && IsKeyJustDown(KEY_T))
        {
            ToggleTransformMode();
        }

        if (IsKeyJustDown(KEY_NUMPAD5))
        {
            CameraComponent* camera = GetWorld()->GetActiveCamera();
            if (camera != nullptr)
            {
                ProjectionMode newMode = (camera->GetProjectionMode() == ProjectionMode::ORTHOGRAPHIC) ? ProjectionMode::PERSPECTIVE : ProjectionMode::ORTHOGRAPHIC;
                camera->SetProjectionMode(newMode);
            }
        }

        if (IsKeyJustDown(KEY_F) ||
            IsKeyJustDown(KEY_DECIMAL))
        {
            // Focus on selected object
            Component* comp = GetSelectedComponent();
            TransformComponent* transComp = (comp && comp->IsTransformComponent()) ? static_cast<TransformComponent*>(comp) : nullptr;

            if (transComp != nullptr && transComp != camera)
            {
                glm::vec3 cameraPos = camera->GetAbsolutePosition();
                glm::vec3 compPos = transComp->GetAbsolutePosition();
                glm::vec3 toCamera = glm::normalize(cameraPos - compPos);
                camera->SetAbsolutePosition(compPos + toCamera * sDefaultFocalDistance);

                glm::quat cameraRot = glm::conjugate(glm::toQuat(glm::lookAt(toCamera, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))));
                camera->SetAbsoluteRotation(cameraRot);

                mFocalDistance = sDefaultFocalDistance;
            }
        }

        if (IsKeyJustDown(KEY_NUMPAD1))
        {
            // Back
            if (controlDown)
            {
                camera->SetAbsoluteRotation(glm::vec3(0.0f, 180.0f, 0.0f));
                camera->SetAbsolutePosition(focus + mFocalDistance * glm::vec3(0.0f, 0.0f, -1.0f));
            }
            // Front
            else
            {
                camera->SetAbsoluteRotation(glm::vec3(0.0f, 0.0f, 0.0f));
                camera->SetAbsolutePosition(focus + mFocalDistance * glm::vec3(0.0f, 0.0f, 1.0f));
            }
        }

        if (IsKeyJustDown(KEY_NUMPAD3))
        {
            // Left
            if (controlDown)
            {
                camera->SetAbsoluteRotation(glm::vec3(0.0f, 270.0f, 0.0f));
                camera->SetAbsolutePosition(focus + mFocalDistance * glm::vec3(-1.0f, 0.0f, 0.0f));
            }
            // Right
            else
            {
                camera->SetAbsoluteRotation(glm::vec3(0.0f, 90.0f, 0.0f));
                camera->SetAbsolutePosition(focus + mFocalDistance * glm::vec3(1.0f, 0.0f, 0.0f));
            }
        }

        if (IsKeyJustDown(KEY_NUMPAD7))
        {
            // Bottom
            if (controlDown)
            {
                camera->SetAbsoluteRotation(glm::vec3(90.0f, 0.0f, 0.0f));
                camera->SetAbsolutePosition(focus + mFocalDistance * glm::vec3(0.0f, -1.0f, 0.0f));
            }
            // Top
            else
            {
                camera->SetAbsoluteRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
                camera->SetAbsolutePosition(focus + mFocalDistance * glm::vec3(0.0f, 1.0f, 0.0f));
            }
        }

        glm::vec3 spawnPos = camera->GetAbsolutePosition() + mFocalDistance * camera->GetForwardVector();
        if (controlDown && IsKeyJustDown(KEY_1))
        {
            ActionManager::Get()->SpawnBasicActor(BASIC_STATIC_MESH, spawnPos);
        }
        else if (controlDown && IsKeyJustDown(KEY_2))
        {
            ActionManager::Get()->SpawnBasicActor(BASIC_POINT_LIGHT, spawnPos);
        }
        else if (controlDown && IsKeyJustDown(KEY_3))
        {
            ActionManager::Get()->SpawnBasicActor(BASIC_TRANSFORM, spawnPos);
        }
        else if (controlDown && IsKeyJustDown(KEY_4) && GetWorld()->GetDirectionalLight() == nullptr)
        {
            ActionManager::Get()->SpawnBasicActor(BASIC_DIRECTIONAL_LIGHT, spawnPos);
        }
        else if (controlDown && IsKeyJustDown(KEY_5))
        {
            ActionManager::Get()->SpawnBasicActor(BASIC_SKELETAL_MESH, spawnPos);
        }
        else if (controlDown && IsKeyJustDown(KEY_6))
        {
            ActionManager::Get()->SpawnBasicActor(BASIC_BOX, spawnPos);
        }
        else if (controlDown && IsKeyJustDown(KEY_7))
        {
            ActionManager::Get()->SpawnBasicActor(BASIC_SPHERE, spawnPos);
        }
        else if (controlDown && IsKeyJustDown(KEY_8))
        {
            ActionManager::Get()->SpawnBasicActor(BASIC_PARTICLE, spawnPos);
        }
        else if (controlDown && IsKeyJustDown(KEY_9))
        {
            ActionManager::Get()->SpawnBasicActor(BASIC_AUDIO, spawnPos);
        }
        else if (controlDown && IsKeyJustDown(KEY_0))
        {
            ActionManager::Get()->SpawnBasicActor(BASIC_BLUEPRINT, spawnPos);
        }


        if (IsKeyJustDown(KEY_DELETE))
        {
            ActionManager::Get()->DeleteSelectedActors();
        }

        if (controlDown && IsKeyJustDown(KEY_D))
        {
            // Duplicate actor
            Actor* selectedActor = GetSelectedActor();

            if (selectedActor != nullptr)
            {
                ActionManager::Get()->DuplicateActor(selectedActor);
                SetControlMode(ControlMode::Translate);
                SavePreTransformVector(ControlMode::Translate);
            }
        }

        if (shiftDown && (IsKeyJustDown(KEY_A) || IsKeyJustDown(KEY_Q)))
        {
            // Set the spawn actor list as the modal widget.
            const bool basic = IsKeyJustDown(KEY_Q);
            ShowSpawnActorPrompt(basic);
        }

        // Handle zoom
        if (scrollDelta != 0)
        {
            CameraComponent* camera = GetWorld()->GetActiveCamera();

            if (camera->GetProjectionMode() == ProjectionMode::PERSPECTIVE)
            {
                glm::vec3 forwardVector = camera->GetForwardVector();
                glm::vec3 cameraPos = camera->GetAbsolutePosition();
                float deltaPos = mFocalDistance * 0.05f * scrollDelta;
                camera->SetAbsolutePosition(cameraPos + forwardVector * deltaPos);
                mFocalDistance = glm::max(0.1f, mFocalDistance - deltaPos);
            }
            else
            {
                // For orthographic, adjust the bounds of the projection.
                const float xySpeed = 0.05f;
                const float zSpeed = 0.05f;
                OrthoSettings settings = camera->GetOrthoSettings();
                settings.mWidth *= 1.0f + (xySpeed * -scrollDelta);
                settings.mHeight *= 1.0f + (xySpeed * -scrollDelta);
                settings.mFar *= 1.0f + (zSpeed * -scrollDelta);

                settings.mWidth = glm::max(settings.mWidth, 0.05f);
                settings.mHeight = glm::max(settings.mHeight, 0.05f);
                settings.mFar = glm::max(settings.mFar, 50.0f);
                camera->SetOrthoSettings(settings.mWidth, settings.mHeight, settings.mNear, settings.mFar);
            }
        }

        const bool middleMouseDown = IsMouseButtonDown(MOUSE_MIDDLE);
        if (middleMouseDown)
        {
            if (shiftDown)
            {
                SetControlMode(ControlMode::Pan);
            }
            else
            {
                SetControlMode(ControlMode::Orbit);
            }
        }
    }
}

void ViewportPanel::HandlePilotControls()
{
    CameraComponent* camera = GetWorld()->GetActiveCamera();
    float deltaTime = GetAppClock()->DeltaTime();

    int32_t scrollDelta = GetScrollWheelDelta();
    if (scrollDelta != 0)
    {
        const float deltaSpeed = 0.1f;
        float totalDelta = 1.0f + (scrollDelta > 0 ? deltaSpeed : -deltaSpeed);
        totalDelta = pow(totalDelta, fabs((float)scrollDelta));
        mFirstPersonMoveSpeed *= totalDelta;
        mFirstPersonMoveSpeed = glm::clamp(mFirstPersonMoveSpeed, 0.1f, 1000.0f);
    }

    glm::vec2 deltaRot = HandleLockedCursor();

    glm::vec3 cameraPosition = camera->GetPosition();
    glm::vec3 cameraRotation = camera->GetRotationEuler();

    float angleY = glm::radians(cameraRotation.y);
    float c = cos(angleY);
    float s = sin(angleY);
    float angleX = glm::radians(cameraRotation.x);
    float xc = cos(angleX);
    float xs = sin(angleX);

    if (IsKeyDown(KEY_A))
    {
        cameraPosition.x -= c * (mFirstPersonMoveSpeed * deltaTime);
        cameraPosition.z += s * (mFirstPersonMoveSpeed * deltaTime);
    }

    if (IsKeyDown(KEY_D))
    {
        cameraPosition.x += c * (mFirstPersonMoveSpeed * deltaTime);
        cameraPosition.z -= s * (mFirstPersonMoveSpeed * deltaTime);
    }

    if (IsKeyDown(KEY_W))
    {
        cameraPosition.z -= xc * (c * (mFirstPersonMoveSpeed * deltaTime));
        cameraPosition.x -= xc * (s * (mFirstPersonMoveSpeed * deltaTime));
        cameraPosition.y += xs * (mFirstPersonMoveSpeed * deltaTime);
    }

    if (IsKeyDown(KEY_S))
    {
        cameraPosition.z += xc * (c * (mFirstPersonMoveSpeed * deltaTime));
        cameraPosition.x += xc * (s * (mFirstPersonMoveSpeed * deltaTime));
        cameraPosition.y -= xs * (mFirstPersonMoveSpeed * deltaTime);
    }

    if (IsKeyDown(KEY_E))
    {
        cameraPosition.y += (mFirstPersonMoveSpeed * deltaTime);
    }

    if (IsKeyDown(KEY_Q))
    {
        cameraPosition.y -= (mFirstPersonMoveSpeed * deltaTime);
    }

    cameraRotation.y -= deltaRot.x * mFirstPersonRotationSpeed * deltaTime;
    cameraRotation.x -= deltaRot.y * mFirstPersonRotationSpeed * deltaTime;

    // Clamp x rotation to -90/90
    cameraRotation.x = glm::clamp(cameraRotation.x, -89.99f, 89.99f);

    camera->SetPosition(cameraPosition);
    camera->SetRotation(cameraRotation);

    if (!IsMouseButtonDown(MOUSE_RIGHT))
    {
        SetControlMode(ControlMode::Default);
    }
}

void ViewportPanel::HandleTransformControls()
{
    ControlMode controlMode = GetControlMode();
    Component* component = GetSelectedComponent();
    const std::vector<Component*>& selectedComps = GetSelectedComponents();

    if (component == nullptr || !component->IsTransformComponent())
        return;

    TransformComponent* transComp = static_cast<TransformComponent*>(component);

    std::vector<TransformComponent*> transComps;
    for (uint32_t i = 0; i < selectedComps.size(); ++i)
    {
        if (selectedComps[i]->IsTransformComponent())
        {
            transComps.push_back((TransformComponent*)selectedComps[i]);
        }
    }

    CameraComponent* camera = GetWorld()->GetActiveCamera();
    glm::mat4 invViewMat = camera->CalculateInvViewMatrix();

    HandleAxisLocking();

    glm::vec2 delta = HandleLockedCursor();
    delta.y *= -1.0f;

    const bool shiftDown = IsShiftDown();
    const float shiftSpeedMult = 0.1f;

    if (delta != glm::vec2(0.0f, 0.0f))
    {
        if (controlMode == ControlMode::Translate)
        {
            const float translateSpeed = 0.025f;
            glm::vec3 worldDelta = glm::vec3(delta.x, delta.y, 0.0f);
            worldDelta = invViewMat * glm::vec4(worldDelta, 0.0);
            worldDelta = GetLockedTranslationDelta(worldDelta);

            float speed = shiftDown ? (shiftSpeedMult * translateSpeed) : translateSpeed;

            for (uint32_t i = 0; i < transComps.size(); ++i)
            {
                glm::vec3 pos = transComps[i]->GetAbsolutePosition();
                pos += speed * glm::vec3(worldDelta.x, worldDelta.y, worldDelta.z);
                transComps[i]->SetAbsolutePosition(pos);
            }
        }
        else if (controlMode == ControlMode::Rotate)
        {
            const float rotateSpeed = 0.025f;

            float speed = shiftDown ? (shiftSpeedMult * rotateSpeed) : rotateSpeed;
            glm::vec3 rotateAxisWS = invViewMat * glm::vec4(0, 0, 1, 0);
            float totalDelta = delta.x - delta.y;

            if (GetEditorState()->mTransformLock != TransformLock::None)
            {
                rotateAxisWS = GetLockedRotationAxis();
            }

            glm::quat addQuat = glm::angleAxis(-totalDelta * speed, rotateAxisWS);

            if (mTransformLocal)
            {
                for (uint32_t i = 0; i < transComps.size(); ++i)
                {
                    transComps[i]->AddAbsoluteRotation(addQuat);
                }
            }
            else
            {
                glm::vec3 pivot = transComp->GetAbsolutePosition();

                for (uint32_t i = 0; i < transComps.size(); ++i)
                {
                    glm::mat4 rotMat = glm::translate(pivot);
                    rotMat *= glm::rotate(-totalDelta * speed, rotateAxisWS);
                    rotMat *= glm::translate(-pivot);

                    glm::mat4 transform = transComps[i]->GetTransform();
                    transComps[i]->SetTransform(rotMat * transform);
                }
            }
        }
        else if (controlMode == ControlMode::Scale)
        {
            // Only enable uniform scaling for now
            const float scaleSpeed = 0.025f;

            float speed = shiftDown ? (shiftSpeedMult * scaleSpeed) : scaleSpeed;
            float totalDelta = delta.x - delta.y;
            float deltaScale = speed * totalDelta;
            glm::vec3 deltaScale3 = glm::vec3(deltaScale, deltaScale, deltaScale);

            if (GetEditorState()->mTransformLock != TransformLock::None)
            {
                deltaScale3 *= GetLockedScaleDelta();
            }

            if (mTransformLocal)
            {
                for (uint32_t i = 0; i < transComps.size(); ++i)
                {
                    glm::vec3 scale = transComps[i]->GetScale();
                    scale += deltaScale;
                    transComps[i]->SetScale(scale);
                }
            }
            else
            {
                glm::vec3 pivot = transComp->GetAbsolutePosition();

                for (uint32_t i = 0; i < transComps.size(); ++i)
                {
                    glm::vec3 scale = transComps[i]->GetScale();
                    glm::vec3 newScale = (deltaScale3 + scale);

                    if (glm::length(scale) != 0.0f &&
                        glm::length(newScale) != 0.0f)
                    {
                        glm::mat4 scaleMat = glm::translate(pivot);
                        scaleMat *= glm::scale(newScale / scale);
                        scaleMat *= glm::translate(-pivot);

                        glm::vec3 scaledPos = scaleMat * glm::vec4(transComps[i]->GetAbsolutePosition(), 1.0f);

                        glm::vec3 newScaleRot = newScale;

                        if (GetEditorState()->mTransformLock != TransformLock::None)
                        {
                            glm::quat absRot = transComps[i]->GetAbsoluteRotationQuat();
                            glm::mat4 rotMat = glm::mat4(absRot);
                            glm::vec3 deltaScaleRot = rotMat * glm::vec4(deltaScale3, 0.0f);

                            if (deltaScale >= 0.0f)
                            {
                                deltaScaleRot = glm::abs(deltaScaleRot);
                            }
                            else
                            {
                                deltaScaleRot = -glm::abs(deltaScaleRot);
                            }

                            newScaleRot = scale + deltaScaleRot;
                        }

                        transComps[i]->SetAbsolutePosition(scaledPos);
                        transComps[i]->SetScale(newScaleRot);
                    }
                }
            }
        }
    }

    if (!IsControlDown() && IsKeyJustDown(KEY_T))
    {
        ToggleTransformMode();
    }

    if (IsMouseButtonDown(MOUSE_LEFT))
    {
        SetControlMode(ControlMode::Default);
    }

    if (IsMouseButtonDown(MOUSE_RIGHT))
    {
        // Cancel transform operation
        RestorePreTransformVector(controlMode);
        SetControlMode(ControlMode::Default);
    }
}

void ViewportPanel::HandlePanControls()
{
    // Pan the camera
    CameraComponent* camera = GetWorld()->GetActiveCamera();
    glm::vec3 cameraPos = camera->GetAbsolutePosition();
    glm::vec3 right = camera->GetRightVector();
    glm::vec3 up = camera->GetUpVector();

    glm::vec2 deltaPosVS = HandleLockedCursor();
    glm::vec3 deltaPosWS = 0.002f * -(right * deltaPosVS.x - up * deltaPosVS.y);
    camera->SetAbsolutePosition(cameraPos + deltaPosWS);

    if (!IsMouseButtonDown(MOUSE_MIDDLE))
    {
        SetControlMode(ControlMode::Default);
    }
}

void ViewportPanel::HandleOrbitControls()
{
    CameraComponent* camera = GetWorld()->GetActiveCamera();
    glm::vec2 delta = HandleLockedCursor();
    glm::vec3 cameraPos = camera->GetAbsolutePosition();
    glm::vec3 cameraRot = camera->GetAbsoluteRotationEuler();
    glm::vec3 focus = cameraPos + camera->GetForwardVector() * mFocalDistance;

    // Transform the camera into focus space
    float deltaYaw = -delta.x * 0.005f;
    float deltaPitch = -delta.y * 0.005f;

    // clamp pitch changes
    if (cameraRot.x > 0.0f)
    {
        deltaPitch = glm::min(deltaPitch, (89.9f - cameraRot.x) * DEGREES_TO_RADIANS);
    }
    else
    {
        deltaPitch = glm::max(deltaPitch, (-89.9f - cameraRot.x) * DEGREES_TO_RADIANS);
    }

    glm::mat4 matYaw = glm::rotate(glm::mat4(1), deltaYaw, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 matPitch = glm::rotate(glm::mat4(1), deltaPitch, camera->GetRightVector());
    glm::vec4 rotPoint = glm::vec4(cameraPos - focus, 1.0f);

    glm::vec3 newPos = focus + glm::vec3(matPitch * matYaw * rotPoint);
    glm::vec3 newRot = cameraRot + glm::vec3(deltaPitch, deltaYaw, 0.0f) * RADIANS_TO_DEGREES;

    
    camera->SetAbsolutePosition(newPos);
    camera->SetAbsoluteRotation(newRot);

    if (!IsMouseButtonDown(MOUSE_MIDDLE))
    {
        SetControlMode(ControlMode::Default);
    }
}

glm::vec2 ViewportPanel::HandleLockedCursor()
{
    // Find mouse delta
    glm::vec2 delta = glm::vec2(0.0f, 0.0f);
    glm::ivec2 centerPoint;
    glm::ivec2 curMousePos;

    EditorGetWindowCenter(centerPoint.x, centerPoint.y);

    INP_GetMousePosition(curMousePos.x, curMousePos.y);

    delta = curMousePos - centerPoint;

    // Reset mouse to center of screen
    EditorSetCursorPos(centerPoint.x, centerPoint.y);

    return delta;
}

void ViewportPanel::HandleAxisLocking()
{
    EditorState* state = GetEditorState();
    TransformLock currentLock = state->mTransformLock;
    TransformLock newLock = TransformLock::None;

    // Plane locking
    if (IsShiftDown())
    {
        if (IsKeyJustDown(KEY_X))
        {
            newLock = TransformLock::PlaneYZ;
        }

        if (IsKeyJustDown(KEY_Y))
        {
            newLock = TransformLock::PlaneXZ;
        }

        if (IsKeyJustDown(KEY_Z))
        {
            newLock = TransformLock::PlaneXY;
        }
    }
    // Axis locking
    else
    {
        if (IsKeyJustDown(KEY_X))
        {
            newLock = TransformLock::AxisX;
        }

        if (IsKeyJustDown(KEY_Y))
        {
            newLock = TransformLock::AxisY;
        }

        if (IsKeyJustDown(KEY_Z))
        {
            newLock = TransformLock::AxisZ;
        }
    }

    if (newLock != TransformLock::None)
    {
        RestorePreTransformVector(GetControlMode());

        if (newLock != currentLock)
        {
            SetTransformLock(newLock);
        }
        else
        {
            // If hitting the same lock key, unlock. (reset to None transform lock).
            SetTransformLock(TransformLock::None);
        }
    }
}

glm::vec2 ViewportPanel::GetTransformDelta() const
{
    glm::vec2 delta = glm::vec2(0.0f, 0.0f);

    int32_t x = 0;
    int32_t y = 0;
    INP_GetMousePosition(x, y);

    delta.x = static_cast<float>(x - mPrevMouseX);
    delta.y = -static_cast<float>(y - mPrevMouseY);

    return delta;
}

void ViewportPanel::SavePreTransformVector(ControlMode mode)
{
    const std::vector<Component*>& selComps = GetSelectedComponents();
    mPreTransforms.clear();

    for (uint32_t i = 0; i < selComps.size(); ++i)
    {
        TransformComponent* transComp = (selComps[i] && selComps[i]->IsTransformComponent()) ? static_cast<TransformComponent*>(selComps[i]) : nullptr;

        if (transComp)
        {
            mPreTransforms.push_back(transComp->GetTransform());
        }
    }
}

void ViewportPanel::RestorePreTransformVector(ControlMode mode)
{
    const std::vector<Component*>& selComps = GetSelectedComponents();

    for (uint32_t i = 0; i < selComps.size(); ++i)
    {
        if (i >= mPreTransforms.size())
        {
            LogError("Component/Transform array mismatch?");
            break;
        }

        TransformComponent* transComp = (selComps[i] && selComps[i]->IsTransformComponent()) ? static_cast<TransformComponent*>(selComps[i]) : nullptr;

        if (transComp)
        {
            transComp->SetTransform(mPreTransforms[i]);
        }
    }
}

void ViewportPanel::ToggleTransformMode()
{
    mTransformLocal = !mTransformLocal;
    LogDebug("Transform Mode = %s", mTransformLocal ? "Local" : "Pivot");
}

glm::vec3 ViewportPanel::GetLockedTranslationDelta(glm::vec3 deltaWS) const
{
    glm::vec3 retDelta = deltaWS;
    retDelta *= GetTransformLockVector(GetEditorState()->mTransformLock);
    return retDelta;
}

glm::vec3 ViewportPanel::GetLockedRotationAxis() const
{
    glm::vec3 ret = glm::vec3(0.0, 1.0, 0.0);

    switch (GetEditorState()->mTransformLock)
    {
    case TransformLock::AxisX:
    case TransformLock::PlaneYZ:
        ret = glm::vec3(1.0, 0.0, 0.0);
        break;
    case TransformLock::AxisY:
    case TransformLock::PlaneXZ:
        ret = glm::vec3(0.0, 1.0, 0.0);
        break;
    case TransformLock::AxisZ:
    case TransformLock::PlaneXY:
        ret = glm::vec3(0.0, 0.0, 1.0);
        break;

    default: break;
    }

    return ret;
}

glm::vec3 ViewportPanel::GetLockedScaleDelta()
{
    glm::vec3 retDelta = glm::vec3(1, 1, 1);
    retDelta *= GetTransformLockVector(GetEditorState()->mTransformLock);
    return retDelta;
}

void ViewportPanel::ShowSpawnActorPrompt(bool basic)
{
    ActionList* actionList = GetActionList();

    std::vector<std::string> actions;

    if (basic)
    {
        actions.push_back(BASIC_BLUEPRINT);
        actions.push_back(BASIC_STATIC_MESH);
        actions.push_back(BASIC_POINT_LIGHT);
        actions.push_back(BASIC_TRANSFORM);
        actions.push_back(BASIC_DIRECTIONAL_LIGHT);
        actions.push_back(BASIC_SKELETAL_MESH);
        actions.push_back(BASIC_BOX);
        actions.push_back(BASIC_SPHERE);
        actions.push_back(BASIC_CAPSULE);
        actions.push_back(BASIC_PARTICLE);
        actions.push_back(BASIC_AUDIO);
        actions.push_back(BASIC_CAMERA);

        actionList->SetActions(actions, HandleSpawnBasicPressed);
    }
    else
    {
        const std::vector<Factory*>& actorFactories = Actor::GetFactoryList();

        for (uint32_t i = 0; i < actorFactories.size(); ++i)
        {
            actions.push_back(actorFactories[i]->GetClassName());
        }

        actionList->SetActions(actions, HandleSpawnActorPressed);
    }
}

#endif
