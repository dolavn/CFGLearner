from TreeGenerator import generate_trees, generate_distribution
from test import calc_prob_non_uniform, get_trees_swap_test
from nltk import PCFG
import numpy as np
import matplotlib.pyplot as plt
from itertools import product
import json
#from test import draw_trees
from functools import reduce
from nltk import Tree
import re
from nltk.grammar import ProbabilisticProduction
from nltk.probability import ImmutableProbabilisticMixIn
from nltk.parse import ViterbiParser
"""from CFGLearner import SimpleTeacher, FrequencyTeacher, DifferenceTeacher, Teacher, learn, TreeComparator, test_arma, \
    SimpleMultiplicityTeacher, learnMult, learnMultPos, set_verbose, SwapComparator, DifferenceMultiplicityTeacher,\
    LoggingLevel
"""

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


def get_leaves(nltk_tree, reverse_dict=None):
    leaves = []
    func = lambda a: a if not reverse_dict else reverse_dict[a]
    for child in nltk_tree:
        if not isinstance(child, Tree):
            leaves.append(func(child))
        else:
            if len(child) > 0:
                leaves.extend(get_leaves(child, reverse_dict))
            else:
                leaves.append(func(child.label()))
    return leaves


def get_grammar(acc, reverse_dict=None):
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
                if reverse_dict:
                    terminal = reverse_dict[terminal_ind]
                else:
                    terminal = terminal_ind
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


def get_compress_dict(grammar):
    comp_dict = {}
    for p in grammar.productions():
        nt_type = type(p.lhs())
        if all([type(elem) is not nt_type for elem in p.rhs()]) and p.prob() == 1.0:
            comp_dict[p.lhs()] = p.rhs()[0]
    return comp_dict


def compress_grammar(grammar):
    comp_dict = get_compress_dict(grammar)
    grammar._productions = list(filter(lambda p: p.lhs() not in comp_dict, grammar._productions))
    for ind, prod in enumerate(grammar._productions):
        rhs = [r if r not in comp_dict else comp_dict[r] for r in prod.rhs()]
        new_prod = ProbabilisticProduction(prod.lhs(), rhs)
        new_prod.prob = prod.prob
        grammar._productions[ind] = new_prod
    return grammar


def create_duplications(grammar, dup_prob):
    dup_prods = []
    for ind, prod in enumerate(grammar._productions):
        rhs = prod.rhs()
        if len(rhs) == 1 and type(rhs[0]) is str:
            if prod.prob() != 1.0:
                raise BaseException("Can't handle this currently")
            new_prod = ProbabilisticProduction(prod.lhs(), rhs, prob=1-dup_prob)
            grammar._productions[ind] = new_prod
            dup_prod = ProbabilisticProduction(prod.lhs(), [rhs[0], prod.lhs()], prob=dup_prob)
            dup_prods.append(dup_prod)
    for dup_prod in dup_prods:
        grammar._productions.append(dup_prod)
    return grammar



MAX_ITER = 100


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
            conv[variable].append(new_value)
        for variable in variables.keys():
            variables[variable]['value'] = new_values[variable]
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
    for nt in g['nt']:
        total = 0
        prod_list = []
        for prod in g['prod']:
            if prod.get_lhs() == nt:
                total = total + prod.get_weight()
                prod_list.append(prod)
        if total > 1:
            for prod in prod_list:
                w = prod.get_weight()/total
                prod.set_weight(w)
    return g


def get_nltk_pcfg(g):
    prod_lists = [[prod for prod in g['prod'] if prod.get_lhs() == nt] for nt in g['nt']]
    #print([len(p) for p in prod_lists])
    prod_lists = filter(lambda a: len(a) > 0, prod_lists)
    strings = ['{0} -> {1}'.format(prod_list[0].get_lhs(),
                                   ('{} [{}] | '*(len(prod_list)-1)+'{} [{}]').format(
                                       *sum([[' '.join(prod.get_rhs()), prod.get_weight()] for prod in prod_list],
                                            [])
                                   )) for prod_list in prod_lists]
    strings = '\n'.join(strings)
    return PCFG.fromstring(strings)


def convert_pmta_to_pcfg(pmta, reverse_dict=None):
    g = get_grammar(pmta, reverse_dict)
    g = normalize_grammar(g)
    return get_nltk_pcfg(g)


def get_subset(lst, indices):
    ans = []
    for index in indices:
        ans.append(lst[index])
    return ans

