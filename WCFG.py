from TreeGenerator import generate_trees, generate_distribution
from test import calc_prob_non_uniform, get_trees_swap_test
from nltk import PCFG
import numpy as np
import matplotlib.pyplot as plt
from itertools import product
import json
from test import draw_trees
from functools import reduce
from nltk import Tree
import re
from nltk.parse import ViterbiParser
from CFGLearner import SimpleTeacher, FrequencyTeacher, DifferenceTeacher, Teacher, learn, TreeComparator, test_arma, \
    SimpleMultiplicityTeacher, learnMult, learnMultPos, set_verbose


lengths = [2, 4]
gen_iter = lambda: generate_distribution([1, 2], lengths=lengths,
                                         calc_prob=lambda a: calc_prob_non_uniform(a, [1, 2],
                                                                                    letter=1,
                                                                                    letter_prob=0.1,
                                                                                    total_prob=total_prob,
                                                                                    lengths=lengths))


class Symbol:

    def __init__(self, symbol):
        self._symbol = symbol


class NonTerminal(Symbol):

    def __init__(self, symbol):
        Symbol.__init__(symbol)


class Terminal(Symbol):

    def __init__(self, symbol):
        Symbol.__init__(symbol)


class WeightedProduction:

    def __init__(self, lhs, rhs, weight):
        self._lhs = lhs
        self._rhs = rhs
        self._weight = weight

    def get_lhs(self):
        return self._lhs

    def get_rhs(self):
        return self._rhs

    def get_weight(self):
        return self._weight

    def set_weight(self, weight):
        self._weight = weight

    def __repr__(self):
        return '{0}->{1} ({2})'.format(self._lhs, ' '.join(self._rhs),
                                       self._weight)


def get_productions(multi_map, terminal=None):
    inp_dim = multi_map.get_input_num()
    out_dim = multi_map.get_dim()
    ans = []
    for a in product(*[range(out_dim) for _ in range(inp_dim+1)]):
        p = multi_map.get_param(a)
        if p != 0:
            lhs = 'N{0}'.format(a[0]+1)
            if terminal is None:
                rhs = ['N{0}'.format(i+1) for i in a[1:]]
            else:
                rhs = ['\'{}\''.format(terminal)]
            prod = WeightedProduction(lhs, rhs, p)
            ans.append(prod)
    return ans


def get_leaves(nltk_tree, reverse_dict):
    leaves = []
    for child in nltk_tree:
        if not isinstance(child, Tree):
            leaves.append(reverse_dict[child])
        else:
            if len(child) > 0:
                leaves.extend(get_leaves(child, reverse_dict))
            else:
                leaves.append(reverse_dict[child.label()])
    return leaves



def get_grammar(acc, reverse_dict):
    prods = []
    g = {'prod': [], 'nt': ['S'], 'terminals': []}
    for ind, weight in enumerate(acc.get_lambda()):
        nt = 'N{0}'.format(ind+1)
        g['nt'].append(nt)
        if weight != 0:
            g['prod'].append(WeightedProduction('S', [nt], weight))
    for rank in acc.get_ranks():
        if rank == 0:
            terminals = acc.get_alphabet(rank)
            for terminal_ind in terminals:
                terminal = reverse_dict[terminal_ind]
                g['terminals'].append(terminal)
                g['prod'] = g['prod'] + get_productions(acc.get_map_terminal(terminal_ind),
                                                        terminal=str(terminal))
        else:
            g['prod'] = g['prod'] + get_productions(acc.get_map_non_terminal(rank))
    return g


def get_equations(g):
    variables = {}
    for nt in g['nt']:
        lst = [prod for prod in g['prod'] if prod.get_lhs() == nt]
        variables[nt] = {'value': 0, 'equation': lst}
    return variables


MAX_ITER = 10


def calc_partition_functions(variables):
    conv = {}
    for variable in variables:
        conv[variable] = [variables[variable]['value']]
    for _ in range(MAX_ITER):
        new_values = {}
        for variable in variables.keys():
            equation = variables[variable]['equation']
            new_value = sum([prod.get_weight()*reduce(lambda x, y: x*y,
                                                      [1 if nt not in variables else variables[
                                                          nt]['value'] for nt in prod.get_rhs()]) for prod in equation])
            new_values[variable] = new_value
            print(variable, new_value)
            conv[variable].append(new_value)
        for variable in variables.keys():
            variables[variable]['value'] = new_values[variable]
    for variable in variables.keys():
        plt.plot(conv[variable])
    plt.show()
    return variables


def normalize_grammar(g):
    variables = get_equations(g)
    variables = calc_partition_functions(variables)
    for prod in g['prod']:
        w = reduce(lambda x, y: x*y, [1 if nt not in variables else
                                      variables[nt]['value'] for nt in prod.get_rhs()])
        w = w * prod.get_weight()
        w = w / variables[prod.get_lhs()]['value']
        prod.set_weight(w)
    return g


def get_nltk_pcfg(g):
    prod_lists = [[prod for prod in g['prod'] if prod.get_lhs() == nt] for nt in g['nt']]
    print([len(p) for p in prod_lists])
    strings = ['{0} -> {1}'.format(prod_list[0].get_lhs(),
                                   ('{} [{}] | '*(len(prod_list)-1)+'{} [{}]').format(
                                       *sum([[' '.join(prod.get_rhs()), prod.get_weight()] for prod in prod_list],
                                            [])
                                   )) for prod_list in prod_lists]
    strings = '\n'.join(strings)
    return PCFG.fromstring(strings)


def convert_pmta_to_pcfg(pmta, reverse_dict):
    g = get_grammar(pmta, reverse_dict)
    g = normalize_grammar(g)
    return get_nltk_pcfg(g)


def get_subset(lst, indices):
    ans = []
    for index in indices:
        ans.append(lst[index])
    return ans


def test_pcfg(trees, pcfg, reverse_dict):
    parser = ViterbiParser(pcfg)
    all_parses = []
    for tree, prob in trees:
        leaves = get_leaves(tree, reverse_dict=reverse_dict)
        parses = parser.parse_all(leaves)
        all_parses.append((parses[0], parses[0].prob()))
        assert abs(parses[0].prob()-prob) < 0.00001
    draw_trees(all_parses, 'parses', print_prob=True)


def load_trees_from_file(path, indices=None):
    trees = open(path)
    trees_dict = {}
    trees_set = set()
    labels_dict = {}
    reverse_dict = {}
    curr_tag = 0
    trees_list = json.load(trees)
    trees_list = [(Tree.fromstring(tup[0]), tup[1]) for tup in trees_list]
    if indices:
        trees_list = get_subset(trees_list, indices)
    for ind, (tree, prob) in enumerate(trees_list):
        curr_str = re.sub('[\+-]', '', str(tree))
        if curr_str not in trees_dict:
            trees_dict[curr_str] = prob
        else:
            trees_dict[curr_str] = trees_dict[curr_str] + prob
    total_weight = sum([t[1] for t in trees_list])
    new_trees_list = []
    for ind in range(len(trees_list)):
        tree = trees_list[ind][0]
        orig_tree_str = re.sub('[\+-]', '', str(tree))
        if orig_tree_str in trees_set:
            continue
        trees_set.add(orig_tree_str)
        for t in tree.treepositions():
            lbl = tree[t].label()
            if lbl == '':
                continue
            if lbl[len(lbl)-1] in ['+', '-']:
                lbl = tree[t].label()[:-1]
            if lbl in labels_dict:
                tag = labels_dict[lbl]
            else:
                tag = curr_tag
                labels_dict[lbl] = tag
                reverse_dict[tag] = lbl
                curr_tag = curr_tag + 1
            tree[t].set_label(tag)
        trees_list[ind] = (trees_list[ind][0], trees_dict[orig_tree_str] / total_weight)
        new_trees_list.append(trees_list[ind])
    return new_trees_list, labels_dict, reverse_dict


subset = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14]
trees_list, labels_dict, reverse_dict = load_trees_from_file('michalTrees', indices=subset)
draw_trees(trees_list, 'treesNumbers', print_prob=True)
print(trees_list)
print(reverse_dict)
#exit()
# gen_iter = lambda : tree_list_cfg1
p, q, r, s = 0.6, 0.8, 0.3, 0.9
toy_grammar = {'prod': [WeightedProduction('A', ['a', 'A'], p),
                        WeightedProduction('A', ['B'], r),
                        WeightedProduction('B', ['B', 'b'], q),
                        WeightedProduction('B', ['b'], s)],
               'nt': ['A', 'B'], 'terminals': ['a', 'b']}
#g = normalize_grammar(toy_grammar)
#g = get_nltk_pcfg(g)
lin = [0.25]
#def_vals = np.linspace(0.0001, 0.0005, 10)
kls = []
kls2 = []
dims = []
dims_pos = []
#total_prob = 0.5
set_verbose(False)
prods = []
for total_prob in lin:
    trees = []
    def_val = 0
    teacher = SimpleMultiplicityTeacher(epsilon=0.0005, default_val=def_val)
    teacher2 = SimpleTeacher()
    #iterator = gen_iter()
    iterator = get_trees_swap_test(swap_prob=0.5)
    print(trees_list)
    for tree, prob in trees_list:
        print(tree, prob)
        teacher.addExample(tree, prob)
        teacher2.addPositiveExample(tree)
        trees.append(tree)
    print('learning')
    acc = learnMultPos(teacher)
    acc.print_desc()
    g = convert_pmta_to_pcfg(acc, reverse_dict)
    #cfg = learn(teacher2, reverse_dict)
    #print(cfg)
    print(g)
test_pcfg(trees_list, g, reverse_dict)
draw_trees(trees_list, 'treesMichalDraw', reverse_dict=reverse_dict, print_prob=True)