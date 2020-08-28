from nltk import Tree
from WCFG import convert_pmta_to_pcfg

def traverse_tree(tree, leaves=False, get_trees=False):
    if type(tree) != Tree:
        raise BaseException("not a tree")
    for child in tree:
        yield from traverse_tree(child, leaves=leaves, get_trees=get_trees)
    if type(tree) == Tree:
        if not leaves or len(tree) == 0:
            yield tree.label() if not get_trees else tree


def traverse_leaves(tree, get_trees=False): yield from traverse_tree(tree, leaves=True, get_trees=get_trees)


def deep_copy_tree(tree):
    if type(tree) == Tree:
        return Tree(tree.label(), [deep_copy_tree(child) for child in tree])
    return tree


def convert_leaf(leaf, translate_dict):
        leaf.set_label(translate_dict[leaf.label()])


class TreesConverter:

    def __init__(self):
        self._alphabet_dict = {}
        self._reverse_dict = {}
        self._curr_ind = 0

    def convert_tree(self, tree):
        for leaf in traverse_leaves(tree):
            if leaf not in self._alphabet_dict:
                self._alphabet_dict[leaf] = self._curr_ind
                self._reverse_dict[self._curr_ind] = leaf
                self._curr_ind = self._curr_ind + 1
        new_tree = deep_copy_tree(tree)
        for leaf in traverse_leaves(new_tree, get_trees=True):
            convert_leaf(leaf, self._alphabet_dict)
        return new_tree

    def reverse_convert_tree(self, tree):
        new_tree = deep_copy_tree(tree)
        for leaf in traverse_leaves(new_tree, get_trees=True):
            convert_leaf(leaf, self._reverse_dict)
        return new_tree

    def get_grammar(self, pmta):
        return convert_pmta_to_pcfg(pmta, self._reverse_dict)


def testing():
    converter = TreesConverter()
    t = Tree('?', [Tree('a', []), Tree('b', [])])
    converted = converter.convert_tree(t)
    backwards = converter.reverse_convert_tree(converted)
    print(t, converted, backwards)


if __name__ == '__main__':
    testing()
