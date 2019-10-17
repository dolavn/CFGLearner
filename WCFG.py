from TreeGenerator import generate_trees, generate_distribution
from test import calc_prob_non_uniform
import numpy as np
from itertools import product
from CFGLearner import SimpleTeacher, FrequencyTeacher, DifferenceTeacher, Teacher, learn, TreeComparator, test_arma, \
    SimpleMultiplicityTeacher, learnMult, learnMultPos, set_verbose


gen_iter = lambda : generate_distribution([1, 2], max_len=5,
                                          calc_prob=lambda a: calc_prob_non_uniform(a, [1, 2],
                                                                                    letter=1,
                                                                                    letter_prob=0.1,
                                                                                    total_prob=total_prob))


class WeightedProduction:

    def __init__(self, lhs, rhs, weight):
        self._lhs = lhs
        self._rhs = rhs
        self._weight = weight

    def __repr__(self):
        return '{0}->{1} ({2})'.format(self._lhs, ' '.join(self._rhs),
                                       self._weight)


def get_productions(multi_map, terminal=None):
    inp_dim = multi_map.get_input_num()
    out_dim = multi_map.get_dim()
    for a in product(*[range(out_dim) for _ in range(inp_dim+1)]):
        p = multi_map.get_param(a)
        if p != 0:
            lhs = 'N{0}'.format(a[0]+1)
            if terminal is None:
                rhs = ['N{0}'.format(i+1) for i in a[1:]]
            else:
                rhs = [terminal]
            prod = WeightedProduction(lhs, rhs, p)
            print(prod)




# gen_iter = lambda : tree_list_cfg1

lin = [0.5]
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
    m = acc.get_map_terminal(1)
    m2 = acc.get_map_terminal(2)
    nt2 = acc.get_map_non_terminal(2)
    get_productions(m, terminal='1')
    get_productions(m2, terminal='2')
    get_productions(nt2)
exit()
