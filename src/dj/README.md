# DJ

## Introduction
DJ est un algorithme de planification de trajectoire permettant à un robot de se déplacer en évitant des obstacles.

Ces obstacles peuvent être statiques comme des zones interdites, des éléments de jeu, des murs, etc., ou dynamiques comme des robots adverses.

DJ ne pilote pas directement le robot : il **génère un ou plusieurs chemins** (une suite de points) vers une ou plusieurs destinations. C'est ensuite au code appelant (la propulsion du robot) de suivre ces chemins. DJ est conçu comme un moteur réutilisable et indépendant du robot : tous les paramètres propres au robot (rayon, propulsion, etc.) lui sont fournis à l'exécution.

## Utilisation
Toute l'API publique de DJ passe par le **moteur** `dj_engine_t`, défini dans [`dj_engine.h`](dj_engine.h).

### Initialiser le moteur
Avant toute utilisation, initialisez un `dj_engine_t` avec `dj_engine_init`. Vous lui fournissez :
- le **rayon du robot** (en millimètres) ;
- un éventuel **polygone de workspace restreint** (`restricted_workspace_polygon`) qui limite la zone de construction du graphe ;
- un **callback de calcul de durée** (`duration_calculator`) qui estime la durée d'un chemin à partir des caractéristiques de propulsion ;
- un **callback de position d'obstacle dynamique** (`get_dynamic_obstacle_position`) qui convertit un obstacle dynamique en positions statiques dans l'espace-temps ;
- un **callback générateur d'arguments** appelé avant chaque génération de chemin.

Le fait que le calcul de durée et la conversion des obstacles dynamiques soient des callbacks permet de découpler DJ d'un modèle de propulsion particulier.

### Faire un déplacement
Pour générer un chemin, appelez `dj_engine_generate_path`. Vous lui passez :
- les **propriétés de propulsion** (`dj_engine_propulsion_properties_t`, initialisées via `dj_engine_init_propulsion_properties`) : position et vitesse de départ, vitesses et accélérations max, condition de fin entre deux points, etc. ;
- un **tableau de destinations** (`dj_goal_point_t`), chacune pouvant imposer un angle d'arrivée ;
- le **type de solveur** à utiliser (`DJ_SOLVER_DIJKSTRA` ou `DJ_SOLVER_ASTAR`) ;
- un drapeau `retry_without_dynamic_obstacle`.

La fonction remplit un tableau de chemins (un par destination) et un tableau de booléens indiquant quelles destinations ont été atteintes.

**Multi-destinations :** une seule génération peut viser plusieurs destinations à la fois. C'est plus efficace que d'appeler la fonction plusieurs fois, car Dijkstra/A\* explorent naturellement le graphe « en éventail » depuis la source : une seule exécution trouve les chemins vers toutes les destinations accessibles sans recalcul redondant. Le nombre maximum de destinations est fixé par `DJ_ENGINE_MAX_DESTINATIONS`.

Attention, si le point de destination est dans l'aire d'un obstacle statique, le robot s'y déplacera quand même. Autrement dit, si l'utilisateur souhaite explicitement aller dans une zone interdite, c'est possible.

Si aucun chemin n'est trouvé à cause d'un obstacle dynamique qui bloque le passage et que `retry_without_dynamic_obstacle` est activé, l'algorithme relance le calcul en ignorant les obstacles dynamiques. Le robot se déplacera alors comme si l'obstacle dynamique n'était pas là, ce qui risque de déclencher un évitement géré par la propulsion.

### Activer / désactiver des obstacles
Pour activer ou désactiver un obstacle, appelez `dj_engine_enable_static_obstacle` ou `dj_engine_enable_dynamic_obstacle`.
On peut savoir si un obstacle est actif en appelant `dj_engine_is_static_obstacle_enabled` ou `dj_engine_is_dynamic_obstacle_enabled`.

### Ajouter des obstacles
Chaque obstacle reçoit un **ID unique** (`dj_static_obstacle_id_t` / `dj_dynamic_obstacle_id_t`, des `int32_t`) au moment de l'import. Cet ID sert à désigner l'obstacle dans toutes les interactions ultérieures avec DJ.

#### Obstacle statique
Importez un obstacle statique avec `dj_engine_import_static_obstacle`. Vous fournissez sa forme (`dj_polygon_t`), son état initial (activé ou non), un **mode d'agrandissement** (voir [Agrandissement des obstacles](#agrandissement-des-obstacles)), une **marge** de sécurité et un **rayon d'extraction douce** (`smooth_extraction_radius`).

> **Attention :** chaque import réserve de la mémoire et un ID unique non libérable. N'importez donc pas trop d'obstacles. Le nombre maximum est borné par `DJ_OBSTACLE_MANAGER_MAX_STATIC_OBSTACLES`.

**Quelques conseils :**
- Faites des obstacles avec le moins de sommets possible. Cela permet de gagner du temps de calcul, de la mémoire, et d'obtenir des solutions plus pertinentes. Voir la partie [Fonctionnement](#fonctionnement) pour comprendre pourquoi.
- Faites des obstacles les plus petits possibles. Cela laisse à l'algorithme plus de choix de chemins et permet de trouver des chemins plus rapides.

#### Obstacle dynamique
Les obstacles dynamiques sont représentés par des polygones qui bougent. Importez-les avec `dj_engine_import_dynamic_obstacle`.

Lorsqu'on définit un obstacle dynamique, on lui donne une forme qui ne changera pas durant la partie. En revanche, on peut changer sa position, sa vitesse et son accélération. Cette forme, représentée par un polygone, doit impérativement être au mieux **centrée autour de l'origine**. En effet, la position de l'obstacle est approximée par un point qui est l'origine de la forme. On parle ici uniquement de la forme de l'obstacle, en aucun cas de sa position sur le terrain. C'est d'ailleurs ce point que vous utiliserez pour définir la position de l'obstacle dynamique.

Après l'import, l'obstacle n'a encore ni position, ni vitesse, ni accélération : vous devez le **rafraîchir**. Pour cela, appelez `dj_engine_refresh_dynamic_obstacle` régulièrement durant la partie, en lui donnant une position, une vitesse et une accélération constante (la vitesse et l'accélération doivent être colinéaires).

Les obstacles dynamiques ont une **durée de vie**. En effet, il est peu réaliste qu'un obstacle dynamique n'ait pas changé de direction, de vitesse ou d'accélération depuis un certain temps ; de plus, l'approximation à accélération constante n'est pas toujours valable. Pour ces raisons, vous devez rafraîchir les obstacles dynamiques régulièrement.

On ne peut pas proprement supprimer un obstacle ajouté durant la partie, mais on peut le **désactiver**. Si vous avez besoin de « supprimer » un obstacle, désactivez-le simplement.

### Agrandissement des obstacles
L'utilisateur définit un obstacle comme la zone où **aucune partie du robot** ne doit pénétrer. Or, DJ raisonne sur le **centre** du robot. Les obstacles sont donc agrandis (« oversize ») d'environ un rayon de robot pour représenter les zones interdites au centre du robot. Voir [`dj_oversize_obstacle.h`](dj_obstacle/dj_oversize_obstacle.h).

Le **mode d'agrandissement** (`dj_obstacle_oversize_mode_t`) contrôle la finesse de l'arrondi des coins :
- `DJ_OBSTACLE_OVERSIZE_MODE_1_POINT` à `..._4_POINTS` : 1 à 4 points ajoutés par coin (plus de points = arrondi plus fidèle, mais plus de sommets dans le graphe) ;
- `DJ_OBSTACLE_OVERSIZE_MODE_END_AT_BRAKE` : l'angle entre les deux points est volontairement petit pour forcer la propulsion à utiliser des rotations `END_AT_BRAKE` ;
- `DJ_OBSTACLE_OVERSIZE_MODE_NO_OVERSIZE` : pas d'agrandissement.

`DJ_DEFAULT_OVERSIZE_MODE` donne le mode à utiliser par défaut. Le nombre de points d'un polygone après agrandissement reste borné par `DJ_POLYGON_MAX_POINTS` : prévoyez un peu de marge selon le mode choisi.

## Configuration
Le fichier de configuration principal de DJ est [`dj_config.h`](dj_config.h). Quelques constantes additionnelles vivent dans les modules concernés.

### `dj_config.h`
- `DJ_GRAPH_GRAPH_MAX_NODES` : nombre maximum de sommets dans le graphe. À régler en fonction du nombre d'obstacles et de leur complexité. Prendre un peu de marge mais éviter de consommer trop de RAM.
- `DJ_GRAPH_GRAPH_AVERAGE_LINKS` : nombre moyen de liens par sommet. Même logique de réglage.
- `DJ_GRAPH_GRAPH_MAX_PATH_LENGHT` : nombre maximum de points dans un chemin.
- `DJ_GRAPH_GRAPH_MAX_PATHS` : nombre maximum de chemins générés/explorés.
- `DJ_GRAPH_NODE_MAX_ADJACENCY` : nombre maximum de liens adjacents par sommet.
- `DJ_ENGINE_MAX_DESTINATIONS` : nombre maximum de destinations lors d'une génération multi-destinations.
- `DJ_POLYGON_MAX_POINTS` : nombre maximum de points dans un polygone. À régler selon le nombre de points de vos obstacles et le mode d'agrandissement utilisé.
- `DJ_ENABLE_REBUILD_OPTIMIZATION` : active (1) ou désactive (0) l'optimisation de la reconstruction du graphe. Cette optimisation consiste à ne pas ajouter les sommets des obstacles dynamiques : elle fait gagner un temps de calcul conséquent, mais le robot ne se déplace alors que le long des obstacles statiques. À désactiver, par exemple, si vous voulez utiliser uniquement des obstacles dynamiques.

### `dj_obstacle_importer.h`
- `DJ_OBSTACLE_MANAGER_MAX_STATIC_OBSTACLES` : nombre maximum d'obstacles statiques importés.
- `DJ_OBSTACLE_MANAGER_MAX_DYNAMIC_OBSTACLES` : nombre maximum d'obstacles dynamiques importés.

### Choix du solveur
Le solveur n'est plus une constante de configuration : il est choisi **à chaque appel** de `dj_engine_generate_path` via le paramètre `dj_solver_type_t` (`DJ_SOLVER_DIJKSTRA` ou `DJ_SOLVER_ASTAR`). Voir [`dj_solver_common.h`](dj_solver/dj_solver_common/dj_solver_common.h).

## Fonctionnement
Cette section explique le fonctionnement de l'algorithme DJ.

### Informations générales
DJ ne cherche pas le chemin le plus court mais le chemin le **plus rapide**. Pour cela, il émule la propulsion du robot en la considérant parfaite : le robot se déplace avec une accélération constante. La durée d'un chemin est estimée par le callback `duration_calculator` fourni au moteur, qui prend en compte la condition de fin entre points (par exemple `END_AT_BRAKE` ou `END_AT_LAST_POINT`).

DJ fonctionne sur un principe de **graphe**. Chaque sommet du graphe est une position possible du centre du robot. Chaque arête représente un déplacement possible.

### Graphe pré-construit
Pour gagner du temps, DJ maintient un **graphe pré-construit** (`prebuilt_graph`) à partir des seuls obstacles statiques. Ce graphe n'est calculé qu'une fois et réutilisé à chaque génération de chemin. Il doit être recalculé lorsque des obstacles sont ajoutés ou retirés ; cela se fait automatiquement au besoin, mais peut aussi être forcé avec `dj_engine_force_recalculate_prebuilt_graph` (le recalcul étant coûteux, on évite de le déclencher au mauvais moment).

#### Sommets
Pour construire le graphe, DJ crée un sommet pour chaque point de chaque obstacle. Comme indiqué plus haut, les obstacles sont agrandis pour représenter les zones interdites au centre du robot. Les points de départ et d'arrivée sont également ajoutés.

#### Liens
Ensuite, DJ crée les liens entre les sommets. Pour chaque sommet, il détermine quels autres sommets sont accessibles sans traverser un obstacle, et crée un lien. Il crée aussi des liens le long des arêtes des obstacles, permettant au robot de les longer. En revanche, il ne crée pas de liens sur les diagonales des obstacles, ce qui empêche le robot de les traverser.

### Calcul du chemin
**Prérequis** : être familier avec des algorithmes de recherche de chemin comme Dijkstra ou A\*.

DJ utilise un algorithme de recherche de chemin classique pour trouver le chemin le plus rapide. Cependant, à chaque sommet exploré lors de la résolution, DJ « reconstruit » une partie du graphe pour tenir compte des obstacles dynamiques.

#### Reconstruire le graphe
Le graphe pré-construit ne contient que les obstacles statiques. Pour prendre en compte les obstacles dynamiques, DJ les convertit en obstacles statiques selon l'endroit (et l'instant) depuis lequel on les observe. C'est le rôle du *rebuilder* ([`dj_graph_rebuilder.h`](dj_graph_builder/dj_graph_rebuilder.h)).

##### Désactiver les liens
Les liens qui passent par un obstacle dynamique sont désactivés : le robot ne peut donc pas les emprunter, ce qui permet d'éviter l'obstacle.

##### Conversion des obstacles dynamiques en obstacles statiques
Pour convertir un obstacle dynamique en obstacle statique, DJ considère sa position dans l'**espace-temps** depuis la position du sommet où se trouve l'algorithme. Plus simplement, il calcule les positions où le robot pourrait croiser le centre de l'obstacle dynamique en suivant les chemins les plus directs.

Un obstacle dynamique peut donc avoir plusieurs positions statiques (jusqu'à 8 au maximum, en prenant en compte les cas avec `END_AT_BRAKE` ou `END_AT_LAST_POINT`). Pour le détail, voir la partie [Mathématiques](#mathématiques). En pratique, il est rare qu'un obstacle dynamique ait plus de 2 positions statiques. Ces positions sont ajoutées au graphe comme des obstacles statiques.

#### Pondération et durée
Pour estimer la durée d'un déplacement, DJ prend en compte les conditions de fin (`END_AT_BRAKE`, `END_AT_LAST_POINT`). Il est donc impossible de pondérer directement chaque arête, car la durée dépend de l'ensemble du chemin. DJ calcule donc la durée d'un chemin entier.

Deux algorithmes sont disponibles : **Dijkstra** et **A\***.

**Avantages et inconvénients :**
- Dijkstra offre des solutions plus pertinentes qu'A\*. A\* est efficace pour des graphes très grands et denses, ce qui n'est pas le cas ici. De plus, A\* est optimal lorsque la pondération des arêtes est liée aux distances entre sommets, ce qui n'est pas le cas ici : le robot accélère ou freine quasiment tout le temps, ce qui plaide pour une pondération basée sur la durée des déplacements.
- A\* est moins coûteux en temps de calcul que Dijkstra.

Le solveur est choisi à chaque appel via le paramètre `dj_solver_type_t` (voir [Choix du solveur](#choix-du-solveur)).

### Conclusion
Une fois le ou les chemins les plus rapides déterminés, DJ a terminé son travail : il renvoie les chemins au code appelant. Le suivi du chemin, le recalcul périodique et la gestion des évitements relèvent de la propulsion du robot, en dehors de DJ. Le seul mécanisme de réessai intégré à DJ est le `retry_without_dynamic_obstacle`, qui relance le calcul sans les obstacles dynamiques lorsqu'aucun chemin n'est trouvé. Pour plus de détails sur l'enchaînement, voir [`dj_launcher.c`](dj_launcher/dj_launcher.c).

## Mathématiques
### Conversion des obstacles dynamiques en obstacles statiques
On peut retrouver l'application de ces mathématiques dans le fichier [`dj_obstacle_dynamic.c`](dj_obstacle/dj_obstacle_dynamic.c).
Si vous n'avez pas d'éditeur LaTeX, vous pouvez utiliser ce fichier, mais il est recommandé d'utiliser un éditeur LaTeX pour une meilleure lisibilité.
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
\iff D_{\text{adv}} = \sqrt{\Big(P_{\text{advix}} + V_{\text{advix}} \cdot (t - t_{\text{adv}}) + \frac{1}{2} \cdot A_{\text{advx}} \cdot (t - t_{\text{adv}})^2 - P_{\text{robix}}\Big)^2 + \Big(P_{\text{adviy}} + V_{\text{adviy}} \cdot (t - t_{\text{adv}}) + \frac{1}{2} \cdot A_{\text{advy}} \cdot (t - t_{\text{adv}})^2 - P_{\text{robiy}}\Big)^2}
$$

---

##### 4) Solutions

Les solutions apparaissent lorsque :
$$
D_{\text{adv}} = D_{\text{rob}}
$$

Donc lorsque :
$$
\sqrt{\Big(P_{\text{advix}} + V_{\text{advix}} \cdot (t - t_{\text{adv}}) + \frac{1}{2} \cdot A_{\text{advx}} \cdot (t - t_{\text{adv}})^2 - P_{\text{robix}}\Big)^2 + \Big(P_{\text{adviy}} + V_{\text{adviy}} \cdot (t - t_{\text{adv}}) + \frac{1}{2} \cdot A_{\text{advy}} \cdot (t - t_{\text{adv}})^2 - P_{\text{robiy}}\Big)^2} = V_{\text{rob}} \cdot (t - t_{\text{rob}}) + \frac{1}{2} \cdot A_{\text{rob}} \cdot (t - t_{\text{rob}})^2
$$

En élevant au carré, on obtient une équation polynomiale en $t$ de degré 4, de la forme :
$$
0 = a \cdot t^4 + b \cdot t^3 + c \cdot t^2 + d \cdot t + e
$$

##### Résolution numérique
Cette équation du quatrième degré a donc de 0 à 4 solutions. Les solutions générales étant complexes, on ne calcule pas la forme analytique.

On a l'avantage de pouvoir limiter les solutions à un intervalle de temps $[0, t_{\text{max}}]$.
En effet, il n'a aucun sens de chercher une solution négative ou supérieure à $t_{\text{max}}$.

De plus, $t_{\text{max}}$ est relativement petit, car :
- On ne cherche pas à prédire la position de l'obstacle dans un futur lointain (imprécision de la prédiction).
- De toute façon, n'importe quel robot aura traversé le terrain ou changé de direction en quelques secondes seulement.

###### Méthode de recherche des solutions
Pour tout $t \in [0, t_{\text{max}}]$, on calcule la valeur de l'équation et on regarde si elle est de signe opposé à la précédente.
Si c'est le cas, alors il y a une solution entre $t$ et $t - 1$.

###### Calcul de la position de l'obstacle pour chaque solution
Pour chaque solution trouvée $t$, on calcule la position de l'obstacle :
$$
P_{\text{advx}} = P_{\text{advix}} + V_{\text{advix}} \cdot t + \frac{1}{2} \cdot A_{\text{advx}} \cdot t^2
$$
$$
P_{\text{advy}} = P_{\text{adviy}} + V_{\text{adviy}} \cdot t + \frac{1}{2} \cdot A_{\text{advy}} \cdot t^2
$$
avec $t$ la solution trouvée.

##### Conclusion
On obtient aucune, une ou plusieurs positions possibles de l'obstacle dans l'espace-temps.
