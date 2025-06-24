# Testeur de DJ

## Introduction
Ce projet est un testeur de l'algorithme DJ. Il vise à évaluer la performance de l'algorithme sur différentes cibles, ou bien à évaluer la capacité d'une cible à exécuter l'algorithme DJ.

Pour plus d'informations sur l'algorithme DJ, vous pouvez consulter la [documentation](./src/dj/README.md).

**Note :** DJ a été initialement développé pour le [club de robotique de l'ESEO](https://robot-eseo.fr).
De ce fait, exécuter l'algorithme DJ sur autre chose qu'un robot n'a pas vraiment de sens, à part pour faire des calculs de performance.

## Installation
**Prérequis :**
- CMake
- Un compilateur C++ (g++ ou clang++)

## Utilisation
Exécutez le programme produit par CMake sur une cible pour évaluer la performance de l'algorithme DJ sur cette cible.
- Un fichier .txt sera généré avec les résultats de l'évaluation.
- Des logs seront affichés dans la console, dont les résultats de l'évaluation.

## Cibles supportées
- Windows
- Linux
- Stm32f4 (Seulement testé sur STM32F407VG)

## Résultats
[HONOR MagicBook 14 - windows](./results/HONOR_MagicBook_14_windows.txt)
[HONOR MagicBook 14 - wsl2 ubuntu](./results/HONOR_MagicBook_14_wsl2_ubuntu.txt)
[STM32F407](./results/STM32F407.txt)

## Licence
Ce projet est sous licence MIT. Vous pouvez consulter le fichier [LICENSE](./LICENCE.txt) pour plus d'informations.

Certains fichiers sont la propriété du club de robotique de l'ESEO. Ils sont sous licence Cecill-C. Vous pouvez consulter le fichier [LICENSE_CECILL-C.txt](./LICENSE_CECILL-C.txt) pour plus d'informations.

## Contribuer
Si vous souhaitez contribuer à ce projet, vous pouvez le faire en ouvrant une [issue](https://github.com/Cmenard001/dj/issues) ou en faisant une [pull request](https://github.com/Cmenard001/dj/pulls). Nous acceptons toutes les contributions, qu'elles soient mineures ou majeures.

## Auteurs
- [Cyprien Ménard](https://github.com/Cmenard001)
- [Robot ESEO](https://robot-eseo.fr)

## Remerciements
Merci au club de robotique de l'ESEO.
Merci à tous les contributeurs, notamment ceux qui ne sont pas cités ici, pour leurs conseils, leur aide, leurs idées, ...
