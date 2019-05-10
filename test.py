from CFGLearner import SimpleTeacher, FrequencyTeacher, DifferenceTeacher, Teacher, learn
from nltk import Tree, CFG
from itertools import product
from TreeGenerator import generate_trees
# from nltk.parse import generate
import matplotlib.pyplot as plt
import json
import numpy as np
#plt.use('Agg')
from nltk.draw.util import CanvasFrame
from nltk.draw import TreeWidget
from nltk.parse.generate import generate


def update_weights(tree):
    for t in tree.treepositions():
        tree[t].set_label(int(tree[t].label())+1)


def count_inner_nodes(tree):
    count = 0
    for t in tree.treepositions():
        if isinstance(tree[t], Tree) and len(tree[t]) > 0:
            count = count + 1
    return count


NUM_TO_KEEP = 5
s = [1, 2, 3, 4, 5]
s2 = [3, 4, 5, 1, 2]
s3 = [1, 2, 3, 4]
s4 = [3, 4, 5]
s5 = [1, 2, 3]
kx = []
ky = []
min = []
curr_sol = []
max_len = -1
terminals = set()
for c in s+s2+s3+s4+s5:
    terminals.add(c)
t = len(terminals)
for t1, t2, t3, t4, t5 in product(generate_trees(s, max_len=max_len), generate_trees(s2, max_len=max_len),
                                  generate_trees(s3, max_len=max_len), generate_trees(s4, max_len=max_len),
                                  generate_trees(s5, max_len=max_len)):
    trees = [t1, t2, t3, t4, t5]
    s = SimpleTeacher()
    total_nodes = sum([count_inner_nodes(t) for t in trees])
    for tree in trees:
        s.addPositiveExample(tree)
    c = learn(s, {})
    nt_set = set()
    for p in c.productions():
        nt_set.add(p.lhs())
    nt = len(nt_set)
    p = float(total_nodes)/float(nt-1-t)
    kx.append(nt)
    ky.append(p)
    if len(min) < NUM_TO_KEEP:
        min.append(p)
        curr_sol.append((c, tuple(trees)))
    if any([p >= m for m in min]):
        for i, m in enumerate(min):
            if p >= m:
                curr_sol[i] = (c, tuple(trees))
                min[i] = p
                break

print(min)
print(curr_sol[0])
plt.scatter(kx, ky)
plt.show()
for ind, sol in enumerate(curr_sol):
    in_row = 4
    len_one = 140
    height = 200
    i = 0
    cf = CanvasFrame()
    for t in sol[1]:
        tc = TreeWidget(cf.canvas(), t)
        x, y = (i % in_row)*len_one, int(i / in_row)*height
        print(x, y)
        cf.add_widget(tc, x, y)
        i = i + 1
    cf.print_to_file('trees' + str(ind) + '.ps')
    cf.destroy()
exit()
a1 = Tree(0, [Tree(0, [Tree(1, []), Tree(2, [])]), Tree(0, [Tree(3, []), Tree(4, [])])])
a2 = Tree(0, [Tree(0, [Tree(3, []), Tree(4, [])]), Tree(0, [Tree(1, []), Tree(2, [])])])
w1 = Tree(4, [Tree(2, [Tree(10, []), Tree(10, [])]), Tree(2, [Tree(10, []), Tree(10, [])])])
w2 = Tree(4, [Tree(2, [Tree(10, []), Tree(10, [])]), Tree(2, [Tree(10, []), Tree(10, [])])])

d = DifferenceTeacher(2)
d.addPositiveExample(a1, w1)
d.addPositiveExample(a2, w2)
di = {}
c = learn(d, di)
print(c)
for sentence in generate(c, n=10):
    print(' '.join(sentence))
exit()
mla = open('output_mla_manual2.txt')
mla_list = json.load(mla)
di = mla_list['cogs_dict']['reverse_dict']
di = {int(key): di[key] for key in di}
mla_list = [(Tree.fromstring(tup[0]), Tree.fromstring(tup[1])) for tup in mla_list['trees']]
for tree, weights in mla_list:
    update_weights(weights)
    d.addPositiveExample(tree, weights)
c = learn(d, di)
print(c)
exit()
"""
a1 = Tree(0, [Tree(1, []), Tree(0, [Tree(0, [Tree(1, []), Tree(2, [])]), Tree(2, [])])])
a2 = Tree(0, [Tree(1, []), Tree(2, [])])
a3 = Tree(0, [Tree(1, []), Tree(0, [Tree(0, [Tree(0, [Tree(1, []), Tree(2, [])]), Tree(2, [])]), Tree(2, [])])])

t1 = Tree(0, [Tree(2, []), Tree(1, [])])
t2 = Tree(0, [Tree(1, []), Tree(2, [])])
t3 = Tree(0, [Tree(0, [Tree(1, []), Tree(2, [])]), Tree(0, [Tree(2, []), Tree(1, [])])])
t4 = Tree(0, [Tree(0, [Tree(1, []), Tree(2, [])]), Tree(0, [Tree(0, [Tree(1, []), Tree(2, [])]), Tree(0, [Tree(2, []), Tree(1, [])])])])
t5 = Tree(0, [Tree(0, [Tree(1, []), Tree(2, [])]), Tree(0, [Tree(0, [Tree(1, []), Tree(2, [])]), Tree(0, [Tree(0, [Tree(1, []), Tree(2, [])]), Tree(0, [Tree(2, []), Tree(1, [])])])])])


p1 = Tree(0, [Tree(1, []), Tree(1, [])])
p2 = Tree(0, [Tree(1, []), Tree(0, [Tree(1, []), Tree(0, [Tree(1, []), Tree(1, [])])])])
p3 = Tree(0, [Tree(1, []), p2])
p4 = Tree(0, [Tree(1, []), p3])
p5 = Tree(0, [Tree(1, []), p4])

T_NUM = 20

pi = [Tree(0, [Tree(10, []), Tree(20, [])])]
for i in range(1, T_NUM):
    pi.append(Tree(0, [Tree(10, []), Tree(0, [pi[i-1], Tree(20, [])])]))


pos = [i for i in range(T_NUM) if i > 1]
t2 = SimpleTeacher()
for i in range(T_NUM):
    if i in pos:
        t2.addPositiveExample(pi[i])
    else:
        t2.addNegativeExample(pi[i])
c = learn(t2)
print(c)
"""
t = FrequencyTeacher(1, 0.95)
mla = open('output_mla_manual2.txt')
mla_list = json.load(mla)
d = mla_list['cogs_dict']['reverse_dict']
d = {int(key): d[key] for key in d}
mla_list = [(Tree.fromstring(tup[0]), tup[1]) for tup in mla_list['trees']]
i = 0
for tree, occ in mla_list:
    i = i + 1
    t.addPositiveExamples(tree, occ)


print('learning')
print(len(mla_list))
c = learn(t, d)
print(c)
exit()
t = FrequencyTeacher(45, 0.9)
fileDict = open('dict.txt')
d = json.load(fileDict)
d = {int(key): d[key] for key in d}
file = open('output_beta.txt')
beta_list = json.load(file)
beta_list = [(Tree.fromstring(tup[0]), tup[1]) for tup in beta_list]
file = open('output_alpha.txt')
alpha_list = json.load(file)
alpha_list = [(Tree.fromstring(tup[0]), tup[1]) for tup in alpha_list]
file = open('output_ins.txt')
ins_list = json.load(file)
ins_list = [(Tree.fromstring(tup[0]), tup[1]) for tup in ins_list]

for tree, occ in alpha_list:
    t.addPositiveExamples(tree, occ)
for tree, occ in beta_list:
    t.addNegativeExamples(tree, occ)

print('learning')
print(len(alpha_list), len(beta_list))
c = learn(t, d)
print(c)
