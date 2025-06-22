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
Pour estimer la durée d'un déplacement, DJ prend en compte les `END_AT_BRAKE` et les `END_AT_LAST_POINT`. Cela signifie qu'il est impossible de pondérer directement les arêtes du graphe, car la durée d'un déplacement dépend de l'ensemble du chemin. DJ calcule donc la durée d'un chemin entier.

Une fois ces informations en place, DJ peut utiliser un algorithme de recherche de chemin classique pour trouver le chemin le plus rapide.

Deux algorithmes sont disponibles dans DJ : **Dijkstra** et **A***.

**Avantages et inconvénients :**
- Dijkstra offre des solutions plus pertinentes qu'A*. A* est efficace pour des graphes très grands et denses, mais ce n'est pas le cas ici. De plus, A* est optimal lorsque la pondération des arêtes est liée aux distances entre les sommets, ce qui n'est pas le cas ici. En effet, le robot accélère ou freine quasiment tout le temps. Cela joue en faveur de une pondération basée sur la durée des déplacements.
- A* est moins coûteux en temps de calcul que Dijkstra.

Il est possible de changer l'algorithme utilisé en modifiant la constante `DJ_SOLVER_TYPE` dans le fichier [`dj_solver/dj_solver.h`](dj_solver/dj_solver.h).

### Conclusion
Une fois le chemin le plus rapide déterminé, il ne reste plus qu'à le suivre. La dernière étape est une gestion classique par MAE (Machine à États). Le chemin est recalculé de temps en temps. Si un évitement est déclenché, DJ recommence le calcul du chemin dans une limite de X tentatives. Pour plus de détails, voir [`dj_try_going`](./dj_launcher/dj_try_going.c).

## Mathématiques
### Conversion des obstacles dynamiques en obstacles statiques
On peut retrouver l'application de ces mathématiques dans le fichier [`dj_obstacle/dj_obstacle_dynamic.c`](dj_obstacle/dj_obstacle_dynamic.c).
Si vous n'avez pas d'editeur LaTeX, vous pouvez utiliser ce fichier mais il est recommandé d'utiliser un éditeur LaTeX pour une meilleure lisibilité.
#### Rappel

Le but est de déterminer la position de l'obstacle dans l'espace-temps depuis la position du robot.

**Espace-temps** : comme l'espace-temps classique, mais avec la vitesse du robot au lieu de celle de la lumière.
Le but est de connaître la ou les positions possibles de l'obstacle pour un trajet optimal.
On limite la position d'un obstacle à son **centre**.

---

#### Notations

##### Temps
- $t$ : temps absolu
- $t_{\text{adv}}$ : temps où l'obstacle a été mis à jour
- $t_{\text{rob}}$ : début de l'observation

##### Obstacle
- $A_{\text{adv}}(x, y)$ : accélération de l'obstacle
- $V_{\text{adv}}(x, y)$ : vitesse initiale de l'obstacle
- $P_{\text{adv}}(x, y)$ : position initiale de l'obstacle

##### Robot
- $A_{\text{rob}}$ : accélération du robot
- $V_{\text{rob}}$ : vitesse initiale du robot
- $P_{\text{rob}}$ : position initiale du robot

---

#### Calculs

##### 1) Distance du robot en fonction du temps
$$
D_{\text{rob}} = V_{\text{rob}} \cdot (t - t_{\text{rob}}) + \frac{1}{2} \cdot A_{\text{rob}} \cdot (t - t_{\text{rob}})^2
$$

##### 2) Position de l'obstacle en fonction du temps
$$
P_{\text{advx}} = P_{\text{advix}} + V_{\text{advix}} \cdot (t - t_{\text{adv}}) + \frac{1}{2} \cdot A_{\text{advx}} \cdot (t - t_{\text{adv}})^2
$$
$$
P_{\text{advy}} = P_{\text{adviy}} + V_{\text{adviy}} \cdot (t - t_{\text{adv}}) + \frac{1}{2} \cdot A_{\text{advy}} \cdot (t - t_{\text{adv}})^2
$$

##### 3) Évolution de la distance entre le robot et l'obstacle en fonction du temps

$$
D_{\text{adv}} = \sqrt{(dx)^2 + (dy)^2}
$$

Avec :
- $dx$ et $dy$ : la différence de position entre le robot et l'obstacle.

Or :
$$
dx = P_{\text{advx}} - P_{\text{robix}}
$$
$$
dy = P_{\text{advy}} - P_{\text{robiy}}
$$

Donc :
$$
D_{\text{adv}} = \sqrt{(P_{\text{advx}} - P_{\text{robix}})^2 + (P_{\text{advy}} - P_{\text{robiy}})^2}
$$
$$
\iff D_{\text{adv}} = \sqrt{\Big(P_{\text{advix}} + V_{\text{advix}} \cdot (t - t_{\text{adv}}) + \frac{1}{2} \cdot A_{\text{advx}} \cdot (t - t_{\text{adv}})^2 - P_{\text{robix}}\Big)^2 + \Big(P_{\text{adviy}} + V_{\text{adviy}} \cdot (t - t_{\text{adv}}) + \frac{1}{2} \cdot A_{\text{advx}} \cdot (t - t_{\text{adv}})^2 - P_{\text{robiy}}\Big)^2}
$$

---

##### 4) Solutions

Les solutions apparaissent lorsque :
$$
D_{\text{adv}} = D_{\text{rob}}
$$

Donc lorsque :
$$
\sqrt{\Big(P_{\text{advix}} + V_{\text{advix}} \cdot (t - t_{\text{adv}}) + \frac{1}{2} \cdot A_{\text{advx}} \cdot (t - t_{\text{adv}})^2 - P_{\text{robix}}\Big)^2 + \Big(P_{\text{adviy}} + V_{\text{adviy}} \cdot (t - t_{\text{adv}}) + \frac{1}{2} \cdot A_{\text{advx}} \cdot (t - t_{\text{adv}})^2 - P_{\text{robiy}}\Big)^2} = V_{\text{rob}} \cdot (t - t_{\text{rob}}) + \frac{1}{2} \cdot A_{\text{rob}} \cdot (t - t_{\text{rob}})^2
$$

Ce qui donne :
$$
\Big(P_{\text{advix}} + V_{\text{advix}} \cdot (t - t_{\text{adv}}) + \frac{1}{2} \cdot A_{\text{advx}} \cdot (t - t_{\text{adv}})^2 - P_{\text{robix}}\Big)^2 + \Big(P_{\text{adviy}} + V_{\text{adviy}} \cdot (t - t_{\text{adv}}) + \frac{1}{2} \cdot A_{\text{advx}} \cdot (t - t_{\text{adv}})^2 - P_{\text{robiy}}\Big)^2 = \Big(V_{\text{rob}} \cdot (t - t_{\text{rob}}) + \frac{1}{2} \cdot A_{\text{rob}} \cdot (t - t_{\text{rob}})^2\Big)^2
$$

Soit :
$$
0 = \left(\frac{A_{\text{advx}}^2}{2} - \frac{A_{\text{rob}}^2}{4}\right) t^4
+ \left(A_{\text{advx}}(V_{\text{advix}} - A_{\text{advx}} \cdot t_{\text{adv}}) + A_{\text{advx}}(V_{\text{adviy}} - A_{\text{advx}} \cdot t_{\text{adv}}) - A_{\text{rob}}(V_{\text{robi}} - A_{\text{rob}} \cdot t_{\text{rob}})\right) t^3
$$
$$
+ \Bigg(\left(V_{\text{advix}} - A_{\text{advx}} \cdot t_{\text{adv}}\right)^2
+ \left(V_{\text{adviy}} - A_{\text{advx}} \cdot t_{\text{adv}}\right)^2
- \left(V_{\text{robi}} - A_{\text{rob}} \cdot t_{\text{rob}}\right)^2
$$
$$
+ A_{\text{advx}} \cdot \left(\frac{A_{\text{advx}} \cdot t_{\text{adv}}^2}{2} - V_{\text{advix}} \cdot t_{\text{adv}} + P_{\text{advix}} - P_{\text{robix}}\right)
+ A_{\text{advx}} \cdot \left(\frac{A_{\text{advx}} \cdot t_{\text{adv}}^2}{2} - V_{\text{adviy}} \cdot t_{\text{adv}} + P_{\text{adviy}} - P_{\text{robiy}}\right)
$$
$$
+ A_{\text{rob}} \cdot \left(V_{\text{robi}} \cdot t_{\text{rob}} - \frac{A_{\text{rob}} \cdot t_{\text{rob}}^2}{2}\right)\Bigg) t^2
$$
$$
+ \Bigg(2 \cdot \left(V_{\text{advix}} - A_{\text{advx}} \cdot t_{\text{adv}}\right) \cdot \left(\frac{A_{\text{advx}} \cdot t_{\text{adv}}^2}{2} - V_{\text{advix}} \cdot t_{\text{adv}} + P_{\text{advix}} - P_{\text{robix}}\right)
$$
$$
+ 2 \cdot \left(V_{\text{adviy}} - A_{\text{advx}} \cdot t_{\text{adv}}\right) \cdot \left(\frac{A_{\text{advx}} \cdot t_{\text{adv}}^2}{2} - V_{\text{adviy}} \cdot t_{\text{adv}} + P_{\text{adviy}} - P_{\text{robiy}}\right)
$$
$$
+ 2 \cdot \left(V_{\text{robi}} - A_{\text{rob}} \cdot t_{\text{rob}}\right) \cdot \left(V_{\text{robi}} \cdot t_{\text{rob}} - \frac{A_{\text{rob}} \cdot t_{\text{rob}}^2}{2}\right)\Bigg) t
$$
$$
+ \left(\frac{A_{\text{advx}} \cdot t_{\text{adv}}^2}{2} - V_{\text{advix}} \cdot t_{\text{adv}} + P_{\text{advix}} - P_{\text{robix}}\right)^2
+ \left(\frac{A_{\text{advx}} \cdot t_{\text{adv}}^2}{2} - V_{\text{adviy}} \cdot t_{\text{adv}} + P_{\text{adviy}} - P_{\text{robiy}}\right)^2
$$
$$
- \left(V_{\text{rob}} \cdot t_{\text{rob}} - \frac{A_{\text{rob}} \cdot t_{\text{rob}}^2}{2}\right)^2
$$


L'équation est un peu longue mais résumons-la :
Elle est de la forme
$$
0 = a \cdot t^4 + b \cdot t^3 + c \cdot t^2 + d \cdot t + e
$$


##### Coefficients de l'équation
- $a = \frac{A_{\text{advx}}^2}{2} - \frac{A_{\text{rob}}^2}{4}$
- $b = A_{\text{advx}}(V_{\text{advix}} - A_{\text{advx}} \cdot t_{\text{adv}}) + A_{\text{advx}}(V_{\text{adviy}} - A_{\text{advx}} \cdot t_{\text{adv}}) - A_{\text{rob}}(V_{\text{rob}} - A_{\text{rob}} \cdot t_{\text{rob}})$
- $c = \big((V_{\text{advix}} - A_{\text{advx}} \cdot t_{\text{adv}})^2 + (V_{\text{adviy}} - A_{\text{advx}} \cdot t_{\text{adv}})^2 - (V_{\text{rob}} - A_{\text{rob}} \cdot t_{\text{rob}})^2 \big)$
  $+ A_{\text{advx}} \cdot \big( \frac{A_{\text{advx}} \cdot t_{\text{adv}}^2}{2} - V_{\text{advix}} \cdot t_{\text{adv}} + P_{\text{advix}} - P_{\text{robix}} \big)$
  $+ A_{\text{advx}} \cdot \big( \frac{A_{\text{advx}} \cdot t_{\text{adv}}^2}{2} - V_{\text{adviy}} \cdot t_{\text{adv}} + P_{\text{adviy}} - P_{\text{robiy}} \big)$
  $+ A_{\text{rob}} \cdot \big( V_{\text{rob}} \cdot t_{\text{rob}} - \frac{A_{\text{rob}} \cdot t_{\text{rob}}^2}{2} \big)$

- $d = 2 \cdot (V_{\text{advix}} - A_{\text{advx}} \cdot t_{\text{adv}}) \cdot \big( \frac{A_{\text{advx}} \cdot t_{\text{adv}}^2}{2} - V_{\text{advix}} \cdot t_{\text{adv}} + P_{\text{advix}} - P_{\text{robix}} \big)$
  $+ 2 \cdot (V_{\text{adviy}} - A_{\text{advx}} \cdot t_{\text{adv}}) \cdot \big( \frac{A_{\text{advx}} \cdot t_{\text{adv}}^2}{2} - V_{\text{adviy}} \cdot t_{\text{adv}} + P_{\text{adviy}} - P_{\text{robiy}} \big)$
  $+ 2 \cdot (V_{\text{rob}} - A_{\text{rob}} \cdot t_{\text{rob}}) \cdot \big( V_{\text{rob}} \cdot t_{\text{rob}} - \frac{A_{\text{rob}} \cdot t_{\text{rob}}^2}{2} \big)$

- $e = \big( \frac{A_{\text{advx}} \cdot t_{\text{adv}}^2}{2} - V_{\text{advix}} \cdot t_{\text{adv}} + P_{\text{advix}} - P_{\text{robix}} \big)^2$
  $+ \big( \frac{A_{\text{advx}} \cdot t_{\text{adv}}^2}{2} - V_{\text{adviy}} \cdot t_{\text{adv}} + P_{\text{adviy}} - P_{\text{robiy}} \big)^2$
  $- \big( V_{\text{rob}} \cdot t_{\text{rob}} - \frac{A_{\text{rob}} \cdot t_{\text{rob}}^2}{2} \big)^2$


##### Résolution numérique
Cette équation est une équation du quatrième degré, donc il y a de 0 à 4 solutions.
Les solutions générales sont complexes, donc on ne calculera pas la forme générale.

Pour rappel, on cherche les racines du polynôme :
$$
a \cdot t^4 + b \cdot t^3 + c \cdot t^2 + d \cdot t + e
$$

On a l'avantage de pouvoir limiter les solutions à un intervalle de temps $[0, t_{\text{max}}]$.
En effet, il n'a aucun sens de chercher une solution négative ou supérieure à $t_{\text{max}}$.

De plus, $t_{\text{max}}$ est relativement petit, car :
- On ne cherche pas à prédire la position de l'obstacle dans un futur lointain (imprécision de la prédiction).
- De toute façon, n'importe quel robot aura traversé le terrain ou changé de direction en quelques secondes seulement.

###### Méthode de recherche des solutions
Pour tout $t \in [0, t_{\text{max}}]$, on calcule la valeur de l'équation et on regarde si elle est de signe opposé à la précédente.
Si c'est le cas, alors il y a une solution entre \(t\) et \(t - 1\).

###### Calcul de la position de l'obstacle pour chaque solution
Pour chaque solution trouvée \(t\), on calcule la position de l'obstacle :
$$
P_{\text{advx}} = P_{\text{advix}} + V_{\text{advix}} \cdot t + \frac{1}{2} \cdot A_{\text{advx}} \cdot t^2
$$
$$
P_{\text{advy}} = P_{\text{adviy}} + V_{\text{adviy}} \cdot t + \frac{1}{2} \cdot A_{\text{advx}} \cdot t^2
$$
avec \(t\) la solution trouvée.


##### Conclusion
On obtient aucune, une ou plusieurs positions possibles de l'obstacle dans l'espace-temps.


## TODO

Les tâches à effectuer pour améliorer DJ :

- [ ] Gérer les obstacles dynamiques qui changent de forme.
- [ ] Prendre en compte la pré-rotation du robot avant la première translation.
- [ ] Prendre en compte le dernier déplacement en fonction de son type (`END_AT_BRAKE` ou `END_AT_LAST_POINT`).
- [ ] Implémenter une interaction avec le système de propulsion pour demander les coefficients de propulsion et éviter la redondance de code. Cela permettrait également de gérer les changements d'accélération pendant le match.
- [ ] Fixer [ce bug](.\dj_obstacle\dj_obstacle_dynamic.c).
