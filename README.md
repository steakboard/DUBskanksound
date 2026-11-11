# Dub Skank Sub — instrument virtuel reggae / dub

Plugin VST3 (et application standalone) pour FL Studio et Ableton Live.
Un seul instrument qui joue **les skanks** (guitare acoustique + électrique) **et le sub** selon la hauteur de la note, avec des **FX dub** intégrés.

## Compilation

Prérequis : CMake ≥ 3.22, Git, et un compilateur
(Windows : Visual Studio 2022 « Développement Desktop C++ » ; macOS : Xcode).
Internet est nécessaire au premier build (JUCE est téléchargé automatiquement).

```
cmake -B build
cmake --build build --config Release
```

Le plugin est généré dans :
`build/DubSkankSub_artefacts/Release/VST3/Dub Skank Sub.vst3`

Copie-le dans le dossier VST3 :
- Windows : `C:\Program Files\Common Files\VST3\`
- macOS : `/Library/Audio/Plug-Ins/VST3/`

Puis :
- **FL Studio** : Options > Gestionnaire de plugins > « Rechercher des plugins ».
- **Ableton Live** : Préférences > Plug-ins > activer « Utiliser les dossiers VST3 système » et rescanner.

## Visuel et interface

L'interface reprend la palette du visuel « sound system » : vert, jaune doré, noir d'encre et crème.
Les boutons sont dessinés comme des cônes de haut-parleur.

Pour afficher l'illustration, enregistre-la sous le nom **`soundsystem.png`** (ou `.jpg`) dans le même dossier que `CMakeLists.txt`, puis relance CMake (`cmake -B build`). Elle est intégrée dans le plugin. Sans fichier, l'interface affiche 4 cônes dessinés à la place.

Si l'illustration n'est pas de toi, vérifie que tu as les droits avant de distribuer le plugin.

## Presets (60)

Le sélecteur en haut à droite de l'interface (flèches `<` `>` pour naviguer) donne accès à 3 familles de 20 presets. Ils sont aussi exposés au format « programmes » de l'hôte, donc visibles dans la liste de presets de FL Studio et d'Ableton.

- **SKANK (20)** : Roots Skank, Studio One Chop, Dry Upstroke, Lovers Rock Warm, Steppers Tight, Rub-a-Dub Electric, Nylon Skank, Steel String Bright, Muted Palm, Clav Pulse, Bubble Pluck, Dub Chop Echo, Telephone Skank, Jangle Skank, Mellow Offbeat, Sharp Stab, Spring Skank, Lazy Riddim, Wide Skank, Heavy Skank.
- **SUB (20)** : Deep Roots, Foundation, Warm Round, Stepper 808, Boomy Dub, Cabinet Sub, Thick Triangle, Gliding Slide, Lazy Glide, Subsonic Pressure, Tight Pluck Bass, Clean Sine, Rubber Bass, Gritty Drive, Soft Round, Kick Bass, Big Octave, Mid Growl, Walking Bass, 808 Long. Le skank est coupé et tout le clavier joue le sub.
- **DUB FX (20)** : Classic Dub Echo, Tape Throw, Infinite Feedback, Slapback, Quarter Note Dub, Eighth Bounce, Cathedral Dub, Spring Splash, Wobbly Tape, Dark Tape, Wash Out, Short Dub Space, Long Dub Echo, Triple Echo, Dub Siren Drift, Rewind Echo, Tight Echo, Roots Radics Echo, Space Echo 800, Ghost Skank.

Les presets sont définis dans `PluginPresets.h` : une ligne par preset, les valeurs sont dans l'ordre indiqué en tête du fichier. Tu peux en modifier ou en ajouter facilement.

## Fonctionnement

Le clavier est coupé en deux par **Split Note** (défaut : Mi 3, note MIDI 52) :
- notes **en dessous** : voix de **sub** (mono, avec glide) ;
- notes **au-dessus** : **skank** (8 voix, joue tes accords).

### Skank
- **Skank Acoustic-Electric** : 1 = corde pincée (Karplus-Strong), 0 = pulse carré électrique dont le filtre se ferme.
- **Skank Decay ms** : longueur du coup. Pour un skank bien sec, 80 à 160 ms.
- **Skank Brightness** : clarté de l'attaque.

### Sub
- **Sub Sine-Triangle** : plus de triangle = plus d'harmoniques, donc plus audible sur petites enceintes.
- **Sub Octave Down** : ajoute une sous-octave pour le poids.
- **Sub Drive** : saturation douce. Elle donne de la présence sans perdre le grave.
- **Sub Tone Hz** : passe-bas après le drive (350 Hz par défaut ; baisse-le pour un sub plus pur).
- **Sub Punch** : petit « boing » de pitch à l'attaque.
- **Sub Glide ms** : glissement entre les notes.

### FX dub (appliqués au skank uniquement ; le sub reste sec)
- **Delay Sync** : Free, 1/4, 1/8 pointée (le classique dub) ou 1/8, calé sur le tempo de l'hôte.
- **Delay Feedback** : au-dessus de 1.0, le delay s'emballe (utile en automation pour les « throws »).
- **Delay Tape Wobble** : petite instabilité de pitch façon bande.
- Le feedback est filtré et saturé à chaque répétition, ce qui donne l'écho qui s'assombrit.
- **Reverb Mix / Size** : reverb de salle (algorithmique).

Astuce dub : automatise **Delay Mix** ou **Delay Feedback** en direct pour faire les montées et les coupures sur les skanks.

## Limites de cette version (v0.1)
- Les skanks sont **synthétisés** (pas d'échantillons réels). Pour un son plus réaliste, la prochaine étape est de remplacer la couche acoustique par des samples de guitare.
- Code non compilé ni testé sur ma machine : il peut demander de petits ajustements au premier build.
