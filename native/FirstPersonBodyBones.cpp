using NativeModule = void*;
using NativeSize = unsigned long long;
using NativeWord = unsigned long;
#include "build/FirstPersonBodyReleaseMetadata.h"
extern "C" int _fltused = 0;

extern "C" __declspec(dllimport) NativeModule __stdcall GetModuleHandleA(const char*);
extern "C" __declspec(dllimport) void* __stdcall GetProcAddress(NativeModule, const char*);
extern "C" __declspec(dllimport) void* __stdcall VirtualAlloc(void*, NativeSize, NativeWord, NativeWord);
extern "C" __declspec(dllimport) int __stdcall VirtualFree(void*, NativeSize, NativeWord);
extern "C" __declspec(dllimport) int __stdcall VirtualProtect(void*, NativeSize, NativeWord, NativeWord*);
extern "C" __declspec(dllimport) int __stdcall FlushInstructionCache(void*, const void*, NativeSize);
extern "C" __declspec(dllimport) void* __stdcall GetCurrentProcess();

using ClonePilotPose = void* (*)(void*, const char*);
using SetClientRenderMode = void (*)(void*, int);
using GetClientModelName = const char* (*)(void*);
using GetSequenceActivity = int (*)(void*, int);
using SelectVisualSequence = int (*)(void*, int, void*, int);
using ResetVisualSequence = void (*)(void*, int);
using SetVisualCycle = void (*)(void*, float);
using GetVisualCycleRate = float (*)(void*, int);
using CreateNorthstarInterface = void* (*)(const char*, int*);
using WriteNorthstarLog = void (*)(void*, long long, int, char*);

struct NativeInterface
{
    void** functions;
};

struct VerifiedNativeHook
{
    unsigned char* entry;
    unsigned char* trampoline;
    unsigned char originalInstructions[32];
    int instructionLength;
};

static NativeInterface* northstarSystem;
static VerifiedNativeHook drawHook;
static VerifiedNativeHook poseHook;
static VerifiedNativeHook shadowBatchHook;
static ClonePilotPose clonePilotPose;
static SetClientRenderMode setClientRenderMode;
static GetClientModelName getClientModelName;
static GetSequenceActivity getSequenceActivity;
static SelectVisualSequence selectVisualSequence;
static ResetVisualSequence resetVisualSequence;
static SetVisualCycle setVisualCycle;
static GetVisualCycleRate getVisualCycleRate;
static unsigned char* clientModuleBase;
static void* clientSquirrelVm;
static void* localPilot;
static void* visualMainBody;
static void* visualShadowBody;
static int visualActivity = -1;
static int visualSequence = -1;
static float visualCycle;
struct NativeVector
{
    float x;
    float y;
    float z;
};

struct NativeBoneMatrix
{
    float values[12];
};

using SetupClientBodyBones = bool (*)(void*, NativeBoneMatrix*, int, int, float);
using GetPilotCameraPosition = const NativeVector* (*)(void*);
using GetPilotOrigin = const NativeVector* (*)(void*);

static GetPilotCameraPosition getPilotCameraPosition;
static NativeVector crouchedSlideHipInPilotFrame;
static float crouchedSlideHipBelowCamera;
static bool crouchedSlideAnchorValid;
static float standingHipBelowCamera;
static bool standingHipAnchorValid;
static bool readinessLogPending;
static bool crouchedJumpSlideLogPending;
static constexpr unsigned int SharedPilotBonesEffect = 1;
static constexpr unsigned int DisableProxyShadowEffect = 16;
static constexpr NativeSize EntityEffectsOffset = 0x3a0;
static constexpr NativeSize EntityRenderAlphaOffset = 0x4b;
static constexpr int TranslucentTextureRenderMode = 2;
static constexpr NativeSize PilotSequenceOffset = 0x104c;
static constexpr int MultiplayerCrouchSlideActivity = 0x1f2;
static constexpr int MultiplayerJumpActivity = 452;
static constexpr int MultiplayerDoubleJumpActivity = 465;
static constexpr int MultiplayerWallJumpLeftActivity = 495;
static constexpr int MultiplayerWallJumpUpActivity = 497;
static constexpr int MultiplayerJumpPrimaryActivity = 553;
static constexpr int MultiplayerJumpLandPrimaryActivity = 556;
static constexpr int MultiplayerJumpMeleeActivity = 566;
static constexpr int MultiplayerJumpLandMeleeActivity = 569;
static constexpr int MultiplayerCrouchWalkActivity = 435;
static constexpr int MultiplayerStaggeredCrouchWalkRightActivity = 444;
static constexpr int MultiplayerCrouchWalkPrimaryActivity = 552;
static constexpr int MultiplayerCrouchWalkMeleeActivity = 565;
static constexpr NativeSize CachedBoneMatricesOffset = 0x1008;
static constexpr NativeSize StudioHeaderOffset = 0x1208;
static constexpr NativeSize CopiedPilotPoseLayerCountOffset = 0xe98;
static constexpr int PilotRootBone = 0;
static constexpr int PilotHipBone = 3;
static constexpr int CompletePilotPoseBoneMask = 0x7ff00;

struct NativeBodyState
{
    void* pilot;
    int requestedActivity;
    int selectedActivity;
    int sequenceActivity;
    unsigned int crouchHeld;
    unsigned int falling;
    unsigned int crouchedJumpSlideRequests;
    float verticalVelocity;
    unsigned int fadeDrawSubmissions;
    float smallestSubmittedAlpha;
    float largestSubmittedAlpha;
    float visualCycle;
    int visualSequence;
    void* visualBody;
    int sourceSequenceActivity;
    unsigned int sharedPilotBones;
    unsigned int lastFadeDrawFlags;
    unsigned int alphaModulatedDrawSubmissions;
    unsigned int airborne;
    unsigned int jumpRequested;
    unsigned int sourceJumpActivity;
    unsigned int groundedCrouchWalk;
    unsigned int translationLocked;
    unsigned int translationLockSubmissions;
    float lockedHipBelowCamera;
    float drawnHipBelowCamera;
    float hipTranslationCorrectionX;
    float hipTranslationCorrectionY;
    float hipTranslationCorrectionZ;
    unsigned int copiedPilotPoseLayers;
    void* shadowBody;
    unsigned int shadowDrawFlags;
    unsigned int shadowDrawSubmissions;
    unsigned int shadowPoseSubmissions;
    unsigned int shadowColorDrawsSuppressed;
    float shadowHipPoseDifference;
    unsigned int shadowBatchSubmissions;
    unsigned int shadowPoseBatchReplacements;
    unsigned int lastShadowBatchFlags;
    unsigned int pilotDrawFlags;
    unsigned int shadowDepthDrawReplacements;
    unsigned int completePilotPoseCopies;
    unsigned int fallingTranslationLockSubmissions;
    float fallingLockedHipBelowCamera;
    float fallingDrawnHipBelowCamera;
    float fallingHipCorrectionZ;
    unsigned int groundedCrouchTransition;
};

extern "C" __declspec(dllexport) NativeBodyState FirstPersonBodyNativeState = {};

struct BodyRenderInstance
{
    float alpha;
    unsigned int flags;
};

using DrawClientBody = int (*)(void*, void*, int, const BodyRenderInstance*);

struct NativeModelBatchEntry
{
    void* renderable;
    void* modelInstance;
    NativeSize renderState;
};

using DrawClientModelBatch = void (*)(void*, NativeModelBatchEntry*, int, int, void*);

static void DrawShadowBatchWithVisualPilotPose(void* renderer, NativeModelBatchEntry* entries, int count, int flags, void* renderContext)
{
    NativeModelBatchEntry* pilotEntry = nullptr;
    void* originalRenderable = nullptr;
    if (localPilot && visualShadowBody && flags > 0 && flags < 4)
    {
        FirstPersonBodyNativeState.shadowBatchSubmissions++;
        FirstPersonBodyNativeState.lastShadowBatchFlags = flags;
        for (int index = 0; index < count; ++index)
        {
            if (entries[index].renderable == static_cast<unsigned char*>(localPilot) + 0x10)
            {
                pilotEntry = entries + index;
                originalRenderable = pilotEntry->renderable;
                pilotEntry->renderable = static_cast<unsigned char*>(visualShadowBody) + 0x10;
                FirstPersonBodyNativeState.shadowPoseBatchReplacements++;
                break;
            }
        }
    }
    reinterpret_cast<DrawClientModelBatch>(shadowBatchHook.trampoline)(renderer, entries, count, flags, renderContext);
    if (pilotEntry)
        pilotEntry->renderable = originalRenderable;
}

static bool HasPrefix(const char* value, const char* prefix)
{
    while (*prefix)
        if (*value++ != *prefix++)
            return false;
    return true;
}

static bool SameText(const char* first, const char* second)
{
    while (*first && *first == *second)
    {
        ++first;
        ++second;
    }
    return *first == *second;
}

static void WritePluginLog(const char* message, int severity = 0)
{
    if (northstarSystem)
        reinterpret_cast<WriteNorthstarLog>(northstarSystem->functions[0])(northstarSystem, 0, severity, const_cast<char*>(message));
}

static void WriteAbsoluteJump(unsigned char* destination, void* target)
{
    destination[0] = 0xff;
    destination[1] = 0x25;
    for (int index = 2; index < 6; ++index)
        destination[index] = 0;
    auto address = reinterpret_cast<NativeSize>(target);
    for (int index = 0; index < 8; ++index)
        destination[6 + index] = static_cast<unsigned char>(address >> (index * 8));
}

static bool HasInstructions(unsigned char* entry, const unsigned char* expected, int length)
{
    for (int index = 0; index < length; ++index)
        if (entry[index] != expected[index])
            return false;
    return true;
}

static bool InstallVerifiedHook(VerifiedNativeHook& hook, unsigned char* entry, void* replacement, int instructionLength = 15)
{
    if (instructionLength < 14 || instructionLength > 32)
        return false;
    hook.trampoline = static_cast<unsigned char*>(VirtualAlloc(nullptr, 64, 0x3000, 0x40));
    if (!hook.trampoline)
        return false;
    for (int index = 0; index < instructionLength; ++index)
    {
        hook.originalInstructions[index] = entry[index];
        hook.trampoline[index] = entry[index];
    }
    WriteAbsoluteJump(hook.trampoline + instructionLength, entry + instructionLength);
    NativeWord previousProtection;
    if (!VirtualProtect(entry, instructionLength, 0x40, &previousProtection))
    {
        VirtualFree(hook.trampoline, 0, 0x8000);
        hook.trampoline = nullptr;
        return false;
    }
    WriteAbsoluteJump(entry, replacement);
    volatile unsigned char* patchedInstructions = entry;
    for (int index = 14; index < instructionLength; ++index)
        patchedInstructions[index] = 0x90;
    NativeWord discardedProtection;
    VirtualProtect(entry, instructionLength, previousProtection, &discardedProtection);
    FlushInstructionCache(GetCurrentProcess(), entry, instructionLength);
    hook.entry = entry;
    hook.instructionLength = instructionLength;
    return true;
}

static bool RemoveVerifiedHook(VerifiedNativeHook& hook)
{
    if (hook.entry)
    {
        NativeWord previousProtection;
        if (!VirtualProtect(hook.entry, hook.instructionLength, 0x40, &previousProtection))
            return false;
        for (int index = 0; index < hook.instructionLength; ++index)
            hook.entry[index] = hook.originalInstructions[index];
        NativeWord discardedProtection;
        VirtualProtect(hook.entry, hook.instructionLength, previousProtection, &discardedProtection);
        FlushInstructionCache(GetCurrentProcess(), hook.entry, hook.instructionLength);
        hook.entry = nullptr;
    }
    if (hook.trampoline)
        VirtualFree(hook.trampoline, 0, 0x8000);
    hook.trampoline = nullptr;
    return true;
}

static void ConfigureWaistFadeBand(unsigned char* body, const char* modelName)
{
    const char* filename = modelName;
    while (*filename)
    {
        if (*filename++ == '/')
            modelName = filename;
    }
    if (modelName[0] != 'f' || modelName[1] < '0' || modelName[1] > '1' || modelName[2] < '0' || modelName[2] > '9' || modelName[3] != '_')
        return;
    int band = (modelName[1] - '0') * 10 + modelName[2] - '0';
    if (band >= 16)
        return;
    float progress = (band + 0.5f) / 16.0f;
    float opacity = 1.0f - progress * progress * (3.0f - 2.0f * progress);
    setClientRenderMode(body, TranslucentTextureRenderMode);
    body[EntityRenderAlphaOffset] = static_cast<unsigned char>(opacity * 255.0f + 0.5f);
}

static void* CreateClientVisualPilotBody(void* pilot, const char* modelName)
{
    auto engine = static_cast<unsigned char*>(GetModuleHandleA("engine.dll"));
    auto modelLoader = *reinterpret_cast<NativeInterface**>(engine + 0x7c4ac0);
    using LoadClientVisualModel = void* (*)(void*, const char*);
    void* visualModel = reinterpret_cast<LoadClientVisualModel>(modelLoader->functions[8])(modelLoader, modelName);
    if (!visualModel)
        return nullptr;
    void* body = clonePilotPose(pilot, getClientModelName(pilot));
    if (body && modelName && HasPrefix(modelName, "models/first_person_body/"))
    {
        using ClearVisualModelCache = void (*)(void*);
        reinterpret_cast<ClearVisualModelCache>(clientModuleBase + 0xfa040)(body);
        auto visualEntity = static_cast<unsigned char*>(body);
        *reinterpret_cast<void**>(visualEntity + 0x40) = visualModel;
        *reinterpret_cast<const char**>(visualEntity + 0x7b8) = static_cast<const char*>(visualModel) + 4;
        auto entityInterface = static_cast<NativeInterface*>(body);
        reinterpret_cast<ClearVisualModelCache>(entityInterface->functions[74])(body);
        resetVisualSequence(body, *reinterpret_cast<int*>(static_cast<unsigned char*>(pilot) + PilotSequenceOffset));
        localPilot = pilot;
        FirstPersonBodyNativeState.pilot = pilot;
        if (!HasPrefix(modelName, "models/first_person_body/f"))
        {
            visualMainBody = body;
            visualActivity = -1;
            visualSequence = -1;
            visualCycle = 0.0f;
            crouchedSlideAnchorValid = false;
            standingHipAnchorValid = false;
        }
        *reinterpret_cast<unsigned int*>(static_cast<unsigned char*>(body) + EntityEffectsOffset) |= SharedPilotBonesEffect | DisableProxyShadowEffect;
        ConfigureWaistFadeBand(static_cast<unsigned char*>(body), modelName);
    }
    return body;
}

static int DrawBodyWithWaistFade(void* renderable, void* renderContext, int flags, const BodyRenderInstance* instance)
{
    auto drawBody = reinterpret_cast<DrawClientBody>(drawHook.trampoline);
    auto body = static_cast<unsigned char*>(renderable) - 0x10;
    if (body == localPilot)
    {
        FirstPersonBodyNativeState.pilotDrawFlags = flags;
        if (visualShadowBody && (flags & 0x2c000000))
        {
            auto shadowEffects = reinterpret_cast<unsigned int*>(static_cast<unsigned char*>(visualShadowBody) + EntityEffectsOffset);
            unsigned int savedEffects = *shadowEffects;
            *shadowEffects &= ~DisableProxyShadowEffect;
            int result = drawBody(static_cast<unsigned char*>(visualShadowBody) + 0x10, renderContext, flags, instance);
            *shadowEffects = savedEffects;
            FirstPersonBodyNativeState.shadowDepthDrawReplacements++;
            return result;
        }
    }
    if (body == visualShadowBody)
    {
        FirstPersonBodyNativeState.shadowDrawFlags = flags;
        if (flags & 4)
        {
            FirstPersonBodyNativeState.shadowColorDrawsSuppressed++;
            flags &= ~4;
        }
        FirstPersonBodyNativeState.shadowDrawSubmissions++;
        return drawBody(renderable, renderContext, flags, instance);
    }
    unsigned int effects = *reinterpret_cast<unsigned int*>(body + EntityEffectsOffset);
    if (!(effects & DisableProxyShadowEffect) || !instance)
        return drawBody(renderable, renderContext, flags, instance);
    const char* modelName = getClientModelName(body);
    if (!modelName || !HasPrefix(modelName, "models/first_person_body/f"))
        return drawBody(renderable, renderContext, flags, instance);
    BodyRenderInstance fadedInstance = *instance;
    fadedInstance.alpha *= body[EntityRenderAlphaOffset] / 255.0f;
    if (!FirstPersonBodyNativeState.fadeDrawSubmissions || fadedInstance.alpha < FirstPersonBodyNativeState.smallestSubmittedAlpha)
        FirstPersonBodyNativeState.smallestSubmittedAlpha = fadedInstance.alpha;
    if (fadedInstance.alpha > FirstPersonBodyNativeState.largestSubmittedAlpha)
        FirstPersonBodyNativeState.largestSubmittedAlpha = fadedInstance.alpha;
    FirstPersonBodyNativeState.fadeDrawSubmissions++;
    FirstPersonBodyNativeState.lastFadeDrawFlags = flags;
    if (flags & 4)
        FirstPersonBodyNativeState.alphaModulatedDrawSubmissions++;
    return drawBody(renderable, renderContext, flags, &fadedInstance);
}

static bool SetupBodyBonesWithTranslationLock(void* renderable, NativeBoneMatrix* destination, int maximumBones, int boneMask, float renderTime)
{
    auto setupBones = reinterpret_cast<SetupClientBodyBones>(poseHook.trampoline);
    auto body = static_cast<unsigned char*>(renderable) - 0x10;
    unsigned int effects = *reinterpret_cast<unsigned int*>(body + EntityEffectsOffset);
    const char* modelName = effects & DisableProxyShadowEffect ? getClientModelName(body) : nullptr;
    bool visibleBody = modelName && HasPrefix(modelName, "models/first_person_body/");
    bool ownedBody = visibleBody || body == visualShadowBody;
    bool ready = setupBones(renderable, destination, maximumBones, ownedBody ? boneMask | CompletePilotPoseBoneMask : boneMask, renderTime);
    if (ready && visibleBody && (effects & SharedPilotBonesEffect) && localPilot)
    {
        bool pilotReady = setupBones(static_cast<unsigned char*>(localPilot) + 0x10, nullptr, 256, boneMask | CompletePilotPoseBoneMask, renderTime);
        auto bodyHeader = *reinterpret_cast<unsigned char**>(body + StudioHeaderOffset);
        auto pilotHeader = *reinterpret_cast<unsigned char**>(static_cast<unsigned char*>(localPilot) + StudioHeaderOffset);
        auto bodyBones = *reinterpret_cast<NativeBoneMatrix**>(body + CachedBoneMatricesOffset);
        auto pilotBones = *reinterpret_cast<NativeBoneMatrix**>(static_cast<unsigned char*>(localPilot) + CachedBoneMatricesOffset);
        if (pilotReady && bodyHeader && pilotHeader && bodyBones && pilotBones)
        {
            auto bodyModelHeader = *reinterpret_cast<unsigned char**>(bodyHeader + 8);
            auto pilotModelHeader = *reinterpret_cast<unsigned char**>(pilotHeader + 8);
            if (bodyModelHeader && pilotModelHeader)
            {
                int boneCount = *reinterpret_cast<int*>(bodyModelHeader + 160);
                if (boneCount > 0 && boneCount <= 256 && boneCount == *reinterpret_cast<int*>(pilotModelHeader + 160))
                {
                    for (int bone = 0; bone < boneCount; ++bone)
                    {
                        bodyBones[bone] = pilotBones[bone];
                        if (destination && destination != bodyBones && bone < maximumBones)
                            destination[bone] = pilotBones[bone];
                    }
                    if (body == visualMainBody)
                        FirstPersonBodyNativeState.completePilotPoseCopies++;
                }
            }
        }
    }
    if (ready && body == visualShadowBody && visualMainBody)
    {
        SetupBodyBonesWithTranslationLock(static_cast<unsigned char*>(visualMainBody) + 0x10, nullptr, 256, boneMask, renderTime);
        auto shadowBones = *reinterpret_cast<NativeBoneMatrix**>(body + CachedBoneMatricesOffset);
        auto visibleBones = *reinterpret_cast<NativeBoneMatrix**>(static_cast<unsigned char*>(visualMainBody) + CachedBoneMatricesOffset);
        auto shadowHeader = *reinterpret_cast<unsigned char**>(body + StudioHeaderOffset);
        auto visibleHeader = *reinterpret_cast<unsigned char**>(static_cast<unsigned char*>(visualMainBody) + StudioHeaderOffset);
        if (shadowBones && visibleBones && shadowHeader && visibleHeader)
        {
            auto shadowModelHeader = *reinterpret_cast<unsigned char**>(shadowHeader + 8);
            auto visibleModelHeader = *reinterpret_cast<unsigned char**>(visibleHeader + 8);
            int boneCount = *reinterpret_cast<int*>(shadowModelHeader + 160);
            if (boneCount > 0 && boneCount <= 256 && boneCount == *reinterpret_cast<int*>(visibleModelHeader + 160))
                for (int bone = 0; bone < boneCount; ++bone)
                {
                    shadowBones[bone] = visibleBones[bone];
                    if (destination && destination != shadowBones && bone < maximumBones)
                        destination[bone] = visibleBones[bone];
                }
            float largestDifference = 0.0f;
            for (int component = 0; component < 12; ++component)
            {
                float difference = shadowBones[PilotHipBone].values[component] - visibleBones[PilotHipBone].values[component];
                if (difference < 0.0f)
                    difference = -difference;
                if (difference > largestDifference)
                    largestDifference = difference;
            }
            FirstPersonBodyNativeState.shadowHipPoseDifference = largestDifference;
            FirstPersonBodyNativeState.shadowPoseSubmissions++;
        }
    }
    if (ready && visibleBody && localPilot && (effects & SharedPilotBonesEffect))
    {
        auto bodyHeader = *reinterpret_cast<unsigned char**>(body + StudioHeaderOffset);
        auto bodyBones = *reinterpret_cast<NativeBoneMatrix**>(body + CachedBoneMatricesOffset);
        auto modelHeader = bodyHeader ? *reinterpret_cast<unsigned char**>(bodyHeader + 8) : nullptr;
        int boneCount = modelHeader ? *reinterpret_cast<int*>(modelHeader + 160) : 0;
        if (bodyBones && boneCount > PilotHipBone && boneCount <= 256)
        {
            NativeVector camera = *getPilotCameraPosition(localPilot);
            float hipBelowCamera = camera.z - bodyBones[PilotHipBone].values[11];
            if (body == visualMainBody && !FirstPersonBodyNativeState.airborne && !FirstPersonBodyNativeState.crouchHeld)
            {
                standingHipBelowCamera = hipBelowCamera;
                standingHipAnchorValid = true;
            }
            if (FirstPersonBodyNativeState.falling && !FirstPersonBodyNativeState.crouchHeld)
            {
                if (!standingHipAnchorValid)
                {
                    standingHipBelowCamera = hipBelowCamera;
                    standingHipAnchorValid = true;
                }
                float correctionZ = camera.z - standingHipBelowCamera - bodyBones[PilotHipBone].values[11];
                for (int bone = 0; bone < boneCount; ++bone)
                {
                    bodyBones[bone].values[11] += correctionZ;
                    if (destination && destination != bodyBones && bone < maximumBones)
                        destination[bone] = bodyBones[bone];
                }
                if (body == visualMainBody)
                {
                    FirstPersonBodyNativeState.fallingTranslationLockSubmissions++;
                    FirstPersonBodyNativeState.fallingLockedHipBelowCamera = standingHipBelowCamera;
                    FirstPersonBodyNativeState.fallingDrawnHipBelowCamera = camera.z - bodyBones[PilotHipBone].values[11];
                    FirstPersonBodyNativeState.fallingHipCorrectionZ = correctionZ;
                }
            }
        }
    }
    if (!ready || visualActivity != MultiplayerCrouchSlideActivity || visualSequence == -1 || !localPilot)
        return ready;
    if ((effects & SharedPilotBonesEffect) || !(effects & DisableProxyShadowEffect))
        return ready;
    if (!modelName || !HasPrefix(modelName, "models/first_person_body/"))
        return ready;
    auto studioHeader = *reinterpret_cast<unsigned char**>(body + StudioHeaderOffset);
    auto boneMatrices = *reinterpret_cast<NativeBoneMatrix**>(body + CachedBoneMatricesOffset);
    if (!studioHeader || !boneMatrices)
        return ready;
    auto modelHeader = *reinterpret_cast<unsigned char**>(studioHeader + 8);
    if (!modelHeader)
        return ready;
    int boneCount = *reinterpret_cast<int*>(modelHeader + 160);
    if (boneCount <= PilotHipBone || boneCount > 256)
        return ready;
    NativeBoneMatrix root = boneMatrices[PilotRootBone];
    NativeBoneMatrix hip = boneMatrices[PilotHipBone];
    auto pilotInterface = static_cast<NativeInterface*>(localPilot);
    NativeVector origin = *reinterpret_cast<GetPilotOrigin>(pilotInterface->functions[8])(localPilot);
    NativeVector camera = *getPilotCameraPosition(localPilot);
    if (!crouchedSlideAnchorValid)
    {
        NativeVector hipFromPilot = {hip.values[3] - origin.x, hip.values[7] - origin.y, hip.values[11] - origin.z};
        crouchedSlideHipInPilotFrame.x = root.values[0] * hipFromPilot.x + root.values[4] * hipFromPilot.y + root.values[8] * hipFromPilot.z;
        crouchedSlideHipInPilotFrame.y = root.values[1] * hipFromPilot.x + root.values[5] * hipFromPilot.y + root.values[9] * hipFromPilot.z;
        crouchedSlideHipInPilotFrame.z = root.values[2] * hipFromPilot.x + root.values[6] * hipFromPilot.y + root.values[10] * hipFromPilot.z;
        crouchedSlideHipBelowCamera = camera.z - hip.values[11];
        crouchedSlideAnchorValid = true;
    }
    NativeVector targetHip = {
        origin.x + root.values[0] * crouchedSlideHipInPilotFrame.x + root.values[1] * crouchedSlideHipInPilotFrame.y + root.values[2] * crouchedSlideHipInPilotFrame.z,
        origin.y + root.values[4] * crouchedSlideHipInPilotFrame.x + root.values[5] * crouchedSlideHipInPilotFrame.y + root.values[6] * crouchedSlideHipInPilotFrame.z,
        camera.z - crouchedSlideHipBelowCamera
    };
    NativeVector correction = {targetHip.x - hip.values[3], targetHip.y - hip.values[7], targetHip.z - hip.values[11]};
    for (int bone = 0; bone < boneCount; ++bone)
    {
        boneMatrices[bone].values[3] += correction.x;
        boneMatrices[bone].values[7] += correction.y;
        boneMatrices[bone].values[11] += correction.z;
        if (destination && destination != boneMatrices && bone < maximumBones)
            destination[bone] = boneMatrices[bone];
    }
    if (body == visualMainBody)
    {
        FirstPersonBodyNativeState.translationLocked = 1;
        FirstPersonBodyNativeState.translationLockSubmissions++;
        FirstPersonBodyNativeState.lockedHipBelowCamera = crouchedSlideHipBelowCamera;
        FirstPersonBodyNativeState.drawnHipBelowCamera = camera.z - boneMatrices[PilotHipBone].values[11];
        FirstPersonBodyNativeState.hipTranslationCorrectionX = correction.x;
        FirstPersonBodyNativeState.hipTranslationCorrectionY = correction.y;
        FirstPersonBodyNativeState.hipTranslationCorrectionZ = correction.z;
    }
    return ready;
}

struct SquirrelObject
{
    unsigned int type;
    int structure;
    NativeSize value;
};

struct SquirrelFunctionRegistration
{
    const char* squirrelName;
    const char* nativeName;
    const char* help;
    const char* returnTypeName;
    const char* arguments;
    unsigned int reserved;
    unsigned int developerLevel;
    const char* shortName;
    unsigned int reservedType;
    unsigned int returnType;
    void* externalBuffer;
    NativeSize externalBufferSize;
    NativeSize reservedBuffer;
    NativeSize reservedFunction;
    int (*callback)(void*);
};

using GetSquirrelObject = void (*)(void*, int, SquirrelObject*);
using GetSquirrelEntity = void* (*)(void*, SquirrelObject*, char**);
using GetSquirrelEntityType = char** (*)();
using GetSquirrelBool = unsigned int (*)(void*, int);
using GetSquirrelFloat = float (*)(void*, int);
using RegisterSquirrelFunction = long long (*)(void*, SquirrelFunctionRegistration*, char);
using DefineSquirrelConstant = void (*)(void*, const char*, int);
using CreateEntityScriptInstance = SquirrelObject* (*)(void*);
using PushSquirrelObject = void (*)(void*, SquirrelObject*);

static int CreateVisualBody(void* squirrelVm)
{
    if (!clientSquirrelVm || !clonePilotPose)
        return 0;
    SquirrelObject pilotObject;
    reinterpret_cast<GetSquirrelObject>(clientModuleBase + 0x6160)(squirrelVm, 1, &pilotObject);
    char** entityType = reinterpret_cast<GetSquirrelEntityType>(clientModuleBase + 0x3e49b0)();
    void* pilot = reinterpret_cast<GetSquirrelEntity>(clientModuleBase + 0x114f0)(clientSquirrelVm, &pilotObject, entityType);
    SquirrelObject modelObject;
    reinterpret_cast<GetSquirrelObject>(clientModuleBase + 0x6160)(squirrelVm, 2, &modelObject);
    const char* modelName = modelObject.type == 0x8000400 ? reinterpret_cast<const char*>(modelObject.value) + 0x30 : nullptr;
    if (!pilot || !modelName || !HasPrefix(modelName, "models/first_person_body/"))
        return 0;
    void* body = CreateClientVisualPilotBody(pilot, modelName);
    if (!body)
    {
        WritePluginLog("FirstPersonBody_ClientModelFactoryUnavailable", 2);
        return 0;
    }
    SquirrelObject* bodyObject = reinterpret_cast<CreateEntityScriptInstance>(clientModuleBase + 0xc20e0)(body);
    if (!bodyObject)
        return 0;
    reinterpret_cast<PushSquirrelObject>(clientModuleBase + 0x83d0)(squirrelVm, bodyObject);
    return 1;
}

static int CreateVisualShadow(void* squirrelVm)
{
    if (!clientSquirrelVm || !clonePilotPose)
        return 0;
    SquirrelObject pilotObject;
    reinterpret_cast<GetSquirrelObject>(clientModuleBase + 0x6160)(squirrelVm, 1, &pilotObject);
    char** entityType = reinterpret_cast<GetSquirrelEntityType>(clientModuleBase + 0x3e49b0)();
    void* pilot = reinterpret_cast<GetSquirrelEntity>(clientModuleBase + 0x114f0)(clientSquirrelVm, &pilotObject, entityType);
    if (!pilot)
        return 0;
    void* shadow = clonePilotPose(pilot, getClientModelName(pilot));
    if (!shadow)
        return 0;
    auto effects = reinterpret_cast<unsigned int*>(static_cast<unsigned char*>(shadow) + EntityEffectsOffset);
    *effects |= SharedPilotBonesEffect | DisableProxyShadowEffect;
    visualShadowBody = shadow;
    FirstPersonBodyNativeState.shadowBody = shadow;
    SquirrelObject* shadowObject = reinterpret_cast<CreateEntityScriptInstance>(clientModuleBase + 0xc20e0)(shadow);
    if (!shadowObject)
        return 0;
    reinterpret_cast<PushSquirrelObject>(clientModuleBase + 0x83d0)(squirrelVm, shadowObject);
    return 1;
}

static int ReleaseVisualBodies(void*)
{
    visualShadowBody = nullptr;
    visualMainBody = nullptr;
    localPilot = nullptr;
    visualActivity = -1;
    visualSequence = -1;
    crouchedSlideAnchorValid = false;
    standingHipAnchorValid = false;
    FirstPersonBodyNativeState.shadowBody = nullptr;
    FirstPersonBodyNativeState.visualBody = nullptr;
    FirstPersonBodyNativeState.pilot = nullptr;
    return 0;
}

static int RequestVisualActivity(void* squirrelVm)
{
    if (!clientSquirrelVm || !localPilot || !visualMainBody)
        return 0;
    SquirrelObject bodyObject;
    reinterpret_cast<GetSquirrelObject>(clientModuleBase + 0x6160)(squirrelVm, 1, &bodyObject);
    char** entityType = reinterpret_cast<GetSquirrelEntityType>(clientModuleBase + 0x3e49b0)();
    void* body = reinterpret_cast<GetSquirrelEntity>(clientModuleBase + 0x114f0)(clientSquirrelVm, &bodyObject, entityType);
    if (!body)
        return 0;
    const char* modelName = getClientModelName(body);
    if (!modelName || !HasPrefix(modelName, "models/first_person_body/"))
        return 0;
    auto getBool = reinterpret_cast<GetSquirrelBool>(clientModuleBase + 0x6130);
    auto getFloat = reinterpret_cast<GetSquirrelFloat>(clientModuleBase + 0x6100);
    bool crouchHeld = getBool(squirrelVm, 2) != 0;
    bool airborne = getBool(squirrelVm, 3) != 0;
    bool jumpRequested = getBool(squirrelVm, 4) != 0;
    float verticalVelocity = getFloat(squirrelVm, 5);
    bool walkingCrouchRequested = getBool(squirrelVm, 7) != 0;
    int sourceActivity = getSequenceActivity(localPilot, *reinterpret_cast<int*>(static_cast<unsigned char*>(localPilot) + PilotSequenceOffset));
    bool sourceJumpActivity = (sourceActivity >= MultiplayerJumpActivity && sourceActivity <= MultiplayerDoubleJumpActivity) || (sourceActivity >= MultiplayerWallJumpLeftActivity && sourceActivity <= MultiplayerWallJumpUpActivity) || (sourceActivity >= MultiplayerJumpPrimaryActivity && sourceActivity <= MultiplayerJumpLandPrimaryActivity) || (sourceActivity >= MultiplayerJumpMeleeActivity && sourceActivity <= MultiplayerJumpLandMeleeActivity);
    bool groundedCrouchWalk = !airborne && ((sourceActivity >= MultiplayerCrouchWalkActivity && sourceActivity <= MultiplayerStaggeredCrouchWalkRightActivity) || sourceActivity == MultiplayerCrouchWalkPrimaryActivity || sourceActivity == MultiplayerCrouchWalkMeleeActivity);
    bool groundedCrouchTransition = crouchHeld && walkingCrouchRequested && !airborne && !jumpRequested && !sourceJumpActivity;
    int activity = crouchHeld && !groundedCrouchWalk && !groundedCrouchTransition ? MultiplayerCrouchSlideActivity : -1;
    auto entity = static_cast<unsigned char*>(body);
    auto effects = reinterpret_cast<unsigned int*>(entity + EntityEffectsOffset);
    *effects |= DisableProxyShadowEffect;
    if (body == visualMainBody)
    {
        if (activity != visualActivity)
        {
            visualActivity = activity;
            visualCycle = 0.0f;
            NativeSize activityModifiers[4] = {};
            visualSequence = activity == -1 ? -1 : selectVisualSequence(body, activity, activityModifiers, -1);
            if (activity != -1 && visualSequence != -1)
                crouchedJumpSlideLogPending = true;
        }
        if (!crouchHeld)
            crouchedSlideAnchorValid = false;
        if (visualSequence != -1)
        {
            float frameDuration = getFloat(squirrelVm, 6);
            if (frameDuration > 0.0f && frameDuration < 0.25f)
                visualCycle += frameDuration * getVisualCycleRate(body, visualSequence);
            if (visualCycle > 1.0f)
                visualCycle = 1.0f;
            FirstPersonBodyNativeState.crouchedJumpSlideRequests++;
        }
        FirstPersonBodyNativeState.requestedActivity = sourceActivity;
        FirstPersonBodyNativeState.selectedActivity = visualSequence == -1 ? sourceActivity : activity;
        FirstPersonBodyNativeState.sourceSequenceActivity = sourceActivity;
        FirstPersonBodyNativeState.crouchHeld = crouchHeld;
        FirstPersonBodyNativeState.falling = airborne && verticalVelocity < 0.0f;
        FirstPersonBodyNativeState.airborne = airborne;
        FirstPersonBodyNativeState.jumpRequested = jumpRequested;
        FirstPersonBodyNativeState.sourceJumpActivity = sourceJumpActivity;
        FirstPersonBodyNativeState.groundedCrouchWalk = groundedCrouchWalk;
        FirstPersonBodyNativeState.groundedCrouchTransition = groundedCrouchTransition;
        if (visualSequence == -1)
            FirstPersonBodyNativeState.translationLocked = 0;
        FirstPersonBodyNativeState.verticalVelocity = verticalVelocity;
        FirstPersonBodyNativeState.visualCycle = visualCycle;
        FirstPersonBodyNativeState.visualSequence = visualSequence;
        FirstPersonBodyNativeState.visualBody = body;
    }
    if (visualSequence != -1)
    {
        if ((*effects & SharedPilotBonesEffect) || *reinterpret_cast<int*>(entity + PilotSequenceOffset) != visualSequence)
            resetVisualSequence(body, visualSequence);
        *effects &= ~SharedPilotBonesEffect;
        *reinterpret_cast<unsigned int*>(entity + CopiedPilotPoseLayerCountOffset) = 0;
        setVisualCycle(body, visualCycle);
    }
    else if (!(*effects & SharedPilotBonesEffect))
    {
        *effects |= SharedPilotBonesEffect;
        setVisualCycle(body, 0.0f);
    }
    if (body == visualMainBody)
    {
        FirstPersonBodyNativeState.sequenceActivity = getSequenceActivity(body, *reinterpret_cast<int*>(entity + PilotSequenceOffset));
        FirstPersonBodyNativeState.sharedPilotBones = (*effects & SharedPilotBonesEffect) != 0;
        FirstPersonBodyNativeState.copiedPilotPoseLayers = *reinterpret_cast<unsigned int*>(entity + CopiedPilotPoseLayerCountOffset);
    }
    return 0;
}

static void InstallClientVisualBodyRuntime(NativeModule clientModule)
{
    if (drawHook.entry || !clientModule)
        return;
    auto client = static_cast<unsigned char*>(clientModule);
    const unsigned char cloneInstructions[15] = {0x48,0x89,0x5c,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57,0x48,0x83,0xec,0x30};
    const unsigned char renderModeInstructions[13] = {0x40,0x53,0x48,0x83,0xec,0x20,0x0f,0xb6,0x81,0xf1,0x03,0x00,0x00};
    const unsigned char drawInstructions[15] = {0x48,0x89,0x74,0x24,0x10,0x48,0x89,0x7c,0x24,0x18,0x4c,0x89,0x64,0x24,0x20};
    const unsigned char sequenceInstructions[15] = {0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x6c,0x24,0x10,0x48,0x89,0x74,0x24,0x18};
    const unsigned char poseInstructions[21] = {0x44,0x89,0x44,0x24,0x18,0x48,0x89,0x54,0x24,0x10,0x55,0x53,0x56,0x48,0x8d,0xac,0x24,0x70,0xd4,0xff,0xff};
    const unsigned char shadowBatchInstructions[19] = {0x40,0x55,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,0x48,0x83,0xec,0x70,0x48,0x8d,0x6c,0x24,0x30};
    if (!HasInstructions(client + 0xf1660, cloneInstructions, 15) || !HasInstructions(client + 0x3f1170, renderModeInstructions, 13) || !HasInstructions(client + 0xf2c60, drawInstructions, 15) || !HasInstructions(client + 0xad8a0, sequenceInstructions, 15) || !HasInstructions(client + 0xfe760, poseInstructions, 21) || !HasInstructions(client + 0x26b9f0, shadowBatchInstructions, 19))
    {
        WritePluginLog("FirstPersonBody_UnsupportedClient", 2);
        return;
    }
    setClientRenderMode = reinterpret_cast<SetClientRenderMode>(client + 0x3f1170);
    getClientModelName = reinterpret_cast<GetClientModelName>(client + 0xc1be0);
    getSequenceActivity = reinterpret_cast<GetSequenceActivity>(client + 0xf6210);
    selectVisualSequence = reinterpret_cast<SelectVisualSequence>(client + 0xad8a0);
    resetVisualSequence = reinterpret_cast<ResetVisualSequence>(client + 0xadb70);
    setVisualCycle = reinterpret_cast<SetVisualCycle>(client + 0xadb20);
    getVisualCycleRate = reinterpret_cast<GetVisualCycleRate>(client + 0xab4b0);
    getPilotCameraPosition = reinterpret_cast<GetPilotCameraPosition>(client + 0x2c2100);
    clientModuleBase = client;
    clonePilotPose = reinterpret_cast<ClonePilotPose>(client + 0xf1660);
    if (!InstallVerifiedHook(drawHook, client + 0xf2c60, reinterpret_cast<void*>(DrawBodyWithWaistFade)))
    {
        return;
    }
    if (!InstallVerifiedHook(poseHook, client + 0xfe760, reinterpret_cast<void*>(SetupBodyBonesWithTranslationLock), 21))
    {
        RemoveVerifiedHook(drawHook);
        return;
    }
    if (!InstallVerifiedHook(shadowBatchHook, client + 0x26b9f0, reinterpret_cast<void*>(DrawShadowBatchWithVisualPilotPose), 19))
    {
        RemoveVerifiedHook(drawHook);
        RemoveVerifiedHook(poseHook);
        return;
    }
    readinessLogPending = true;
}

static const char* PluginStringValue(void*, int field)
{
    if (field == 0)
        return FirstPersonBodyPluginName;
    if (field == 1)
        return "BODYBONES";
    return "FirstPersonBodyBones";
}

static long long PluginFieldValue(void*, int field)
{
    return field == 0 ? 2 : 0;
}

static void InitializePlugin(void*, NativeModule northstarModule, const void*, bool)
{
    auto createInterface = reinterpret_cast<CreateNorthstarInterface>(GetProcAddress(northstarModule, "CreateInterface"));
    if (createInterface)
        northstarSystem = static_cast<NativeInterface*>(createInterface("NSSys001", nullptr));
    InstallClientVisualBodyRuntime(GetModuleHandleA("client.dll"));
}

static void FinalizePlugin(void*)
{
    InstallClientVisualBodyRuntime(GetModuleHandleA("client.dll"));
}

static bool UnloadPlugin(void*)
{
    if (clientSquirrelVm)
        return false;
    bool drawRestored = RemoveVerifiedHook(drawHook);
    bool poseRestored = RemoveVerifiedHook(poseHook);
    bool shadowBatchRestored = RemoveVerifiedHook(shadowBatchHook);
    localPilot = nullptr;
    visualMainBody = nullptr;
    visualShadowBody = nullptr;
    clientSquirrelVm = nullptr;
    return drawRestored && poseRestored && shadowBatchRestored;
}

static SquirrelFunctionRegistration createVisualBodyRegistration = {
    "FirstPersonBody_CreateVisualBody", "CreateVisualBody", "", "entity",
    "entity pilot, asset bodyModel",
    0, 0, nullptr, 0, 0xd, nullptr, 0, 0, 0, CreateVisualBody
};

static SquirrelFunctionRegistration createVisualShadowRegistration = {
    "FirstPersonBody_CreateVisualShadow", "CreateVisualShadow", "", "entity",
    "entity pilot",
    0, 0, nullptr, 0, 0xd, nullptr, 0, 0, 0, CreateVisualShadow
};

static SquirrelFunctionRegistration releaseVisualBodiesRegistration = {
    "FirstPersonBody_ReleaseVisualBodies", "ReleaseVisualBodies", "", "void",
    "",
    0, 0, nullptr, 0, 0x20, nullptr, 0, 0, 0, ReleaseVisualBodies
};

static SquirrelFunctionRegistration visualActivityRegistration = {
    "FirstPersonBody_RequestVisualActivity", "RequestVisualActivity", "", "void",
    "entity bodyPart, bool crouchHeld, bool airborne, bool jumpRequested, float verticalVelocity, float frameDuration, bool walkingCrouchRequested",
    0, 0, nullptr, 0, 0x20, nullptr, 0, 0, 0, RequestVisualActivity
};

static void SquirrelVmCreated(void*, void* squirrelVm)
{
    if (clientModuleBase && *reinterpret_cast<int*>(static_cast<unsigned char*>(squirrelVm) + 0x3c) == 1)
    {
        clientSquirrelVm = squirrelVm;
        reinterpret_cast<RegisterSquirrelFunction>(clientModuleBase + 0x108e0)(squirrelVm, &createVisualBodyRegistration, 1);
        reinterpret_cast<RegisterSquirrelFunction>(clientModuleBase + 0x108e0)(squirrelVm, &createVisualShadowRegistration, 1);
        reinterpret_cast<RegisterSquirrelFunction>(clientModuleBase + 0x108e0)(squirrelVm, &releaseVisualBodiesRegistration, 1);
        reinterpret_cast<RegisterSquirrelFunction>(clientModuleBase + 0x108e0)(squirrelVm, &visualActivityRegistration, 1);
        bool nativeReady = drawHook.entry && poseHook.entry && shadowBatchHook.entry && clonePilotPose;
        reinterpret_cast<DefineSquirrelConstant>(clientModuleBase + 0x12120)(squirrelVm, "FirstPersonBodyNativeReady", nativeReady ? 1 : 0);
    }
}

static void SquirrelVmDestroying(void*, void* squirrelVm)
{
    if (squirrelVm == clientSquirrelVm)
    {
        clientSquirrelVm = nullptr;
        visualMainBody = nullptr;
        visualShadowBody = nullptr;
        localPilot = nullptr;
    }
}

static void LibraryLoaded(void*, NativeModule module, const char* name)
{
    if (SameText(name, "client.dll"))
        InstallClientVisualBodyRuntime(module);
}

static void RunPluginFrame(void*)
{
    if (readinessLogPending)
    {
        WritePluginLog("FirstPersonBody_CompletePilotPoseAndMatchingFullBodyShadowReady");
        readinessLogPending = false;
    }
    if (crouchedJumpSlideLogPending)
    {
        WritePluginLog("FirstPersonBody_CrouchedJumpRequestedCrouchSlide");
        crouchedJumpSlideLogPending = false;
    }
}

static void* pluginIdentifierFunctions[] = {reinterpret_cast<void*>(PluginStringValue), reinterpret_cast<void*>(PluginFieldValue)};
static void* pluginCallbackFunctions[] = {
    reinterpret_cast<void*>(InitializePlugin),
    reinterpret_cast<void*>(FinalizePlugin),
    reinterpret_cast<void*>(UnloadPlugin),
    reinterpret_cast<void*>(SquirrelVmCreated),
    reinterpret_cast<void*>(SquirrelVmDestroying),
    reinterpret_cast<void*>(LibraryLoaded),
    reinterpret_cast<void*>(RunPluginFrame)
};
static NativeInterface pluginIdentifier = {pluginIdentifierFunctions};
static NativeInterface pluginCallbacks = {pluginCallbackFunctions};

extern "C" __declspec(dllexport) void* CreateInterface(const char* name, int* status)
{
    if (status)
        *status = 0;
    if (SameText(name, "PluginId001"))
        return &pluginIdentifier;
    if (SameText(name, "PluginCallbacks001"))
        return &pluginCallbacks;
    if (status)
        *status = 1;
    return nullptr;
}

extern "C" int __stdcall DllMain(NativeModule, NativeWord, void*)
{
    return 1;
}
