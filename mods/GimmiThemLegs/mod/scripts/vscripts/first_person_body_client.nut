untyped

global function FirstPersonBody_Start
global function FirstPersonBody_Toggle
global function FirstPersonBody_PrintStatus

struct
{
    entity pilot
    entity shadowPilot
    entity shadowBody
    array<entity> bodyParts
    asset pilotModel
    bool visible = false
    int scriptUpdateFrames = 0
    int bodyCreations = 0
    int skin = -1
    int camo = -1
    int decal = -1
    bool crouchHeld = false
    bool airborne = false
    bool falling = false
    bool nativeReady = false
} file

void function FirstPersonBody_Start()
{
#if FirstPersonBodyNativeReady
    file.nativeReady = true
    print( "FirstPersonBody_ClientStarted" )
    thread FirstPersonBody_Update()
#else
    printt( "FirstPersonBody_NativePluginMissing", "FirstPersonBodyBones.dll" )
#endif
}

void function FirstPersonBody_Toggle()
{
    SetConVarInt( "first_person_body_enabled", GetConVarBool( "first_person_body_enabled" ) ? 0 : 1 )
    FirstPersonBody_PrintStatus()
}

void function FirstPersonBody_PrintStatus()
{
    printt( "FirstPersonBody_Status", "version", GetConVarString( "first_person_body_version" ), "enabled", GetConVarBool( "first_person_body_enabled" ), "nativeReady", file.nativeReady, "bodyParts", file.bodyParts.len(), "visible", file.visible, "crouchHeld", file.crouchHeld, "airborne", file.airborne, "falling", file.falling, "scriptUpdateFrames", file.scriptUpdateFrames, "bodyCreations", file.bodyCreations, "frameTime", FrameTime() )
    if ( file.bodyParts.len() && IsValid( file.pilot ) )
        printt( "FirstPersonBody_LowerBodyFollowing", file.pilot.GetModelName(), file.pilot.GetOrigin(), file.pilot.GetAngles() )
}

void function FirstPersonBody_Destroy()
{
#if FirstPersonBodyNativeReady
    FirstPersonBody_ReleaseVisualBodies()
#endif
    if ( IsValid( file.shadowBody ) )
        file.shadowBody.Destroy()
    file.shadowBody = null
    if ( IsValid( file.shadowPilot ) )
        file.shadowPilot.ForceShadowVisible( true )
    foreach ( entity bodyPart in file.bodyParts )
        if ( IsValid( bodyPart ) )
            bodyPart.Destroy()
    file.bodyParts.clear()
    file.pilot = null
    file.pilotModel = $""
    file.visible = false
    file.skin = -1
    file.camo = -1
    file.decal = -1
    file.crouchHeld = false
    file.airborne = false
    file.falling = false
}

void function FirstPersonBody_Update()
{
#if FirstPersonBodyNativeReady
    OnThreadEnd( FirstPersonBody_Destroy )
    while ( true )
    {
        WaitFrame()
        entity pilot = GetLocalClientPlayer()
        bool pilotEligible = IsValid( pilot ) && IsAlive( pilot ) && !pilot.IsTitan() && !pilot.IsObserver()
        if ( !pilotEligible )
        {
            FirstPersonBody_Destroy()
            continue
        }
        if ( file.shadowPilot != pilot )
        {
            pilot.ForceShadowVisible( true )
            file.shadowPilot = pilot
        }
        bool visible = GetConVarBool( "first_person_body_enabled" ) && GetLocalViewPlayer() == pilot && !IsWatchingReplay() && !pilot.ContextAction_IsActive()
        if ( !visible )
        {
            FirstPersonBody_Destroy()
            continue
        }
        asset pilotModel = pilot.GetModelName()
        array<asset> bodyModels = FirstPersonBody_GetModels( pilotModel )
        if ( !bodyModels.len() )
        {
            FirstPersonBody_Destroy()
            continue
        }
        if ( file.pilot != pilot || file.pilotModel != pilotModel || !file.bodyParts.len() || !IsValid( file.bodyParts[0] ) )
        {
            FirstPersonBody_Destroy()
            file.pilot = pilot
            file.pilotModel = pilotModel
            foreach ( asset bodyModel in bodyModels )
            {
                entity bodyPart = FirstPersonBody_CreateVisualBody( pilot, bodyModel )
                file.bodyParts.append( bodyPart )
                bodyPart.SetParent( pilot, "", false, 0.0 )
                bodyPart.EnableRenderAlways()
                bodyPart.Show()
            }
            file.shadowBody = FirstPersonBody_CreateVisualShadow( pilot )
            file.shadowBody.SetParent( file.bodyParts[0], "", false, 0.0 )
            file.shadowBody.EnableRenderAlways()
            file.shadowBody.SetVisibleForLocalPlayer( 0 )
            file.shadowBody.Show()
            pilot.ForceShadowVisible( true )
            file.bodyCreations++
            printt( "FirstPersonBody_FadingLowerBodyFollowing", pilotModel, "bodyParts", file.bodyParts.len() )
        }
        int skin = pilot.GetSkin()
        int camo = pilot.GetCamo()
        int decal = pilot.GetDecal()
        if ( skin != file.skin || camo != file.camo || decal != file.decal )
        {
            foreach ( entity bodyPart in file.bodyParts )
            {
                bodyPart.SetSkin( skin )
                bodyPart.SetCamo( camo )
                bodyPart.SetDecal( decal )
            }
            if ( IsValid( file.shadowBody ) )
            {
                file.shadowBody.SetSkin( skin )
                file.shadowBody.SetCamo( camo )
                file.shadowBody.SetDecal( decal )
            }
            file.skin = skin
            file.camo = camo
            file.decal = decal
        }
        vector velocity = pilot.GetVelocity()
        file.crouchHeld = pilot.IsInputCommandHeld( IN_DUCK ) || pilot.IsInputCommandHeld( IN_DUCKTOGGLE )
        file.airborne = !pilot.IsOnGround()
        file.falling = file.airborne && velocity.z < 0.0
        bool jumpRequested = pilot.IsInputCommandPressed( IN_JUMP )
        float horizontalSpeedSquared = velocity.x * velocity.x + velocity.y * velocity.y
        float slideStartSpeed = expect float( pilot.GetPlayerSettingsField( "slideRequiredStartSpeed" ) )
        bool walkingCrouchRequested = !file.airborne && horizontalSpeedSquared > 0.0 && horizontalSpeedSquared < slideStartSpeed * slideStartSpeed
        foreach ( entity bodyPart in file.bodyParts )
            FirstPersonBody_RequestVisualActivity( bodyPart, file.crouchHeld, file.airborne, jumpRequested, velocity.z, FrameTime(), walkingCrouchRequested )
        file.visible = true
        file.scriptUpdateFrames++
    }
#endif
}
