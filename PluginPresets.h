#pragma once

// 60 presets : 20 SKANK, 20 SUB, 20 DUB FX
// Ordre des valeurs :
//   skankLevel, pluckMix (1 = acoustique), skankDecay ms, skankBright,
//   subLevel, subTone Hz, subDrive, subTri, subOct, subGlide ms, subPunch, splitNote,
//   delaySync (0 Free, 1 1/4, 2 dotted 1/8, 3 1/8), delayTime ms, delayFB, delayMix,
//   delayWobble, reverbMix, reverbSize, master

struct Preset
{
    const char* category;
    const char* name;
    float skankLevel, pluckMix, skankDecay, skankBright;
    float subLevel, subTone, subDrive, subTri, subOct, subGlide, subPunch;
    int   splitNote;
    int   delaySync;
    float delayTime, delayFB, delayMix, delayWobble;
    float reverbMix, reverbSize, master;
};

// Sub "d'accompagnement" pour les presets de skank et de FX
#define SKANK_SUB 0.6f, 300.f, 0.2f, 0.3f, 0.2f, 20.f, 0.3f, 52
// Presets de sub pur : skank coupe, tout le clavier joue le sub
#define NO_SKANK  0.f, 0.5f, 100.f, 0.5f
#define NO_FX     2, 450.f, 0.5f, 0.f, 0.2f, 0.f, 0.5f
// Skank de base pour les presets de FX
#define FX_SKANK  0.75f, 0.6f, 110.f, 0.55f

inline constexpr Preset kPresets[] =
{
    //================================= SKANK (20) =================================
    { "SKANK", "Roots Skank",          0.75f, 0.60f, 110.f, 0.55f, SKANK_SUB, 2, 450.f, 0.45f, 0.30f, 0.20f, 0.15f, 0.50f, 0.8f },
    { "SKANK", "Studio One Chop",      0.75f, 0.45f,  85.f, 0.50f, SKANK_SUB, 2, 450.f, 0.40f, 0.25f, 0.20f, 0.10f, 0.40f, 0.8f },
    { "SKANK", "Dry Upstroke",         0.80f, 0.50f,  60.f, 0.60f, SKANK_SUB, 2, 450.f, 0.20f, 0.10f, 0.10f, 0.05f, 0.30f, 0.8f },
    { "SKANK", "Lovers Rock Warm",     0.70f, 0.80f, 180.f, 0.35f, 0.6f, 260.f, 0.15f, 0.25f, 0.25f, 30.f, 0.2f, 52,
                                       1, 450.f, 0.35f, 0.20f, 0.15f, 0.25f, 0.60f, 0.8f },
    { "SKANK", "Steppers Tight",       0.80f, 0.30f,  70.f, 0.70f, 0.65f, 350.f, 0.25f, 0.3f, 0.15f, 15.f, 0.35f, 52,
                                       3, 450.f, 0.35f, 0.25f, 0.20f, 0.10f, 0.40f, 0.8f },
    { "SKANK", "Rub-a-Dub Electric",   0.75f, 0.15f,  95.f, 0.65f, SKANK_SUB, 2, 450.f, 0.50f, 0.35f, 0.30f, 0.15f, 0.50f, 0.8f },
    { "SKANK", "Nylon Skank",          0.70f, 1.00f, 130.f, 0.30f, SKANK_SUB, 2, 450.f, 0.35f, 0.15f, 0.15f, 0.20f, 0.50f, 0.8f },
    { "SKANK", "Steel String Bright",  0.70f, 0.95f, 150.f, 0.85f, SKANK_SUB, 2, 450.f, 0.40f, 0.25f, 0.20f, 0.15f, 0.50f, 0.8f },
    { "SKANK", "Muted Palm",           0.85f, 0.70f,  45.f, 0.25f, SKANK_SUB, 2, 450.f, 0.25f, 0.10f, 0.10f, 0.05f, 0.30f, 0.8f },
    { "SKANK", "Clav Pulse",           0.75f, 0.05f,  90.f, 0.80f, SKANK_SUB, 3, 450.f, 0.40f, 0.20f, 0.20f, 0.10f, 0.40f, 0.8f },
    { "SKANK", "Bubble Pluck",         0.75f, 0.35f, 120.f, 0.50f, SKANK_SUB, 3, 450.f, 0.50f, 0.30f, 0.25f, 0.20f, 0.50f, 0.8f },
    { "SKANK", "Dub Chop Echo",        0.75f, 0.50f, 100.f, 0.55f, SKANK_SUB, 2, 450.f, 0.75f, 0.55f, 0.35f, 0.20f, 0.50f, 0.8f },
    { "SKANK", "Telephone Skank",      0.70f, 0.40f,  80.f, 0.15f, SKANK_SUB, 2, 450.f, 0.30f, 0.20f, 0.20f, 0.05f, 0.30f, 0.8f },
    { "SKANK", "Jangle Skank",         0.70f, 0.85f, 100.f, 0.95f, SKANK_SUB, 2, 450.f, 0.35f, 0.20f, 0.15f, 0.15f, 0.50f, 0.8f },
    { "SKANK", "Mellow Offbeat",       0.70f, 0.60f, 200.f, 0.30f, SKANK_SUB, 1, 450.f, 0.40f, 0.20f, 0.15f, 0.30f, 0.60f, 0.8f },
    { "SKANK", "Sharp Stab",           0.80f, 0.20f,  55.f, 0.90f, SKANK_SUB, 3, 450.f, 0.30f, 0.15f, 0.10f, 0.05f, 0.30f, 0.8f },
    { "SKANK", "Spring Skank",         0.70f, 0.65f, 110.f, 0.50f, SKANK_SUB, 2, 450.f, 0.35f, 0.15f, 0.15f, 0.45f, 0.80f, 0.8f },
    { "SKANK", "Lazy Riddim",          0.70f, 0.60f, 160.f, 0.40f, SKANK_SUB, 1, 450.f, 0.50f, 0.30f, 0.40f, 0.20f, 0.50f, 0.8f },
    { "SKANK", "Wide Skank",           0.75f, 0.55f, 120.f, 0.60f, SKANK_SUB, 0, 380.f, 0.50f, 0.40f, 0.25f, 0.30f, 0.70f, 0.8f },
    { "SKANK", "Heavy Skank",          0.90f, 0.40f, 100.f, 0.50f, 0.8f, 260.f, 0.35f, 0.3f, 0.3f, 20.f, 0.4f, 52,
                                       2, 450.f, 0.50f, 0.30f, 0.20f, 0.20f, 0.50f, 0.8f },

    //================================== SUB (20) ==================================
    { "SUB", "Deep Roots",        NO_SKANK, 0.85f, 180.f,  0.05f, 0.10f, 0.35f,  20.f, 0.10f, 96, NO_FX, 0.80f },
    { "SUB", "Foundation",        NO_SKANK, 0.85f, 280.f,  0.20f, 0.30f, 0.25f,  30.f, 0.25f, 96, NO_FX, 0.80f },
    { "SUB", "Warm Round",        NO_SKANK, 0.85f, 220.f,  0.10f, 0.15f, 0.20f,  35.f, 0.10f, 96, NO_FX, 0.80f },
    { "SUB", "Stepper 808",       NO_SKANK, 0.85f, 500.f,  0.35f, 0.10f, 0.10f,  50.f, 0.85f, 96, NO_FX, 0.80f },
    { "SUB", "Boomy Dub",         NO_SKANK, 0.80f, 160.f,  0.25f, 0.20f, 0.50f,  60.f, 0.40f, 96, NO_FX, 0.80f },
    { "SUB", "Cabinet Sub",       NO_SKANK, 0.75f, 800.f,  0.70f, 0.40f, 0.20f,  30.f, 0.30f, 96, NO_FX, 0.80f },
    { "SUB", "Thick Triangle",    NO_SKANK, 0.80f, 350.f,  0.30f, 0.60f, 0.20f,  30.f, 0.20f, 96, NO_FX, 0.80f },
    { "SUB", "Gliding Slide",     NO_SKANK, 0.85f, 300.f,  0.20f, 0.20f, 0.20f, 150.f, 0.10f, 96, NO_FX, 0.80f },
    { "SUB", "Lazy Glide",        NO_SKANK, 0.85f, 260.f,  0.15f, 0.20f, 0.25f, 250.f, 0.10f, 96, NO_FX, 0.80f },
    { "SUB", "Subsonic Pressure", NO_SKANK, 1.00f, 120.f,  0.10f, 0.05f, 0.60f,  25.f, 0.15f, 96, NO_FX, 0.70f },
    { "SUB", "Tight Pluck Bass",  NO_SKANK, 0.85f, 600.f,  0.30f, 0.50f, 0.00f,   0.f, 0.60f, 96, NO_FX, 0.80f },
    { "SUB", "Clean Sine",        NO_SKANK, 0.90f, 200.f,  0.00f, 0.00f, 0.00f,  20.f, 0.05f, 96, NO_FX, 0.80f },
    { "SUB", "Rubber Bass",       NO_SKANK, 0.80f, 900.f,  0.50f, 0.70f, 0.10f,  40.f, 0.50f, 96, NO_FX, 0.80f },
    { "SUB", "Gritty Drive",      NO_SKANK, 0.70f, 1200.f, 0.85f, 0.50f, 0.15f,  30.f, 0.30f, 96, NO_FX, 0.75f },
    { "SUB", "Soft Round",        NO_SKANK, 0.90f, 150.f,  0.00f, 0.05f, 0.30f,  40.f, 0.05f, 96, NO_FX, 0.80f },
    { "SUB", "Kick Bass",         NO_SKANK, 0.85f, 300.f,  0.30f, 0.10f, 0.20f,   0.f, 1.00f, 96, NO_FX, 0.80f },
    { "SUB", "Big Octave",        NO_SKANK, 0.80f, 250.f,  0.20f, 0.20f, 0.80f,  25.f, 0.20f, 96, NO_FX, 0.75f },
    { "SUB", "Mid Growl",         NO_SKANK, 0.65f, 1500.f, 0.90f, 0.80f, 0.10f,  30.f, 0.30f, 96, NO_FX, 0.75f },
    { "SUB", "Walking Bass",      NO_SKANK, 0.85f, 450.f,  0.25f, 0.40f, 0.15f,  15.f, 0.20f, 96, NO_FX, 0.80f },
    { "SUB", "808 Long",          NO_SKANK, 0.85f, 260.f,  0.30f, 0.20f, 0.30f, 120.f, 0.70f, 96, NO_FX, 0.80f },

    //================================ DUB FX (20) =================================
    { "DUB FX", "Classic Dub Echo",     FX_SKANK, SKANK_SUB, 2,  450.f, 0.65f, 0.50f, 0.30f, 0.20f, 0.50f, 0.8f },
    { "DUB FX", "Tape Throw",           FX_SKANK, SKANK_SUB, 2,  450.f, 0.85f, 0.70f, 0.40f, 0.25f, 0.50f, 0.8f },
    { "DUB FX", "Infinite Feedback",    FX_SKANK, SKANK_SUB, 2,  450.f, 1.05f, 0.60f, 0.35f, 0.20f, 0.50f, 0.8f },
    { "DUB FX", "Slapback",             FX_SKANK, SKANK_SUB, 0,  110.f, 0.25f, 0.35f, 0.10f, 0.10f, 0.40f, 0.8f },
    { "DUB FX", "Quarter Note Dub",     FX_SKANK, SKANK_SUB, 1,  450.f, 0.60f, 0.45f, 0.25f, 0.20f, 0.50f, 0.8f },
    { "DUB FX", "Eighth Bounce",        FX_SKANK, SKANK_SUB, 3,  450.f, 0.55f, 0.40f, 0.20f, 0.15f, 0.50f, 0.8f },
    { "DUB FX", "Cathedral Dub",        FX_SKANK, SKANK_SUB, 2,  450.f, 0.50f, 0.30f, 0.20f, 0.60f, 0.95f, 0.8f },
    { "DUB FX", "Spring Splash",        FX_SKANK, SKANK_SUB, 2,  450.f, 0.30f, 0.15f, 0.10f, 0.45f, 0.40f, 0.8f },
    { "DUB FX", "Wobbly Tape",          FX_SKANK, SKANK_SUB, 2,  450.f, 0.60f, 0.50f, 0.90f, 0.20f, 0.50f, 0.8f },
    { "DUB FX", "Dark Tape",            0.75f, 0.60f, 110.f, 0.20f, SKANK_SUB, 2, 450.f, 0.70f, 0.50f, 0.40f, 0.20f, 0.50f, 0.8f },
    { "DUB FX", "Wash Out",             FX_SKANK, SKANK_SUB, 2,  450.f, 0.60f, 0.50f, 0.30f, 0.70f, 1.00f, 0.8f },
    { "DUB FX", "Short Dub Space",      FX_SKANK, SKANK_SUB, 0,  180.f, 0.45f, 0.40f, 0.20f, 0.30f, 0.50f, 0.8f },
    { "DUB FX", "Long Dub Echo",        FX_SKANK, SKANK_SUB, 0,  900.f, 0.55f, 0.50f, 0.30f, 0.20f, 0.50f, 0.8f },
    { "DUB FX", "Triple Echo",          FX_SKANK, SKANK_SUB, 0,  330.f, 0.60f, 0.50f, 0.25f, 0.20f, 0.50f, 0.8f },
    { "DUB FX", "Dub Siren Drift",      FX_SKANK, SKANK_SUB, 0,  600.f, 0.90f, 0.60f, 1.00f, 0.20f, 0.50f, 0.8f },
    { "DUB FX", "Rewind Echo",          FX_SKANK, SKANK_SUB, 0,  250.f, 1.00f, 0.65f, 0.60f, 0.20f, 0.50f, 0.8f },
    { "DUB FX", "Tight Echo",           FX_SKANK, SKANK_SUB, 0,   90.f, 0.35f, 0.30f, 0.10f, 0.10f, 0.30f, 0.8f },
    { "DUB FX", "Roots Radics Echo",    FX_SKANK, SKANK_SUB, 2,  450.f, 0.60f, 0.40f, 0.25f, 0.15f, 0.50f, 0.8f },
    { "DUB FX", "Space Echo 800",       FX_SKANK, SKANK_SUB, 0,  800.f, 0.65f, 0.55f, 0.35f, 0.35f, 0.70f, 0.8f },
    { "DUB FX", "Ghost Skank",          0.50f, 0.50f, 50.f, 0.55f, SKANK_SUB, 2, 450.f, 0.80f, 0.80f, 0.30f, 0.40f, 0.60f, 0.8f },
};

inline constexpr int kNumPresets = (int) (sizeof (kPresets) / sizeof (kPresets[0]));

#undef SKANK_SUB
#undef NO_SKANK
#undef NO_FX
#undef FX_SKANK
