import re
import matplotlib.pyplot as plt
import networkx as nx

def parse_obstacle_data(data):
    obstacles = {}
    pattern = re.compile(r"Obstacle (\d+) added, points : ((?:\[I\]\(-?\d+, -?\d+\) )+)")
    points_pattern = re.compile(r"\((-?\d+), (-?\d+)\)")

    for match in pattern.finditer(data):
        obstacle_id = int(match.group(1))
        points_str = match.group(2)
        points = [(int(x), int(y)) for x, y in points_pattern.findall(points_str)]
        points = [(y, -x) for x, y in points]
        obstacles[obstacle_id] = points

    return obstacles

def parse_graph_data(data):
    # Parse edges and their enabled status
    pattern = re.compile(r'\((\-?\d+), (\-?\d+)\).*?\((\-?\d+), (\-?\d+)\).*?enabled\s*=\s*(\d)')
    enabled_edges = []
    disabled_edges = []
    for line in data.split('\n'):
        m = pattern.search(line)
        if m:
            x1, y1, x2, y2, enabled = m.groups()
            node1 = (int(y1), -int(x1))
            node2 = (int(y2), -int(x2))
            edge = (node1, node2)
            if enabled == '1':
                enabled_edges.append(edge)
            else:
                disabled_edges.append(edge)
    return enabled_edges, disabled_edges

def plot_combined(obstacles, edge_data):
    enabled_edges, disabled_edges = edge_data

    plt.figure(figsize=(10, 10), dpi=500)  # Increase dpi for better quality

    # Plot obstacles
    for obstacle_id, points in obstacles.items():
        x, y = zip(*points)
        plt.fill(x, y, alpha=0.5, label=f"Obstacle {obstacle_id}")

    # Collect all nodes and mark disabled nodes
    enabled_nodes = set()
    for e in enabled_edges:
        enabled_nodes.update(e)
    disabled_nodes = set()
    for e in disabled_edges:
        disabled_nodes.update(e)
    # Remove enabled nodes from disabled set (so only nodes that are only in disabled edges are marked disabled)
    disabled_nodes = disabled_nodes - enabled_nodes

    # Plot enabled graph
    G_enabled = nx.Graph()
    G_enabled.add_edges_from(enabled_edges)
    pos = {node: node for node in G_enabled.nodes()}
    nx.draw(
        G_enabled, pos, with_labels=False, node_size=0, node_color='skyblue',
        edge_color='black', width=0.1, alpha=1
    )

    # Plot disabled graph
    if disabled_edges:
        G_disabled = nx.Graph()
        G_disabled.add_edges_from(disabled_edges)
        pos_disabled = {node: node for node in G_disabled.nodes()}
        nx.draw(
            G_disabled, pos_disabled, with_labels=False, node_size=0, node_color='red',
            edge_color='red', width=0.1, alpha=0.7, style='dashed'
        )

    # Highlight disabled nodes
    if disabled_nodes:
        # Only plot nodes that have a position in the enabled graph
        disabled_nodes_in_enabled = [n for n in disabled_nodes if n in pos]
        if disabled_nodes_in_enabled:
            nx.draw_networkx_nodes(
                G_enabled, pos, nodelist=disabled_nodes_in_enabled,
                node_color='red', node_size=20, alpha=0.8
            )
        # Optionally, plot nodes only present in disabled edges (not in enabled graph)
        # using their own positions:
        # disabled_only_nodes = [n for n in disabled_nodes if n not in pos]
        # if disabled_only_nodes:
        #     nx.draw_networkx_nodes(
        #         G_disabled, pos_disabled, nodelist=disabled_only_nodes,
        #         node_color='red', node_size=20, alpha=0.8
        #     )

    plt.xlabel("X")
    plt.ylabel("Y")
    plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
    plt.grid(True)
    plt.axis("equal")
    plt.savefig("dj_graph.png", bbox_inches='tight')
    # plt.show()

if __name__ == "__main__":
    obstacle_data = """
[I]Obstacle 0 added, points : [I](688, 2162) [I](688, 3088) [I](-238, 3088) [I](-238, 2162) [I]
[I]Obstacle 1 added, points : [I](1305, 2342) [I](1402, 2398) [I](1458, 2494) [I](1458, 3055) [I](1402, 3152) [I](1305, 3208) [I](845, 3208) [I](748, 3152) [I](692, 3055) [I](692, 2495) [I](748, 2398) [I](845, 2342) [I]
[I]Obstacle 5 added, points : [I](1305, -208) [I](1402, -152) [I](1458, -56) [I](1458, 505) [I](1402, 602) [I](1305, 658) [I](845, 658) [I](748, 602) [I](692, 505) [I](692, -55) [I](748, -152) [I](845, -208) [I]
[I]Obstacle 2 added, points : [I](2208, 792) [I](2208, 1658) [I](1342, 1658) [I](1342, 792) [I]
[I]Obstacle 6 added, points : [I](2208, 1342) [I](2208, 2208) [I](1342, 2208) [I](1342, 1342) [I]
[I]Obstacle 3 added, points : [I](2238, 312) [I](2238, 1238) [I](1612, 1238) [I](1612, 312) [I]
[I]Obstacle 7 added, points : [I](2238, 1767) [I](2238, 2693) [I](1612, 2693) [I](1612, 1767) [I]
[I]Obstacle 4 added, points : [I](2238, 2312) [I](2238, 3238) [I](1612, 3238) [I](1612, 2312) [I]
[I]Obstacle 8 added, points : [I](2238, -238) [I](2238, 688) [I](1612, 688) [I](1612, -238) [I]
[I]Obstacle 9 added, points : [I](1113, -213) [I](1113, 363) [I](237, 363) [I](237, -213) [I]
[I]Obstacle 10 added, points : [I](2038, -213) [I](2038, 363) [I](1162, 363) [I](1162, -213) [I]
[I]Obstacle 11 added, points : [I](563, 387) [I](563, 1263) [I](-13, 1263) [I](-13, 387) [I]
[I]Obstacle 12 added, points : [I](1863, 337) [I](1974, 401) [I](2038, 511) [I](2038, 1038) [I](1974, 1149) [I](1863, 1213) [I](1637, 1213) [I](1526, 1149) [I](1462, 1038) [I](1462, 512) [I](1526, 401) [I](1637, 337) [I]
[I]Obstacle 13 added, points : [I](1163, 662) [I](1274, 726) [I](1338, 836) [I](1338, 1363) [I](1274, 1474) [I](1163, 1538) [I](937, 1538) [I](826, 1474) [I](762, 1363) [I](762, 837) [I](826, 726) [I](937, 662) [I]
[I]Obstacle 14 added, points : [I](1163, 1462) [I](1274, 1526) [I](1338, 1636) [I](1338, 2163) [I](1274, 2274) [I](1163, 2338) [I](937, 2338) [I](826, 2274) [I](762, 2163) [I](762, 1637) [I](826, 1526) [I](937, 1462) [I]
[I]Obstacle 15 added, points : [I](563, 1737) [I](563, 2613) [I](-13, 2613) [I](-13, 1737) [I]
[I]Obstacle 16 added, points : [I](1863, 1787) [I](1974, 1851) [I](2038, 1961) [I](2038, 2488) [I](1974, 2599) [I](1863, 2663) [I](1637, 2663) [I](1526, 2599) [I](1462, 2488) [I](1462, 1962) [I](1526, 1851) [I](1637, 1787) [I]
[I]Obstacle 17 added, points : [I](1113, 2637) [I](1113, 3213) [I](237, 3213) [I](237, 2637) [I]
[I]Obstacle 18 added, points : [I](2038, 2637) [I](2038, 3213) [I](1162, 3213) [I](1162, 2637) [I]
[I]Obstacle 19 added, points : [I](688, -238) [I](688, 388) [I](-238, 388) [I](-238, -238) [I]
[I]Obstacle 20 added, points : [I](688, 2612) [I](688, 3238) [I](-238, 3238) [I](-238, 2612) [I]
[I]Obstacle 21 added, points : [I](-238, 412) [I](438, 412) [I](438, 812) [I](688, 812) [I](688, 2188) [I](438, 2188) [I](438, 2588) [I](-238, 2588) [I]
[I]Obstacle 22 added, points : [I](838, -238) [I](838, 1038) [I](-238, 1038) [I](-238, -238) [I]
[I]Obstacle 23 added, points : [I](1238, 762) [I](1238, 2126) [I](1138, 2576) [I](1138, 3238) [I](-238, 3238) [I](-238, 762) [I]
    """

    graph_data = """
[I]Link 0 : (688, 2162) --- (762, 1363) enabled = 1
[I]Link 1 : (688, 2162) --- (762, 837) enabled = 1
[I]Link 2 : (688, 2162) --- (826, 2274) enabled = 1
[I]Link 3 : (688, 2162) --- (762, 2163) enabled = 1
[I]Link 4 : (688, 2162) --- (762, 1637) enabled = 1
[I]Link 5 : (688, 2162) --- (1113, 2637) enabled = 1
[I]Link 6 : (688, 2162) --- (1162, 2637) enabled = 1
[I]Link 7 : (688, 2162) --- (688, 812) enabled = 1
[I]Link 8 : (688, 2162) --- (688, 2188) enabled = 1
[I]Link 9 : (1612, 1767) --- (1637, 1213) enabled = 1
[I]Link 10 : (1612, 1767) --- (1526, 1149) enabled = 1
[I]Link 11 : (1612, 1767) --- (1462, 1038) enabled = 1
[I]Link 12 : (1612, 1767) --- (1338, 836) enabled = 1
[I]Link 13 : (1612, 1767) --- (1338, 1363) enabled = 1
[I]Link 14 : (1612, 1767) --- (1274, 1474) enabled = 1
[I]Link 15 : (1612, 1767) --- (1274, 1526) enabled = 1
[I]Link 16 : (1612, 1767) --- (1338, 1636) enabled = 1
[I]Link 17 : (1612, 1767) --- (1526, 1851) enabled = 1
[I]Link 18 : (1113, 363) --- (1162, 363) enabled = 1
[I]Link 19 : (1113, 363) --- (1462, 1038) enabled = 1
[I]Link 20 : (1113, 363) --- (1462, 512) enabled = 1
[I]Link 21 : (1113, 363) --- (1526, 401) enabled = 1
[I]Link 22 : (1113, 363) --- (1163, 662) enabled = 1
[I]Link 23 : (1113, 363) --- (1274, 726) enabled = 1
[I]Link 24 : (1113, 363) --- (1338, 836) enabled = 1
[I]Link 25 : (1113, 363) --- (826, 726) enabled = 1
[I]Link 26 : (1113, 363) --- (937, 662) enabled = 1
[I]Link 27 : (1113, 363) --- (688, 388) enabled = 1
[I]Link 28 : (1113, 363) --- (688, 812) enabled = 1
[I]Link 29 : (1162, 363) --- (1462, 1038) enabled = 1
[I]Link 30 : (1162, 363) --- (1462, 512) enabled = 1
[I]Link 31 : (1162, 363) --- (1526, 401) enabled = 1
[I]Link 32 : (1162, 363) --- (1163, 662) enabled = 1
[I]Link 33 : (1162, 363) --- (1274, 726) enabled = 1
[I]Link 34 : (1162, 363) --- (1338, 836) enabled = 1
[I]Link 35 : (1162, 363) --- (826, 726) enabled = 1
[I]Link 36 : (1162, 363) --- (937, 662) enabled = 1
[I]Link 37 : (1162, 363) --- (688, 388) enabled = 1
[I]Link 38 : (1162, 363) --- (688, 812) enabled = 1
[I]Link 39 : (1637, 1213) --- (1526, 1149) enabled = 1
[I]Link 40 : (1637, 1213) --- (1338, 1363) enabled = 1
[I]Link 41 : (1637, 1213) --- (1274, 1474) enabled = 1
[I]Link 42 : (1637, 1213) --- (1274, 1526) enabled = 1
[I]Link 43 : (1637, 1213) --- (1338, 1636) enabled = 1
[I]Link 44 : (1637, 1213) --- (1338, 2163) enabled = 1
[I]Link 45 : (1637, 1213) --- (1462, 1962) enabled = 1
[I]Link 46 : (1637, 1213) --- (1526, 1851) enabled = 1
[I]Link 47 : (1526, 1149) --- (1462, 1038) enabled = 1
[I]Link 48 : (1526, 1149) --- (1274, 726) enabled = 1
[I]Link 49 : (1526, 1149) --- (1338, 836) enabled = 1
[I]Link 50 : (1526, 1149) --- (1338, 1363) enabled = 1
[I]Link 51 : (1526, 1149) --- (1274, 1474) enabled = 1
[I]Link 52 : (1526, 1149) --- (1274, 1526) enabled = 1
[I]Link 53 : (1526, 1149) --- (1338, 1636) enabled = 1
[I]Link 54 : (1526, 1149) --- (1338, 2163) enabled = 1
[I]Link 55 : (1526, 1149) --- (1462, 1962) enabled = 1
[I]Link 56 : (1526, 1149) --- (1526, 1851) enabled = 1
[I]Link 57 : (1462, 1038) --- (1462, 512) enabled = 1
[I]Link 58 : (1462, 1038) --- (1274, 726) enabled = 1
[I]Link 59 : (1462, 1038) --- (1338, 836) enabled = 1
[I]Link 60 : (1462, 1038) --- (1338, 1363) enabled = 1
[I]Link 61 : (1462, 1038) --- (1338, 1636) enabled = 1
[I]Link 62 : (1462, 1038) --- (1338, 2163) enabled = 1
[I]Link 63 : (1462, 1038) --- (1462, 1962) enabled = 1
[I]Link 64 : (1462, 1038) --- (1526, 1851) enabled = 1
[I]Link 65 : (1462, 512) --- (1526, 401) enabled = 1
[I]Link 66 : (1462, 512) --- (1163, 662) enabled = 1
[I]Link 67 : (1462, 512) --- (1274, 726) enabled = 1
[I]Link 68 : (1462, 512) --- (1338, 836) enabled = 1
[I]Link 69 : (1462, 512) --- (1338, 1363) enabled = 1
[I]Link 70 : (1462, 512) --- (937, 662) enabled = 1
[I]Link 71 : (1462, 512) --- (1338, 1636) enabled = 1
[I]Link 72 : (1462, 512) --- (1338, 2163) enabled = 1
[I]Link 73 : (1462, 512) --- (688, 388) enabled = 1
[I]Link 74 : (1526, 401) --- (1163, 662) enabled = 1
[I]Link 75 : (1526, 401) --- (1274, 726) enabled = 1
[I]Link 76 : (1526, 401) --- (937, 662) enabled = 1
[I]Link 77 : (1526, 401) --- (688, 388) enabled = 1
[I]Link 78 : (1163, 662) --- (1274, 726) enabled = 1
[I]Link 79 : (1163, 662) --- (937, 662) enabled = 1
[I]Link 80 : (1163, 662) --- (688, 388) enabled = 1
[I]Link 81 : (1274, 726) --- (1338, 836) enabled = 1
[I]Link 82 : (1274, 726) --- (688, 388) enabled = 1
[I]Link 83 : (1338, 836) --- (1338, 1363) enabled = 1
[I]Link 84 : (1338, 836) --- (1462, 2488) enabled = 1
[I]Link 85 : (1338, 836) --- (1462, 1962) enabled = 1
[I]Link 86 : (1338, 836) --- (1526, 1851) enabled = 1
[I]Link 87 : (1338, 1363) --- (1274, 1474) enabled = 1
[I]Link 88 : (1338, 1363) --- (1274, 1526) enabled = 1
[I]Link 89 : (1338, 1363) --- (1338, 1636) enabled = 1
[I]Link 90 : (1338, 1363) --- (1462, 2488) enabled = 1
[I]Link 91 : (1338, 1363) --- (1462, 1962) enabled = 1
[I]Link 92 : (1338, 1363) --- (1526, 1851) enabled = 1
[I]Link 93 : (1274, 1474) --- (1274, 1526) enabled = 1
[I]Link 94 : (1274, 1474) --- (1338, 1636) enabled = 1
[I]Link 95 : (1274, 1474) --- (1526, 1851) enabled = 1
[I]Link 96 : (826, 1474) --- (762, 1363) enabled = 1
[I]Link 97 : (826, 1474) --- (762, 1637) enabled = 1
[I]Link 98 : (826, 1474) --- (826, 1526) enabled = 1
[I]Link 99 : (762, 1363) --- (762, 837) enabled = 1
[I]Link 100 : (762, 1363) --- (762, 1637) enabled = 1
[I]Link 101 : (762, 1363) --- (826, 1526) enabled = 1
[I]Link 102 : (762, 1363) --- (688, 388) enabled = 1
[I]Link 103 : (762, 1363) --- (688, 2612) enabled = 1
[I]Link 104 : (762, 1363) --- (688, 812) enabled = 1
[I]Link 105 : (762, 1363) --- (688, 2188) enabled = 1
[I]Link 106 : (762, 837) --- (826, 726) enabled = 1
[I]Link 107 : (762, 837) --- (688, 388) enabled = 1
[I]Link 108 : (762, 837) --- (688, 2612) enabled = 1
[I]Link 109 : (762, 837) --- (688, 812) enabled = 1
[I]Link 110 : (762, 837) --- (688, 2188) enabled = 1
[I]Link 111 : (826, 726) --- (937, 662) enabled = 1
[I]Link 112 : (826, 726) --- (688, 388) enabled = 1
[I]Link 113 : (826, 726) --- (688, 812) enabled = 1
[I]Link 114 : (937, 662) --- (688, 388) enabled = 1
[I]Link 115 : (1274, 1526) --- (1338, 1636) enabled = 1
[I]Link 116 : (1274, 1526) --- (1526, 1851) enabled = 1
[I]Link 117 : (1338, 1636) --- (1338, 2163) enabled = 1
[I]Link 118 : (1338, 1636) --- (1462, 2488) enabled = 1
[I]Link 119 : (1338, 1636) --- (1462, 1962) enabled = 1
[I]Link 120 : (1338, 1636) --- (1526, 1851) enabled = 1
[I]Link 121 : (1338, 2163) --- (1274, 2274) enabled = 1
[I]Link 122 : (1338, 2163) --- (1462, 2488) enabled = 1
[I]Link 123 : (1338, 2163) --- (1462, 1962) enabled = 1
[I]Link 124 : (1338, 2163) --- (1526, 1851) enabled = 1
[I]Link 125 : (1338, 2163) --- (1113, 2637) enabled = 1
[I]Link 126 : (1338, 2163) --- (1162, 2637) enabled = 1
[I]Link 127 : (1274, 2274) --- (1163, 2338) enabled = 1
[I]Link 128 : (1274, 2274) --- (1526, 2599) enabled = 1
[I]Link 129 : (1274, 2274) --- (1462, 2488) enabled = 1
[I]Link 130 : (1274, 2274) --- (1462, 1962) enabled = 1
[I]Link 131 : (1274, 2274) --- (1526, 1851) enabled = 1
[I]Link 132 : (1274, 2274) --- (1113, 2637) enabled = 1
[I]Link 133 : (1274, 2274) --- (1162, 2637) enabled = 1
[I]Link 134 : (1274, 2274) --- (688, 2612) enabled = 1
[I]Link 135 : (1163, 2338) --- (937, 2338) enabled = 1
[I]Link 136 : (1163, 2338) --- (1526, 2599) enabled = 1
[I]Link 137 : (1163, 2338) --- (1462, 2488) enabled = 1
[I]Link 138 : (1163, 2338) --- (1113, 2637) enabled = 1
[I]Link 139 : (1163, 2338) --- (1162, 2637) enabled = 1
[I]Link 140 : (1163, 2338) --- (688, 2612) enabled = 1
[I]Link 141 : (937, 2338) --- (826, 2274) enabled = 1
[I]Link 142 : (937, 2338) --- (1526, 2599) enabled = 1
[I]Link 143 : (937, 2338) --- (1462, 2488) enabled = 1
[I]Link 144 : (937, 2338) --- (1113, 2637) enabled = 1
[I]Link 145 : (937, 2338) --- (1162, 2637) enabled = 1
[I]Link 146 : (937, 2338) --- (688, 2612) enabled = 1
[I]Link 147 : (826, 2274) --- (762, 2163) enabled = 1
[I]Link 148 : (826, 2274) --- (1113, 2637) enabled = 1
[I]Link 149 : (826, 2274) --- (1162, 2637) enabled = 1
[I]Link 150 : (826, 2274) --- (688, 2612) enabled = 1
[I]Link 151 : (826, 2274) --- (688, 2188) enabled = 1
[I]Link 152 : (762, 2163) --- (762, 1637) enabled = 1
[I]Link 153 : (762, 2163) --- (688, 388) enabled = 1
[I]Link 154 : (762, 2163) --- (688, 2612) enabled = 1
[I]Link 155 : (762, 2163) --- (688, 812) enabled = 1
[I]Link 156 : (762, 2163) --- (688, 2188) enabled = 1
[I]Link 157 : (762, 1637) --- (826, 1526) enabled = 1
[I]Link 158 : (762, 1637) --- (688, 388) enabled = 1
[I]Link 159 : (762, 1637) --- (688, 2612) enabled = 1
[I]Link 160 : (762, 1637) --- (688, 812) enabled = 1
[I]Link 161 : (762, 1637) --- (688, 2188) enabled = 1
[I]Link 162 : (1526, 2599) --- (1462, 2488) enabled = 1
[I]Link 163 : (1526, 2599) --- (1113, 2637) enabled = 1
[I]Link 164 : (1526, 2599) --- (1162, 2637) enabled = 1
[I]Link 165 : (1526, 2599) --- (688, 2612) enabled = 1
[I]Link 166 : (1462, 2488) --- (1462, 1962) enabled = 1
[I]Link 167 : (1462, 2488) --- (1113, 2637) enabled = 1
[I]Link 168 : (1462, 2488) --- (1162, 2637) enabled = 1
[I]Link 169 : (1462, 2488) --- (688, 2612) enabled = 1
[I]Link 170 : (1462, 1962) --- (1526, 1851) enabled = 1
[I]Link 171 : (1462, 1962) --- (1113, 2637) enabled = 1
[I]Link 172 : (1462, 1962) --- (1162, 2637) enabled = 1
[I]Link 173 : (1113, 2637) --- (1162, 2637) enabled = 1
[I]Link 174 : (1113, 2637) --- (688, 2612) enabled = 1
[I]Link 175 : (1113, 2637) --- (688, 2188) enabled = 1
[I]Link 176 : (1162, 2637) --- (688, 2612) enabled = 1
[I]Link 177 : (1162, 2637) --- (688, 2188) enabled = 1
[I]Link 178 : (688, 388) --- (688, 812) enabled = 1
[I]Link 179 : (688, 2612) --- (688, 2188) enabled = 1
    """

    obstacles = parse_obstacle_data(obstacle_data)
    edge_data = parse_graph_data(graph_data)
    plot_combined(obstacles, edge_data)
