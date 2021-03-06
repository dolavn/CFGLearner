from CFGLearner import SimpleTeacher, FrequencyTeacher, DifferenceTeacher, Teacher, learn, TreeComparator, test_arma,\
    SimpleMultiplicityTeacher, learnMult, learnMultPos, set_verbose
from nltk import Tree, CFG
from itertools import product, tee
from TreeGenerator import generate_trees, generate_distribution
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


def calc_prob_non_uniform(seq, alphabet, index=0, letter=0, letter_prob=0.9,
                          total_prob=1.0, lengths=None):
    size = len(seq)
    alphabet_size = len(alphabet)
    sum_lengths = alphabet_size**(size-1) if lengths is None else sum([
        alphabet_size**length for
        length in lengths])
    curr_prob = total_prob/(alphabet_size**(size-1))
    if len(seq) >= index:
        if seq[index] == letter:
            curr_prob = curr_prob*letter_prob
        else:
            curr_prob = curr_prob*(1-letter_prob)
    else:
        curr_prob = curr_prob*(1/alphabet_size)
    curr_prob = curr_prob * (alphabet_size**size)
    curr_prob = curr_prob / sum_lengths
    return curr_prob


def get_trees_swap_test(swap_prob=0.5):
    t = Tree(0, [Tree(1, []), Tree(2, [])])
    t2 = Tree(0, [Tree(1, []), Tree(0, [Tree(1, []), Tree(2, [])])])
    t3 = Tree(0, [Tree(2, []), Tree(0, [Tree(1, []), Tree(2, [])])])
    t4 = Tree(0, [Tree(0, [Tree(1, []), Tree(2, [])]), Tree(1, [])])
    s = Tree(0, [Tree(2, []), Tree(1, [])])
    s2 = Tree(0, [Tree(1, []), Tree(0, [Tree(2, []), Tree(1, [])])])
    s3 = Tree(0, [Tree(2, []), Tree(0, [Tree(2, []), Tree(1, [])])])
    s4 = Tree(0, [Tree(0, [Tree(2, []), Tree(1, [])]), Tree(1, [])])
    no_swap = [t, t2, t3, t4]
    swap = [s, s2, s3, s4]
    total_prob = 1.0
    p = total_prob/(len(no_swap+swap)*(1+swap_prob))
    trees = [(t, p) for t in no_swap]+[(t, swap_prob*p) for t in swap]
    for tree in trees:
        yield tree


def foo(A):
    if len(A) == 0:
        return 0

    if len(A) == 1:
        return 1

    curr_max = 1

    if len(A) == 2:
        curr_max = 1 if A[0] == A[1] else 2
        return curr_max
    curr = curr_max
    last = A[:2]
    for i in range(2, len(A)):
        if (A[i] > last[1] and last[1] < last[0]) or (A[i] < last[1] and last[1] > last[0]):
            curr = curr + 1
        if A[i] > last[1] > last[0] or A[i] < last[1] < last[0]:
            curr = 2
        if A[i] == last[1]:
            curr = 1
        last[0] = last[1]
        last[1] = A[i]
        curr_max = max(curr, curr_max)
    return curr_max


def calc_kl(a1, a2, tree_list):
    ans = 0
    sum_probs = [0, 0]
    for tree in tree_list:
        for ind, auto in zip([0, 1], [a1, a2]):
            sum_probs[ind] = sum_probs[ind] + auto.run(tree)
        ans = ans + a1.run(tree)*np.log((a1.run(tree))/(a2.run(tree)))
    return ans


def change_leaves(tree, reverse_dict):
    for ind in tree.treepositions():
        t = tree[ind]
        if len(t) == 0:
            tree[ind].set_label(reverse_dict[tree[ind].label()])


def draw_trees(trees, name, reverse_dict=None, print_prob=False):
    for ind, tree in enumerate(trees):
        if reverse_dict:
            change_leaves(tree[0], reverse_dict)
        in_row = 1
        len_one = 140
        height = 200
        i = 0
        cf = CanvasFrame()
        tc = TreeWidget(cf.canvas(), tree[0])
        x, y = (i % in_row)*len_one, int(i / in_row)*height
        cf.add_widget(tc, x, y)
        if print_prob:
            tp = TreeWidget(cf.canvas(), tree[1])
            cf.add_widget(tp, x+len_one, y)
        i = i + 1
        cf.print_to_file('{0}{1}.ps'.format(name, ind))
        cf.destroy()


"""
Grammar:
S->1N1 0.6 | 1N2 0.4
N1->11 0.8 | 22 0.2
N2->12 0.8 | 21 0.2
"""


def main():
    trees = open('michalTrees')
    labels_dict = {}
    reverse_dict = {}
    curr_tag = 0
    trees_list = json.load(trees)
    trees_list = [(Tree.fromstring(tup[0]), tup[1]) for tup in trees_list]

    sum_all = sum([count for tree, count in trees_list])
    for idx, tup in enumerate(trees_list):
        trees_list[idx] = (trees_list[idx][0], trees_list[idx][1]/sum_all)
    # mult_teacher = SimpleMultiplicityTeacher(epsilon=0.0005, default_val=0)
    teacher = SimpleTeacher()
    pos_examples = [(tree, prob) for tree, prob in trees_list]
    for tree, prob in pos_examples:
        print(tree)
        for t in tree.treepositions():
            lbl = tree[t].label()
            if lbl in labels_dict:
                tag = labels_dict[lbl]
            else:
                print('-----')
                print(tree)
                tag = curr_tag
                labels_dict[lbl] = tag
                reverse_dict[tag] = lbl
                curr_tag = curr_tag + 1
            tree[t].set_label(int(tag))
    for tree, prob in pos_examples:
        # mult_teacher.addExample(tree, prob)
        teacher.addPositiveExample(tree)
    print('learning')
    print(reverse_dict)
    cfg = learn(teacher, reverse_dict)
    print(cfg)
    exit()


    tree_list_cfg1 = [(Tree(0, [Tree(1, []), Tree(0, [Tree(1, []), Tree(1, [])])]), 0.48),
                      (Tree(0, [Tree(1, []), Tree(0, [Tree(1, []), Tree(2, [])])]), 0.32),
                      (Tree(0, [Tree(1, []), Tree(0, [Tree(2, []), Tree(1, [])])]), 0.12),
                      (Tree(0, [Tree(1, []), Tree(0, [Tree(2, []), Tree(2, [])])]), 0.08)]


    gen_iter = lambda : generate_distribution([1, 2], max_len=5,
                                               calc_prob=lambda a: calc_prob_non_uniform(a, [1, 2],
                                                                                         letter=1,
                                                                                         letter_prob=0.1,
                                                                                         total_prob=total_prob))

    # gen_iter = lambda : tree_list_cfg1

    lin = np.linspace(0.05, 1.0, 5)
    #def_vals = np.linspace(0.0001, 0.0005, 10)
    kls = []
    kls2 = []
    dims = []
    dims_pos = []
    #total_prob = 0.5
    set_verbose(False)
    for total_prob in lin:
        trees = []
        def_val = (1-total_prob)/10
        teacher = SimpleMultiplicityTeacher(epsilon=0.0005, default_val=def_val)
        teacher2 = SimpleMultiplicityTeacher(epsilon=0.0005, default_val=0)
        iterator = gen_iter()
        for tree, prob in iterator:
            teacher.addExample(tree, prob)
            teacher2.addExample(tree, prob)
            trees.append(tree)
        print('learning')
        acc = learnMultPos(teacher)
        acc2 = learnMult(teacher2)
        norm = acc.get_normalized_acceptor()
        norm.print_desc()
        for tree in trees[:10]:
            print(tree)
            print("acc:{}, normalized:{}".format(acc.run(tree), norm.run(tree)))
        kls.append(calc_kl(norm, acc, trees))
        #dims.append(acc.get_dimension())
        #dims_pos.append(acc2.get_dimension())
    plt.plot(def_vals, kls)
    #plt.plot(def_vals, dims_pos)
    plt.show()
    exit()

    teacher = SimpleMultiplicityTeacher(epsilon=0.0005, default_val=0)
    t = Tree(1, [])
    t2 = Tree(0, [Tree(1, []), Tree(1, [])])
    t3 = Tree(0, [Tree(1, []), Tree(2, [])])
    t4 = Tree(0, [Tree(2, []), Tree(1, [])])
    t5 = Tree(0, [Tree(2, []), Tree(2, [])])
    t6 = Tree(0, [t, t2])
    t7 = Tree(0, [t6, t2])
    t8 = Tree(0, [t2, t7])
    teacher.addExample(t, 0.05)
    teacher.addExample(t2, 0.1)
    teacher.addExample(t3, 0.1)
    teacher.addExample(t4, 0.1)
    teacher.addExample(t5, 0.1)
    teacher.addExample(t6, 0.5)
    teacher.addExample(t7, 0.025)
    teacher.addExample(t8, 0.025)
    print('learning')
    acc = learnMult(teacher)
    print(acc.run(t))
    print(acc.run(t8))
    print(acc.run(Tree(0, [t, t2])))
    print(acc.run(Tree(0, [t2, t])))
    norm = acc.get_normalized_acceptor()
    print(calc_kl(acc, norm, [t, t2, t3, t4, t5, t6, t7, t8]))
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

def old_main():
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

def very_old_main():
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


if __name__ == '__main__':
    main()
