# Testeur de DJ

## Introduction
Ce projet est un banc de test (benchmark) de l'algorithme de planification de trajectoire **DJ**. Il vise à évaluer la performance de l'algorithme sur différentes cibles, ou bien à évaluer la capacité d'une cible à exécuter l'algorithme DJ.

Le testeur exécute des **suites de tests** : pour chaque suite, il configure un terrain avec ses obstacles, génère un grand nombre de déplacements aléatoires (départ/arrivée), demande à DJ de calculer le chemin de chacun et mesure le taux de réussite ainsi que le temps de calcul (temps « mur » et temps CPU).

Pour plus d'informations sur l'algorithme DJ lui-même, vous pouvez consulter sa [documentation](./src/dj/README.md).

**Note :** DJ a été initialement développé pour le [club de robotique de l'ESEO](https://robot-eseo.fr).
De ce fait, exécuter l'algorithme DJ sur autre chose qu'un robot n'a pas vraiment de sens, à part pour faire des calculs de performance, ce qui est précisément l'objet de ce testeur.

## Installation
**Prérequis :**
- CMake (≥ 3.5)
- Ninja
- Un compilateur C (gcc ou clang)
- Pour générer la documentation : Doxygen + Graphviz (optionnel)
- Pour les cibles embarquées : la toolchain ARM correspondante (voir [Cibles supportées](#cibles-supportées))

**Dev Container (recommandé) :**
Un [Dev Container](./.devcontainer/) est fourni. Il contient déjà tous les outils nécessaires (CMake, Ninja, toolchain `arm-none-eabi`, OpenOCD, GDB multi-arch, Doxygen/Graphviz, client SSH, Python). Ouvrez le projet dans VS Code et choisissez « Reopen in Container ».

## Compilation
La configuration repose sur des **presets CMake** (voir [`CMakePresets.json`](./CMakePresets.json)). Chaque preset fixe la cible et la toolchain.

```bash
# Configurer puis compiler la cible native Linux
cmake --preset linux_preset
cmake --build build/linux_preset
```

Presets disponibles :
- `linux_preset` — exécutable natif Linux
- `windows_preset` — exécutable natif Windows
- `stm32f4_preset` — firmware STM32F4 (toolchain `arm-none-eabi`)
- `stm32mp15_preset` — STM32MP157 (SDK Yocto/Poky, à sourcer avant configuration)
- `tests` — build Linux + tests unitaires GTest (voir [Tests unitaires](#tests-unitaires))

## Utilisation
Exécutez le programme produit par CMake sur la cible pour évaluer la performance de l'algorithme DJ.
- Sur Linux, le programme demande un nom de matériel (laisser vide pour utiliser le nom de la cible).
- Les suites de tests sont exécutées les unes après les autres, et les résultats (réussite, temps mur, temps CPU) sont affichés dans la console.
- Un fichier `results/<date>_<nom_matériel>.txt` est généré avec le récapitulatif de l'évaluation.

Les suites de tests et leurs obstacles sont définis directement dans [`src/dj_tester/dj_tester.c`](./src/dj_tester/dj_tester.c). Deux suites sont fournies par défaut : **Playground 2025** et **Playground 2026**.

## Tests unitaires
Des tests unitaires (GoogleTest, natif Linux uniquement) vérifient que chaque test de chaque suite du benchmark trouve bien un chemin. GoogleTest est récupéré automatiquement par CMake (`FetchContent`).

```bash
cmake --preset tests
cmake --build --preset tests
ctest --preset tests
```

## Documentation
La documentation du code est générée par **Doxygen** (thème `doxygen-awesome-css`, avec graphes d'appel et diagrammes Graphviz). La page d'accueil est ce README.

```bash
cmake --preset linux_preset
cmake --build build/linux_preset --target doxygen
# HTML généré dans build/linux_preset/docs/html
```

## Intégration continue
Un workflow [GitHub Actions](./.github/workflows/ci.yml) :
- construit l'image du Dev Container et la pousse sur GHCR pour que les jobs s'exécutent dans le même environnement que le développement local ;
- compile la cible Linux et exécute les tests unitaires GTest ;
- sur la branche `main`, génère la documentation Doxygen et la publie sur GitHub Pages.

## Cibles supportées
- Windows
- Linux
- STM32F4 (seulement testé sur STM32F407VG)
- STM32MP157C-DK2

## Résultats
Les résultats sont enregistrés dans le dossier [`results/`](./results), nommés `<date>_<matériel>.txt`.

- [Intel Core Ultra 7 165H](./results/2026-06-03_Intel-Core-Ultra-7-165H.txt)

## Licence
Ce projet est sous licence MIT. Vous pouvez consulter le fichier [LICENSE](./LICENCE.txt) pour plus d'informations.

Certains fichiers sont la propriété du club de robotique de l'ESEO. Ils sont sous licence Cecill-C. Vous pouvez consulter le fichier [LICENSE_CECILL-C.txt](./LICENSE_CECILL-C.txt) pour plus d'informations.

## Contribuer
Si vous souhaitez contribuer à ce projet, vous pouvez le faire en ouvrant une [issue](https://github.com/Cmenard001/dj/issues) ou en faisant une [pull request](https://github.com/Cmenard001/dj/pulls). Nous acceptons toutes les contributions, qu'elles soient mineures ou majeures.

N'oubliez pas de vous rajouter dans la liste des [contributeurs](#auteurs) si vous le souhaitez.

## Auteurs
- [Cyprien Ménard](https://github.com/Cmenard001)
- [Dorian Benech](https://github.com/xmow49)
- [Robot ESEO](https://robot-eseo.fr)

## Remerciements
Merci au club de robotique de l'ESEO.
Merci à tous les contributeurs, notamment ceux qui ne sont pas cités ici, pour leurs conseils, leur aide, leurs idées, ...
