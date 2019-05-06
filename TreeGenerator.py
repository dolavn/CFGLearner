from nltk import Tree
from typing import List
from itertools import product


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
