from CFGLearner import SimpleTeacher, FrequencyTeacher, DifferenceTeacher, Teacher, learn, TreeComparator, test_np
from nltk import Tree, CFG
from itertools import product
from TreeGenerator import generate_trees
# from nltk.parse import generate
import matplotlib.pyplot as plt
import json
import tkinter as tk
import numpy as np
#plt.use('Agg')
from nltk.draw.util import CanvasFrame
from nltk.draw import TreeWidget
from nltk.parse.generate import generate
import tkinter

a = test_np()
print(a)
exit()
def update_weights(tree):
    for t in tree.treepositions():
        tree[t].set_label(int(tree[t].label())+1)


def count_inner_nodes(tree):
    count = 0
    for t in tree.treepositions():
        if isinstance(tree[t], Tree) and len(tree[t]) > 0:
            count = count + 1
    return count


def get_alphabet_len(trees):
    t_set = set()
    for tree in trees:
        for t in tree.treepositions():
            if isinstance(tree[t], Tree) and len(tree[t]) == 0:
                t_set.add(tree[t].label())
    return len(t_set)


def measure_generalization(trees, grammar):
    total_nodes = sum([count_inner_nodes(t) for t in trees])
    t = get_alphabet_len(trees)
    nt_set = set()
    for p in grammar.productions():
        nt_set.add(p.lhs())
    nt = len(nt_set)
    p = float(total_nodes)/float(nt)
    return p, nt


def generate_all_trees():
    NUM_TO_KEEP = 10
    s1 = [1, 2, 3, 4, 5]
    s2 = [3, 4, 5, 1, 2]
    s3 = [1, 2, 3, 4]
    s4 = [3, 4, 5]
    s5 = [1, 2]
    kx = []
    ky = []
    min = []
    strings = [s1, s2, s4, s5]
    strings = [s for s in strings]
    curr_sol = []
    max_len = -1
    terminals = set()
    for c in sum(strings, []):
        terminals.add(c)
    t = len(terminals)
    for t1, t2, t3, t4 in product(*[generate_trees(s, max_len=max_len) for s in strings]):
        trees = [t1, t2, t3, t4]
        s = SimpleTeacher()
        total_nodes = sum([count_inner_nodes(t) for t in trees])
        for tree in trees:
            s.addPositiveExample(tree)
        c = learn(s, {})
        """
        nt_set = set()
        for p in c.productions():
            nt_set.add(p.lhs())
        nt = len(nt_set)
        p = float(total_nodes)/float(nt-1-t)
        """
        p, nt = measure_generalization(trees, c)
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
    print(sum(min)/len(min))
    print(curr_sol[0])
    plt.scatter(kx, ky)
    plt.show()
    for ind, sol in enumerate(curr_sol):
        print('p')
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


"""
cmp = TreeComparator(0, 1, 2)
mla = open('output_mla_manual2.txt')
mla_list = json.load(mla)
mla_list = [(Tree.fromstring(tup[0]), Tree.fromstring(tup[1])) for tup in mla_list['trees']]
for (t1, _), (t2, _) in product(mla_list[-1:], mla_list[-1:]):
    print(t1, t2)
    s = cmp.compare(t1, t2)
    print(s)
exit()
"""
g = []
x = range(10)
for param in x:
    d = DifferenceTeacher(param)
    cmp = TreeComparator(0, 20, 20)
    d.setTreeComparator(cmp)
    mla = open('output_mla_manual2.txt')
    mla_list = json.load(mla)
    di = mla_list['cogs_dict']['reverse_dict']
    di = {int(key): di[key] for key in di}
    mla_list = [(Tree.fromstring(tup[0]), Tree.fromstring(tup[1])) for tup in mla_list['trees']]
    for tree, weights in mla_list:
        update_weights(weights)
        d.addPositiveExample(tree, weights)
    c = learn(d, di)
    p, nt = measure_generalization([tup[0] for tup in mla_list], c)
    g.append(p)
    if param == 2 or param == 8:
        print(c)
    print("param: {0}, generalization: {1}, nt: {2}".format(param, p, nt))
plt.plot(x, g)
plt.show()
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
