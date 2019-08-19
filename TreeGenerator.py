from nltk import Tree
from typing import List, Tuple, Callable
from itertools import product



def convert_to_tree(seq: List[int]) -> Tree:
    last_tree = None
    while len(seq) >= 1:
        if last_tree is None:
            curr_suffix = [Tree(s, []) for s in seq[-2:]]
            seq = seq[:-2]
            last_tree = Tree(0, curr_suffix)
            continue
        last_tree = Tree(0, [last_tree, Tree(seq[-1], [])])
        seq = seq[:-1]
    return last_tree


def generate_distribution(alphabet: List[int], calc_prob: Callable[[List[int]], float] = None,
                          max_len: int = 1) -> Tuple[Tree, float]:
    if not calc_prob:
        calc_prob = lambda a: 1/(len(alphabet)**max_len)
    total_prob = 0
    for tuples in product(*[range(len(alphabet)) for _ in range(max_len)]):
        curr_seq = [0]*max_len
        for i in range(max_len):
            curr_seq[i] = alphabet[tuples[i]]
        curr_prob = calc_prob(curr_seq)
        total_prob += curr_prob
        if total_prob > 1:
            raise BaseException("Total probability exceeds 1")
        yield (convert_to_tree(curr_seq), curr_prob)


def generate_trees(string: List[int], max_len=-1) -> Tree:
    yield from generate_trees_rec(string, 0, len(string)-1, max_len)


def generate_trees_rec(string: List[int], start: int, fin: int, max_len=-1) -> Tree:
    if fin-start > 1:
        for i in range(start, fin):
            for tree_l, tree_r in product(generate_trees_rec(string, start, i, max_len),
                                          generate_trees_rec(string, i+1, fin, max_len)):
                yield Tree(0, [tree_l, tree_r])
    if start == fin:
        yield Tree(string[start], [])
    else:
        if fin+1-start <= max_len or max_len == -1:
            yield Tree(0, [Tree(string[i], []) for i in range(start, fin+1)])
