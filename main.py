import json
import sys
import tkinter
from tkinter import *
from tkinter.font import Font
from time import time
from nltk import Tree
from itertools import product
from nltk import PCFG
from nltk.parse.viterbi import ViterbiParser
from nltk import treetransforms
from nltk.draw.util import CanvasFrame
from nltk.draw import TreeWidget
from CFGLearner import SimpleTeacher, FrequencyTeacher, DifferenceTeacher, Teacher, learn, TreeComparator, \
    SimpleMultiplicityTeacher, learnMult, learnMultPos, set_verbose, SwapComparator, DifferenceMultiplicityTeacher, \
    LoggingLevel, TreeConstructor, LOG_DETAILS, LOG_DEBUG
from WCFG import load_trees_from_file, convert_pmta_to_pcfg, compress_grammar
from threading import Thread
from GUI import Table, MapTable, Checklist, PopupWindow, load_object_window
from PIL import Image, ImageFont, ImageDraw
import os

MAIN_FRAME_WIDTH = 1200
MAIN_FRAME_HEIGHT = 1000
TITLE = 'CFG Learner'
MLA_PATH = 'output_mla_manual2.txt'
FILE_PATH = 'michalTrees'
WEIGHTED = True


with open('michalTest') as testData:
    seq = json.load(testData)

for s in seq:
    print(s)


def get_nonterminals(pcfg):
    prods = pcfg.productions()
    alphabet = set()
    for prod in prods:
        for r in prod.rhs():
            if type(r) is str:
                alphabet.add(r)
    return list(alphabet)


UNKNOWN = 'UNKNOWN'


def get_parse_options(seq, alphabet):
    unknown_indices = [ind for ind, s in enumerate(seq) if s==UNKNOWN]
    for tup in product(*[alphabet]*len(unknown_indices)):
        curr_seq = [s for s in seq]
        for ind, token in zip(unknown_indices, tup):
            curr_seq[ind] = token
        yield curr_seq


class MainGUI:

    def __init__(self, title, width, height):
        self._top = None
        self._tree_frame = None
        self._secondary_tree_frame = None
        self._title = title
        self._width = width
        self._height = height
        self._table = None
        self._parse_button = None
        self._pcfg = None
        self._parsed_trees = []

    def set_pcfg(self, pcfg):
        self._pcfg = pcfg
        self._parse_button['state'] = 'normal'

    def parse_command(self, seqs):
        non_terminals = get_nonterminals(self._pcfg)
        viterbi = ViterbiParser(self._pcfg)
        for seq, id in seqs:
            for parse_option in get_parse_options(seq, non_terminals):
                for t in viterbi.parse(parse_option):
                    self._parsed_trees.append((parse_option, t, id))
        trees = [(tree[1], tree[2]) for tree in self._parsed_trees]
        for i, (tree, ind) in enumerate(trees):
            save_tree(tree, None, 'parse{}'.format(i), prob=tree.prob(), csb_id=ind)



    def load_grammar(self):
        grammar = load_object_window()
        grammar = PCFG.fromstring(grammar)
        self.set_pcfg(grammar)


    def init_GUI(self, sequences, alphabet, alphabet_rev, table):
        self._top = tkinter.Tk()
        self._top.geometry('{0}x{1}'.format(self._width, self._height))
        self._top.title(self._title)
        self._scoring_table = table
        seqFrame = Frame(self._top)
        seqFrame.grid(row=0, column=0)
        treeFrame = Frame(self._top)
        treeFrame.grid(row=1, column=0)
        self.create_sequences_list(seqFrame, sequences, alphabet, alphabet_rev)
        self.add_trees_list(treeFrame, [], alphabet_rev, weighted=WEIGHTED)

    def main_loop(self):
        self._top.mainloop()

    def add_trees_list(self, top, trees_list, d, weighted=False):
        secondary_frame = Frame(top)  # To be able to delete this frame.
        secondary_frame.pack()
        save_frame = Frame(secondary_frame)
        save_frame.grid(column=1, row=3)
        save_tree_button = Button(save_frame, pady=10, text='Save tree',
                                  command=lambda: self.save_trees_popup([trees_list[0]], d, save_tree_button))
        save_tree_button.grid(column=0, row=0)
        save_all_trees_button = Button(save_frame, pady=10, text='Save all trees',
                                       command=lambda: self.save_trees_popup(trees_list, d, save_tree_button))
        save_all_trees_button.grid(column=1, row=0)
        tree_frame = Frame(secondary_frame)
        tree_frame.grid(column=1, row=0, rowspan=3)
        cf = CanvasFrame(tree_frame)
        cf.canvas()['width'] = 1000
        cf.canvas()['height'] = 400
        cf.pack()
        myFont = Font(family="Arial", size=16)
        text = Label(secondary_frame, width=20)
        text.configure(font=myFont)
        text.grid(column=0, row=1)
        weight_text = None
        if weighted:
            weight_text = Label(secondary_frame, width=20)
            weight_text.configure(font=myFont)
            weight_text.grid(column=1, row=4)
        trees_check_list = Checklist(['Tree {}'.format(ind) for ind in range(len(trees_list))], secondary_frame,
                                     command=lambda ind: onselect2(ind, trees_list, cf, text, d,
                                                                   weight_text=weight_text))
        trees_check_list.grid(column=0, row=0)
        learn_func = lambda: learn_cmd(trees_check_list.get_selected(), trees_list, d)
        if weighted:
            learn_func = lambda: learn_cmd_prob(trees_check_list.get_selected(), trees_list, d, gui=self)
        grammar_frame = Frame(secondary_frame)
        grammar_frame.grid(column=0, row=1)
        learn_button = Button(grammar_frame, pady=10, text='Learn', command=learn_func)
        learn_button.grid(column=0, row=0)
        load_grammar_button = Button(grammar_frame, text='Load Grammar', command=self.load_grammar)
        load_grammar_button.grid(column=1, row=0)
        parse_button = Button(grammar_frame, text='Parse', command=lambda: self.parse_command(seq))
        parse_button.grid(column=0, row=1)
        parse_button['state'] = 'disabled'
        self._tree_frame = top
        self._parse_button = parse_button
        self._secondary_tree_frame = secondary_frame

    def create_trees_command(self, lambda_val, sequences, alphabet, alphabet_rev):
        try:
            val = float(lambda_val)
        except Exception:
            tkinter.messagebox.showerror("Create Trees", "Invalid value of lambda")
            return
        table = self._scoring_table
        secondary_tree_frame = self._secondary_tree_frame
        tree_frame = self._tree_frame
        self._secondary_tree_frame = None
        secondary_tree_frame.destroy()
        trees = create_trees(sequences, table, alphabet_rev, alphabet, lambda_val=val)
        self.add_trees_list(tree_frame, trees, alphabet_rev, weighted=WEIGHTED)

    @staticmethod
    def convert_sequence(sequence, alphabet_rev_dict):
        return [alphabet_rev_dict[elem] for elem in sequence]

    def create_sequences_list(self, top, sequences, alphabet_dict, alphabet_rev_dict):
        table = self._scoring_table
        t = Table(sequences, ['Sequence', 'Occurrences'], [False, False], top, print_cols=[0],
                  print_funcs=[lambda s: self.convert_sequence(s, alphabet_rev_dict)])
        t2 = MapTable(table, alphabet_rev_dict, top, cols=['Subseq', 'Score'])
        t2.grid(column=0, row=0)
        calculate_map_button = Button(top, text='Calculate scores')
        calculate_map_button.grid(column=0, row=1)
        input_frame = Frame(top)
        input_frame.grid(column=2, row=0)
        lambda_label = Label(input_frame, text='Lambda val:')
        lambda_label.grid(column=0, row=0)
        lambda_text = Entry(input_frame, width=10)
        lambda_text.grid(column=1, row=0)
        create_trees_button = Button(input_frame, text='Create Trees',
                                     command=lambda: self.create_trees_command(lambda_text.get(),
                                                                               sequences, alphabet_dict,
                                                                               alphabet_rev_dict))
        create_trees_button.grid(column=0, row=1, columnspan=2)
        t.grid(column=1, row=0)

    def save_trees_popup(self, trees, d, button):
        top = self._top
        w = PopupWindow(top, 'Choose file name')
        button["state"] = "disabled"
        top.wait_window(w.top)
        button["state"] = "normal"
        if w.value is not None:
            if len(trees) == 1:
                save_tree(trees[0], d, w.value)
            else:
                for ind, tree in enumerate(trees):
                    save_tree(tree, d, '{}{}'.format(w.value, ind))


def learn_cmd(indices, tree_list, d):
    trees = [tree_list[ind][0] for ind in indices]
    teacher = SimpleTeacher()
    for tree in trees:
        teacher.addPositiveExample(tree)
    cfg = learn(teacher, d)
    print(cfg)


def convert_pcfg_to_str(pcfg):
    lst = str(pcfg).split('\n')
    lst = lst[1:]
    for i, l in enumerate(lst):
        lst[i] = l.strip()
    ans = '\n'.join(lst)
    return ans


def learn_cmd_prob(indices, tree_list, reverse_dict, gui=None):
    trees = [tree_list[ind] for ind in indices]
    teacher = SimpleMultiplicityTeacher(epsilon=0.0005, default_val=0)
    for tree in trees:
        teacher.addExample(*tree)

    def thread_task():
        print('starting')
        t = time()
        set_verbose(LOG_DETAILS)
        acc = learnMultPos(teacher)
        g = convert_pmta_to_pcfg(acc, reverse_dict)
        g = compress_grammar(g)
        gui.set_pcfg(g)
        g = convert_pcfg_to_str(g)
        with open('grammar.json', 'w') as json_file:
            json.dump(g, json_file)
        print('took {}'.format(time()-t))
    thread = Thread(target=thread_task)
    thread.start()


def convert_tree(tree, d):
    for ind in tree.treepositions():
        t = tree[ind]
        if len(t) == 0:
            t.set_label(d[int(tree[ind].label())])


def copy_tree(tree):
    if len(tree) == 0:
        return Tree(tree.label(), [])
    ans = Tree(tree.label(), [copy_tree(child) for child in tree])
    return ans


def get_drawable_tree(tree, d):
    copy = copy_tree(tree)
    for ind in copy.treepositions():
        if len(copy[ind]) != 0:
            copy[ind].set_label('?')
            continue
        lbl = int(copy[ind].label())
        if lbl not in d:
            continue
        copy[ind].set_label(d[lbl])
    return copy


def draw_tree(tree, cf, text_box, d, prob=None):
    cf.canvas().delete('all')
    copy = get_drawable_tree(tree, d)
    tc = TreeWidget(cf.canvas(), copy, xspace=50, yspace=50,
                    line_width=2, node_font=('helvetica', -28))

    def show_description(sth):
        t = sth.__repr__()
        t = t.replace('\'', '')
        t = int(t[t.find('Text:')+len('Text: '):t.find(']')])
        text_box['text'] = d[t]
    tc.bind_click_nodes(show_description, button=1)
    cf.add_widget(tc, 400, 0)
    if prob is not None:
        tp = TreeWidget(cf.canvas(), prob)
        cf.add_widget(tp, 0, 400)


def onselect(evt, trees_list, cf, text_box, d, weight_text=None):
    w = evt.widget
    if len(w.curselection()) == 0:
        return
    index = int(w.curselection()[0])
    tree = trees_list[index][0]
    prob = trees_list[index][1]
    weight_text['text'] = 'Probability:{0:.4f}'.format(trees_list[index][1])
    draw_tree(tree, cf, text_box, d, prob=prob)


def onselect2(index, trees_list, cf, text_box, d, weight_text=None):
    tree = trees_list[index][0]
    prob = trees_list[index][1]
    weight_text['text'] = 'Probability:{0:.4f}'.format(trees_list[index][1])
    draw_tree(tree, cf, text_box, d, prob=prob)


def save_tree(tree, d, name, prob=None, csb_id=None):
    drawable_tree = tree if d is None else get_drawable_tree(tree[0], d)
    cf = CanvasFrame()
    tc = TreeWidget(cf.canvas(), drawable_tree)
    tc['node_font'] = 'arial 22 bold'
    tc['leaf_font'] = 'arial 22'
    tc['node_color'] = '#005990'
    tc['leaf_color'] = '#3F8F57'
    tc['line_color'] = '#175252'
    tc['xspace'] = 20
    tc['yspace'] = 20
    cf.add_widget(tc, 0, 0)
    cf.print_to_file('{0}.png'.format(name))
    image1 = Image.open('{0}.png'.format(name))
    im1 = image1.convert('RGB')
    if prob is not None:
        font = ImageFont.truetype("/fonts/Ubuntu-L.ttf", 24)
        draw = ImageDraw.Draw(im1)
        draw.text((10, 10), 'p={0:.4f}'.format(prob), (0, 0, 0), font)
        draw.text((10, 40), 'csb_id={}'.format(csb_id), (0, 0, 0), font)
    im1.save('{}.pdf'.format(name))
    cf.destroy()
    os.remove('{}.png'.format(name))


def create_window(root):
    window = Toplevel(root)
    window.geometry('{0}x{1}'.format(400, 400))
    b = Button(window, text='close', command=lambda: print('hello'))
    b.grid(column=0, row=0)


def convert_string(curr_str, alphabet, alphabet_rev, last_ind):
    for ind, elem in enumerate(curr_str):
        if elem not in alphabet:
            alphabet[elem] = last_ind[0]
            alphabet_rev[last_ind[0]] = elem
            last_ind[0] += 1
        curr_str[ind] = alphabet[elem]


def get_all_score_tuples(seq, alphabet, alphabet_rev):
    non_duplicates = [alphabet_rev[seq[ind]] for ind in range(len(seq))]
    non_duplicates = [dup if '$' not in dup else dup[:dup.index('$')] for dup in non_duplicates]
    yield seq
    return
    for tup in product(*[[0, 2, 3, 4]]*len(seq)):
        new_tup = []
        for non_dup, dup_factor in zip(non_duplicates, tup):
            if dup_factor == 0:
                duplicated = non_dup
            else:
                duplicated = '{}${}'.format(non_dup, dup_factor)
            if duplicated not in alphabet:
                continue
            new_tup.append(duplicated)
        if len(new_tup) == 0:
            continue
        yield tuple([alphabet[t] for t in new_tup])


def get_score_table(sequences, alphabet, alphabet_rev):
    table = {}
    for ngram_len in range(2, len(sequences)):
        for seq, occ in sequences:
            for i in range(0, len(seq)-ngram_len+1):
                subseq = seq[i: i+ngram_len]
                for tup in get_all_score_tuples(subseq, alphabet, alphabet_rev):
                    if tup not in table:
                        table[tup] = occ
                    else:
                        table[tup] += occ
    return table


def pre_process_seq(seq):
    pairs = []
    ans = []
    last_ind = 0
    for i in range(len(seq)):
        if seq[last_ind] != seq[i]:
            pairs.append((last_ind, i-1))
            last_ind = i
    pairs.append((last_ind, len(seq)-1))
    pairs = list(filter(lambda p: p[0] != p[1], pairs))
    last = 0
    for i1, i2 in pairs:
        ans = ans + seq[last:i1] + ['{}${}'.format(seq[i1], (i2-i1+1))]
        last = i2+1
    ans = ans + seq[last:]
    return ans


def pre_process_tree(tree, rev_map, alphabet):
    for ind in tree.treepositions():
        t = tree[ind]
        if len(t) == 0:
            lbl = rev_map[int(tree[ind].label())]
            if '$' in lbl:
                lbl = lbl.split('$')
                lbl, occ = lbl
                occ = int(occ)
                tree[ind] = Tree(0, [Tree(alphabet[lbl], [])]*occ)
    return tree


def read_strings(file_path):
    seq_dict = {}
    alphabet = {}
    alphabet_rev = {}
    curr_ind = [0]
    with open(file_path) as file:
        data = json.load(file)
        for seq, occ in data:
            # seq = pre_process_seq(seq)
            convert_string(seq, alphabet, alphabet_rev, curr_ind)
            s = tuple(seq)
            if s not in seq_dict:
                seq_dict[s] = occ
            else:
                seq_dict[s] += occ
    sequences = []
    for seq in seq_dict.keys():
        sequences.append((seq, seq_dict[seq]))
    sequences = sorted(sequences, key=lambda a: a[1])
    if False:
        seq_l = []
        for seq in sequences:
            seq2 = [alphabet_rev[i] for i in seq[0]]
            seq_l.append((seq2, seq[1]))
        for s in seq_l:
            print(s)
        exit()
    table = get_score_table(sequences, alphabet, alphabet_rev)
    return sequences, alphabet, alphabet_rev, table


def convert_tree_to_cnf(tree):
    treetransforms.chomsky_normal_form(tree)
    for ind in tree.treepositions():
        t = tree[ind]
        if len(t) != 0:
            tree[ind].set_label('0')


def normalize_trees(trees):
    total_sum = sum([occ for _, occ in trees])
    for ind, tup in enumerate(trees):
        trees[ind] = (tup[0], tup[1]/total_sum)


def create_trees(sequences, table, alphabet_rev, alphabet, contract=False, lambda_val=0.0):
    ans = []
    constructor = TreeConstructor(table)
    sequences = filter(lambda a: a is not None, sequences)
    for seq, occ in sequences:
        constructor.set_lambda(lambda_val)
        curr_tree = constructor.construct_tree(seq)
        if contract:
            convert_tree_to_cnf(curr_tree)
        ans.append((curr_tree, occ))
    normalize_trees(ans)
    for ind, tree in enumerate(ans):
        ans[ind] = (pre_process_tree(tree[0], alphabet_rev, alphabet), tree[1])
    return ans


def get_table_as_list(table, alphabet_rev):
    keys = table.keys()
    keys_copy = []
    for ind, key in enumerate(keys):
        keys_copy.append(tuple([alphabet_rev[k] for k in key]))
    return [(key, table[orig_key]) for key, orig_key in zip(keys_copy, keys)]


def update_table(table, alphabet, list):
    for key, val in list:
        key_new = tuple([alphabet[k] for k in key])
        table[key_new] = val


def get_trees_list(file_path, weighted=False):
    if weighted:
        trees_list, labels_dict, reverse_dict = load_trees_from_file(file_path)
        return trees_list, reverse_dict
    file = open(file_path)
    trees_list = json.load(file)
    d = trees_list['cogs_dict']['reverse_dict']
    d = {int(key): d[key] for key in d}
    trees_list = [(Tree.fromstring(tup[0]), tup[1]) for tup in trees_list['trees']]
    return trees_list, d


if __name__ == '__main__':
    args = sys.argv
    if len(args) < 2:
        print('Not enough arguments')
        exit()
    sequences, alphabet, alphabet_rev, table = read_strings(args[1])
    gui = MainGUI(TITLE, MAIN_FRAME_WIDTH, MAIN_FRAME_HEIGHT)
    gui.init_GUI(sequences, alphabet, alphabet_rev, table)
    gui.main_loop()
