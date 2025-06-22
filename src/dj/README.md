# DJ

## Introduction
DJ est un algorithme permettant au robot de se déplacer en évitant des obstacles.

Ces obstacles peuvent être statiques comme des zones interdites, des éléments de jeu, des murs, etc., ou dynamiques comme des robots adverses.

## Utilisation
### Faire un déplacement
Pour faire un déplacement, il faut appeler la fonction `dj_try_going` dans le fichier [`dj.h`](dj.h). Cette fonction a la même interface qu'un `try_going` classique.

Attention, si le point de destination du déplacement est dans l'aire d'un obstacle statique, le robot s'y déplacera. Autrement dit, si l'utilisateur souhaite explicitement aller dans une zone interdite, c'est possible.

Si aucun chemin n'est trouvé à cause d'un obstacle dynamique qui bloque le chemin, l'algorithme ignorera cet obstacle dynamique dans son calcul. Cela signifie que le robot se déplacera comme si l'obstacle dynamique n'était pas là. Cela risque de déclencher un évitement.

### Activer / désactiver des obstacles
Pour activer ou désactiver un obstacle, il faut appeler les fonctions `dj_enable_static_polygon` et `dj_enable_dynamic_polygon` dans le fichier [`dj.h`](dj.h).
On peut aussi savoir si un obstacle est actif ou non en appelant les fonctions `dj_is_static_polygon_enabled` et `dj_is_dynamic_polygon_enabled`.

### Ajouter des obstacles
Chaque obstacle a un ID unique. Cet ID est utilisé pour savoir de quel obstacle on parle lorsqu'on interagit avec l'algorithme DJ.

#### Ajouter un obstacle statique ou dynamique de manière statique
##### ID d'obstacle par défaut
Vous pouvez ajouter des ID par défaut dans le fichier [`dj_obstacle_importer/dj_obstacle_id.h`](dj_obstacle_importer/dj_obstacle_id.h). Ajoutez simplement une nouvelle ligne dans l'énumération `dj_static_obstacle_id_e` ou `dj_dynamic_obstacle_id_e`.

**Quelques conseils :**
- Faites des obstacles avec le moins de sommets possible. Cela permet de gagner du temps de calcul, de mémoire et d'obtenir des solutions plus pertinentes. Voir la partie [fonctionnement](##Fonctionnement) pour comprendre pourquoi.
- Faites des obstacles les plus petits possibles. Cela permet à l'algorithme d'avoir plus de choix de chemins et de trouver des chemins plus rapides.
- Utilisez l'outil de visualisation de terrain disponible [`ici`](./dj_playground_vizualizer.py) pour vérifier que vos obstacles sont bien placés. Cet outil vous montrera les zones où le centre du robot n'a pas le droit de passer. Vous pouvez aussi voir les chemins possibles pour le robot.

#### Obstacle par défaut
Pour ajouter un obstacle statique, il faut rajouter un polygone dans le fichier [`dj_obstacle_importer/dj_obstacle_importer.c`](dj_obstacle_importer/dj_obstacle_importer.c).
Pour cela, rajoutez un appel à la fonction `dj_importer_static_init` ou `dj_importer_dynamic_init` dans la fonction `dj_obstacle_importer_init`.
Suivez les exemples déjà présents dans le fichier. Attention, les ID des obstacles doivent être ajoutés dans les énumérations `dj_static_obstacle_id_e` ou `dj_dynamic_obstacle_id_e`.

#### Obstacle ajouté durant la partie
On peut aussi ajouter des obstacles durant la partie. Pour cela, il faut appeler les fonctions `dj_obstacle_importer_import_static_obstacle` ou `dj_obstacle_importer_import_dynamic_obstacle` dans le fichier [`dj_obstacle_importer/dj_obstacle_importer.c`](dj_obstacle_importer/dj_obstacle_importer.c).
Attention cependant, il faut utiliser ces fonctions avec parcimonie, car elles sont très coûteuses en temps de calcul. On ne peut pas proprement supprimer un obstacle ajouté durant la partie, mais on peut le désactiver. Si vous avez besoin de supprimer un obstacle, désactivez-le simplement. Cela permet de gagner du temps de calcul.

#### Obstacle dynamique
Les obstacles dynamiques sont représentés par des polygones qui bougent.
Lorsqu'on définit un obstacle dynamique, on lui donne une forme qui ne changera pas durant la partie. Cependant, on peut changer sa position, sa vitesse et son accélération. Cette forme, représentée par un polygone, doit impérativement être au mieux centrée autour de l'origine. En effet, la position de cet obstacle sera approximée par un point qui sera l'origine de la forme. Attention on parle uniquement de la forme de l'obstacle ici, en aucun cas de sa position sur le terrain. C'est d'ailleurs ce point que vous devez utiliser lorsque vous définissez la position de l'obstacle dynamique.

Pour les obstacles dynamiques, il faut les rafraîchir durant la partie. Pour cela, il faut appeler la fonction `dj_obstacle_importer_refresh_dynamic_obstacle` dans votre code. Lorsque vous rafraîchissez un obstacle dynamique, il faut lui donner une position, une vitesse et une accélération constante.

Les obstacles dynamiques ont une durée de vie. En effet, il est impensable qu'un obstacle dynamique n'ait changé de direction, de vitesse ou d'accélération depuis un certain temps. De plus, les obstacles dynamiques sont approximés avec une accélération constante, mais en réalité, ce n'est pas toujours le cas. Pour ces raisons, les obstacles dynamiques ont une durée de vie.
Cette durée de vie est définie dans le fichier [`dj_obstacle/dj_obstacle_dynamic`](dj_obstacle/dj_obstacle_dynamic) par la constante `OBSTACLE_LIFETIME_MS`. Tout cela implique que vous devez rafraîchir les obstacles dynamiques régulièrement.

## Configuration
Le fichier de configuration de DJ est le fichier [`dj_config.h`](dj_config.h). Ce fichier contient toutes les constantes de configuration de DJ.
### Paramètres de configuration
- `DJ_ROBOT_WIDTH` : Largeur du robot. À régler en fonction de la largeur réelle du robot, éléments compris.
- `DJ_ROBOT_FRONT_BORDER_DISTANCE` : Distance entre le centre du robot et le bord avant du robot. À régler en fonction de la distance réelle, éléments compris.
- `DJ_ROBOT_BACK_BORDER_DISTANCE` : Distance entre le centre du robot et le bord arrière du robot. À régler en fonction de la distance réelle, éléments compris.
- `ROBOT_MARGIN` : Marge de sécurité à ajouter autour du robot.

- `DJ_GRAPH_GRAPH_MAX_NODES` : Nombre maximum de sommets dans le graphe. À régler en fonction du nombre d'obstacles et de leur complexité. Prendre un peu de marge mais éviter de prendre trop de RAM.
- `DJ_GRAPH_GRAPH_AVERAGE_LINKS` : Nombre moyen de liens par sommet. À régler en fonction du nombre d'obstacles et de leur complexité. Prendre un peu de marge mais éviter de prendre trop de RAM.

- `DJ_ENABLE_DEBUG_LOGS` : Activer ou désactiver les logs de débogage de DJ.
- `DJ_ENABLE_BUILD_GRAPH_DEBUG_LOGS` : Activer ou désactiver les logs de débogage de la construction du graphe.
- `DJ_ENABLE_REBUILD_GRAPH_DEBUG_LOGS` : Activer ou désactiver les logs de débogage de la reconstruction du graphe.

- `DJ_ENABLE_REBUILD_OPTIMIZATION` : Macro à commenter pour désactiver l'optimisation de la reconstruction du graphe. Cette optimisation vise à ne pas lier les positions calculées des adversaires dans l'espace-temps. Cela permet de gagner un temps conséquent de calcul, mais les performances sont donc moins importantes. Cette optimisation a pour conséquence que le robot se déplace uniquement le long des obstacles statiques. Cette optimisation est par exemple à désactiver si l'on veut utiliser uniquement des obstacles dynamiques.

- `ENABLE_RECALCULATION` : Active ou non le recalcul du chemin pendant un déplacement.
- `ENABLE_RETRY` : Active ou non la réessai du chemin si le robot ne trouve pas de chemin ou s'il y a un évitement.
- `MAX_RETRIES` : Nombre maximum de réessais du chemin si le robot ne trouve pas de chemin ou s'il y a un évitement.
- `DJ_TIMEOUT_MS` : Timeout pour le déplacement. Si le robot n'atteint pas le point de destination dans ce délai, une erreur est retournée. Ce délai est en millisecondes.
- `TOO_SHORT_DISTANCE` : Distance minimale entre chaque point de la trajectoire. Si la distance entre deux points est inférieure à cette valeur, le deuxième point est ignoré. Cela permet de ne pas demander de choses trop bizarres à la propulsion. Cette valeur est en millimètres.
- `OBSTACLE_LIFETIME_MS` : Durée de vie d'un obstacle dynamique après son rafraîchissement. Cette valeur est en millisecondes.
- `DJ_OBSTACLE_MANAGER_MAX_IMPORTED_OBSTACLES` : Nombre maximum d'obstacles importés. À régler en fonction du nombre d'obstacles dynamiques que vous avez. Prendre un peu de marge mais éviter de prendre trop de RAM.
- `DJ_POLYGON_MAX_POINTS` : Nombre maximum de points dans un polygone. À régler en fonction du nombre de points que vous avez dans vos obstacles. Prendre un peu de marge suivant le type d'agrandissement que vous utilisez (voir [ici](./dj_obstacle_importer/dj_obstacle_importer.c)).

## Portage
En cas de portage de DJ, voici la liste des fichiers à modifier :
- [Le fichier de configuration](dj_config.h) : il faut adapter les constantes et les configurations à votre robot.
- [Le fichier de dépendances](./dj_dependencies/dj_dependencies.c) et son [.h](./dj_dependencies/dj_dependencies.h) : il faut adapter les dépendances à votre environnement.
- Quelques fichiers de la QS (QS_maths..., QS_magic_array, QS_macros).
- [Le fichier de la MAE principale](./dj_launcher/dj_try_going.c).

## Fonctionnement
Cette section explique le fonctionnement de l'algorithme DJ.

### Informations générales
DJ ne cherche pas le chemin le plus court mais le chemin le plus rapide. Pour cela, il émule la propulsion du robot en considérant qu'elle est parfaite. Cela signifie que le robot se déplace avec une accélération constante. Il prend en compte les `END_AT_BRAKE` et les `END_AT_LAST_POINT` lors de ses calculs.

DJ fonctionne sur un principe de graphe. Chaque sommet du graphe est une position possible du centre du robot. Chaque arête du graphe représente un déplacement possible du robot.

### Création du graphe
#### Sommets
Pour créer le graphe, DJ commence par créer ses sommets. Pour cela, il prend chaque obstacle et crée un sommet pour chaque point du polygone. Comme indiqué précédemment, les obstacles sont agrandis de façon à représenter les zones où le centre du robot n'a pas le droit de passer. Les points de départ et d'arrivée sont également ajoutés.

#### Liens
Ensuite, DJ crée les liens entre les sommets. Pour cela, il prend chaque sommet et détermine quels autres sommets sont accessibles depuis celui-ci sans passer par un obstacle. Il crée un lien entre ces sommets. Il crée aussi des liens sur les arêtes des obstacles, permettant ainsi au robot de longer ces derniers. Cependant, il ne crée pas de liens sur les diagonales des obstacles, ce qui empêche le robot de traverser les obstacles.

### Calcul du chemin
**Prérequis** : être familier avec des algorithmes de recherche de chemin comme Dijkstra ou A*.

DJ utilise un algorithme de recherche de chemin classique pour trouver le chemin le plus rapide. Cependant, à chaque sommet exploré lors de la résolution de l'algorithme, DJ "reconstruit" une partie du graphe.

#### Reconstruire le graphe
Lors de la précédente construction du graphe, DJ a créé des sommets et des liens à partir uniquement des obstacles statiques. Maintenant, il doit prendre en compte les obstacles dynamiques. Pour cela, il convertit les obstacles dynamiques en obstacles statiques selon l'endroit depuis lequel on les observe.

##### Désactiver les liens
Les liens qui passent par un obstacle dynamique sont désactivés. Cela signifie que le robot ne peut pas passer par ces liens. Cela permet d'éviter les obstacles dynamiques.

##### Conversion des obstacles dynamiques en obstacles statiques
Pour convertir un obstacle dynamique en obstacle statique, DJ considère la position de l'obstacle dynamique dans l'espace-temps depuis la position du sommet où se trouve l'algorithme. Plus simplement, il calcule les positions où le robot pourrait croiser le centre de l'obstacle dynamique en suivant les chemins les plus directs.

Cela signifie qu'un obstacle dynamique peut avoir plusieurs positions statiques (jusqu'à 8 au maximum, en prenant en compte des cas avec `END_AT_BRAKE` ou `END_AT_LAST_POINT`). Pour le détail des mathématiques, voir la partie [mathématiques](##Mathématiques). En pratique, il est rare qu'un obstacle dynamique ait plus de 2 positions statiques.

Ces positions statiques sont ajoutées au graphe de la même manière que les obstacles statiques.


#### Calcul du chemin
Pour estimer la durée d'un déplacement, DJ prend en compte les `END_AT_BRAKE` et les `END_AT_LAST_POINT`. Cela signifie qu'il est impossible de pondérer directement les arêtes du graphe, car la durée d'un d
