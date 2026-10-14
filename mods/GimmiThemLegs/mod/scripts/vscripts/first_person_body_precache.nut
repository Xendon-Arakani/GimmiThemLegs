global function FirstPersonBody_Precache

void function FirstPersonBody_Precache()
{
    foreach ( asset pilotModel in FirstPersonBody_GetPilotModels() )
        foreach ( asset bodyModel in FirstPersonBody_GetModels( pilotModel ) )
            PrecacheModel( bodyModel )
    print( "FirstPersonBody_FadingModelsPrecached" )
}
