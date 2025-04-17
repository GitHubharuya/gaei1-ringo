import numpy as np
from scipy.spatial import Delaunay

# 点群データの読み込み
point = np.loadtxt("Points.txt")

# x, y, z座標とラベルを格納
point_all = []
point_all_with_z = []
labels = []

for i in range(len(point)-1, -1, -1):
    # 東西反転: x座標を反転
    point_all.append([point[i][0] * -1, point[i][1]])  # x, y座標（x座標を反転）
    point_all_with_z.append([point[i][0] * -1, point[i][1], point[i][2]])  # x, y, z座標（x座標を反転）
    labels.append(point[i][3])  # ラベル（地面: 1, 建物: 0）

# ドロネー三角形分割を実行
tri = Delaunay(point_all)  # 全体のドロネー分割
index = tri.simplices  # 三角形分割された頂点のインデックス

# y座標を-1倍して向きを変える
for i in range(len(point_all_with_z)):
    point_all_with_z[i][1] *= -1

# VRMLファイルに書き込む
with open('vrml.wrl', 'w', encoding='utf-8') as f:
    f.write("#VRML V2.0 utf8\n")
    f.write("Shape {\n")
    f.write("geometry IndexedFaceSet {\n")
    
    # 頂点の座標
    f.write("\tcoord Coordinate {\n\t\tpoint [\n")
    for point in point_all_with_z:
        f.write(f"\t\t\t{point[0]} {point[2]} {point[1]},\n")
    f.write("\t\t]\n\t}\n")
    
    # 各三角形のインデックス
    f.write("\tcoordIndex [\n")
    for tri_idx in index:
        f.write(f"\t\t{tri_idx[0]}, {tri_idx[1]}, {tri_idx[2]}, -1,\n")
    f.write("\t]\n")
    
    # ポリゴンごとに色を設定
    f.write("\tcolor Color {\n\t\tcolor [\n")
    for tri_idx in index:
        if labels[tri_idx[0]] != 1 or labels[tri_idx[1]] != 1 or labels[tri_idx[2]] != 1:
            f.write("\t\t\t0.5 0.78 0.78,\n")  # 建物（青）
        else:
            f.write("\t\t\t0.43 0.43 0.43,\n")  # 地面（灰）
    f.write("\t\t]\n\t}\n")
    
    # 各ポリゴンごとに色を適用
    f.write("\tcolorPerVertex FALSE\n")
    f.write("}\n")

    # Material設定: 発光を抑える
    f.write("\tappearance Appearance {\n\t\tmaterial Material {\n")
    f.write("\t\t\temissiveColor 0 0 0\n")
    f.write("\t\t}\n\t}\n")

    f.write("}\n")

    f.write("Transform {\n\ttranslation 33000 50 7000\n\tscale 100 100 100\n\tchildren[\n\t\tInline{\n\t\t\turl \"maeda.wrl\"\n\t\t}\n\t]\n}")
    
    f.write("Transform {\n\ttranslation 32500 50 7000\n\tscale 100 100 100\n\tchildren[\n\t\tInline{\n\t\t\turl \"komiya.wrl\"\n\t\t}\n\t]\n}")
    
    f.write("Transform {\n\ttranslation 33000 50 6000\n\tscale 100 100 100\n\tchildren[\n\t\tInline{\n\t\t\turl \"ota.wrl\"\n\t\t}\n\t]\n}")

    f.write("Transform {\n\ttranslation 32500 50 6000\n\tscale 100 100 100\n\tchildren[\n\t\tInline{\n\t\t\turl \"sasaki.wrl\"\n\t\t}\n\t]\n}")

f.close()
